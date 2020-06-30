/*
 * University of Illinois/NCSA Open Source License
 *
 * Copyright (c) 2020, Advanced Micro Devices, Inc.
 * All rights reserved.
 *
 * Developed by:
 *
 *                 AMD Research and AMD Software Development
 *
 *                 Advanced Micro Devices, Inc.
 *
 *                 www.amd.com
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

#ifndef INCLUDE_ESMI_OOB_UTILS_H_
#define INCLUDE_ESMI_OOB_UTILS_H_

#include <linux/i2c-dev.h>
#include "esmi_common.h"

/** \file esmi_i2c.h
 *  Main header file for the ESMI-OOB library.
 *  All required function, structure, enum, etc. definitions should be defined
 *  in this file.
 *
 *  @details  This header file contains the following:
 *  APIs prototype of the APIs exported by the ESMI-OOB library.
 *  Description of the API, arguments and return values.
 *  The Error codes returned by the API.
 */

typedef enum {
	SBRMI_OUTBNDMSG0 = 0x30,
	SBRMI_OUTBNDMSG1,
	SBRMI_OUTBNDMSG2,
	SBRMI_OUTBNDMSG3,
	SBRMI_OUTBNDMSG4,
	SBRMI_OUTBNDMSG5,
	SBRMI_OUTBNDMSG6,
	SBRMI_OUTBNDMSG7,
} sbrmi_outbnd_msg;

/*
 * Usage convention is:
 • SBRMI::InBndMsg_inst0 is command
 • SBRMI::InBndMsg_inst[4:1] are 32 bit data
 • SBRMI::InBndMsg_inst[6:5] are reserved.
 • SBRMI::InBndMsg_inst7: [7] Must be 1'b1 to send message to firmware
 */
typedef enum {
	SBRMI_INBNDMSG0 = 0x38,
	SBRMI_INBNDMSG1,
	SBRMI_INBNDMSG2,
	SBRMI_INBNDMSG3,
	SBRMI_INBNDMSG4,
	SBRMI_INBNDMSG5,
	SBRMI_INBNDMSG6,
	SBRMI_INBNDMSG7,
} sbrmi_inbnd_msg;

/* Software Interrupt for triggering */
#define SBRMI_SW_INT	0x40

/* Default message lengths as per the APML command protocol */
#define WRITE_NMSGS	0x7
#define READ_NMSGS	0x4
#define I2C_WR_RD_NMSGS	0x2
#define I2C_WR_NMSGS	0x1

/* Mailbox message packet format */
typedef struct esb_mailbox_pkt{
	uint8_t regaddr;
	uint8_t value;
} mailbox_packet;

/* Mailbox message data format */
typedef union {
	uint8_t bytes[4];
	uint32_t value;
} mailbox_word;

/* I2C slave for communication */
typedef enum {
	SLAVE_RMI,
	SLAVE_TSI,
}slave_type_t;

oob_status_t esmi_oob_read_byte(int thread, slave_type_t type,
				int cmd, int8_t *buffer);

oob_status_t esmi_oob_write_byte(int thread, slave_type_t type,
				 int cmd, uint8_t value);

oob_status_t esmi_oob_i2c_read(int thread, int wr_len,
			       int rd_len, uint8_t *input, uint8_t *output);

oob_status_t esmi_oob_read_mailbox(int thread, int cmd,
				   uint32_t input, uint32_t *buffer);

oob_status_t esmi_oob_write_mailbox(int thread, int cmd, uint32_t limit);


#endif  // INCLUDE_ESMI_OOB_UTILS_H_
