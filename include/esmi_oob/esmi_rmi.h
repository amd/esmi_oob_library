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
#ifndef INCLUDE_ESMI_OOB_RMI_H_
#define INCLUDE_ESMI_OOB_RMI_H_

#include "esmi_common.h"

/** \file esmi_rmi.h
 *  Header file for the E-SMI-OOB library for SB-RMI functionality access.
 *  All required function, structure, enum, etc. definitions should be defined
 *  in this file for SB-RMI Register accessing.
 *
 *  @details  This header file contains the following:
 *  APIs prototype of the APIs exported by the E-SMI-OOB library.
 *  Description of the API, arguments and return values.
 *  The Error codes returned by the API.
 */

/**
 * @brief Error codes retured by E-SMI-OOB mailbox functions
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
	SBRMI_THREADENABLESTATUS,
	SBRMI_SOFTWAREINTERRUPT = 0x40,
	SBRMI_THREADNUMBER
} sbrmi_registers;

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
 *  @param[in] i2c_bus i2c bus number
 *
 *  @param[in] i2c_addr device address on the i2c bus
 *
 *  @param[inout] buffer a pointer to a uint8_t that indicates value to hold
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
/**
 *  @brief This value specifies the APML specification revision that the
 *  product is compliant to. 0x10 = 1.0x Revision.
 */
oob_status_t read_sbrmi_revision(uint32_t i2c_bus, uint32_t i2c_addr,
				 uint8_t *buffer);
/**
 *  @brief Read Control byte from SB_RMI register command.
 */
oob_status_t read_sbrmi_control(uint32_t i2c_bus, uint32_t i2c_addr,
				uint8_t *buffer);
/**
 *  @brief Read one byte of Status value from SB_RMI register command.
 */
oob_status_t read_sbrmi_status(uint32_t i2c_bus, uint32_t i2c_addr,
			       uint8_t *buffer);
/**
 *  @brief This register specifies the number of bytes to return when using
 *  the block read protocol to read SBRMI_x[4F:10].
 */
oob_status_t read_sbrmi_readsize(uint32_t i2c_bus, uint32_t i2c_addr,
				 uint8_t *buffer);
/**
 *  @brief Read one byte of Thread Status from SB_RMI register command.
 */
oob_status_t read_sbrmi_threadenablestatus(uint32_t i2c_bus, uint32_t i2c_addr,
					   uint8_t *buffer);
/**
 *  @brief This register is used by the SMBus master to generate an
 *  interrupt to the processor to indicate that a message is
 *  available..
 */
oob_status_t read_sbrmi_swinterrupt(uint32_t i2c_bus, uint32_t i2c_addr,
				    uint8_t *buffer);
/**
 *  @brief This register indicates the maximum number of threads present.
 */
oob_status_t read_sbrmi_threadnumber(uint32_t i2c_bus, uint32_t i2c_addr,
				     uint8_t *buffer);

/** @} */  // end of SB-RMI Register access
/*****************************************************************************/

#endif  // INCLUDE_ESMI_OOB_RMI_H_
