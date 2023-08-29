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

#ifndef INCLUDE_APML_H_
#define INCLUDE_APML_H_

#include <stdbool.h>

#include <linux/amd-apml.h>
#include "apml_err.h"

/** \file apml.h
 *  Main header file for the APML library.
 *  All required function, structure, enum, etc. definitions should be defined
 *  in this file.
 *
 *  @details  This header file contains the following:
 *  APIs prototype of the APIs exported by the APML library.
 *  Description of the API, arguments and return values.
 *  The Error codes returned by the API.
 */

 /**
 * @brief SBRMI outbound messages defined in the APML library
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

 /**
 * @brief SBRMI inbound messages defined in the APML library
 *
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

#define SBRMI		"sbrmi"		//!< SBRMI module //
#define SBTSI		"sbtsi"		//!< SBTSI module //

/**
 *  @brief Reads data for the given register.
 *
 *  @details This function will read the data for the given register.
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[in] reg_offset Register offset for RMI/TSI I/F.
 *
 *  @param[in] file_name Character device file name for RMI/TSI I/F.
 *
 *  @param[out] buffer output value for the register.
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval Non-zero is returned upon failure.
 *
 */
oob_status_t esmi_oob_read_byte(uint8_t soc_num, uint8_t reg_offset,
				char *file_name, uint8_t *buffer);

/**
 *  @brief Writes data to the specified register.
 *
 *  @details This function will write the data to the specified register.
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[in] file_name Character device file name for RMI/TSI I/F.
 *
 *  @param[in] reg_offset Register offset for RMI/TSI I/F.
 *
 *  @param[in] value data to write to the register.
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval Non-zero is returned upon failure.
 *
 */
oob_status_t esmi_oob_write_byte(uint8_t soc_num, uint8_t reg_offset,
				 char *file_name, uint8_t value);

/**
 *  @brief Reads mailbox command data.
 *
 *  @details This function will read mailbox command data.
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[in] cmd mailbox command.
 *
 *  @param[in] input data.
 *
 *  @param[out] buffer output data for the given mailbox command.
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval Non-zero is returned upon failure.
 *
 */
oob_status_t esmi_oob_read_mailbox(uint8_t soc_num, uint32_t cmd,
				   uint32_t input, uint32_t *buffer);

/**
 *  @brief Writes data to the given  mailbox command.
 *
 *  @details This function will writes data to mailbox command.
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[in] cmd mailbox command.
 *
 *  @param[in] data input data.
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval Non-zero is returned upon failure.
 *
 */
oob_status_t esmi_oob_write_mailbox(uint8_t soc_num,
				    uint32_t cmd, uint32_t data);
/**
 *  @brief Writes data to device file
 *
 *  @details This function will write data to character device file,
 *  through ioctl.
 *
 *  @param[in] soc_num  Socket index.
 *
 *  @param[in] file_name Character device file name for RMI/TSI I/F
 *
 *  @param[in] msg struct apml_message which contains information about the protocol,
 *  input/output data etc.
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval Non-zero is returned upon failure.
 *
 */
oob_status_t sbrmi_xfer_msg(uint8_t soc_num, char *file_name,
			    struct apml_message *msg);

#endif  // INCLUDE_APML_H_
