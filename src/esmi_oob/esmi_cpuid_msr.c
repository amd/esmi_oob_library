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

/* Size of CPU ID reg in bytes */
#define REG_SIZE 4

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

static oob_status_t esb_rmi_read(uint32_t i2c_bus, uint32_t i2c_addr,
				 int wr_len, int rd_len,
				 rmi_indata *indata, rmi_outdata *outdata)
{
	oob_status_t ret;

	ret = esmi_oob_i2c_read(i2c_bus, i2c_addr, wr_len, rd_len,
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

static oob_status_t esmi_convert_reg_val(uint32_t reg, char *id)
{
	int i;
	char id_l[REG_SIZE +1];
	oob_status_t ret;

	for (i = 0; i < (sizeof(id_l) - 1) ; i++)
	{
		id_l[i] = reg;
		reg = reg >> 8;
	}

	id_l[i] = '\0';

	ret = snprintf(id, sizeof(id_l), "%s", id_l);
	if (ret < 0)
		return ret;

	return 0;

}

oob_status_t esmi_get_vendor_id(uint32_t i2c_bus, uint32_t i2c_addr,
			        char *vendor_id)
{
	uint32_t eax, ebx, ecx, edx;
	uint32_t core_id=0;
	char ebx_id[REG_SIZE + 1], ecx_id[REG_SIZE + 1], edx_id[REG_SIZE + 1];
	oob_status_t ret;

	if(vendor_id == NULL)
		return ENOMEM;

	eax = 0;
	ebx = 0;

	ret = esmi_oob_cpuid(i2c_bus, i2c_addr, core_id,
			     &eax, &ebx, &ecx, &edx);
	if (ret != 0)
		return ret;
	/*
	 * Processor Vendor EBX [(ASCII Bytes [3:0])]
	 * Processor Vendor ECX [(ASCII Bytes [8:11])]
	 * Processor Vendor EDX [(ASCII Bytes [4:7])]
	 */
	ret = esmi_convert_reg_val(ebx, ebx_id);
	if (ret != 0)
		return ret;
	ret = esmi_convert_reg_val(edx, edx_id);
	if (ret != 0)
		return ret;
	ret = esmi_convert_reg_val(ecx, ecx_id);
	if (ret != 0)
		return ret;

	ret = snprintf(vendor_id, 4 * REG_SIZE + 1, "%s%s%s",
			     ebx_id, edx_id, ecx_id);
	if (ret < 0)
		return ret;

	return 0;
}

static oob_status_t esmi_reg_offset_conv(uint32_t reg, uint32_t offset, uint32_t flag)
{
	oob_status_t ret;
	ret = (reg >> offset) & flag;
	return ret;
}

oob_status_t esmi_get_processor_info(uint32_t i2c_bus, uint32_t i2c_addr,
			       struct processor_info *proc_info)
{
	oob_status_t ret;
	uint32_t eax=1, ebx, ecx, edx;
	uint32_t core_id=0;

	ret = esmi_oob_cpuid(i2c_bus, i2c_addr, core_id,
			     &eax, &ebx, &ecx, &edx);
	if (ret != 0)
		return ret;
	/*
	 * Family[7:0]=({0000b,BaseFamily[3:0]}+ExtendedFamily[7:0])
	 */
	proc_info->family = esmi_reg_offset_conv(eax, 8, 0xf) +
			esmi_reg_offset_conv(eax, 20, 0xff);
	/*
	 * Model[7:0]={ExtendedModel[3:0],BaseModel[3:0]}
	 */
	proc_info->model = esmi_reg_offset_conv(eax, 16, 0xf) * 0x10 +
			esmi_reg_offset_conv(eax, 4, 0xf);
	/*
	 * Stepping = Processor stepping (revision) for a specific model
	 */
	proc_info->step_id = esmi_reg_offset_conv(eax, 0, 0xf);

	return ret;
}

oob_status_t esmi_get_threads_per_socket(uint32_t i2c_bus, uint32_t i2c_addr,
					 uint32_t *threads_per_socket)
{
	oob_status_t ret;
	uint32_t value;
	uint32_t thread_ind = 0;
	uint32_t cpuid_fn = 1;
	uint32_t cpuid_extd_fn = 0;

	ret = esmi_oob_cpuid_ebx(i2c_bus, i2c_addr, thread_ind, cpuid_fn,
				 cpuid_extd_fn, &value);

	if (ret != OOB_SUCCESS) {
		return ret;
	}
	/*
	 * In CPUID_Fn00000001_EBX, Bits 23:16 logical processor count.
	 * Specifies the number of threads in the processor
	 */
	*threads_per_socket = (value >> 16) & 0xFF;
	return ret;
}

oob_status_t esmi_get_threads_per_core(uint32_t i2c_bus, uint32_t i2c_addr,
				       uint32_t *threads_per_core)
{
	oob_status_t ret;
	uint32_t value;
	uint32_t thread_ind = 0;
	uint32_t cpuid_fn = 1;
	uint32_t cpuid_extd_fn = 0;

	cpuid_fn = 0x8000001e; // CPUID_Fn8000001E_EBX [Core Identifiers]
	ret = esmi_oob_cpuid_ebx(i2c_bus, i2c_addr, thread_ind, cpuid_fn,
				 cpuid_extd_fn, &value);
	if (ret != OOB_SUCCESS) {
		return ret;
	}
	/*
	 * bits 15:8 Threads per core. Read-only.
	 * Reset: XXh. The number of threads per core is ThreadsPerCore+1.
	 */
	*threads_per_core = ((value >> 8) & 0xFF) + 1;

	return ret;
}

oob_status_t
esmi_get_logical_cores_per_socket(uint32_t i2c_bus, uint32_t i2c_addr,
				  uint32_t *logical_cores_per_socket)
{
	oob_status_t ret;
	uint32_t value;
	uint32_t thread_ind = 0;
	uint32_t cpuid_fn = 1;
	uint32_t cpuid_extd_fn = 0;
	/*
	 * CPUID_Fn0000000B_EBX_x01 [Extended Topology Enumeration]
	 */
	cpuid_fn = 0xB;
	cpuid_extd_fn = 1;
	ret = esmi_oob_cpuid_ebx(i2c_bus, i2c_addr, thread_ind, cpuid_fn,
				 cpuid_extd_fn, &value);
	if (ret != OOB_SUCCESS) {
		return ret;
	}
	*logical_cores_per_socket = value & 0xFFFF;

	return ret;
}

oob_status_t esmi_oob_read_msr(uint32_t i2c_bus, uint32_t i2c_addr,
			       uint32_t thread, uint32_t msraddr,
			       uint64_t *buffer)
{
	rmi_indata indata;
	rmi_outdata outdata;
	oob_status_t ret;

	PREPARE_MSR_MSG_IN(thread, msraddr, indata);
	ret = esb_rmi_read(i2c_bus, i2c_addr, MSR_WR_LEN, MSR_RD_LEN,
			   &indata, &outdata);
	if (ret != OOB_SUCCESS) {
		return ret;
	}
	*buffer = outdata.value;

	return outdata.status;
}

oob_status_t esmi_oob_cpuid(uint32_t i2c_bus, uint32_t i2c_addr,
			    uint32_t thread, uint32_t *eax, uint32_t *ebx,
			    uint32_t *ecx, uint32_t *edx)
{
	rmi_indata indata;
	rmi_outdata outdata;
	oob_status_t ret;

	/* TODO:
	 * Bits [7:1] select the thread to address. 00h=thread0 ...
	 * 7Fh=thread127. conform thread or logical core
	 */
	PREPARE_CPUID_MSG_IN(thread, eax, ecx, indata);

	ret = esb_rmi_read(i2c_bus, i2c_addr, CPUID_WR_LEN, CPUID_RD_LEN,
			   &indata, &outdata);
	if (ret != OOB_SUCCESS) {
		return ret;
	}
	*eax = outdata.value;
	*ebx = outdata.value >> 32;

	// ecx LSB 1 is to read ecx and edx
	indata.ecx = indata.ecx | 1;

	ret = esb_rmi_read(i2c_bus, i2c_addr, CPUID_WR_LEN, CPUID_RD_LEN,
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
oob_status_t esmi_oob_cpuid_eax(uint32_t i2c_bus, uint32_t i2c_addr,
				uint32_t thread, uint32_t fn_eax,
				uint32_t fn_ecx, uint32_t *eax)
{
	uint32_t ebx, ecx, edx;

	*eax = fn_eax;
	ecx = fn_ecx;
	return esmi_oob_cpuid(i2c_bus, i2c_addr, thread,
			      eax, &ebx, &ecx, &edx);
}

oob_status_t esmi_oob_cpuid_ebx(uint32_t i2c_bus, uint32_t i2c_addr,
				uint32_t thread, uint32_t fn_eax,
				uint32_t fn_ecx, uint32_t *ebx)
{
	uint32_t eax, ecx, edx;

	eax = fn_eax;
	ecx = fn_ecx;
	return esmi_oob_cpuid(i2c_bus, i2c_addr, thread,
			     &eax, ebx, &ecx, &edx);
}

oob_status_t esmi_oob_cpuid_ecx(uint32_t i2c_bus, uint32_t i2c_addr,
				uint32_t thread, uint32_t fn_eax,
				uint32_t fn_ecx, uint32_t *ecx)
{
	uint32_t eax, ebx, edx;

	eax = fn_eax;
	*ecx = fn_ecx;
	return esmi_oob_cpuid(i2c_bus, i2c_addr, thread,
			     &eax, &ebx, ecx, &edx);
}

oob_status_t esmi_oob_cpuid_edx(uint32_t i2c_bus, uint32_t i2c_addr,
				uint32_t thread, uint32_t fn_eax,
				uint32_t fn_ecx, uint32_t *edx)
{
	uint32_t eax, ebx, ecx;

	eax = fn_eax;
	ecx = fn_ecx;
	return esmi_oob_cpuid(i2c_bus, i2c_addr, thread,
			     &eax, &ebx, &ecx, edx);
}

