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
#include <stdio.h>
#include <i2c/smbus.h>

#include <esmi_oob/esmi_common.h>
#include <esmi_oob/esmi_i2c.h>

static oob_status_t esmi_oob_i2c_rdwr_ioctl(uint32_t i2c_bus,
					    struct  i2c_msg *i2c_msgs,
					    uint32_t i2c_nmsgs)
{
	struct i2c_rdwr_ioctl_data ioctl_msg;
	char i2c_path[FILEPATHSIZ];
	int fd;

	snprintf(i2c_path, FILEPATHSIZ, "/dev/i2c-%d", i2c_bus);
	fd = open(i2c_path, O_RDWR);
	if (fd < 0)
		return OOB_NOT_FOUND;

	ioctl_msg.msgs = i2c_msgs;
	ioctl_msg.nmsgs = i2c_nmsgs;

	if (ioctl(fd, I2C_RDWR, &ioctl_msg) < 0) {
		close(fd);
		return errno_to_oob_status(errno);
	}

	close(fd);
	return OOB_SUCCESS;
}

oob_status_t esmi_oob_read_byte(uint32_t i2c_bus, uint32_t i2c_addr,
				uint32_t cmd, uint8_t *buffer)
{
	int fd, data;
	oob_status_t ret;
	char i2c_path[FILEPATHSIZ];

        if (NULL == buffer)
                return OOB_ARG_PTR_NULL;

	snprintf(i2c_path, FILEPATHSIZ, "/dev/i2c-%d", i2c_bus);
	fd = open(i2c_path, O_RDWR);
	if (fd < 0)
		return OOB_NOT_FOUND;

	/* Set I2C_SLAVE for I2C_DEV to access SB-RMI or SB-TSI*/
	if (ioctl(fd, I2C_SLAVE, i2c_addr) < 0) {
		close(fd);
		return errno_to_oob_status(errno);
	}

	data = i2c_smbus_read_byte_data(fd, cmd);
	if (data < 0) {
		close(fd);
		return errno_to_oob_status(errno);
	}
	close(fd);

	 *buffer = (uint8_t)(data & 0xff);
	return OOB_SUCCESS;
}

oob_status_t esmi_oob_write_byte(uint32_t i2c_bus, uint32_t i2c_addr,
				 uint32_t cmd, uint8_t value)
{
	int fd;
	oob_status_t ret;
	char i2c_path[FILEPATHSIZ];

	snprintf(i2c_path, FILEPATHSIZ, "/dev/i2c-%d", i2c_bus);
	fd = open(i2c_path, O_RDWR);
        if (fd < 0)
                return OOB_NOT_FOUND;

	if (ioctl(fd, I2C_SLAVE, i2c_addr) < 0) {
		close(fd);
		return errno_to_oob_status(errno);
	}

	if (i2c_smbus_write_byte_data(fd, cmd, value) != 0) {
		close(fd);
		return errno_to_oob_status(errno);
	}

	close(fd);

	return OOB_SUCCESS;
}

oob_status_t esmi_oob_i2c_read(uint32_t i2c_bus, uint32_t i2c_addr,
			       int wr_len, int rd_len, uint8_t *input,
			       uint8_t *output)
{
	struct i2c_msg i2c_msgs[I2C_WR_RD_NMSGS];
	oob_status_t ret;

	i2c_msgs[0].addr = i2c_addr;
	i2c_msgs[0].flags = 0;
	i2c_msgs[0].len = wr_len;
	i2c_msgs[0].buf = input;

	i2c_msgs[1].addr = i2c_addr;
	i2c_msgs[1].flags = I2C_M_RD;
	i2c_msgs[1].len = rd_len;
	i2c_msgs[1].buf = output;

	return esmi_oob_i2c_rdwr_ioctl(i2c_bus, i2c_msgs, I2C_WR_RD_NMSGS);
}

static oob_status_t esmi_oob_i2c_write(uint32_t i2c_bus,
				       uint32_t i2c_addr, int wr_len,
				       uint8_t *input)
{
	struct i2c_msg i2c_msgs;
	oob_status_t ret;

	i2c_msgs.addr = i2c_addr;
	i2c_msgs.flags = 0;
	i2c_msgs.len = wr_len;
	i2c_msgs.buf = input;

	return esmi_oob_i2c_rdwr_ioctl(i2c_bus, &i2c_msgs, I2C_WR_NMSGS);
}

static void mailbox_message_fill(mailbox_packet *mb_pkt,
				 int cmd, uint8_t *value)
{
	int i;

	/*
	 * The first command is to write 0x3f(SBRMI_INBNDMSG7) to 0x80.
	 * This indicates that the next set of commands are
	 * the mailbox commands
	 *
	 * The Command(For ex: 0x1) for the mailbox is written to
	 * 0x38(SBRMI_INBNDMSG0).
	 * and the desired value written to 0x39 through 3C.
	 */
	mb_pkt[0].regaddr = SBRMI_INBNDMSG7; //start
	mb_pkt[0].value = 0x80;
	mb_pkt[1].regaddr = SBRMI_INBNDMSG0; //cmd
	mb_pkt[1].value = cmd;

	for(i = 2; i < 6; i++) {
		mb_pkt[i].regaddr = SBRMI_INBNDMSG1 + (i - 2);
		mb_pkt[i].value = value[i - 2];
	}
	/* write 0x40 to 0x1 to indicate that, done with
	 * mailbox query
	 */
	mb_pkt[i].regaddr = SBRMI_SW_INT;  //stop
	mb_pkt[i].value = 0x01;
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
oob_status_t esmi_oob_write_mailbox(uint32_t i2c_bus, uint32_t i2c_addr,
				    int cmd, uint32_t limit)
{
	mailbox_packet mb_pkt[WRITE_NMSGS];
	mailbox_word input;
	oob_status_t ret;
	int i;

	input.value = limit;
	/*
	 * Fill message that help in querying the SMU for various data.
	 */
	mailbox_message_fill(mb_pkt, cmd, input.bytes);

	for(i = 0; i < WRITE_NMSGS; i++) {
		/* send message for querying the SMU for mailbox command */
		ret = esmi_oob_i2c_write(i2c_bus, i2c_addr, 2,
					 (uint8_t *)&mb_pkt[i]);
		if (ret != OOB_SUCCESS) {
			return ret;
		}
	}
        return OOB_SUCCESS;
}

/*
 * The answer for our mailbox request is placed on registers 0x31-0x34
 */
oob_status_t esmi_oob_read_mailbox(uint32_t i2c_bus, uint32_t i2c_addr,
				   int cmd, uint32_t input, uint32_t *buffer)
{
	uint8_t rdata[READ_NMSGS] = {SBRMI_OUTBNDMSG1, SBRMI_OUTBNDMSG2,
				     SBRMI_OUTBNDMSG3, SBRMI_OUTBNDMSG4};
	mailbox_word output;
	int i;
	oob_status_t ret;

	if (NULL == buffer)
		return OOB_ARG_PTR_NULL;
	ret = esmi_oob_write_mailbox(i2c_bus, i2c_addr, cmd, input);
	if (ret != OOB_SUCCESS)
		return ret;
	for (i = 0; i < READ_NMSGS; i++) {
		ret = esmi_oob_i2c_read(i2c_bus, i2c_addr, 0x1, 0x1,
					&rdata[i], &output.bytes[i]);
		if (ret	!= OOB_SUCCESS) {
			return ret;
		}
	}
	*buffer = output.value;
	return OOB_SUCCESS;
}
