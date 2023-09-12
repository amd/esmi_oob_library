/*
 * University of Illinois/NCSA Open Source License
 *
 * Copyright (c) 2023, Advanced Micro Devices, Inc.
 * All rights reserved.
 *
 * Developed by:
 *
 *              AMD Research and AMD Software Development
 *
 *              Advanced Micro Devices, Inc.
 *
 *              www.amd.com
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

#include <esmi_oob/apml.h>
#include <esmi_oob/apml_common.h>
#include <esmi_oob/rmi_mailbox_mi300.h>

/* Default data for input */
#define MOD_ID_POS		1
#define MOD_ID_LINK_CONFG_SIZE	5
#define LINK_CONFIG_POS		6

oob_status_t set_gfx_core_clock(uint8_t soc_num, enum range_type freq_type,
				uint32_t freq)
{
	uint8_t cmd;

	switch (freq_type) {
	case MIN:
		cmd = SET_MIN_GFX_CORE_CLOCK;
		break;
	case MAX:
		cmd = SET_MAX_GFX_CORE_CLOCK;
		break;
	default:
		return OOB_INVALID_INPUT;
	}

	return esmi_oob_write_mailbox(soc_num, cmd, freq);
}

oob_status_t set_mclk_fclk_max_pstate(uint8_t soc_num, uint32_t pstate)
{
	return esmi_oob_write_mailbox(soc_num, SET_MAX_PSTATE, pstate);
}

oob_status_t get_mclk_fclk_pstates(uint8_t soc_num, uint8_t pstate_ind,
				   struct mclk_fclk_pstates *pstate)
{
	uint32_t buffer;
	oob_status_t ret;

	if (!pstate)
		return OOB_ARG_PTR_NULL;

	ret =  esmi_oob_read_mailbox(soc_num, GET_PSTATES, pstate_ind,
				     &buffer);
	if (!ret) {
		pstate->mem_clk = buffer;
		pstate->f_clk =  (buffer >> WORD_BITS);
	}

	return ret;
}

oob_status_t set_xgmi_pstate(uint8_t soc_num, uint32_t pstate)
{
	return esmi_oob_write_mailbox(soc_num, SET_XGMI_PSTATE, pstate);
}

oob_status_t unset_xgmi_pstate(uint8_t soc_num)
{
	return esmi_oob_write_mailbox(soc_num, UNSET_XGMI_PSTATE, DEFAULT_DATA);
}

oob_status_t get_xgmi_pstates(uint8_t soc_num, uint8_t pstate_ind,
			      struct xgmi_speed_rate_n_width *xgmi_pstate)
{
	uint32_t buffer = 0;
	uint8_t pos = 1, n_bits = 4, data;
	oob_status_t ret;

	if (!xgmi_pstate)
		return OOB_ARG_PTR_NULL;
	ret = esmi_oob_read_mailbox(soc_num, GET_XGMI_PSTATES, pstate_ind,
				    &buffer);
	if (ret)
		return ret;
	xgmi_pstate->speed_rate = buffer >> WORD_BITS;
	ret = extract_n_bits(buffer, n_bits, pos, &data);
	xgmi_pstate->link_width = data;

	return OOB_SUCCESS;
}

oob_status_t get_xcc_idle_residency(uint8_t soc_num, uint32_t *gfx_cores_idle_res)
{
	return esmi_oob_read_mailbox(soc_num, GET_XCC_IDLE_RESIDENCY,
				     DEFAULT_DATA, gfx_cores_idle_res);
}

oob_status_t get_energy_accum_with_timestamp(uint8_t soc_num, uint64_t *energy,
					     uint64_t *time_stamp)
{
	uint32_t buffer[4] = {0};
	uint32_t value;
	uint8_t index = 0;
	oob_status_t ret;

	if (!energy || !time_stamp)
		return OOB_ARG_PTR_NULL;

	/* Read energy accumulator(index 0 & 1) and timestamp(index 2 & 3) */
	for (index = 0; index < 4; index++) {
		/* Read 32 bit data for each index */
		ret = esmi_oob_read_mailbox(soc_num, GET_ENERGY_ACCUMULATOR,
					    index, &value);
		if (ret)
			return ret;
		buffer[index] = value;
	}

	/* Read 64 bit of energy accumulator */
	*energy = (uint64_t)buffer[1] << D_WORD_BITS | buffer[0];

	/* Read 64 bit of timestamp */
	*time_stamp = ((uint64_t)buffer[3] & TWO_BYTE_MASK)
			<< D_WORD_BITS | buffer[2];
	return ret;
}

