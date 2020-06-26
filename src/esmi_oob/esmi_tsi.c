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
#include <stdint.h>
#include <unistd.h>

#include <esmi_oob/esmi_tsi.h>
#include <esmi_oob/esmi_i2c.h>
#include <math.h>

/* sb-tsi registers read one byte value*/
static oob_status_t esb_tsi_read_byte(int socket, int cmd, int8_t *buffer)
{
	return esmi_oob_read_byte(socket, SLAVE_TSI, cmd, buffer);
}

static oob_status_t esb_tsi_write_byte(int socket, int cmd, int8_t value)
{
        return esmi_oob_write_byte(socket, SLAVE_TSI, cmd, value);
}

/* sb-tsi register access */
oob_status_t read_sbtsi_cpuinttemp(int socket, int8_t *buffer)
{
	return esb_tsi_read_byte(socket, SBTSI_CPU_INT_TEMP, buffer);
}

oob_status_t read_sbtsi_status(int socket, int8_t *buffer)
{
	return esb_tsi_read_byte(socket, SBTSI_STATUS, buffer);
}

oob_status_t read_sbtsi_config(int socket, int8_t *buffer)
{
	return esb_tsi_read_byte(socket, SBTSI_CONFIGURATION, buffer);
}

oob_status_t read_sbtsi_updaterate(int socket, int8_t *buffer)
{
	return esb_tsi_read_byte(socket, SBTSI_UPDATERATE, buffer);
}

oob_status_t write_sbtsi_updaterate(int socket, int8_t buffer)
{
	return esb_tsi_write_byte(socket, SBTSI_UPDATERATE, buffer);
}

oob_status_t read_sbtsi_updateratehz(int socket, float *buffer)
{
	oob_status_t ret;
	uint8_t rdbyte;
	ret = esb_tsi_read_byte(socket, SBTSI_UPDATERATE, &rdbyte);
	if (ret != OOB_SUCCESS) {
		return ret;
	}
	*buffer = pow(2, rdbyte - 4);

	return OOB_SUCCESS;
}

oob_status_t write_sbtsi_updateratehz(int socket, float uprate)
{
	oob_status_t ret;
	uint8_t wrbyte;
	float val;

	val = uprate * 16;

	if (val < 1 || val > 1024) {
		return OOB_INVALID_INPUT;
	}
	wrbyte = (log(uprate)/log(2)) + 4;

	ret = esb_tsi_write_byte(socket, SBTSI_UPDATERATE, wrbyte);
	if (ret != OOB_SUCCESS) {
		return ret;
	}
	return OOB_SUCCESS;
}

oob_status_t sbtsi_set_hightemp_threshold(int socket, int temp_int, float temp_dec)
{
	oob_status_t ret;
	int dec, current;
	int8_t prev;
	ret = esb_tsi_write_byte(socket, SBTSI_HITEMPINT, temp_int);
	if (ret != OOB_SUCCESS) {
		return ret;
	}

	dec = temp_dec / TEMP_ENC;
	ret = esb_tsi_read_byte(socket, SBTSI_HITEMPDEC, &prev);
	if (ret != OOB_SUCCESS) {
		return ret;
	}
	current = ((prev & 0x1F) | (dec << 5));
	ret = esb_tsi_write_byte(socket, SBTSI_HITEMPDEC, current);
	if (ret != OOB_SUCCESS) {
		return ret;
	}
	return OOB_SUCCESS;

}

oob_status_t sbtsi_set_lowtemp_threshold(int socket, int temp_int, float temp_dec)
{
	oob_status_t ret;
	int dec, current;
	int8_t prev;
	ret = esb_tsi_write_byte(socket, SBTSI_LOTEMPINT, temp_int);
	if (ret != OOB_SUCCESS) {
		return ret;
	}

	dec = temp_dec / TEMP_ENC;
	ret = esb_tsi_read_byte(socket, SBTSI_LOTEMPDEC, &prev);
	if (ret != OOB_SUCCESS) {
		return ret;
	}
	current = ((prev & 0x1F) | (dec << 5));
	ret = esb_tsi_write_byte(socket, SBTSI_LOTEMPDEC, current);
	if (ret != OOB_SUCCESS) {
		return ret;
	}
	return OOB_SUCCESS;

}

