/*
 * University of Illinois/NCSA Open Source License
 *
 * Copyright (c) 2023, Advanced Micro Devices, Inc.
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
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>

#include <esmi_oob/apml.h>
#include <esmi_oob/esmi_tsi.h>
#include <esmi_oob/tsi_mi300.h>

/* Decimal portion bits */
#define DEC_PORTION_BITS	5	//!< Decimal portion bits
/* Delay time or wait time */
#define WAIT_TIME		1000	//!< Delay time
/* Min temperature */
#define MIN_TEMP		0	//!< Min Temp
/* Max temperature */
#define MAX_TEMP		255	//!< Max Temp

/* sb-tsi register access for MI*/

oob_status_t read_sbtsi_hbm_hi_temp_int_th(uint8_t soc_num, uint8_t *buffer)
{
	return esmi_oob_read_byte(soc_num, SBTSI_HBM_HITEMPINT_LIMIT,
				  SBTSI, buffer);
}

oob_status_t read_sbtsi_hbm_hi_temp_dec_th(uint8_t soc_num, float *buffer)
{
	uint8_t d_out;
	oob_status_t ret;

	if (!buffer)
		return OOB_ARG_PTR_NULL;

	ret = esmi_oob_read_byte(soc_num, SBTSI_HBM_HITEMPDEC_LIMIT,
				 SBTSI, &d_out);
	if (!ret)
		*buffer = ((d_out >> DEC_PORTION_BITS) * TEMP_INC);

	return ret;
}

oob_status_t write_sbtsi_hbm_hi_temp_th(uint8_t soc_num, float hi_temp_th)
{
	uint8_t int_temp, cur_temp_dec, byte_dec;
	float temp_dec;
	oob_status_t ret;

	if (hi_temp_th < MIN_TEMP || hi_temp_th >= MAX_TEMP)
		return OOB_INVALID_INPUT;

	int_temp = hi_temp_th;
	temp_dec = hi_temp_th - int_temp;

	/* Write integer part of high temp threshold */
	ret = esmi_oob_write_byte(soc_num, SBTSI_HBM_HITEMPINT_LIMIT,
				  SBTSI, int_temp);
	if (ret)
		return ret;

	/* Read decimal part of current high temp threshold */
	ret = esmi_oob_read_byte(soc_num, SBTSI_HBM_HITEMPDEC_LIMIT, SBTSI,
				 &cur_temp_dec);
	if (ret)
		return ret;

	/* get number of steps increment */
	byte_dec = temp_dec / TEMP_INC;
	/* [7:5] HiTempDec and [4:0] Reserved */
	byte_dec = ((byte_dec << 5) | (cur_temp_dec & 0x1F));

	return esmi_oob_write_byte(soc_num, SBTSI_HBM_HITEMPDEC_LIMIT,
				   SBTSI, byte_dec);
}

oob_status_t read_sbtsi_hbm_hi_temp_th(uint8_t soc_num, float *buffer)
{
	float dec_temp;
	uint8_t int_temp;
	oob_status_t ret;

	if (!buffer)
		return OOB_ARG_PTR_NULL;

	ret = read_sbtsi_hbm_hi_temp_int_th(soc_num, &int_temp);
	if (ret)
		return ret;

	usleep(WAIT_TIME);
	ret = read_sbtsi_hbm_hi_temp_dec_th(soc_num, &dec_temp);
	if (!ret)
		*buffer = int_temp + dec_temp;

	return ret;
}

oob_status_t read_sbtsi_hbm_lo_temp_int_th(uint8_t soc_num, uint8_t *buffer)
{
	return esmi_oob_read_byte(soc_num, SBTSI_HBM_LOTEMPINT_LIMIT,
				  SBTSI, buffer);
}

oob_status_t read_sbtsi_hbm_lo_temp_dec_th(uint8_t soc_num, float *buffer)
{
	uint8_t d_out;
	oob_status_t ret;

	if (!buffer)
		return OOB_ARG_PTR_NULL;

	ret =  esmi_oob_read_byte(soc_num, SBTSI_HBM_LOTEMPDEC_LIMIT,
				  SBTSI, &d_out);
	if (!ret)
		*buffer = ((d_out >> DEC_PORTION_BITS) * TEMP_INC);

	return ret;
}

