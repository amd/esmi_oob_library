/*
 * University of Illinois/NCSA Open Source License
 *
 * Copyright (c) 2020, Advanced Micro Devices, Inc.
 * All rights reserved.
 *
 * Developed by:
 *
 *		AMD Research and AMD Software Development
 *
 *		Advanced Micro Devices, Inc.
 *
 *		www.amd.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal with the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 *  - Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimers.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimers in
 *    the documentation and/or other materials provided with the distribution.
 *  - Neither the names of <Name of Development Group, Name of Institution>,
 *    nor the names of its contributors may be used to endorse or promote
 *    products derived from this Software without specific prior written
 *    permission.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS WITH THE SOFTWARE.
 *
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>

#include <esmi_oob/esmi_mailbox.h>
#include <esmi_oob/apml.h>
#include <esmi_oob/esmi_cpuid_msr.h>
#include <esmi_oob/esmi_rmi.h>

/* MASKS */

/* Mask for bmc control pcie rate */
#define GEN5_RATE_MASK		3
/* one byte mask for DDR bandwidth */
#define ONE_BYTE_MASK		0XFF
/* Teo byte mask */
#define TWO_BYTE_MASK		0xFFFF
/* Four byte mask used in raplcoreenergy, raplpackageenergy */
#define FOUR_BYTE_MASK		0xFFFFFFFF
/* CPU index mask used in boost limit write */
#define CPU_INDEX_MASK		0xFFFF0000
/* TU Mask used in read bmc rapl units */
#define TU_MASK			0xF
/* ESU Mask in read bmc rapl units */
#define ESU_MASK		0x1F
/* FCLK Mask used in read current df-pstate frequency */
#define FCLK_MASK		0xFFF
/* Bandwidth Mask used in reading ddr bandwidth */
#define BW_MASK			0xFFF

/* CONSTANTS OR MAGIC NUMBERS */

/* Max limit for DPM level */
#define MAX_DPM_LIMIT		3
/* Maximum link width */
#define FULL_WIDTH		2
/* Max Gen5 rate for bmc control pcie rate */
#define GEN5_RATE		2
/* Max limit for XGMI link */
#define MAX_XGMI_LINK		2
/* Maximum value for df p-state limit */
#define MAX_DF_PSTATE_LIMIT	2

float esu_multiplier;
struct processor_info plat_info[1];

/*
 * Validates max and min values.Max values should always be greater
 * than or equal to the min value.
 */
static oob_status_t validate_max_min_values(uint8_t max_value,
					    uint8_t min_value,
					    uint8_t max_limit)
{
	if (max_value > max_limit | max_value < min_value)
		return OOB_INVALID_INPUT;

	return OOB_SUCCESS;
}

static oob_status_t validate_pwr_efficiency_mode(uint8_t value)
{
	switch (value) {
	case 0:
	case 1:
	case 2:
		return OOB_SUCCESS;
	default:
		return OOB_INVALID_INPUT;
	}
}

oob_status_t read_socket_power(uint8_t soc_num, uint32_t *buffer)
{
	return esmi_oob_read_mailbox(soc_num, READ_PACKAGE_POWER_CONSUMPTION,
				     0, buffer);
}

oob_status_t read_socket_power_limit(uint8_t soc_num, uint32_t *buffer)
{
	return esmi_oob_read_mailbox(soc_num, READ_PACKAGE_POWER_LIMIT,
				     0, buffer);
}

oob_status_t read_max_socket_power_limit(uint8_t soc_num, uint32_t *buffer)
{
	return esmi_oob_read_mailbox(soc_num, READ_MAX_PACKAGE_POWER_LIMIT,
				     0, buffer);
}

oob_status_t read_tdp(uint8_t soc_num, uint32_t *buffer)
{
	return esmi_oob_read_mailbox(soc_num,
				     READ_TDP, 0, buffer);
}

oob_status_t read_max_tdp(uint8_t soc_num, uint32_t *buffer)
{
	return esmi_oob_read_mailbox(soc_num, READ_MAX_cTDP,
				     0, buffer);
}