oob_status_t sbtsi_set_timeout_config(int socket, int value)
{
	oob_status_t ret;
	int8_t prev;
	int new;

	if (value < 0 || value > 1) {
		return OOB_INVALID_INPUT;
	}
	ret = esb_tsi_read_byte(socket, SBTSI_TIMEOUTCONFIG, &prev);
	if (ret != OOB_SUCCESS) {
		return ret;
	}
	new = (prev & 0x7F) | (value << 7);
	ret = esb_tsi_write_byte(socket, SBTSI_TIMEOUTCONFIG, new);
	if (ret != OOB_SUCCESS) {
		return ret;
	}
	return OOB_SUCCESS;
}

oob_status_t sbtsi_set_alert_threshold(int socket, int value)
{
	oob_status_t ret;
	int8_t prev;
	int new;

	if (value < 1 || value > 8) {
		return OOB_INVALID_INPUT;
	}
	/**
	 * 0h: 1 sample
	 * 6h-1h: (value + 1) sample
	 * 7h: 8 sample
	 */
	value = value - 1;
	ret = esb_tsi_read_byte(socket, SBTSI_ALERTTHRESHOLD, &prev);
	if (ret != OOB_SUCCESS) {
		return ret;
	}
	new = (prev & 0xF8) | value;
	ret = esb_tsi_write_byte(socket, SBTSI_ALERTTHRESHOLD, new);
	if (ret != OOB_SUCCESS) {
		return ret;
	}
	return OOB_SUCCESS;
}

oob_status_t sbtsi_set_alert_config(int socket, int value)
{
	oob_status_t ret;
	int8_t prev;
	int new;

	if (value < 0 || value > 1) {
		return OOB_INVALID_INPUT;
	}
	ret = esb_tsi_read_byte(socket, SBTSI_ALERTCONFIG, &prev);
	if (ret != OOB_SUCCESS) {
		return ret;
	}
	new = (prev & 0xFE) | value;
	ret = esb_tsi_write_byte(socket, SBTSI_ALERTCONFIG, new);
	if (ret != OOB_SUCCESS) {
		return ret;
	}
	return OOB_SUCCESS;
}

oob_status_t sbtsi_set_tsi_config(int socket, int value, int check)
{
	oob_status_t ret;
	int8_t prev;
	int new;

	if (value < 0 || value > 1) {
		return OOB_INVALID_INPUT;
	}
	ret = esb_tsi_read_byte(socket, SBTSI_CONFIGWR, &prev);
	if (ret != OOB_SUCCESS) {
		return ret;
	}
	switch (check) {
		case ALERTMASK_MASK:
			new = (prev & ~(ALERTMASK_MASK)) | (value << 7);
			break;
		case RUNSTOP_MASK:
			new = (prev & ~(RUNSTOP_MASK)) | (value << 6);
			break;
		case READORDER_MASK:
			new = (prev & ~(READORDER_MASK)) | (value << 5);
			break;
		case ARA_MASK:
			new = (prev & ~(ARA_MASK)) | (value << 1);
			break;
		default:
			return OOB_INVALID_INPUT;
	}
	ret = esb_tsi_write_byte(socket, SBTSI_CONFIGWR, new);
	if (ret != OOB_SUCCESS) {
		return ret;
	}
	return OOB_SUCCESS;
}

oob_status_t read_sbtsi_hitempint(int socket, int8_t *buffer)
{
	return esb_tsi_read_byte(socket, SBTSI_HITEMPINT, buffer);
}

oob_status_t read_sbtsi_lotempint(int socket, int8_t *buffer)
{
	return esb_tsi_read_byte(socket, SBTSI_LOTEMPINT, buffer);
}

oob_status_t read_sbtsi_configwrite(int socket, int8_t *buffer)
{
	return esb_tsi_read_byte(socket, SBTSI_CONFIGWR, buffer);
}

oob_status_t read_sbtsi_cputempdecimal(int socket, uint8_t *buffer)
{
	return esb_tsi_read_byte(socket, SBTSI_CPUTEMPDECIMAL, buffer);
}

