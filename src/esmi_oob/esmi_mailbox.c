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

#include <esmi_oob/esmi_common.h>
#include <esmi_oob/esmi_mailbox.h>
#include <esmi_oob/esmi_i2c.h>
#include <esmi_oob/esmi_cpuid_msr.h>

static oob_status_t validate_thread(uint32_t i2c_bus, uint32_t i2c_addr,
				    uint32_t thread)
{
	uint32_t threads_per_socket;
	int ret;

	ret = esmi_get_threads_per_socket(i2c_bus, i2c_addr,
					  &threads_per_socket);
	if (ret != OOB_SUCCESS)
		return ret;

	if (thread >= threads_per_socket) {
		return OOB_INVALID_INPUT;
	}
	return OOB_SUCCESS;
}

oob_status_t read_socket_power(uint32_t i2c_bus, uint32_t i2c_addr,
			       uint32_t *buffer)
{
	return esmi_oob_read_mailbox(i2c_bus, i2c_addr,
				     READ_PACKAGE_POWER_CONSUMPTION,
				     0, buffer);
}

oob_status_t read_socket_power_limit(uint32_t i2c_bus, uint32_t i2c_addr,
				     uint32_t *buffer)
{
	return esmi_oob_read_mailbox(i2c_bus, i2c_addr,
				     READ_PACKAGE_POWER_LIMIT,
				     0, buffer);
}

oob_status_t read_max_socket_power_limit(uint32_t i2c_bus, uint32_t i2c_addr,
					 uint32_t *buffer)
{
	return esmi_oob_read_mailbox(i2c_bus, i2c_addr,
				     READ_MAX_PACKAGE_POWER_LIMIT,
				     0, buffer);
}

oob_status_t read_tdp(uint32_t i2c_bus, uint32_t i2c_addr, uint32_t *buffer)
{
	return esmi_oob_read_mailbox(i2c_bus, i2c_addr, READ_TDP, 0, buffer);
}

oob_status_t read_max_tdp(uint32_t i2c_bus, uint32_t i2c_addr,
			  uint32_t *buffer)
{
	return esmi_oob_read_mailbox(i2c_bus, i2c_addr, READ_MAX_cTDP,
				     0, buffer);
}

oob_status_t read_min_tdp(uint32_t i2c_bus, uint32_t i2c_addr,
			  uint32_t *buffer)
{
	return esmi_oob_read_mailbox(i2c_bus, i2c_addr, READ_MIN_cTDP,
				     0, buffer);
}

oob_status_t write_socket_power_limit(uint32_t i2c_bus, uint32_t i2c_addr,
				      uint32_t limit)
{
        return esmi_oob_write_mailbox(i2c_bus, i2c_addr,
				      WRITE_PACKAGE_POWER_LIMIT, limit);
}

oob_status_t read_bios_boost_fmax(uint32_t i2c_bus, uint32_t i2c_addr,
				  uint32_t value,
				  uint32_t *buffer)
{
	if (validate_thread(i2c_bus, i2c_addr, value)) {
		return OOB_INVALID_INPUT;
	}
	return esmi_oob_read_mailbox(i2c_bus, i2c_addr,
				     READ_BIOS_BOOST_Fmax, value, buffer);
}

oob_status_t read_esb_boost_limit(uint32_t i2c_bus, uint32_t i2c_addr,
				  uint32_t value,
				  uint32_t *buffer)
{
	if (validate_thread(i2c_bus, i2c_addr, value)) {
		return OOB_INVALID_INPUT;
	}
	return esmi_oob_read_mailbox(i2c_bus, i2c_addr,
				     READ_APML_BOOST_LIMIT, value, buffer);
}

oob_status_t write_esb_boost_limit(uint32_t i2c_bus, uint32_t i2c_addr,
				   int cpu_ind, uint32_t limit)
{
	if (validate_thread(i2c_bus, i2c_addr, cpu_ind)) {
		return OOB_INVALID_INPUT;
	}
        limit = (limit & 0xFFFF) | ((cpu_ind << 16) & 0xFFFF0000);
        return esmi_oob_write_mailbox(i2c_bus, i2c_addr,
				      WRITE_APML_BOOST_LIMIT, limit);
}

oob_status_t write_esb_boost_limit_allcores(uint32_t i2c_bus,
					    uint32_t i2c_addr, uint32_t limit)
{
        limit &= 0xFFFF;
        return esmi_oob_write_mailbox(i2c_bus, i2c_addr,
				      WRITE_APML_BOOST_LIMIT_ALLCORES, limit);
}

