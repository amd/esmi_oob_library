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
#ifndef INCLUDE_ESMI_OOB_COMMON_H_
#define INCLUDE_ESMI_OOB_COMMON_H_

/** \file common.h
 *  Header file for the ESMI-OOB library common functions.
 *  use of this library is to init the functionality and exit after use.
 *
 *  @details  This header file has init and exit functionalities to open
 *  and close the particular i2c channel.
 */

#define TOTAL_SOCKETS 2 //!< Total number of sockets in the system.

#define FILEPATHSIZ 128 //!< Buffer to hold size of file path

/**
 * @brief I2C slave address for SB-RMI on socket P0 on SSP
 */
#define P0_RMI_ADDR	0x3c	// 0x3c is 7-bit address[7:1] of 0x78(0x3c<<1)
/**
 * @brief I2C slave address for SB-RMI on socket P1 on SSP
 */
#define P1_RMI_ADDR	0x38	// 0x38 is 7-bit address[7:1] of 0x70(0x38<<1)
/**
 * @brief I2C slave address for SB-TSI on socket P0 on SSP
 */
#define P0_TSI_ADDR	0x4c 	// 0x4c is 7-bit address[7:1] of 0x98(0x4c<<1)
/**
 * @brief I2C slave address for SB-TSI on socket P1 on SSP
 */
#define P1_TSI_ADDR	0x48 	// 0x48 is 7-bit address[7:1] of 0x90(0x48<<1)

/**
 * @brief Error codes retured by ESMI_OOB_COMMON functions
 */
typedef enum {
	OOB_SUCCESS = 0,	//!< Operation was successful
	OOB_NOT_FOUND,		//!< An item was searched for but not found
	OOB_PERMISSION,		//!< Permission denied/EACCESS file error.
				//!< many functions require root access to run.
	OOB_NOT_SUPPORTED,	//!< The requested information or
				//!< action is not available for the
				//!< given input, on the given system
	OOB_FILE_ERROR,		//!< Problem accessing a file. This
				//!< may because the operation is not
				//!< supported by the Linux kernel
				//!< version running on the executing
				//!< machine
	OOB_INTERRUPTED,	//!< An interrupt occurred during
				//!< execution of function
	OOB_UNEXPECTED_SIZE,	//!< An unexpected amount of data
				//!< was read
	OOB_UNKNOWN_ERROR,	//!< An unknown error occurred
	OOB_ARG_PTR_NULL,	//!< Parsed argument ptr null
	OOB_NO_MEMORY,		//!< Not enough memory to allocate
	OOB_NOT_INITIALIZED,	//!< ESMI-OOB object not initialized
	OOB_NO_I2C_ADDR,	//!< i2c address not available
	OOB_RD_LENGTH_ERR,	//!< read bytes from cpuid or msr failed
	OOB_RMI_STATUS_ERR,	//!< cpuid or msr read status failed
	OOB_INVALID_INPUT,	//!< Input value is invalid
} oob_status_t;

/****************************************************************************/
/** @defgroup InitShut Initialization and Shutdown
 *  This function handles the i2c device file used by the APIs.
 *  @{
 */

/**
 *  @brief maintain the file descriptor of i2c device.
 *
 *  @details get the file descriptor by opening the particular i2c channel.
 *
 *  @retval ::OOB_SUCCESS non-negative integer, file descriptor is returned upon
 *  	      successful call.
 *  @retval -1 is returned upon failure.
 *
 */
oob_status_t esmi_oob_init(int i2c_channel);

/**
 *  @brief Closes the i2c channel device which was opened during init.
 */
void esmi_oob_exit(void);

/** @} */  // end of InitShut

/*****************************************************************************/
/** @defgroup AuxilQuer Auxiliary functions
 *  Below functions provide interfaces to get the total number of cores,
 *  sockets and threads per core in the system.
 *  @{
*/

/**
 *  @brief convert linux error to esmi error.
 *
 *  @details Get the appropriate esmi error for linux error.
 *
 *  @param[in] err a linux error number
 *
 *  @retval oob_status_t is returned upon particular esmi error
 *
 */
oob_status_t errno_to_oob_status(int err);

/**
 *  @brief Get the number of logical cores per socket
 *
 *  @details Get the total number of logical cores in a socket.
 *
 *  @retval uint32_t is returned upon successful call.
 *
 */
uint32_t esmi_get_logical_cores_per_socket(void);

/**
 *  @brief Get the number of threads per socket
 *
 *  @details Get the total number of threads in a socket.
 *
 *  @retval uint32_t is returned upon successful call.
 *
 */
uint32_t esmi_get_threads_per_socket(void);

/**
 *  @brief Get number of threads per core.
 *
 *  @details Get the number of threads per core.
 *
 *  @retval uint32_t is returned upon successful call.
 */
uint32_t esmi_get_threads_per_core(void);

/**
 * @brief Get the error string message for esmi oob errors.
 *
 *  @details Get the error message for the esmi oob error numbers
 *
 *  @param[in] oob_err is a esmi oob error number
 *
 *  @retval char* value returned upon successful call.
 */
char * esmi_get_err_msg(oob_status_t oob_err);

/** @} */  // end of AuxilQuer
/*****************************************************************************/

#endif  // INCLUDE_ESMI_OOB_COMMON_H_

