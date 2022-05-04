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
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/ioctl.h>

#include <esmi_oob/apml.h>

#define SBRMI_CTRL	0x1
#define SBRMI_STATUS	0x2
#define SW_ALERT_MASK	0x2
/* SBRMI DEVICE FILE PATH */
#define SBRMI_DEV	"/dev/"
/* READ MODE */
#define READ_MODE		1
/*WRITE MODE */
#define WRITE_MODE		0

oob_status_t sbrmi_xfer_msg(uint8_t socket_num, char *filename, struct apml_message *msg)
{
	int fd = 0, ret = 0;
	char dev_file[12];

	sprintf(dev_file, "/dev/%s%d", filename, socket_num);

	fd = open(dev_file, O_RDWR);
	if (fd < 0)
		return OOB_FILE_ERROR;

	if (ioctl(fd, SBRMI_IOCTL_CMD, msg) < 0)
		ret = errno;

	close(fd);

	if (ret == EPROTOTYPE) {
		if (msg->cmd == APML_CPUID || msg->cmd == APML_MCA_MSR)
			ret = OOB_CPUID_MSR_ERR_BASE + msg->fw_ret_code;
		else
			ret = OOB_MAILBOX_ERR_BASE + msg->fw_ret_code;
	}

	return errno_to_oob_status(ret);
}

oob_status_t esmi_oob_read_byte(uint8_t soc_num,
				uint8_t reg_offset,
				char *file_name,
				uint8_t *buffer)
{
	struct apml_message msg = {0};
	oob_status_t ret;

	/* NULL Pointer check */
	if (!buffer)
		return OOB_ARG_PTR_NULL;
	/* Readi/write register command is 0x1002 */
	msg.cmd = 0x1002;
	/* Assign register_offset to msg.data_in[0] */
	msg.data_in.reg_in[0] = reg_offset;
	/* Assign 1  to the msg.data_in[7] for the read operation */
	msg.data_in.reg_in[7] = 1;

	ret = sbrmi_xfer_msg(soc_num, file_name, &msg);
	if (ret)
		return ret;

	*buffer = msg.data_out.reg_out[0];

        return OOB_SUCCESS;
}

oob_status_t esmi_oob_write_byte(uint8_t soc_num,
				 uint8_t reg_offset,
				 char *file_name,
				 uint8_t value)
{
	struct apml_message msg = {0};

	/* Read/Write register command is 0x1002 */
	msg.cmd = 0x1002;
	/* Assign register_offset to msg.data_in[0] */
	msg.data_in.reg_in[0] = reg_offset;

	/* Assign value to write to the data_in[4] */
	msg.data_in.reg_in[4] = value;

	/* Assign 0 to the msg.data_in[7] */
	msg.data_in.reg_in[7] = 0;

	return sbrmi_xfer_msg(soc_num, file_name, &msg);
}

/*
 * For a Mailbox write:
 *
 * Write 0x3f to an 0x80, Send the command to 0x38 and write the desired
 * value to 0x39 through 3C. The arguments of logical core numbers,
 * the % of DRAM throttle, NBIO Quadrant, CCD and CCX instances are all
 * written to 0x39.
 * The answer for our mailbox request is placed on registers 0x31-0x34.
 */
oob_status_t esmi_oob_write_mailbox(uint8_t soc_num,
                                    uint32_t cmd, uint32_t data)
{
	struct apml_message msg = {0};

	msg.cmd = cmd;
	msg.data_in.mb_in[0] = data;

	msg.data_in.mb_in[1] = (uint32_t)WRITE_MODE << 24;

	return sbrmi_xfer_msg(soc_num, SBRMI, &msg);
}

/*
 * The answer for our mailbox request is placed on registers 0x31-0x34
 */
oob_status_t esmi_oob_read_mailbox(uint8_t soc_num,
                                   uint32_t cmd, uint32_t input, uint32_t *buffer)
{
	struct apml_message msg = {0};
	oob_status_t ret = 0;

	/* NULL pointer check */
	if (!buffer)
		return OOB_ARG_PTR_NULL;

	msg.cmd = cmd;
	msg.data_in.mb_in[0] = input;

	msg.data_in.mb_in[1] = (uint32_t)READ_MODE << 24;
	ret = sbrmi_xfer_msg(soc_num, SBRMI, &msg);
	if (ret)
		return ret;

	*buffer = msg.data_out.mb_out[0];
	return OOB_SUCCESS;
}