oob_status_t read_sbtsi_cputempoffsethibyte(int socket, uint8_t *buffer)
{
	return esb_tsi_read_byte(socket, SBTSI_CPUTEMPOFFINT, buffer);
}

oob_status_t read_sbtsi_cputempoffsetdecimal(int socket, uint8_t *buffer)
{
	return esb_tsi_read_byte(socket, SBTSI_CPUTEMPOFFDEC, buffer);
}

oob_status_t read_sbtsi_hitempdecimal(int socket, uint8_t *buffer)
{
	return esb_tsi_read_byte(socket, SBTSI_HITEMPDEC, buffer);
}

oob_status_t read_sbtsi_lotempdecimal(int socket, uint8_t *buffer)
{
	return esb_tsi_read_byte(socket, SBTSI_LOTEMPDEC, buffer);
}

oob_status_t read_sbtsi_timeoutconfig(int socket, uint8_t *buffer)
{
	return esb_tsi_read_byte(socket, SBTSI_TIMEOUTCONFIG, buffer);
}

oob_status_t read_sbtsi_cputempoffset(int socket, float *temp_offset)
{
	oob_status_t ret;
	uint8_t value_int, value_dec;
	uint16_t intdec;

	ret = esb_tsi_read_byte(socket, SBTSI_CPUTEMPOFFINT, &value_int);
	if (ret != OOB_SUCCESS) {
		return ret;
	}
	ret = esb_tsi_read_byte(socket, SBTSI_CPUTEMPOFFDEC, &value_dec);
	if (ret != OOB_SUCCESS) {
		return ret;
	}
	if (value_int >> 7) {
		intdec = (value_int << 3) | (value_dec >> 5);
		intdec = ~(intdec) + 1;
		*temp_offset = ((intdec >> 3) & 0xFF) + (intdec & 7) * TEMP_ENC;
		*temp_offset = -(*temp_offset);
	} else {
		*temp_offset = value_int + (value_dec >> 5) * TEMP_ENC;
	}
	return OOB_SUCCESS;
}

oob_status_t write_sbtsi_cputempoffset(uint32_t socket, float temp_offset)
{
	oob_status_t ret;
	int dec, current;
	int8_t prev;
	int8_t temp_int;
	float temp_dec, temp_value;

	ret = sbtsi_get_cputemp(socket, &temp_value);
	if (ret != OOB_SUCCESS) {
		return ret;
	}
	if ((temp_offset + temp_value) < 0 || temp_offset > 0x7f)
		return OOB_INVALID_INPUT;

	if (temp_offset >= 0) {
		temp_int = temp_offset;
		temp_dec = (temp_offset - temp_int);
	} else {
		temp_int = temp_offset;
		temp_dec = (temp_int - temp_offset);
		if (temp_dec) {
			temp_int = temp_int - 1;
			temp_dec = 1 - temp_dec;
		}
	}

	ret = esb_tsi_write_byte(socket, SBTSI_CPUTEMPOFFINT, temp_int);
	if (ret != OOB_SUCCESS) {
		return ret;
	}

	dec = temp_dec / TEMP_ENC;
	ret = esb_tsi_read_byte(socket, SBTSI_CPUTEMPOFFDEC, &prev);
	if (ret != OOB_SUCCESS) {
		return ret;
	}
	current = ((prev & 0x1F) | (dec << 5));
	ret = esb_tsi_write_byte(socket, SBTSI_CPUTEMPOFFDEC, current);
	if (ret != OOB_SUCCESS) {
		return ret;
	}
	return OOB_SUCCESS;
}

oob_status_t read_sbtsi_alertthreshold(int socket, int8_t *buffer)
{
	oob_status_t ret;
	int8_t value;
	ret = esb_tsi_read_byte(socket, SBTSI_ALERTTHRESHOLD, &value);
	if (ret != OOB_SUCCESS) {
		return ret;
	}
	*buffer = (value & 7) + 1;

	return OOB_SUCCESS;
}

oob_status_t read_sbtsi_alertconfig(int socket, int8_t *buffer)
{
	oob_status_t ret;
	int8_t value;
	ret = esb_tsi_read_byte(socket, SBTSI_ALERTCONFIG, &value);
	if (ret != OOB_SUCCESS) {
		return ret;
	}
	*buffer = (value & 1);

	return OOB_SUCCESS;
}

