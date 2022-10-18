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
#ifndef INCLUDE_APML_RMI_H_
#define INCLUDE_APML_RMI_H_

#include "apml_err.h"

/** \file esmi_rmi.h
 *  Header file for the APML library for SB-RMI functionality access.
 *  All required function, structure, enum, etc. definitions should be defined
 *  in this file for SB-RMI Register accessing.
 *
 *  @details  This header file contains the following:
 *  APIs prototype of the APIs exported by the APML library.
 *  Description of the API, arguments and return values.
 *  The Error codes returned by the API.
 */

#define MAX_ALERT_REG_V20	32	//!< Max alert register for rev 20 //
#define MAX_THREAD_REG_V20	24	//!< Max thread register for rev 20 //
#define MAX_ALERT_REG_V10	16	//!< Max alert register for rev 10 //
#define MAX_THREAD_REG_V10	16	//!< Max thread register for rev 10 //

/**
 * @brief Error codes retured by APML mailbox functions
 */
typedef enum {
	SBRMI_SUCCESS = 0x0,
	SBRMI_CMD_TIMEOUT = 0x11,
	SBRMI_WARM_RESET = 0x22,
	SBRMI_UNKNOWN_CMD_FORMAT = 0x40,
	SBRMI_INVALID_READ_LENGTH = 0x41,
	SBRMI_EXCESSIVE_DATA_LENGTH = 0x42,
	SBRMI_INVALID_THREAD = 0x44,
	SBRMI_UNSUPPORTED_CMD = 0x45,
	SBRMI_CMD_ABORTED = 0x81
} sbrmi_status_code;

/**
 * @brief SB-RMI(Side-Band Remote Management Interface) features
 * register access
 */
typedef enum {
	SBRMI_REVISION = 0x0,
	SBRMI_CONTROL,
	SBRMI_STATUS,
	SBRMI_READSIZE,
	SBRMI_THREADENABLESTATUS0,
	SBRMI_ALERTSTATUS0 = 0x10,
	SBRMI_ALERTSTATUS15 = 0x1F,
	SBRMI_ALERTMASK0 = 0x20,
	SBRMI_ALERTMASK15 = 0x2F,
	SBRMI_SOFTWAREINTERRUPT = 0x40,
	SBRMI_THREADNUMBER,
	SBRMI_THREAD128CS = 0x4B,
	SBRMI_RASSTATUS,
	SBRMI_THREADNUMBERLOW = 0x4E,
	SBRMI_THREADNUMBERHIGH = 0x4F,
	SBRMI_ALERTSTATUS16 = 0x50,
	SBRMI_ALERTSTATUS31 = 0x5F,
	SBRMI_MP0OUTBNDMSG0 = 0x80,
	SBRMI_MP0OUTBNDMSG7 = 0x87,
	SBRMI_ALERTMASK16 = 0xC0,
	SBRMI_ALERTMASK31 = 0xCF,
} sbrmi_registers;

/* SBRMI registers Revision 0x10 */
/**
 * @brief thread enable register revision 0x10
 */
extern const uint8_t thread_en_reg_v10[MAX_THREAD_REG_V10];

/**
 * @brief alert status register revision 0x10
 */
extern const uint8_t alert_status_v10[MAX_ALERT_REG_V10];

/**
 * @brief alert mask revision 0x10
 */
extern const uint8_t alert_mask_v10[MAX_ALERT_REG_V10];

/* SBRMI registers Revision 0x20 */
/**
 * @brief thread enable register revision 0x20
 */
extern const uint8_t thread_en_reg_v20[MAX_THREAD_REG_V20];

/**
 * @brief alert status register revision 0x20
 */
extern const uint8_t alert_status_v20[MAX_ALERT_REG_V20];

/**
 * @brief alert mask revision 0x20
 */
extern const uint8_t alert_mask_v20[MAX_ALERT_REG_V20];

/*****************************************************************************/
/** @defgroup SB-RMIRegisterAccess SB-RMI Register Read Byte Protocol
 *  The SB-RMI registers can be read or written from the SMBus interface using
 *  the SMBus defined PEC-optional Read Byte and Write Byte protocols with the
 *  SB-RMI register number in the command byte.
 *  @{
 */

