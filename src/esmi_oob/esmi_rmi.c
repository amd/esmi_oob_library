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

#include <esmi_oob/esmi_rmi.h>
#include <esmi_oob/apml.h>

/* REVISION 0x10 */
/* Thread enable status registers */
const uint8_t thread_en_reg_v10[] = {0x4, 0x5, 0x8, 0x9,
				     0xA, 0xB, 0xC, 0xD,
				     0x43, 0x44, 0x45, 0x46,
				     0x47, 0x48, 0x49, 0x4A};

/* REVISION 0x20 */
/* Thread enable status registers */
const uint8_t thread_en_reg_v20[] = {0x4, 0x5, 0x8, 0x9,
				     0xA, 0xB, 0xC, 0xD,
				     0x43, 0x44, 0x45, 0x46,
				     0x47, 0x48, 0x49, 0x4A,
				     0x91, 0x92, 0x93, 0x94,
				     0x95, 0x96, 0x97, 0x98,
				     0xD8, 0xD9, 0xDA, 0xDB,
				     0xDC, 0xDD, 0xDE, 0xDF};

/* Alert status registers */
const uint8_t alert_status[] = {0x10, 0x11, 0x12, 0x13,
				0x14, 0x15, 0x16, 0x17,
				0x18, 0x19, 0x1A, 0x1B,
				0x1C, 0x1D, 0x1E, 0x1F,
				0x50, 0x51, 0x52, 0x53,
				0x54, 0x55, 0x56, 0x57,
				0x58, 0x59, 0x5A, 0x5B,
				0x5C, 0x5D, 0x5E, 0x5F};

/* Alert Mask registers */
const uint8_t alert_mask[] = {0x20, 0x21, 0x22, 0x23,
			      0x24, 0x25, 0x26, 0x27,
			      0x28, 0x29, 0x2A, 0x2B,
			      0x2C, 0x2D, 0x2E, 0x2F,
			      0xC0, 0xC1, 0xC2, 0xC3,
			      0xC4, 0xC5, 0xC6, 0xC7,
			      0xC8, 0xC9, 0xCA, 0xCB,
			      0xCC, 0xCD, 0xCE, 0xCF};

/* sb-rmi register access */
oob_status_t read_sbrmi_revision(uint8_t soc_num,
				 uint8_t *buffer)
{
	return esmi_oob_rmi_read_byte(soc_num, SBRMI_REVISION, buffer);
}

oob_status_t read_sbrmi_control(uint8_t soc_num,
				uint8_t *buffer)
{
	return esmi_oob_rmi_read_byte(soc_num, SBRMI_CONTROL, buffer);
}

oob_status_t read_sbrmi_status(uint8_t soc_num,
			       uint8_t *buffer)
{
	return esmi_oob_rmi_read_byte(soc_num, SBRMI_STATUS, buffer);
}

oob_status_t read_sbrmi_readsize(uint8_t soc_num,
				 uint8_t *buffer)
{
	return esmi_oob_rmi_read_byte(soc_num, SBRMI_READSIZE, buffer);
}

oob_status_t read_sbrmi_threadenablestatus(uint8_t soc_num,
					   uint8_t *buffer)
{
	return esmi_oob_rmi_read_byte(soc_num, SBRMI_THREADENABLESTATUS0,
				      buffer);
}

oob_status_t read_sbrmi_multithreadenablestatus(uint8_t soc_num,
						uint8_t *buffer)
{
	oob_status_t ret;
	int i;
	uint8_t rev;

	if (!buffer)
		return OOB_ARG_PTR_NULL;

	ret = read_sbrmi_revision(soc_num, &rev);
	if (ret)
		return ret;
	if (rev == 0x10) {
		for (i = 0; i < sizeof(thread_en_reg_v10); i++) {
			ret = esmi_oob_rmi_read_byte(soc_num, thread_en_reg_v10[i],
						     &buffer[i]);
			if (ret)
				return ret;
		}
	} else {
		for (i = 0; i < sizeof(thread_en_reg_v20); i++) {
			ret = esmi_oob_rmi_read_byte(soc_num, thread_en_reg_v20[i],
						     &buffer[i]);
			if (ret)
				return ret;
		}
	}

	return OOB_SUCCESS;
}

oob_status_t read_sbrmi_swinterrupt(uint8_t soc_num,
				    uint8_t *buffer)
{
	return esmi_oob_rmi_read_byte(soc_num, SBRMI_SOFTWAREINTERRUPT, buffer);
}

oob_status_t read_sbrmi_threadnumber(uint8_t soc_num,
				     uint8_t *buffer)
{
	return esmi_oob_rmi_read_byte(soc_num, SBRMI_THREADNUMBER, buffer);
}

oob_status_t read_sbrmi_threadnumberlow(uint8_t soc_num,
					uint8_t *buffer)
{
	return esmi_oob_rmi_read_byte(soc_num, SBRMI_THREADNUMBERLOW, buffer);
}

oob_status_t read_sbrmi_threadnumberhi(uint8_t soc_num,
				       uint8_t *buffer)
{
	return esmi_oob_rmi_read_byte(soc_num, SBRMI_THREADNUMBERHIGH, buffer);
}