oob_status_t read_min_tdp(uint8_t soc_num, uint32_t *buffer)
{
	return esmi_oob_read_mailbox(soc_num, READ_MIN_cTDP,
				     0, buffer);
}

oob_status_t write_socket_power_limit(uint8_t soc_num, uint32_t limit)
{
	return esmi_oob_write_mailbox(soc_num,
				      WRITE_PACKAGE_POWER_LIMIT, limit);
}

oob_status_t read_bios_boost_fmax(uint8_t soc_num,
				  uint32_t value, uint32_t *buffer)
{
	uint8_t rev;
	oob_status_t ret;

	ret = read_sbrmi_revision(soc_num, &rev);
	if (ret)
		return ret;
	if (rev == 0x20) {
		if (!plat_info->family) {
			ret = esmi_get_processor_info(soc_num, plat_info);
			if (ret)
				return ret;
			}
		if (plat_info->family == 0x19) {
			switch (plat_info->model) {
			case 0x30 ... 0x3F:
				break;
			default:
				value <<= 16;
				break;
			}
		}
	}
	return esmi_oob_read_mailbox(soc_num,
				     READ_BIOS_BOOST_Fmax,
				     value, buffer);
}

oob_status_t read_esb_boost_limit(uint8_t soc_num,
				  uint32_t value, uint32_t *buffer)
{
	uint8_t rev;
	oob_status_t ret;

	ret = read_sbrmi_revision(soc_num, &rev);
	if (ret)
		return ret;
	if (rev == 0x20) {
		if (!plat_info->family) {
			ret = esmi_get_processor_info(soc_num, plat_info);
			if (ret)
				return ret;
		}

		if (plat_info->family == 0x19) {
			switch (plat_info->model) {
			case 0x30 ... 0x3F:
				break;
			default:
				value <<= 16;
				break;
			}
		}
	}
	return esmi_oob_read_mailbox(soc_num,
				     READ_APML_BOOST_LIMIT,
				     value, buffer);
}

oob_status_t write_esb_boost_limit(uint8_t soc_num,
				   uint32_t cpu_ind, uint32_t limit)
{
	limit = (limit & TWO_BYTE_MASK) | ((cpu_ind << 16) & CPU_INDEX_MASK);

	return esmi_oob_write_mailbox(soc_num,
				      WRITE_APML_BOOST_LIMIT, limit);
}

oob_status_t write_esb_boost_limit_allcores(uint8_t soc_num,
					    uint32_t limit)
{
	limit &= TWO_BYTE_MASK;
	return esmi_oob_write_mailbox(soc_num,
				      WRITE_APML_BOOST_LIMIT_ALLCORES, limit);
}

oob_status_t read_dram_throttle(uint8_t soc_num, uint32_t *buffer)
{
	return esmi_oob_read_mailbox(soc_num,
				     READ_DRAM_THROTTLE, 0, buffer);
}

oob_status_t write_dram_throttle(uint8_t soc_num, uint32_t limit)
{
	/* As per SSP PPR, Write can be 0 to 80%, But read is 0 to 100% */
	return esmi_oob_write_mailbox(soc_num,
				      WRITE_DRAM_THROTTLE, limit);
}

oob_status_t read_prochot_status(uint8_t soc_num, uint32_t *buffer)
{
	return esmi_oob_read_mailbox(soc_num, READ_PROCHOT_STATUS, 0, buffer);
}

oob_status_t read_prochot_residency(uint8_t soc_num, float *buffer)
{
	uint32_t residency;
	oob_status_t ret;

	if (!buffer)
		return OOB_ARG_PTR_NULL;

	ret = esmi_oob_read_mailbox(soc_num,
				    READ_PROCHOT_RESIDENCY, 0, &residency);
	if (ret)
		return ret;
	*buffer = ((float)(residency & TWO_BYTE_MASK) / TWO_BYTE_MASK) * 100;

	return OOB_SUCCESS;
}