/**
 *  @brief Read one byte from a given SB_RMI register number
 *  provided socket index and buffer to get the read data for a particular
 *  SB-RMI command register.
 *
 *  @details Given a socket index @p socket_ind and a pointer to hold the
 *  output at uint8_t @p buffer, this function will get the value from a
 *  particular command of SB_RMI register.
 *
 *  @param[in] soc_num Socket uindex.
 *
 *  @param[inout] buffer a pointer to a uint8_t that indicates value to hold
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval Non-zero is returned upon failure.
 *
 */
/**
 *  @brief This value specifies the APML specification revision that the
 *  product is compliant to. 0x10 = 1.0x Revision.
 */
oob_status_t read_sbrmi_revision(uint8_t soc_num,
				 uint8_t *buffer);
/**
 *  @brief Read Control byte from SB_RMI register command.
 */
oob_status_t read_sbrmi_control(uint8_t soc_num,
				uint8_t *buffer);
/**
 *  @brief Read one byte of Status value from SB_RMI register command.
 */
oob_status_t read_sbrmi_status(uint8_t soc_num,
			       uint8_t *buffer);
/**
 *  @brief This register specifies the number of bytes to return when using
 *  the block read protocol to read SBRMI_x[4F:10].
 */
oob_status_t read_sbrmi_readsize(uint8_t soc_num,
				 uint8_t *buffer);
/**
 *  @brief Read one byte of Thread Status from SB_RMI register command.
 */
oob_status_t read_sbrmi_threadenablestatus(uint8_t soc_num,
					   uint8_t *buffer);
/**
 *  @brief Read one byte of Thread Status from SB_RMI register command.
 */
oob_status_t read_sbrmi_multithreadenablestatus(uint8_t soc_num,
						uint8_t *buffer);
/**
 *  @brief This register is used by the SMBus master to generate an
 *  interrupt to the processor to indicate that a message is
 *  available..
 */
oob_status_t read_sbrmi_swinterrupt(uint8_t soc_num,
				    uint8_t *buffer);
/**
 *  @brief This register indicates the maximum number of threads present.
 */
oob_status_t read_sbrmi_threadnumber(uint8_t soc_num,
				     uint8_t *buffer);

/**
 *  @brief This register will read the message running on the MP0.
 */
oob_status_t read_sbrmi_mp0_msg(uint8_t soc_num,
				uint8_t *buffer);

/**
 *  @brief This register will read the alert status.
 */
oob_status_t read_sbrmi_alert_status(uint8_t soc_num,
				     uint8_t *buffer);

/**
 *  @brief This register will read the alert mask.
 */
oob_status_t read_sbrmi_alert_mask(uint8_t soc_num,
				   uint8_t *buffer);

/**
 *  @brief This register will read the inbound message.
 */
oob_status_t read_sbrmi_inbound_msg(uint8_t soc_num,
				    uint8_t *buffer);

/**
 *  @brief This register will read the outbound message.
 */
oob_status_t read_sbrmi_outbound_msg(uint8_t soc_num,
				     uint8_t *buffer);

/**
 *  @brief This register indicates the low part of maximum number of threads.
 */
oob_status_t read_sbrmi_threadnumberlow(uint8_t soc_num,
					uint8_t *buffer);

/**
 *  @brief This register indicates the upper part of maximum number of threads.
 */
oob_status_t read_sbrmi_threadnumberhi(uint8_t soc_num,
				       uint8_t *buffer);

/**
 *  @brief This register is used to read the thread cs.
 */
oob_status_t read_sbrmi_thread_cs(uint8_t soc_num,
				  uint8_t *buffer);

/**
 *  @brief This register will read the ras status.
 */
oob_status_t read_sbrmi_ras_status(uint8_t soc_num,
				   uint8_t *buffer);

/** @} */  // end of SB-RMI Register access
/*****************************************************************************/

#endif  // INCLUDE_APML_RMI_H_