oob_status_t get_alarms(uint8_t soc_num, enum alarms_type type,
			uint32_t *buffer)
{
	uint8_t cmd;

	switch (type) {
	case RAS:
		cmd = GET_RAS_ALARMS;
		break;
	case PM:
		cmd = GET_PM_ALARMS;
		break;
	default:
		return OOB_INVALID_INPUT;
	}

	return esmi_oob_read_mailbox(soc_num, cmd, DEFAULT_DATA, buffer);
}

oob_status_t get_psn(uint8_t soc_num, uint32_t die_index, uint64_t *buffer)
{
	uint32_t input, data;
	oob_status_t ret;

	if (!buffer)
		return OOB_ARG_PTR_NULL;

	/* Read the lower 32 bits of psn data */
	input = die_index << BIT_LEN | LO_WORD_REG;
	ret = esmi_oob_read_mailbox(soc_num, GET_PSN, input, &data);
	if (ret)
		return ret;

	*buffer = data;

	/* Read upper 32 bits of psn data */
	input = die_index << BIT_LEN | HI_WORD_REG;
	ret = esmi_oob_read_mailbox(soc_num, GET_PSN, input, &data);
	if (!ret)
		*buffer = (uint64_t) data << D_WORD_BITS | *buffer;
	else
		/* If error occurs in reading the upper 32 bits of psn */
		/* then buffer value will be cleared */
		*buffer = 0;
	return ret;
}

oob_status_t get_link_info(uint8_t soc_num, uint8_t *link_config,
			   uint8_t *module_id)
{
	uint32_t buffer;
	uint8_t pos, n_bits;
	oob_status_t ret;

	if (!link_config || !module_id)
		return OOB_ARG_PTR_NULL;

	pos = MOD_ID_POS;
	n_bits = MOD_ID_LINK_CONFG_SIZE;
	ret = esmi_oob_read_mailbox(soc_num, GET_LINK_INFO, DEFAULT_DATA,
				    &buffer);
	if (ret)
		return ret;

	ret = extract_n_bits(buffer, n_bits, pos, module_id);
	pos = LINK_CONFIG_POS;
	ret = extract_n_bits(buffer, n_bits, pos, link_config);

	return OOB_SUCCESS;
}

oob_status_t get_gfx_freq(uint8_t soc_num, enum gfx_domain_type type,
			  uint16_t *freq)
{
	uint32_t buffer;
	uint8_t cmd;
	oob_status_t ret;

	if (!freq)
		return OOB_ARG_PTR_NULL;

	switch (type) {
	case ABS_MAX_GFX:
		cmd = GET_ABS_MAX_GFX_FREQ;
		break;
	case CUR_GFX:
		cmd = GET_ACT_GFX_FREQ_CAP_SELECTED;
		break;
	default:
		return OOB_INVALID_INPUT;
	}

	ret = esmi_oob_read_mailbox(soc_num, cmd, DEFAULT_DATA, &buffer);
	if (!ret)
		*freq = buffer;
	return ret;
}

oob_status_t get_svi_rail_telemetry(uint8_t soc_num,
				    struct svi_port_domain port,
				    struct svi_telemetry_power *pow)
{
	uint32_t input, buffer;
	oob_status_t ret;

	if (!pow)
		return OOB_ARG_PTR_NULL;

	input = (uint32_t)port.slave_addr << SEMI_NIBBLE_BITS | port.port;
	ret = esmi_oob_read_mailbox(soc_num, GET_SVI_TELEMETRY_BY_RAIL,
				    input, &buffer);
	if (!ret) {
		/* lower 16 bits is update rate in ms */
		pow->update_rate = buffer;
		/* Upper 16 bits is power in watts */
		pow->power = buffer >> WORD_BITS;
	}

	return ret;
}

oob_status_t get_die_hotspot_info(uint8_t soc_num, uint8_t *die_id,
				  uint16_t *temp)
{
	uint32_t buffer;
	oob_status_t ret;

	if (!die_id || !temp)
		return OOB_ARG_PTR_NULL;

	ret = esmi_oob_read_mailbox(soc_num, GET_DIE_HOT_SPOT_INFO,
				    DEFAULT_DATA, &buffer);
	if (ret)
		return ret;
	/* Get Die-ID */
	*die_id = buffer;
	/* Upper 16 bits */
	*temp = buffer >> WORD_BITS;

	return ret;
}