oob_status_t
read_nbio_error_logging_register(uint8_t soc_num,
				 struct nbio_err_log nbio,
				 uint32_t *buffer)
{
	uint32_t input;

	input = nbio.quadrant << 24 | nbio.offset;
	return esmi_oob_read_mailbox(soc_num,
				     READ_NBIO_ERROR_LOGGING_REGISTER,
				     input, buffer);
}

oob_status_t read_iod_bist(uint8_t soc_num, uint32_t *buffer)
{
	if (!buffer)
		return OOB_ARG_PTR_NULL;

	return esmi_oob_read_mailbox(soc_num, READ_IOD_BIST,
				     0, buffer);
}

oob_status_t read_ccd_bist_result(uint8_t soc_num,
				  uint32_t input, uint32_t *buffer)
{
	return esmi_oob_read_mailbox(soc_num,
				     READ_CCD_BIST_RESULT, input, buffer);
}

oob_status_t read_ccx_bist_result(uint8_t soc_num,
				  uint32_t value, uint32_t *ccx_bist)
{
	return esmi_oob_read_mailbox(soc_num, READ_CCX_BIST_RESULT,
				     value, ccx_bist);
}

oob_status_t read_ddr_bandwidth(uint8_t soc_num,
				struct max_ddr_bw *max_ddr)
{
	uint32_t result;
	oob_status_t ret;

	if (!max_ddr)
		return OOB_ARG_PTR_NULL;

	ret = esmi_oob_read_mailbox(soc_num,
				    READ_DDR_BANDWIDTH, 0, &result);
	if (ret == OOB_SUCCESS) {
		max_ddr->max_bw = result >> 20;
		max_ddr->utilized_bw = (result >> 8) & BW_MASK;
		max_ddr->utilized_pct = result & ONE_BYTE_MASK;
	}
	return ret;
}

oob_status_t write_bmc_report_dimm_power(uint8_t soc_num,
					 struct dimm_power dp_info)
{
	uint32_t input = 0;

	input = dp_info.dimm_addr | dp_info.update_rate << 8
		| dp_info.power << 17;

	return esmi_oob_write_mailbox(soc_num,
				      WRITE_BMC_REPORT_DIMM_POWER, input);
}

oob_status_t write_bmc_report_dimm_thermal_sensor(uint8_t soc_num,
						  struct dimm_thermal dt_info)
{
	uint32_t input = 0;

	input = dt_info.dimm_addr | dt_info.update_rate << 8
		| dt_info.sensor << 21;

	return esmi_oob_write_mailbox(soc_num,
				      WRITE_BMC_REPORT_DIMM_THERMAL_SENSOR,
				      input);
}

oob_status_t read_bmc_ras_pcie_config_access(uint8_t soc_num,
					     struct pci_address pci_addr,
					     uint32_t *buffer)
{
	uint32_t input;

	/* SEGMENT:0 BUS 0:DEVICE 18 and SEGMENT:0 BUS 0:DEVICE 19 are
	 * inaccessable
	 */
	if (pci_addr.segment == 0 && pci_addr.bus == 0x0 &&
		(pci_addr.device == 0x18 || pci_addr.device == 0x19))
		return OOB_NOT_SUPPORTED;

	input = pci_addr.func | pci_addr.device << 3 | pci_addr.bus << 8\
		| pci_addr.offset << 16 | pci_addr.segment << 28;

	return esmi_oob_read_mailbox(soc_num,
				     READ_BMC_RAS_PCIE_CONFIG_ACCESS,
				     input, buffer);
}

oob_status_t read_bmc_ras_mca_validity_check(uint8_t soc_num,
					     uint16_t *bytes_per_mca,
					     uint16_t *mca_banks)
{
	uint32_t output;
	oob_status_t ret;

	if ((!mca_banks) || (!bytes_per_mca))
		return OOB_ARG_PTR_NULL;

	ret = esmi_oob_read_mailbox(soc_num,
				    READ_BMC_RAS_MCA_VALIDITY_CHECK,
				    0, &output);
	if (ret)
		return ret;

	*bytes_per_mca = output >> 16;
	*mca_banks = output & TWO_BYTE_MASK;

	return ret;
}