oob_status_t read_sbtsi_manufid(int socket, int8_t *buffer)
{
	oob_status_t ret;
	int8_t value;
	ret = esb_tsi_read_byte(socket, SBTSI_MANUFID, &value);
	if (ret != OOB_SUCCESS) {
		return ret;
	}
	*buffer = (value & 1);

	return OOB_SUCCESS;
}

oob_status_t read_sbtsi_revision(int socket, int8_t *buffer)
{
	return esb_tsi_read_byte(socket, SBTSI_REVISION, buffer);
}

oob_status_t sbtsi_get_cputemp(int socket, float *temp_value)
{
	oob_status_t ret;
	uint8_t intbyte, decibyte;

	ret = esb_tsi_read_byte(socket, SBTSI_CPU_INT_TEMP, &intbyte);
	if (ret != OOB_SUCCESS) {
		return ret;
	}
	usleep(1000);
	ret = esb_tsi_read_byte(socket, SBTSI_CPUTEMPDECIMAL, &decibyte);
	if (ret != OOB_SUCCESS) {
		return ret;
	}
	*temp_value = intbyte + ((decibyte >> 5) * TEMP_ENC);

	return OOB_SUCCESS;
}

oob_status_t sbtsi_get_htemp_threshold(int socket, int8_t *integer, float *decimal)
{
	oob_status_t ret;
	uint8_t intbyte, decibyte;

	ret = esb_tsi_read_byte(socket, SBTSI_HITEMPINT, &intbyte);
	if (ret != OOB_SUCCESS) {
		return ret;
	}
	*integer = intbyte;
	usleep(1000);
	ret = esb_tsi_read_byte(socket, SBTSI_HITEMPDEC ,&decibyte);
	if (ret != OOB_SUCCESS) {
		return ret;
	}
	*decimal = (decibyte >> 5) * TEMP_ENC ;

	return OOB_SUCCESS;
}

oob_status_t sbtsi_get_ltemp_threshold(int socket, int8_t *integer, float *decimal)
{
	oob_status_t ret;
	uint8_t intbyte, decibyte;

	ret = esb_tsi_read_byte(socket, SBTSI_LOTEMPINT, &intbyte);
	if (ret != OOB_SUCCESS) {
		return ret;
	}
	*integer = intbyte;
	usleep(1000);
	ret = esb_tsi_read_byte(socket, SBTSI_LOTEMPDEC, &decibyte);
	if (ret != OOB_SUCCESS) {
		return ret;
	}
	*decimal = (decibyte >> 5) * TEMP_ENC ;

	return OOB_SUCCESS;
}

oob_status_t sbtsi_get_temp_status(int socket, uint8_t *loalert, uint8_t *hialert)
{
	oob_status_t ret;
	uint8_t rdbyte;
	ret = esb_tsi_read_byte(socket, SBTSI_STATUS, &rdbyte);
	if (ret != OOB_SUCCESS) {
		return ret;
	}
	*loalert  = rdbyte & (1 << 3);
	*hialert = rdbyte & (1 << 4);

	return OOB_SUCCESS;
}

oob_status_t sbtsi_get_config(int socket, uint8_t *al_mask,
			      uint8_t *run_stop, uint8_t *read_ord,
			      uint8_t *ara)
{
	oob_status_t ret;
	uint8_t rdbytes;
	ret = esb_tsi_read_byte(socket, SBTSI_CONFIGURATION, &rdbytes);
	if (ret != OOB_SUCCESS) {
		return ret;
	}
	*al_mask = rdbytes & (1 << 7);
	*run_stop = rdbytes & (1 << 6);
	*read_ord = rdbytes & (1 << 5);
	*ara = rdbytes & (1 << 1);

	return OOB_SUCCESS;
}

oob_status_t sbtsi_get_timeout(int socket, uint8_t *timeout)
{
	oob_status_t ret;
	uint8_t rdbyte;
	ret = esb_tsi_read_byte(socket, SBTSI_TIMEOUTCONFIG, &rdbyte);
	if (ret != OOB_SUCCESS) {
		return ret;
	}
	*timeout = rdbyte & (1 << 7);

	return OOB_SUCCESS;
}