oob_status_t write_sbtsi_hbm_lo_temp_th(uint8_t soc_num, float temp_th)
{
	uint8_t int_temp, cur_temp_dec, byte_dec;
	float temp_dec;
	oob_status_t ret;

	if (temp_th < MIN_TEMP || temp_th >= MAX_TEMP)
		return OOB_INVALID_INPUT;

	int_temp = temp_th;
	temp_dec = temp_th - int_temp;

	/* Write integer part of low temp threshold */
	ret = esmi_oob_write_byte(soc_num, SBTSI_HBM_LOTEMPINT_LIMIT,
				  SBTSI, int_temp);
	if (ret)
		return ret;

	/* Read decimal part of current low temp threshold */
	ret = esmi_oob_read_byte(soc_num, SBTSI_HBM_LOTEMPDEC_LIMIT, SBTSI,
				 &cur_temp_dec);
	if (ret)
		return ret;

	/* get number of steps increment */
	byte_dec = temp_dec / TEMP_INC;
	/* [7:5] HiTempDec and [4:0] Reserved */
	byte_dec = ((byte_dec << 5) | (cur_temp_dec & 0x1F));

	return esmi_oob_write_byte(soc_num, SBTSI_HBM_LOTEMPDEC_LIMIT,
				   SBTSI, byte_dec);
}

oob_status_t read_sbtsi_hbm_lo_temp_th(uint8_t soc_num, float *buffer)
{
	float dec_temp;
	uint8_t int_temp;
	oob_status_t ret;

	if (!buffer)
		return OOB_ARG_PTR_NULL;

	ret = read_sbtsi_hbm_lo_temp_int_th(soc_num, &int_temp);
	if (ret)
		return ret;

	usleep(WAIT_TIME);
	ret = read_sbtsi_hbm_lo_temp_dec_th(soc_num, &dec_temp);
	if (!ret)
		*buffer = int_temp + dec_temp;

	return ret;
}

oob_status_t read_sbtsi_max_hbm_temp_int(uint8_t soc_num, uint8_t *buffer)
{
	return esmi_oob_read_byte(soc_num, SBTSI_MAX_HBMTEMPINT,
				  SBTSI, buffer);
}

oob_status_t read_sbtsi_max_hbm_temp_dec(uint8_t soc_num, float *buffer)
{
        uint8_t d_out;
        oob_status_t ret;

	if (!buffer)
		return OOB_ARG_PTR_NULL;

	ret = esmi_oob_read_byte(soc_num, SBTSI_MAX_HBMTEMPDEC,
				 SBTSI, &d_out);
	if (!ret)
		*buffer = ((d_out >> DEC_PORTION_BITS) * TEMP_INC);

	return ret;
}

oob_status_t read_sbtsi_max_hbm_temp(uint8_t soc_num, float *buffer)
{
	float dec_temp;
	uint8_t int_temp;
	oob_status_t ret;

	if (!buffer)
		return OOB_ARG_PTR_NULL;

	ret = read_sbtsi_max_hbm_temp_int(soc_num, &int_temp);
	if (ret)
		return ret;

	usleep(WAIT_TIME);
	ret = read_sbtsi_max_hbm_temp_dec(soc_num, &dec_temp);
	if (!ret)
		*buffer = int_temp + dec_temp;

	return ret;
}

oob_status_t read_sbtsi_hbm_temp_int(uint8_t soc_num, uint8_t *buffer)
{
	return esmi_oob_read_byte(soc_num, SBTSI_HBMTEMPINT,
				  SBTSI, buffer);
}

oob_status_t read_sbtsi_hbm_temp_dec(uint8_t soc_num, float *buffer)
{
	uint8_t d_out;
	oob_status_t ret;

	if (!buffer)
		return OOB_ARG_PTR_NULL;

	ret = esmi_oob_read_byte(soc_num, SBTSI_HBMTEMPDEC,
				 SBTSI, &d_out);
	if (!ret)
		*buffer = ((d_out >> DEC_PORTION_BITS) * TEMP_INC);

	return ret;
}

oob_status_t read_sbtsi_hbm_temp(uint8_t soc_num, float *buffer)
{
	float dec_temp;
	uint8_t int_temp;
	oob_status_t ret;

	if (!buffer)
		return OOB_ARG_PTR_NULL;

	ret = read_sbtsi_hbm_temp_int(soc_num, &int_temp);
	if (ret)
		return ret;

	usleep(WAIT_TIME);
	ret = read_sbtsi_hbm_temp_dec(soc_num, &dec_temp);
	if (!ret)
		*buffer = int_temp + dec_temp;

	return ret;
}