oob_status_t read_bmc_ras_mca_msr_dump(uint8_t soc_num,
				       struct mca_bank mca_dump,
				       uint32_t *buffer)
{
	uint32_t input;

	input = mca_dump.index << 16 | mca_dump.offset;

	return esmi_oob_read_mailbox(soc_num,
				     READ_BMC_RAS_MCA_MSR_DUMP,
				     input, buffer);
}

oob_status_t read_bmc_ras_fch_reset_reason(uint8_t soc_num,
					   uint32_t input,
					   uint32_t *buffer)
{
	if (input > 1)
		return OOB_INVALID_INPUT;

	return esmi_oob_read_mailbox(soc_num,
				     READ_BMC_RAS_FCH_RESET_REASON,
				     input, buffer);
}

oob_status_t read_dimm_temp_range_and_refresh_rate(uint8_t soc_num,
						   uint32_t dimm_addr,
						   struct temp_refresh_rate *rate)
{
	uint32_t input, output;
	oob_status_t ret;

	if (!rate)
		return OOB_ARG_PTR_NULL;

	input = dimm_addr & 0xFF;

	ret = esmi_oob_read_mailbox(soc_num,
				    READ_DIMM_TEMP_RANGE_AND_REFRESH_RATE,
				    input, &output);
	if (ret)
		return ret;

	rate->ref_rate = output >> 3;
	rate->range = output;

	return ret;
}

oob_status_t read_dimm_power_consumption(uint8_t soc_num,
					 uint32_t dimm_addr,
					 struct dimm_power *dimm_pow)

{
	uint32_t input, output;
	oob_status_t ret;

	if (!dimm_pow)
		return OOB_ARG_PTR_NULL;

	input = dimm_addr & 0xFF;
	ret = esmi_oob_read_mailbox(soc_num,
				    READ_DIMM_POWER_CONSUMPTION,
				    input, &output);
	if (ret)
		return ret;

	dimm_pow->dimm_addr = output;
	dimm_pow->update_rate = output >> 8;
	dimm_pow->power = output >> 17;

	return ret;
}

oob_status_t read_dimm_thermal_sensor(uint8_t soc_num,
				      uint32_t dimm_addr,
				      struct dimm_thermal *dimm_temp)
{
	uint32_t input, output;
	oob_status_t ret;

	if (!dimm_temp)
		return OOB_ARG_PTR_NULL;

	input = dimm_addr & 0xFF;
	ret = esmi_oob_read_mailbox(soc_num,
				    READ_DIMM_THERMAL_SENSOR,
				    input, &output);
	if (ret)
		return ret;

	dimm_temp->dimm_addr = output;
	dimm_temp->update_rate = output >> 8;
	dimm_temp->sensor = output >> 21;

	return ret;
}

oob_status_t read_pwr_current_active_freq_limit_socket(uint8_t soc_num,
						       uint16_t *freq,
						       char **source_type)
{
	uint32_t output;
	uint16_t limit;
	uint8_t index = 0;
	uint8_t ind = 0;
	uint8_t src_length = 0;
	oob_status_t ret;

	if (!freq)
		return OOB_ARG_PTR_NULL;

	// frequency limit source names array length
	src_length = ARRAY_SIZE(freqlimitsrcnames);
	ret = esmi_oob_read_mailbox(soc_num,
				    READ_PWR_CURRENT_ACTIVE_FREQ_LIMIT_SOCKET,
				    0, &output);
	if (ret)
		return ret;

	*freq = output >> 16;
	limit = output & TWO_BYTE_MASK;

	while (limit != 0 && index < src_length) {
		if ((limit & 1) == 1) {
			source_type[ind] = freqlimitsrcnames[index];
			ind++;
		}
		index += 1;
		limit = limit >> 1;
	}

	return ret;
}

oob_status_t read_pwr_current_active_freq_limit_core(uint8_t soc_num,
						     uint32_t core_id,
						     uint16_t *base_freq)
{
	return esmi_oob_read_mailbox(soc_num,
				     READ_PWR_CURRENT_ACTIVE_FREQ_LIMIT_CORE,
				     core_id, (uint32_t *)base_freq);
}

