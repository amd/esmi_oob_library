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
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include <esmi_oob/esmi_common.h>
#include <esmi_oob/esmi_cpuid_msr.h>

int fd = -1;
/* Default values considered for calculating the below variables */
uint32_t threads_per_socket = 1;
uint32_t logical_cores_per_socket = 1;
uint32_t threads_per_core  = 1;
uint32_t cores_per_socket  = 1;
uint32_t total_cores = 1;
uint32_t total_threads = 1;

/*
 * Get appropriate error strings for the esmi oob error numbers
 */
char * esmi_get_err_msg(oob_status_t oob_err)
{
        switch(oob_err) {
                case OOB_SUCCESS:
                        return "Success";
		case OOB_NOT_FOUND:
			return "I2C device not found";
		case OOB_PERMISSION:
			return "Permission denied to access I2C dev";
		case OOB_NOT_SUPPORTED:
			return "Not supported";
		case OOB_FILE_ERROR:
			return "File error";
		case OOB_INTERRUPTED:
			return "Task Interrupted";
		case OOB_UNEXPECTED_SIZE:
			return "I/O Error";
		case OOB_ARG_PTR_NULL:
			return "Invalid pointer";
		case OOB_NO_MEMORY:
			return "Memory Error";
		case OOB_NOT_INITIALIZED:
			return "ESMI-OOB not initialized";
		case OOB_NO_I2C_ADDR:
			return "i2c slave address not available";
		case OOB_RD_LENGTH_ERR:
			return "Read bytes from cpuid or msr failed.";
		case OOB_RMI_STATUS_ERR:
			return "cpuid or msr read status failed";
		case OOB_TRY_AGAIN:
			return "Doesnot match Try Again";
		case OOB_INVALID_INPUT:
			return "Input value is invalid";
                default:
                        return "Unknown error";
        }
}

/*
 * Map linux errors to esmi oob errors.
 */
oob_status_t errno_to_oob_status(int err)
{
        switch (err) {
                case 0:         return OOB_SUCCESS;
                case ESRCH:     return OOB_NOT_FOUND;
                case EACCES:    return OOB_PERMISSION;
                case EPERM:
                case ENOENT:    return OOB_NOT_SUPPORTED;
                case EBADF:
                case EOF:
                case EISDIR:    return OOB_FILE_ERROR;
                case EINTR:     return OOB_INTERRUPTED;
                case EIO:       return OOB_UNEXPECTED_SIZE;
                case ENOMEM:    return OOB_NO_MEMORY;
		case EAGAIN:    return OOB_TRY_AGAIN;
                case EDESTADDRREQ:    return OOB_NO_I2C_ADDR;
                default:        return OOB_UNKNOWN_ERROR;
        }
}

uint32_t esmi_get_logical_cores_per_socket(void)
{
	return logical_cores_per_socket;
}

uint32_t esmi_get_threads_per_socket(void)
{
	return threads_per_socket;
}

uint32_t esmi_get_threads_per_core(void)
{
	return threads_per_core;
}

oob_status_t detect_topology()
{
	uint32_t value;
	oob_status_t ret;
	uint32_t thread_ind = 0;
	uint32_t cpuid_fn = 1;
	uint32_t cpuid_extd_fn = 0;

	ret = esmi_oob_cpuid_ebx(thread_ind, cpuid_fn,
				 cpuid_extd_fn, &value);

	if (ret != OOB_SUCCESS) {
		return ret;
	}
	/*
	 * In CPUID_Fn00000001_EBX, Bits 23:16 logical processor count.
	 * Specifies the number of threads in the processor
	 */
	threads_per_socket = (value >> 16) & 0xFF;

	cpuid_fn = 0x8000001e; // CPUID_Fn8000001E_EBX [Core Identifiers]
	ret = esmi_oob_cpuid_ebx(thread_ind, cpuid_fn,
				 cpuid_extd_fn, &value);
	if (ret != OOB_SUCCESS) {
		return ret;
	}
	/*
	 * bits 15:8 Threads per core. Read-only.
	 * Reset: XXh. The number of threads per core is ThreadsPerCore+1.
	 */
	threads_per_core = ((value >> 8) & 0xFF) + 1;
	cores_per_socket = threads_per_socket / threads_per_core;
	total_cores = cores_per_socket * TOTAL_SOCKETS;
	total_threads = threads_per_socket * TOTAL_SOCKETS;

	/*
	 * CPUID_Fn0000000B_EBX_x01 [Extended Topology Enumeration]
	 */
	cpuid_fn = 0xB;
	cpuid_extd_fn = 1;
	ret = esmi_oob_cpuid_ebx(thread_ind, cpuid_fn,
				 cpuid_extd_fn, &value);
	if (ret != OOB_SUCCESS) {
		return ret;
	}
	logical_cores_per_socket = value & 0xFFFF;

	return OOB_SUCCESS;
}

/*
 * Initialization step, where Opening the i2c device file.
 */
oob_status_t esmi_oob_init(int i2c_channel)
{
	char i2c_path[FILEPATHSIZ];
	oob_status_t ret;

	snprintf(i2c_path, FILEPATHSIZ, "/dev/i2c-%d", i2c_channel);

	if (fd < 0) {
		fd = open(i2c_path, O_RDWR);
		if (fd < 0) {
			return errno_to_oob_status(errno);
		}
	}
	ret = detect_topology();
	if (ret != OOB_SUCCESS) {
		return ret;
	}
	return errno_to_oob_status(errno);
}

/*
 * Close the i2c device file in the exit.
 */
void esmi_oob_exit(void)
{
	if (fd >= 0) {
		close(fd);
	}

	fd = -1;
}