oob_status_t read_sbrmi_mp0_msg(uint8_t soc_num,
				uint8_t *buffer)
{
	int i, range;
	oob_status_t ret;

	range = SBRMI_MP0OUTBNDMSG7 - SBRMI_MP0OUTBNDMSG0 + 1;
	for (i = 0; i < range; i++) {
		ret = esmi_oob_rmi_read_byte(soc_num, SBRMI_MP0OUTBNDMSG0 + i,
					     &buffer[i]);
		if (ret)
			return ret;
	}

	return OOB_SUCCESS;
}

oob_status_t read_sbrmi_alert_status(uint8_t soc_num,
				     uint8_t num_of_alert_mask_reg,
				     uint8_t **buffer)
{
	oob_status_t ret;
	int i;
	uint8_t rev;

	if (!buffer || !(*buffer))
		return OOB_ARG_PTR_NULL;

	/* Number of alert mask regsiters should be */
	/* equal to size of alert status array */
	if (num_of_alert_mask_reg != sizeof(alert_status))
		return OOB_UNEXPECTED_SIZE;

	for (i = 0; i < sizeof(alert_status); i++) {
		ret = esmi_oob_rmi_read_byte(soc_num, alert_status[i],
					     (*buffer) + i);
		if (ret)
			return ret;
	}

	return OOB_SUCCESS;
}

oob_status_t read_sbrmi_alert_mask(uint8_t soc_num,
				   uint8_t num_of_alert_mask_reg,
				   uint8_t **buffer)
{
	oob_status_t ret;
	int i;
	uint8_t rev;

	if (!buffer || !(*buffer))
		return OOB_ARG_PTR_NULL;

	/* Number of alert status registers should be */
	/* equal to size of alert mask arrary */
	if (num_of_alert_mask_reg != sizeof(alert_mask))
		return OOB_UNEXPECTED_SIZE;

	for (i = 0; i < sizeof(alert_mask); i++) {
		ret = esmi_oob_rmi_read_byte(soc_num, alert_mask[i],
					     (*buffer) + i);
		if (ret)
			return ret;
	}

	return OOB_SUCCESS;
}

oob_status_t read_sbrmi_inbound_msg(uint8_t soc_num,
				    uint8_t *buffer)
{
	int i, range;
	oob_status_t ret;

	range = SBRMI_INBNDMSG7 - SBRMI_INBNDMSG0 + 1;
	for (i = 0; i < range; i++) {
		ret = esmi_oob_rmi_read_byte(soc_num, SBRMI_INBNDMSG0 + i,
					     &buffer[i]);
		if (ret)
			return ret;
	}

	return OOB_SUCCESS;
}

oob_status_t read_sbrmi_outbound_msg(uint8_t soc_num,
				     uint8_t *buffer)
{
	int i, range;
	oob_status_t ret;

	range = SBRMI_OUTBNDMSG7 - SBRMI_OUTBNDMSG0 + 1;
	for (i = 0; i < range; i++) {
		ret = esmi_oob_rmi_read_byte(soc_num, SBRMI_OUTBNDMSG0 + i,
					     &buffer[i]);
		if (ret)
			return ret;
	}

	return OOB_SUCCESS;
}

oob_status_t read_sbrmi_thread_cs(uint8_t soc_num,
				  uint8_t *buffer)
{
	oob_status_t ret;

	ret = esmi_oob_rmi_read_byte(soc_num, SBRMI_THREAD128CS, buffer);
	*buffer &= 1;

	return OOB_SUCCESS;
}

oob_status_t read_sbrmi_ras_status(uint8_t soc_num,
				   uint8_t *buffer)
{
	return esmi_oob_rmi_read_byte(soc_num, SBRMI_RASSTATUS, buffer);
}

oob_status_t clear_sbrmi_ras_status(uint8_t soc_num, uint8_t buffer)
{
	return esmi_oob_rmi_write_byte(soc_num, SBRMI_RASSTATUS, buffer);
}

oob_status_t esmi_get_threads_per_socket(uint8_t soc_num,
					 uint32_t *threads_per_socket)
{
	oob_status_t ret = 0;
	uint8_t thread_num_low = 0;
	uint8_t thread_num_hi = 0;

	if (!threads_per_socket)
		return OOB_ARG_PTR_NULL;

	/* Verify if requested thread number is for Milan */
	ret = esmi_oob_read_byte(soc_num, SBRMI_THREADNUMBER, SBRMI, &thread_num_low);
	if (ret != OOB_SUCCESS)
		return ret;

	if (!thread_num_low) {
		ret = esmi_oob_read_byte(soc_num, SBRMI_THREADNUMBERLOW, SBRMI, &thread_num_low);
		if (ret != OOB_SUCCESS)
			return ret;

		ret = esmi_oob_read_byte(soc_num, SBRMI_THREADNUMBERHIGH, SBRMI, &thread_num_hi);
		if (ret != OOB_SUCCESS)
			return ret;
	}

	*threads_per_socket = ((uint32_t)thread_num_hi << 8) | thread_num_low;
	return ret;
}