oob_status_t read_pwr_svi_telemetry_all_rails(uint8_t soc_num,
					      uint32_t *power)
{
	if (!power)
		return OOB_ARG_PTR_NULL;

	return esmi_oob_read_mailbox(soc_num, READ_PWR_SVI_TELEMETRY_ALL_RAILS,
				     0, power);
}

oob_status_t read_socket_freq_range(uint8_t soc_num,
				    uint16_t *fmax,
				    uint16_t *fmin)
{
	uint32_t output;
	oob_status_t ret;

	if ((!fmax) || (!fmin))
		return OOB_ARG_PTR_NULL;

	ret = esmi_oob_read_mailbox(soc_num,
				    READ_SOCKET_FREQ_RANGE,
				    0, &output);
	if (ret)
		return ret;

	*fmax = output >> 16;
	*fmin = output & TWO_BYTE_MASK;

	return ret;
}

static oob_status_t validate_bw_type(uint8_t bw_type)
{
	oob_status_t ret;

	switch (bw_type) {
	case AGG_BW:
	case RD_BW:
	case WR_BW:
		ret = OOB_SUCCESS;
		break;
	default:
		ret = OOB_INVALID_INPUT;
	};

	return ret;
}

static oob_status_t validate_link_id_encoding(uint8_t link_id)
{
	oob_status_t ret;

	switch (link_id) {
	case P0:
	case P1:
	case P2:
	case P3:
	case G0:
	case G1:
	case G2:
	case G3:
		ret = OOB_SUCCESS;
		break;
	default:
		ret = OOB_INVALID_INPUT;
	};

	return ret;
}

oob_status_t read_current_io_bandwidth(uint8_t soc_num,
				       struct link_id_bw_type link,
				       uint32_t *io_bw)
{
	uint32_t input;

	// Only Aggregate Banwdith is valid Bandwidth type
	if (link.bw_type != 1)
		return OOB_INVALID_INPUT;

	if (validate_link_id_encoding(link.link_id))
		return OOB_INVALID_INPUT;

	input = link.bw_type | link.link_id << 8;

	return esmi_oob_read_mailbox(soc_num,
				     READ_CURRENT_IO_BANDWIDTH,
				     input, io_bw);
}

oob_status_t read_current_xgmi_bandwidth(uint8_t soc_num,
					 struct link_id_bw_type link,
					 uint32_t *xgmi_bw)
{
	uint32_t input;

	if (validate_bw_type(link.bw_type))
		return OOB_INVALID_INPUT;

	if (validate_link_id_encoding(link.link_id))
		return OOB_INVALID_INPUT;

	input = link.bw_type | link.link_id << 8;

	return esmi_oob_read_mailbox(soc_num,
				     READ_CURRENT_XGMI_BANDWIDTH,
				     input, xgmi_bw);
}

oob_status_t write_gmi3_link_width_range(uint8_t soc_num,
					 uint8_t min_link_width,
					 uint8_t max_link_width)
{
	uint32_t input;
	oob_status_t ret;

	ret = validate_max_min_values(max_link_width, min_link_width,
				      FULL_WIDTH);
	if (ret)
		return ret;

	input = max_link_width | min_link_width << 8;

	return esmi_oob_write_mailbox(soc_num,
				      WRITE_GMI3_LINK_WIDTH_RANGE, input);
}

oob_status_t write_xgmi_link_width_range(uint8_t soc_num,
					 uint8_t min_link_width,
					 uint8_t max_link_width)
{
	uint32_t input;
	oob_status_t ret;

	ret = validate_max_min_values(max_link_width, min_link_width,
				      MAX_XGMI_LINK);
	if (ret)
		return ret;

	input = max_link_width | min_link_width << 8;

	return esmi_oob_write_mailbox(soc_num,
				      WRITE_XGMI_LINK_WIDTH_RANGE, input);
}

