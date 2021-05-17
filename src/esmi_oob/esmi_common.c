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
		case EREMOTEIO:
		case EIO:       return OOB_UNEXPECTED_SIZE;
		case ENOMEM:    return OOB_NO_MEMORY;
		case EAGAIN:    return OOB_TRY_AGAIN;
		case EDESTADDRREQ:    return OOB_NO_I2C_ADDR;
		default:        return OOB_UNKNOWN_ERROR;
	}
}