oob_status_t get_mem_hotspot_info(uint8_t soc_num,
				  uint8_t *hbm_stack_id,
				  uint16_t *hbm_temp)
{
	uint32_t buffer;
	oob_status_t ret;

	if (!hbm_stack_id || !hbm_temp)
		return OOB_ARG_PTR_NULL;

	ret = esmi_oob_read_mailbox(soc_num, GET_MEM_HOT_SPOT_INFO,
				    DEFAULT_DATA, &buffer);
	if (ret)
		return ret;
	/* hbm stack id */
	*hbm_stack_id = buffer & NIBBLE_MASK;
	*hbm_temp = buffer >> WORD_BITS;

	return ret;
}

oob_status_t get_controller_status(uint8_t soc_num, bool *status)
{
	uint32_t buffer;
	oob_status_t ret;

	ret = esmi_oob_read_mailbox(soc_num, GET_STATUS, DEFAULT_DATA, &buffer);
	if (!ret)
		*status = buffer & BIT_LEN;
	return ret;
}

oob_status_t get_max_mem_bw_util(uint8_t soc_num,
				 struct max_mem_bw *bw)
{
	uint32_t result;
	oob_status_t ret;

	if (!bw)
		return OOB_ARG_PTR_NULL;

	ret = esmi_oob_read_mailbox(soc_num, GET_MAX_MEM_BW_UTILIZATION,
				    DEFAULT_DATA, &result);
	if (!ret) {
		bw->max_bw = result >> 20;
		bw->utilized_bw = (result >> 8) & BW_MASK;
		bw->utilized_pct = result & ONE_BYTE_MASK;
	}

	return ret;
}

oob_status_t get_hbm_throttle(uint8_t soc_num, uint32_t *mem_th)
{
	return esmi_oob_read_mailbox(soc_num, GET_HBM_THROTTLE,
				     DEFAULT_DATA, mem_th);
}

oob_status_t set_hbm_throttle(uint8_t soc_num, uint32_t mem_th)
{
	return esmi_oob_write_mailbox(soc_num, SET_HBM_THROTTLE, mem_th);
}

oob_status_t get_hbm_temperature(uint8_t soc_num, uint32_t index,
				 uint16_t *temp)
{
	uint32_t buffer;
	oob_status_t ret;

	ret = esmi_oob_read_mailbox(soc_num, GET_HBM_STACK_TEMP,
				    index, &buffer);
	if (!ret)
		*temp = buffer;
	return ret;
}

oob_status_t get_clk_freq_limits(uint8_t soc_num, enum clk_type type,
				 struct freq_limits *limit)
{
	uint32_t buffer;
	uint8_t cmd;
	oob_status_t ret;

	if (!limit)
		return OOB_ARG_PTR_NULL;

	switch (type) {
	case GFX_CLK:
		cmd = GET_GFX_CLK_FREQ_LIMITS;
		break;
	case F_CLK:
		cmd = GET_FCLK_FREQ_LIMITS;
		break;
	default:
		return OOB_INVALID_INPUT;
	}

	ret =  esmi_oob_read_mailbox(soc_num, cmd, DEFAULT_DATA, &buffer);
	if (!ret) {
		limit->min = buffer;
		limit->max = (buffer >> WORD_BITS);
	}

	return ret;
}

oob_status_t get_sockets_in_system(uint32_t *sockets_count)
{
	/* Socket number is not needed to get the number of sockets */
	/* hence passing DEFAULT_DATA i.e. 0 as socket number to the */
	/* read_mailbox function */
	return esmi_oob_read_mailbox(DEFAULT_DATA, GET_SOCKETS_IN_SYSTEM,
				     DEFAULT_DATA, sockets_count);
}

oob_status_t get_bist_results(uint8_t soc_num, uint8_t die_id,
			      uint32_t *bist_result)
{
	return esmi_oob_read_mailbox(soc_num, GET_BIST_RESULTS,
				     die_id, bist_result);
}

oob_status_t get_statistics(uint8_t soc_num, struct statistics stat,
			    uint32_t *param)
{
	uint32_t input;
	oob_status_t ret;

	/* Stop the data collection to freeze data collection and timestamp */
	ret = esmi_oob_read_mailbox(soc_num, QUERY_STATISTICS,
				    DEFAULT_DATA, param);
	if (ret)
		return ret;

	input = ((uint32_t)stat.output_control << 16) | stat.stat_param;
	return esmi_oob_read_mailbox(soc_num, QUERY_STATISTICS,
				     input, param);
}

oob_status_t clear_statistics(uint8_t soc_num)
{
	return esmi_oob_write_mailbox(soc_num, CLEAR_STATISTICS, DEFAULT_DATA);
}