oob_status_t write_apb_disable(uint8_t soc_num, uint8_t df_pstate,
			       bool *prochot_asserted)
{
	uint32_t prochat_status;
	oob_status_t ret;

	if (!prochot_asserted)
		return OOB_ARG_PTR_NULL;

	if (df_pstate > MAX_DF_PSTATE_LIMIT)
		return OOB_INVALID_INPUT;

	ret = read_prochot_status(soc_num, &prochat_status);
	if (ret)
		return ret;

	if (ret == OOB_SUCCESS && prochat_status) {
		*prochot_asserted = true;
		return OOB_SUCCESS;
	}

	return esmi_oob_write_mailbox(soc_num,
				      WRITE_APB_DISABLE, (uint32_t)df_pstate);
}

oob_status_t write_apb_enable(uint8_t soc_num, bool *prochot_asserted)
{
	uint32_t prochat_status;
	oob_status_t ret;

	if (!prochot_asserted)
		return OOB_ARG_PTR_NULL;

	ret = read_prochot_status(soc_num, &prochat_status);
	if (ret)
		return ret;

	if (ret == OOB_SUCCESS && prochat_status) {
		*prochot_asserted = true;
		return OOB_SUCCESS;
	}

	return esmi_oob_write_mailbox(soc_num, WRITE_APB_ENABLE, 0);
}

oob_status_t read_current_dfpstate_frequency(uint8_t soc_num,
					     struct pstate_freq *df_pstate)
{
	uint32_t output;
	oob_status_t ret;

	if (!df_pstate)
		return OOB_ARG_PTR_NULL;

	ret = esmi_oob_read_mailbox(soc_num,
				    READ_CURRENT_DFPSTATE_FREQUENCY,
				    0, &output);
	if (ret)
		return ret;

	df_pstate->mem_clk = output >> 16;
	df_pstate->uclk = (output >> 15) & 1;
	df_pstate->fclk = output & FCLK_MASK;

	return ret;
}

oob_status_t write_lclk_dpm_level_range(uint8_t soc_num,
					struct lclk_dpm_level_range lclk)
{
	uint32_t input;
	oob_status_t ret;

	ret = validate_max_min_values(lclk.dpm.max_dpm_level,
				      lclk.dpm.min_dpm_level,
				      MAX_DPM_LIMIT);
	if (ret || lclk.nbio_id > 3)
		return OOB_INVALID_INPUT;

	input = lclk.dpm.min_dpm_level | lclk.dpm.max_dpm_level << 8
		| lclk.nbio_id << 16;

	return esmi_oob_write_mailbox(soc_num,
				      WRITE_LCLK_DPM_LEVEL_RANGE, input);
}

oob_status_t read_bmc_rapl_units(uint8_t soc_num,
				 uint8_t *tu_value,
				 uint8_t *esu_value)
{
	uint32_t output;
	oob_status_t ret;

	if ((!tu_value) || (!esu_value))
		return OOB_ARG_PTR_NULL;

	ret = esmi_oob_read_mailbox(soc_num,
				    READ_BMC_RAPL_UNITS,
				    0, &output);
	if (ret)
		return ret;

	*tu_value  = (output >> 16) & TU_MASK;
	*esu_value = (output >> 8) & ESU_MASK;

	return ret;
}

static oob_status_t read_bmc_rapl_core_lo_counter(uint8_t soc_num,
						  uint32_t core_id,
						  uint32_t *value)
{
	return esmi_oob_read_mailbox(soc_num,
				     READ_BMC_RAPL_CORE_LO_COUNTER,
				     core_id, value);
}

static oob_status_t read_bmc_rapl_core_hi_counter(uint8_t soc_num,
						  uint32_t core_id,
						  uint32_t *value)
{
	return esmi_oob_read_mailbox(soc_num, READ_BMC_RAPL_CORE_HI_COUNTER,
				     core_id, value);
}

static oob_status_t read_bmc_rapl_pkg_counter(uint8_t soc_num,
					      uint8_t counter,
					      uint32_t *counter_value)
{
	return esmi_oob_read_mailbox(soc_num,
				     READ_BMC_RAPL_PKG_COUNTER,
				     counter, counter_value);
}