oob_status_t read_dram_throttle(uint32_t i2c_bus, uint32_t i2c_addr,
				uint32_t *buffer)
{
	return esmi_oob_read_mailbox(i2c_bus, i2c_addr, READ_DRAM_THROTTLE,
				     0, buffer);
}

oob_status_t write_dram_throttle(uint32_t i2c_bus, uint32_t i2c_addr,
				 uint32_t limit)
{
	/* As per SSP PPR, Write can be 0 to 80%, But read is 0 to 100% */
        return esmi_oob_write_mailbox(i2c_bus, i2c_addr,
				      WRITE_DRAM_THROTTLE, limit);
}

oob_status_t read_prochot_status(uint32_t i2c_bus, uint32_t i2c_addr,
				 uint32_t *buffer)
{
	return esmi_oob_read_mailbox(i2c_bus, i2c_addr, READ_PROCHOT_STATUS,
				     0, buffer);
}

oob_status_t read_prochot_residency(uint32_t i2c_bus, uint32_t i2c_addr,
				    uint32_t *buffer)
{
	return esmi_oob_read_mailbox(i2c_bus, i2c_addr, READ_PROCHOT_RESIDENCY,
				     0, buffer);
}

oob_status_t read_vddio_mem_power(uint32_t i2c_bus, uint32_t i2c_addr,
				  uint32_t *buffer)
{
	return esmi_oob_read_mailbox(i2c_bus, i2c_addr, READ_VDDIO_MEM_POWER,
				     0, buffer);
}

oob_status_t
read_nbio_error_logging_register(uint32_t i2c_bus, uint32_t i2c_addr,
				 uint8_t quadrant, uint32_t offset,
				 uint32_t *buffer)
{
	uint32_t input;

	input = (quadrant << 24) | (offset & 0xFFFFFF);
	return esmi_oob_read_mailbox(i2c_bus, i2c_addr,
				     READ_NBIO_ERROR_LOGGING_REGISTER,
				     input, buffer);
}

oob_status_t read_iod_bist(uint32_t i2c_bus, uint32_t i2c_addr,
			   uint32_t *buffer)
{
	return esmi_oob_read_mailbox(i2c_bus, i2c_addr, READ_IOD_BIST,
				     0, buffer);
}

oob_status_t read_ccd_bist_result(uint32_t i2c_bus, uint32_t i2c_addr,
				  uint32_t input, uint32_t *buffer)
{
	return esmi_oob_read_mailbox(i2c_bus, i2c_addr, READ_CCD_BIST_RESULT,
				     input, buffer);
}

oob_status_t read_ccx_bist_result(uint32_t i2c_bus, uint32_t i2c_addr,
				  uint32_t value, uint32_t *buffer)
{
	return esmi_oob_read_mailbox(i2c_bus, i2c_addr,
				     READ_CCX_BIST_RESULT, value, buffer);
}

oob_status_t read_cclk_freq_limit(uint32_t i2c_bus, uint32_t i2c_addr,
				  uint32_t *buffer)
{
	return esmi_oob_read_mailbox(i2c_bus, i2c_addr,
				     READ_PACKAGE_CCLK_FREQ_LIMIT,
				     0, buffer);
}

oob_status_t read_socket_c0_residency(uint32_t i2c_bus, uint32_t i2c_addr,
				      uint32_t *buffer)
{
	return esmi_oob_read_mailbox(i2c_bus, i2c_addr,
				     READ_PACKAGE_C0_RESIDENCY,
				     0, buffer);
}

oob_status_t read_ddr_bandwidth(uint32_t i2c_bus,
				uint32_t i2c_addr,
				uint32_t *max_bw,
				uint32_t *utilized_bw,
				uint32_t *utilized_pct)
{
	oob_status_t ret;
	uint32_t result;

	if (max_bw == NULL ||
	    utilized_bw == NULL ||
	    utilized_pct == NULL) {
		return OOB_ARG_PTR_NULL;
	}

	ret = esmi_oob_read_mailbox(i2c_bus, i2c_addr,
				    READ_DDR_BANDWIDTH,
				    0, &result);
	if (ret == OOB_SUCCESS) {
		*max_bw = result >> 20;
		*utilized_bw = (result >> 8) & 0xFFF;
		*utilized_pct = result & 0xFF;
	}
	return ret;
}
