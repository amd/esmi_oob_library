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

#include <esmi_oob/esmi_common.h>
#include <esmi_oob/esmi_rmi.h>
#include <esmi_oob/esmi_i2c.h>

/* sb-rmi register access */
oob_status_t read_sbrmi_revision(uint32_t i2c_bus, uint32_t i2c_addr,
				 uint8_t *buffer)
{
	return esmi_oob_read_byte(i2c_bus, i2c_addr, SBRMI_REVISION, buffer);
}

oob_status_t read_sbrmi_control(uint32_t i2c_bus, uint32_t i2c_addr,
				uint8_t *buffer)
{
	return esmi_oob_read_byte(i2c_bus, i2c_addr, SBRMI_CONTROL, buffer);
}

oob_status_t read_sbrmi_status(uint32_t i2c_bus, uint32_t i2c_addr,
			       uint8_t *buffer)
{
	return esmi_oob_read_byte(i2c_bus, i2c_addr, SBRMI_STATUS, buffer);
}

oob_status_t read_sbrmi_readsize(uint32_t i2c_bus, uint32_t i2c_addr,
				 uint8_t *buffer)
{
	return esmi_oob_read_byte(i2c_bus, i2c_addr, SBRMI_READSIZE, buffer);
}

oob_status_t read_sbrmi_threadenablestatus(uint32_t i2c_bus, uint32_t i2c_addr,
					   uint8_t *buffer)
{
	return esmi_oob_read_byte(i2c_bus, i2c_addr,
				  SBRMI_THREADENABLESTATUS, buffer);
}

oob_status_t read_sbrmi_swinterrupt(uint32_t i2c_bus, uint32_t i2c_addr,
				    uint8_t *buffer)
{
	return esmi_oob_read_byte(i2c_bus, i2c_addr,
				  SBRMI_SOFTWAREINTERRUPT, buffer);
}

oob_status_t read_sbrmi_threadnumber(uint32_t i2c_bus, uint32_t i2c_addr,
				     uint8_t *buffer)
{
	return esmi_oob_read_byte(i2c_bus, i2c_addr,
				  SBRMI_THREADNUMBER, buffer);
}