oob_status_t read_bmc_cpu_base_frequency(uint8_t soc_num,
					 uint16_t *base_freq)
{
	return esmi_oob_read_mailbox(soc_num,
				     READ_BMC_CPU_BASE_FREQUENCY,
				     0, (uint32_t *)base_freq);
}

oob_status_t read_bmc_control_pcie_gen5_rate(uint8_t soc_num,
					     uint8_t rate,
					     uint8_t *mode)
{
	oob_status_t ret;

	if (rate > GEN5_RATE)
		return OOB_INVALID_INPUT;

	ret = esmi_oob_read_mailbox(soc_num,
				    READ_BMC_CONTROL_PCIE_GEN5_RATE,
				    rate, (uint32_t *)mode);
	if (ret)
		return ret;

	*mode = *mode & GEN5_RATE_MASK;
	return ret;
}

oob_status_t write_pwr_efficiency_mode(uint8_t soc_num,
				       uint8_t mode)
{
	if (validate_pwr_efficiency_mode(mode))
		return OOB_INVALID_INPUT;

	return esmi_oob_write_mailbox(soc_num,
				      WRITE_PWR_EFFICIENCY_MODE,
				      (uint32_t)mode);
}

oob_status_t write_df_pstate_range(uint8_t soc_num,
				   uint8_t max_pstate,
				   uint8_t min_pstate)
{
	uint32_t input;

	if (max_pstate > min_pstate || min_pstate > MAX_DF_PSTATE_LIMIT)
		return OOB_INVALID_INPUT;

	input = ((uint16_t)min_pstate << 8 | max_pstate) & TWO_BYTE_MASK;

	return esmi_oob_write_mailbox(soc_num,
				      WRITE_DF_PSTATE_RANGE,
				      input);
}

oob_status_t read_lclk_dpm_level_range(uint8_t soc_num, uint8_t nbio_id,
				       struct dpm_level *dpm)
{
	uint32_t input, output;
	oob_status_t ret;

	if (!dpm)
		return OOB_ARG_PTR_NULL;

	if (nbio_id > 3)
		return OOB_INVALID_INPUT;

	input = (uint32_t)nbio_id << 16;
	ret = esmi_oob_read_mailbox(soc_num, READ_LCLK_DPM_LEVEL_RANGE,
				    input, &output);
	if (ret)
		return ret;
	dpm->min_dpm_level = output;
	dpm->max_dpm_level = output >> 8;

	return OOB_SUCCESS;
}

static oob_status_t read_bmc_esu_multiplier(uint8_t soc_num)
{
	uint8_t tu_value, esu_value;
	oob_status_t ret;

	ret = read_bmc_rapl_units(soc_num, &tu_value, &esu_value);
	if (ret)
		return ret;

	esu_multiplier = pow(2, -1 * (esu_value));
	return ret;
}

oob_status_t read_rapl_core_energy_counters(uint8_t soc_num,
					    uint32_t core_id,
					    double *energy_counters)
{
	uint64_t counter;
	uint32_t hi_counter, new_hi_counter, lo_counter;
	oob_status_t ret;

	if (!energy_counters)
		return OOB_ARG_PTR_NULL;

	/* Read Package High count Register Value */
	ret = read_bmc_rapl_core_hi_counter(soc_num, core_id, &hi_counter);

	if (ret)
		return ret;

	/* Read Package Low count Register Value */
	ret = read_bmc_rapl_core_lo_counter(soc_num, core_id, &lo_counter);
	if (ret)
		return ret;

	/* Read Package High count Register Value */
	ret = read_bmc_rapl_core_hi_counter(soc_num, core_id, &new_hi_counter);
	if (ret)
		return ret;

	if (hi_counter != new_hi_counter) {
		/* Read Package low count Register Value */
		ret = read_bmc_rapl_core_lo_counter(soc_num, core_id, &lo_counter);
		if (ret)
			return ret;
	}

	/* Get the 64-bit counter from high and low word counters */
	counter = (uint64_t)new_hi_counter << 32\
		  | (uint64_t)lo_counter & FOUR_BYTE_MASK;

	/* Get the esu multiplier */
	if (!esu_multiplier) {
		ret = read_bmc_esu_multiplier(soc_num);
		if (ret)
			return ret;
	}

	/* Calculate the energy counters(64bit counter * esu_multiplier) */
	/* Convert the energy counters to Kilo Joules by dividing it by 1000 */
	*energy_counters = (counter * esu_multiplier) / 1000;

	return ret;
}

