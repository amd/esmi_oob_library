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
#ifndef INCLUDE_ESMI_OOB_CPUID_MSR_H_
#define INCLUDE_ESMI_OOB_CPUID_MSR_H_

#include "esmi_common.h"

/** \file esmi_cpuid_msr.h
 *  Header file for the ESMI-OOB library cpuid and msr read functions.
 *  All required function, structure, enum and protocol specific data etc.
 *  definitions should be defined in this header.
 *
 *  @details  This header file contains the following:
 *  APIs prototype of the APIs exported by the E-SMI-OOB library.
 *  Description of the API, arguments and return values.
 *  The Error codes returned by the API.
 *
 */

/**
 * @brief SB-RMI Read Proccessor Register command protocol and
 * read CPUID command protocol
 *
 * I2C/SMBUS Input message packet data format for SB-RMI Read Processor
 * Register Command and CPUID command Protocol.
 */
typedef struct sbrmi_indata {
	uint8_t cmd;	//!< command protocol
			//!< Read CPUID/Read Register Command Format is 0x73.
	uint8_t wr_ln;	//!< 0x8 bytes is WrDataLen.
	uint8_t rd_ln;	//!< Number of bytes to read from register, Valid values
			//!< are 0x1 through 0x8.
			//!< 0x8 bytes Number of CPUID bytes to read.
	uint8_t regcmd;	//!< Read Processor Register command is 0x86
			//!< read CPUID command is 0x91
	uint8_t thread;	//!< bit 0 is reserved, bit 1:7 selects the 0x127 threads)
	union {
		uint32_t value; //!< value
		uint8_t reg[4]; //!< Register address or CPUID function
	}; //!< maximum 4 register address for CPUID function data to hold
	uint8_t ecx;	//!< 0b = Return ebx:eax
			//!< 1b = Return edx:ecx.
} __attribute__((packed)) rmi_indata;

/**
 * I2C/SMBUS message Output message poacket data format for SB-RMI Read
 * Processor Register Command and CPUID command Protocol for Output data.
 */
typedef struct sbrmi_outdata {
	uint8_t num_bytes;   //!< Number of bytes returned = rd_ln + 1
	uint8_t status;	     //!< status code
	union {
		uint64_t value;  //!< 8bytes, [4,4] bytes of [eax, ebx] or [ecx, edx]
		uint8_t bytes[8]; //!< RdData 1 to RdData 8>
	};
} __attribute__((packed)) rmi_outdata;

/** @} */  // end of SB-RMI-CPUID

/*****************************************************************************/
/** @defgroup ProcessorAccess SB_RMI Read Processor Register Access
 *  Below function provide interface to read the SB-RMI MCA MSR register.
 *  output from MCA MSR commmand will be written into the buffer.
 *  @{
 */

/**
 *  @brief Read the MCA MSR register for a given thread.
 *
 *  @details Given a @p thread and SB-RMI register command, this function reads
 *  msr value.
 *
 *  @param[in] thread is a particular thread in the system.
 *
 *  @param[in] msraddr MCA MSR register to read
 *
 *  @param[out] buffer is to hold the return output of msr value.
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 */
oob_status_t esmi_oob_read_msr(uint32_t thread,
                               uint32_t msraddr, uint64_t *buffer);

/** @} */  // end of ProcessorAccess

/*****************************************************************************/
/** @defgroup cpuidAccess SB-RMI CPUID Register Access
 *  Below function provide interface to get the CPUID access via the SBRMI.
 *
 *  Output from CPUID commmand will be written into registers eax, ebx, ecx
 *  and edx.
 *  @{
 */

/**
 *  @brief Read CPUID functionality for a particular thread in a system.
 *
 *  @details Given a @p thread , @p eax as function input and @p ecx as
 *  extended function input. this function will get the cpuid details
 *  for a particular thread in a pointer to @p eax, @p ebx, @p ecx, @p edx
 *
 *  @param[in] thread is a particular thread in the system.
 *
 *  @param[inout] eax a pointer uint32_t to get eax value
 *
 *  @param[out] ebx a pointer uint32_t to get ebx value
 *
 *  @param[inout] ecx a pointer uint32_t to get ecx value
 *
 *  @param[out] edx a pointer uint32_t to get edx value
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 */
oob_status_t esmi_oob_cpuid(uint32_t thread,
                            uint32_t *eax, uint32_t *ebx,
                            uint32_t *ecx, uint32_t *edx);

/**
 *  @brief Read eax register on CPUID functionality.
 *
 *  @details Given a @p thread, @p fn_eax as function and @p fn_ecx as
 *  extended function input, this function will get the cpuid
 *  details for a particular thread at @p eax.
 *
 *  @param[in] thread is a particular thread in the system.
 *
 *  @param[in] fn_eax cpuid function
 *
 *  @param[in] fn_ecx cpuid extended function
 *
 *  @param[out] eax is to read eax from cpuid functionality.
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 */
oob_status_t esmi_oob_cpuid_eax(uint32_t thread, uint32_t fn_eax,
                                uint32_t fn_ecx, uint32_t *eax);

/**
 *  @brief Read ebx register on CPUID functionality.
 *
 *  @details Given a @p thread, @p fn_eax as function and @p fn_ecx as
 *  extended function input, this function will get the cpuid
 *  details for a particular thread at @p ebx.
 *
 *  @param[in] thread is a particular thread in the system.
 *
 *  @param[in] fn_eax cpuid function
 *
 *  @param[in] fn_ecx cpuid extended function
 *
 *  @param[out] ebx is to read ebx from cpuid functionality.
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 */
oob_status_t esmi_oob_cpuid_ebx(uint32_t thread, uint32_t fn_eax,
                                uint32_t fn_ecx, uint32_t *ebx);

/**
 *  @brief Read ecx register on CPUID functionality.
 *
 *  @details Given a @p thread, @p fn_eax as function and @p fn_ecx as
 *  extended function input, this function will get the cpuid
 *  details for a particular thread at @p ecx.
 *
 *  @param[in] thread is a particular thread in the system.
 *
 *  @param[in] fn_eax cpuid function
 *
 *  @param[in] fn_ecx cpuid extended function
 *
 *  @param[out] ecx is to read ecx from cpuid functionality.
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 */
oob_status_t esmi_oob_cpuid_ecx(uint32_t thread, uint32_t fn_eax,
                                uint32_t fn_ecx, uint32_t *ecx);

/**
 *  @brief Read edx register on CPUID functionality.
 *
 *  @details Given a @p thread, @p fn_eax as function and @p fn_ecx as
 *  extended function input, this function will get the cpuid
 *  details for a particular thread at @p edx.
 *
 *  @param[in] thread is a particular thread in the system.
 *
 *  @param[in] fn_eax cpuid function
 *
 *  @param[in] fn_ecx cpuid extended function
 *
 *  @param[out] edx is to read edx from cpuid functionality.
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 */
oob_status_t esmi_oob_cpuid_edx(uint32_t thread, uint32_t fn_eax,
                                uint32_t fn_ecx, uint32_t *edx);

/** @} */  // end of cpuidAccess

/*****************************************************************************/
#endif  // INCLUDE_ESB_SMI_CPUID_MSR_H_

