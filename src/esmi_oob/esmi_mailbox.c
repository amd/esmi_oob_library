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

#include <esmi_oob/esmi_common.h>
#include <esmi_oob/esmi_mailbox.h>
#include <esmi_oob/esmi_i2c.h>

extern uint32_t threads_per_socket;

oob_status_t read_socket_power(int socket_ind, uint32_t *buffer)
{
	return esmi_oob_read_mailbox(socket_ind, READ_PACKAGE_POWER_CONSUMPTION,
				     0, buffer);
}

oob_status_t read_socket_power_limit(int socket_ind, uint32_t *buffer)
{
	return esmi_oob_read_mailbox(socket_ind, READ_PACKAGE_POWER_LIMIT,
				     0, buffer);
}

oob_status_t read_max_socket_power_limit(int socket_ind, uint32_t *buffer)
{
	return esmi_oob_read_mailbox(socket_ind, READ_MAX_PACKAGE_POWER_LIMIT,
				     0, buffer);
}

oob_status_t read_tdp(int socket_ind, uint32_t *buffer)
{
	return esmi_oob_read_mailbox(socket_ind, READ_TDP, 0, buffer);
}

oob_status_t read_max_tdp(int socket_ind, uint32_t *buffer)
{
	return esmi_oob_read_mailbox(socket_ind, READ_MAX_cTDP, 0, buffer);
}

oob_status_t read_min_tdp(int socket_ind, uint32_t *buffer)
{
	return esmi_oob_read_mailbox(socket_ind, READ_MIN_cTDP, 0, buffer);
}

oob_status_t write_socket_power_limit(int socket_ind, uint32_t limit)
{
        return esmi_oob_write_mailbox(socket_ind, WRITE_PACKAGE_POWER_LIMIT,
				      limit);
}

oob_status_t read_bios_boost_fmax(int socket_ind, uint32_t value,
				  uint32_t *buffer)
{
	if (value >= threads_per_socket) {
		return OOB_INVALID_INPUT;
	}
	return esmi_oob_read_mailbox(socket_ind,
				     READ_BIOS_BOOST_Fmax, value, buffer);
}

oob_status_t read_esb_boost_limit(int socket_ind, uint32_t value,
				  uint32_t *buffer)
{
	if (value >= threads_per_socket) {
		return OOB_INVALID_INPUT;
	}
	return esmi_oob_read_mailbox(socket_ind,
				     READ_APML_BOOST_LIMIT, value, buffer);
}

oob_status_t write_esb_boost_limit(int socket_ind, int cpu_ind, uint32_t limit)
{
	if (cpu_ind >= threads_per_socket) {
		return OOB_INVALID_INPUT;
	}
        return esmi_oob_write_mailbox(socket_ind, WRITE_APML_BOOST_LIMIT,
				      limit);
}

oob_status_t write_esb_boost_limit_allcores(int socket_ind, uint32_t limit)
{
        return esmi_oob_write_mailbox(socket_ind,
				      WRITE_APML_BOOST_LIMIT_ALLCORES, limit);
}

oob_status_t read_dram_throttle(int socket_ind, uint32_t *buffer)
{
	return esmi_oob_read_mailbox(socket_ind, READ_DRAM_THROTTLE, 0, buffer);
}

oob_status_t write_dram_throttle(int socket_ind, uint32_t limit)
{
	if (limit < 0 || limit > 80) {
		return OOB_INVALID_INPUT;
	}
        return esmi_oob_write_mailbox(socket_ind, WRITE_DRAM_THROTTLE, limit);
}

oob_status_t read_prochot_status(int socket_ind, uint32_t *buffer)
{
	return esmi_oob_read_mailbox(socket_ind, READ_PROCHOT_STATUS,
				     0, buffer);
}

oob_status_t read_prochot_residency(int socket_ind, uint32_t *buffer)
{
	return esmi_oob_read_mailbox(socket_ind, READ_PROCHOT_RESIDENCY,
				     0, buffer);
}

oob_status_t read_vddio_mem_power(int socket_ind, uint32_t *buffer)
{
	return esmi_oob_read_mailbox(socket_ind, READ_VDDIO_MEM_POWER,
				     0, buffer);
}

oob_status_t read_nbio_error_logging_register(int socket_ind, uint8_t quadrant,
					      uint32_t offset, uint32_t *buffer)
{
	uint32_t input;

	input = (quadrant << 24) | (offset & 0xFFFFFF);
	return esmi_oob_read_mailbox(socket_ind,
				     READ_NBIO_ERROR_LOGGING_REGISTER,
				     input, buffer);
}

oob_status_t read_iod_bist(int socket_ind, uint32_t *buffer)
{
	return esmi_oob_read_mailbox(socket_ind, READ_IOD_BIST, 0, buffer);
}

oob_status_t read_ccd_bist_result(int socket_ind, uint32_t input,
				  uint32_t *buffer)
{
	return esmi_oob_read_mailbox(socket_ind, READ_CCD_BIST_RESULT,
				     input, buffer);
}

oob_status_t read_ccx_bist_result(int socket_ind, uint32_t value,
				  uint32_t *buffer)
{
	return esmi_oob_read_mailbox(socket_ind,
				     READ_CCX_BIST_RESULT, value, buffer);
}

oob_status_t read_cclk_freq_limit(int socket_ind, uint32_t *buffer)
{
	return esmi_oob_read_mailbox(socket_ind, READ_PACKAGE_CCLK_FREQ_LIMIT,
				     0, buffer);
}

oob_status_t read_socket_c0_residency(int socket_ind, uint32_t *buffer)
{
	return esmi_oob_read_mailbox(socket_ind, READ_PACKAGE_C0_RESIDENCY,
				     0, buffer);
}