oob_status_t read_rapl_pckg_energy_counters(uint8_t soc_num,
					    double *energy_counters)
{
	uint64_t counter;
	uint32_t hi_counter, new_hi_counter, lo_counter;
	oob_status_t ret;

	if (!energy_counters)
		return OOB_ARG_PTR_NULL;

	/* Read Package High count Register Value */
	ret = read_bmc_rapl_pkg_counter(soc_num, HI_WORD_REG,
					&hi_counter);
	if (ret)
		return ret;

	/* Read Package low count Register Value */
	ret = read_bmc_rapl_pkg_counter(soc_num, LO_WORD_REG,
					&lo_counter);
	if (ret)
		return ret;

	/* Read Package High count Register value */
	ret = read_bmc_rapl_pkg_counter(soc_num, HI_WORD_REG,
					&new_hi_counter);
	if (ret)
		return ret;

	if (hi_counter != new_hi_counter) {
		/* Read Package low count Register value */
		ret = read_bmc_rapl_pkg_counter(soc_num,
						LO_WORD_REG,
						&lo_counter);
		if (ret)
			return ret;
	}
	counter = (uint64_t)new_hi_counter << 32 |\
		  (uint64_t)lo_counter & FOUR_BYTE_MASK;

	/* Get the esu multiplier */
	if (!esu_multiplier) {
		ret = read_bmc_esu_multiplier(soc_num);
		if (ret)
			return ret;
	}

	/* Calculate the energy counters(64bit counter * esu_multiplier) */
	/* Convert the energy counters to Mega Joules by dividing it by 1000000 */
	*energy_counters = (counter * esu_multiplier) / 1000000;
	return ret;
}

oob_status_t read_ucode_revision(uint8_t soc_num, uint32_t *ucode_rev)
{
	return esmi_oob_read_mailbox(soc_num, READ_UCODE_REVISION,
				     0, ucode_rev);
}

oob_status_t read_ras_df_err_validity_check(uint8_t soc_num,
					    uint8_t df_block_id,
					    struct ras_df_err_chk *err_chk)
{
	uint32_t buffer;
	oob_status_t ret;

	if (!err_chk)
		return OOB_ARG_PTR_NULL;

	if (df_block_id > (MAX_DF_BLOCK_IDS - 1))
		return OOB_INVALID_INPUT;

	ret = esmi_oob_read_mailbox(soc_num, READ_RAS_LAST_TRANS_ADDR_CHK,
				    (uint32_t)df_block_id, &buffer);
	if (!ret) {
		/* Number of df block instances */
		err_chk->df_block_instances = buffer;
		/* bits 16 - 24 of buffer will length of error log */
		/* in bytes per instance  */
		err_chk->err_log_len = buffer >> 16;
	}

	return ret;
}

oob_status_t read_ras_df_err_dump(uint8_t soc_num,
                                  union ras_df_err_dump ras_err,
                                  uint32_t *data)
{
	/* Validate error log offset, DF_BLOCK_ID and DF_BLOCK_INSTANCE */
	if ((ras_err.input[0] & 3) != 0 ||
	     ras_err.input[0] > (MAX_ERR_LOG_LEN - 1) ||
	     ras_err.input[1] > (MAX_DF_BLOCK_IDS - 1) ||
	     ras_err.input[2] > (MAX_DF_BLOCK_INSTS - 1))
		return OOB_INVALID_INPUT;

	return esmi_oob_read_mailbox(soc_num, READ_RAS_LAST_TRANS_ADDR_DUMP,
				     ras_err.data_in, data);
}
