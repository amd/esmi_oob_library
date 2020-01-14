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

#include <esmi_oob/esmi_tsi.h>
#include <esmi_oob/esmi_i2c.h>

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

oob_status_t read_sbtsi_alertthreshold(int socket, int8_t *buffer)
{
	return esb_tsi_read_byte(socket, SBTSI_ALERTTHRESHOLD, buffer);
}

oob_status_t read_sbtsi_alertconfig(int socket, int8_t *buffer)
{
	return esb_tsi_read_byte(socket, SBTSI_ALERTCONFIG, buffer);
}

oob_status_t read_sbtsi_manufid(int socket, int8_t *buffer)
{
	return esb_tsi_read_byte(socket, SBTSI_MANUFID, buffer);
}

oob_status_t read_sbtsi_revision(int socket, int8_t *buffer)
{
	return esb_tsi_read_byte(socket, SBTSI_REVISION, buffer);
}
