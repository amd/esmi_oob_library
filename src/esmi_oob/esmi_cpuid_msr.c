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
#include <stdint.h>
#include <stdio.h>

#include <esmi_oob/esmi_common.h>
#include <esmi_oob/esmi_cpuid_msr.h>
#include <esmi_oob/esmi_i2c.h>

/* Default message lengths as per APML command protocol */
#define MSR_RD_LEN	0xa
#define MSR_WR_LEN	0x9
#define CPUID_RD_LEN	0xa
#define CPUID_WR_LEN	0xa

extern uint32_t threads_per_socket;
extern uint32_t threads_per_core;
extern uint32_t cores_per_socket;
extern uint32_t total_cores;
extern uint32_t total_threads;

/*
 * Ref APML in PPR
 * Table 118: SB-RMI Read Processor Register Command Protocol
 */
#define PREPARE_MSR_MSG_IN(thread_id, msraddr, indata) { \
	indata.cmd = 0x73; /* command protocol*/ \
	indata.wr_ln = 0x07; /* write length*/ \
	indata.rd_ln = 0x08; /* read length*/ \
	indata.regcmd = 0x86; /* read register command */ \
	/* bit 0 is reserved, bit 1:7 selects the 127 threads)*/ \
	indata.thread = thread_id << 1; \
	/*  fill  reg address */ \
	indata.value = msraddr; \
}

/*
 * Ref APML in PPR
 * Table 119: SB-RMI Read CPUID Command Protocol
 */
#define PREPARE_CPUID_MSG_IN(thread_id, eax, ecx, indata) { \
	indata.cmd = 0x73; /* command protocol */\
	indata.wr_ln = 0x8; /* write length */\
	indata.rd_ln = 0x8; /* read length */ \
	indata.regcmd = 0x91; /* read cpuid command */ \
	/* bit 0 is reserved, bit 1:7 selects the 127 threads)*/ \
	indata.thread = thread_id << 1; \
	/* fill CPUID function */ \
	indata.value = *eax; \
	/* 0b=Return ebx:eax; 1b=Return edx:ecx. */ \
	indata.ecx = (*ecx & 0xf) << 4; \
}

#define THREAD_NUM_IN_SOCKET(thread, thread_num_in_socket) { \
	thread_num_in_socket = cores_per_socket * (thread / total_cores); \
	thread_num_in_socket += (thread % cores_per_socket); \
}

static oob_status_t esb_rmi_read(int thread, int wr_len, int rd_len,
				 rmi_indata *indata, rmi_outdata *outdata)
{
	oob_status_t ret;
	int socket;

        socket = (thread % total_cores) / cores_per_socket;
	ret = esmi_oob_i2c_read(socket, wr_len, rd_len,
				(uint8_t *)indata, (uint8_t *)outdata);
	if (ret != OOB_SUCCESS){
		return ret;
	}
	if (outdata->status != 0) {
		return OOB_RMI_STATUS_ERR;
	}
	if (outdata->num_bytes != (rd_len - 1)) {
		return OOB_RD_LENGTH_ERR;
	}

	return OOB_SUCCESS;
}

oob_status_t esmi_oob_read_msr(uint32_t thread,
			       uint32_t msraddr, uint64_t *buffer)
{
	rmi_indata indata;
	rmi_outdata outdata;
	oob_status_t ret;
	uint32_t thread_num_in_socket;

	THREAD_NUM_IN_SOCKET(thread, thread_num_in_socket);
	PREPARE_MSR_MSG_IN(thread_num_in_socket, msraddr, indata);
	ret = esb_rmi_read(thread, MSR_WR_LEN, MSR_RD_LEN,
			   &indata, &outdata);
	if (ret != OOB_SUCCESS) {
		return ret;
	}
	*buffer = outdata.value;

	return outdata.status;
}

oob_status_t esmi_oob_cpuid(uint32_t thread,
			    uint32_t *eax, uint32_t *ebx,
			    uint32_t *ecx, uint32_t *edx)
{
	rmi_indata indata;
	rmi_outdata outdata;
	oob_status_t ret;
	uint32_t thread_num_in_socket;

	THREAD_NUM_IN_SOCKET(thread, thread_num_in_socket);

	/* TODO:
	 * Bits [7:1] select the thread to address. 00h=thread0 ...
	 * 7Fh=thread127. conform thread or logical core
	 */
	PREPARE_CPUID_MSG_IN(thread_num_in_socket, eax, ecx, indata);

	ret = esb_rmi_read(thread, CPUID_WR_LEN, CPUID_RD_LEN,
			   &indata, &outdata);
	if (ret != OOB_SUCCESS) {
		return ret;
	}
	*eax = outdata.value;
	*ebx = outdata.value >> 32;

	// ecx LSB 1 is to read ecx and edx
	indata.ecx = indata.ecx | 1;

	ret = esb_rmi_read(thread, CPUID_WR_LEN, CPUID_RD_LEN,
			   &indata, &outdata);
	if (ret != OOB_SUCCESS) {
		return ret;
	}
	*ecx = outdata.value;
	*edx = outdata.value >> 32;

	return OOB_SUCCESS;
}

/*
 * CPUID functions returning a single datum
 */
oob_status_t esmi_oob_cpuid_eax(uint32_t thread, uint32_t fn_eax,
				uint32_t fn_ecx, uint32_t *eax)
{
	uint32_t ebx, ecx, edx;
	*eax = fn_eax;
	ecx = fn_ecx;
	return esmi_oob_cpuid(thread, eax, &ebx, &ecx, &edx);
}

oob_status_t esmi_oob_cpuid_ebx(uint32_t thread, uint32_t fn_eax,
				uint32_t fn_ecx, uint32_t *ebx)
{
	uint32_t eax, ecx, edx;
	eax = fn_eax;
	ecx = fn_ecx;
	return esmi_oob_cpuid(thread, &eax, ebx, &ecx, &edx);
}

oob_status_t esmi_oob_cpuid_ecx(uint32_t thread, uint32_t fn_eax,
				uint32_t fn_ecx, uint32_t *ecx)
{
	uint32_t eax, ebx, edx;
	eax = fn_eax;
	*ecx = fn_ecx;
	return esmi_oob_cpuid(thread, &eax, &ebx, ecx, &edx);
}

oob_status_t esmi_oob_cpuid_edx(uint32_t thread, uint32_t fn_eax,
				uint32_t fn_ecx, uint32_t *edx)
{
	uint32_t eax, ebx, ecx;
	eax = fn_eax;
	ecx = fn_ecx;
	return esmi_oob_cpuid(thread, &eax, &ebx, &ecx, edx);
}

