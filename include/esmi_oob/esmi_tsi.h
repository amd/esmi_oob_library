/*
 * University of Illinois/NCSA Open Source License
 *
 * Copyright (c) 2020, Advanced Micro Devices, Inc.
 * All rights reserved.
 *
 * Developed by:
 *
 *                 AMD Research and AMD Software Development
 *
 *                 Advanced Micro Devices, Inc.
 *
 *                 www.amd.com
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
#ifndef INCLUDE_ESMI_OOB_TSI_H_
#define INCLUDE_ESMI_OOB_TSI_H_

#include "esmi_common.h"

/** \file tsi.h
 *  Header file for the E-SMI-OOB library for SB-TSI functionality access.
 *  All required function, structure, enum, etc. definitions should be defined
 *  in this file for SB-TSI Register accessing.
 *
 *  @details  This header file contains the following:
 *  APIs prototype of the APIs exported by the E-SMI-OOB library.
 *  Description of the API, arguments and return values.
 *  The Error codes returned by the API.
 */

/**
 * @brief SB-TSI(Side-Band Temperature Sensor Interface) commands register
 * access.
 * The below registers mentioned as per Genessis PPR
 */
typedef enum {
	SBTSI_CPU_INT_TEMP = 0x1,
	SBTSI_STATUS,
	SBTSI_CONFIGURATION,
	SBTSI_UPDATERATE,
	SBTSI_HITEMPINT = 0x7,
	SBTSI_LOTEMPINT,
	SBTSI_CONFIGWR,
	SBTSI_CPUTEMPDECIMAL = 0x10,
	SBTSI_CPUTEMPOFFINT,
	SBTSI_CPUTEMPOFFDEC,
	SBTSI_HITEMPDEC,
	SBTSI_LOTEMPDEC,
	SBTSI_TIMEOUTCONFIG = 0x22,
	SBTSI_ALERTTHRESHOLD = 0x32,
	SBTSI_ALERTCONFIG = 0xBF,
	SBTSI_MANUFID = 0xFE,
	SBTSI_REVISION = 0xFF
} sbtsi_registers;

/*****************************************************************************/
/** @defgroup SB-TSIRegisterAccess SBTSI Register Read Byte Protocol

 *  Below functions provide interface to read one byte from the SB-TSI register
 *  and output is from a given SB_TSI register command.
 *  @{
 */

/**
 *  @brief Read one byte from a given SB_TSI register with
 *  provided socket index and buffer to get the read data of a given command
 *
 *  @details Given a socket index @p socket_ind and a pointer to hold the
 *  output at uint8_t @p buffer, this function will get the value from a
 *  particular command of SB_TSI register.
 *
 *  @param[in] socket a socket index
 *
 *  @param[inout] buffer a pointer to a int8_t that indicates
 *  value to hold
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
/**
 *  @brief integer CPU temperature value
 *  The CPU temperature is calculated by adding the CPU temperature
 *  offset(SBTSI::CpuTempOffInt, SBTSI::CpuTempOffDec) to the processor control
 *  temperature (Tctl). SBTSI::CpuTempInt and SBTSI::CpuTempDec
 *  combine to return the CPU temperature.
 *
 *  This field returns the integer portion of the CPU temperature
 */
oob_status_t read_sbtsi_cpuinttemp(int socket, int8_t *buffer);
/**
 * @brief Status register is Read-only, volatile field
 * If SBTSI::AlertConfig[AlertCompEn] == 0 , the temperature alert is latched
 * high until the alert is read. If SBTSI::AlertConfig[AlertCompEn] == 1,
 * the alert is cleared when the temperature does not meet the threshold
 * conditions for temperature and number of samples.
 */
oob_status_t read_sbtsi_status(int socket, int8_t *buffer);
/**
 * @brief The bits in this register are Read-only and can be written
 * by Writing to the corresponding bits in SBTSI::ConfigWr
 */
oob_status_t read_sbtsi_config(int socket, int8_t *buffer);
/**
 * @brief This register value specifies the rate at which CPU temperature
 * is compared against the temperature thresholds to determine if an alert
 * event has occurred.
 */
oob_status_t read_sbtsi_updaterate(int socket, int8_t *buffer);
/**
 * @brief This value specifies the integer  portion of the high temperature
 * threshold.
 * The high temperature threshold specifies the CPU temperature that
 * causes ALERT_L to assert if the CPU temperature is greater than or
 * equal to the threshold.
 */
oob_status_t read_sbtsi_hitempint(int socket, int8_t *buffer);
/**
 * @brief This value specifies the integer portion of the low temperature
 * threshold.
 * The low temperature threshold specifies the CPU temperature that causes
 * ALERT_L to assert if the CPU temperature is less than or equal to the
 * threshold
 */
oob_status_t read_sbtsi_lotempint(int socket, int8_t *buffer);
/**
 * @brief This register provides write access to SBTSI::Config
 */
oob_status_t read_sbtsi_configwrite(int socket, int8_t *buffer);
/**
 * @brief The value returns the decimal portion of the CPU temperature
 */
oob_status_t read_sbtsi_cputempdecimal(int socket, uint8_t *buffer);
/**
 * @brief SBTSI::CpuTempOffInt and SBTSI::CpuTempOffDec combine to specify
 * the CPU temperature offset
 */
oob_status_t read_sbtsi_cputempoffsethibyte(int socket, uint8_t *buffer);
/**
 * @brief This value specifies the decimal/fractional portion of the
 * CPU temperature offset added to Tctl to calculate the CPU temperature.
 */
oob_status_t read_sbtsi_cputempoffsetdecimal(int socket, uint8_t *buffer);
/**
 * @brief This value specifies the decimal portion of the high temperature
 * threshold.
 */
oob_status_t read_sbtsi_hitempdecimal(int socket, uint8_t *buffer);
/**
 * @brief value specifies the decimal portion of the low temperature
 * threshold.
 */
oob_status_t read_sbtsi_lotempdecimal(int socket, uint8_t *buffer);
/**
 * @brief value specifies 0=SMBus defined timeout support disabled.
 * 1=SMBus defined timeout support enabled. SMBus timeout enable.
 * If SB-RMI is in use, SMBus timeouts should
 * be enabled or disabled in a consistent manner on both interfaces.
 * SMBus defined timeouts are not disabled for SB-RMI when this bit is set
 * to 0.
 */
oob_status_t read_sbtsi_timeoutconfig(int socket, uint8_t *buffer);
/**
 * @brief Status register is Read-only, volatile field
 * If SBTSI::AlertConfig[AlertCompEn] == 0 , the temperature alert is latched
 * high until the alert is read. If SBTSI::AlertConfig[AlertCompEn] == 1,
 * the alert is cleared when the temperature does not meet the threshold
 * conditions for temperature and number of samples.
 */
oob_status_t read_sbtsi_alertthreshold(int socket, int8_t *buffer);
/**
 * @brief Specifies the number of consecutive CPU temperature
 * samples for which a temperature alert condition needs to remain valid
 * before the corresponding alert bit is set.
 */
oob_status_t read_sbtsi_alertconfig(int socket, int8_t *buffer);
/**
 * @brief Returns the AMD manufacture ID
 */
oob_status_t read_sbtsi_manufid(int socket, int8_t *buffer);
/**
 * @brief Specifies the SBI temperature sensor interface revision
 */
oob_status_t read_sbtsi_revision(int socket, int8_t *buffer);

/** @} */  // end of SB-TSI Register access
/*****************************************************************************/

#endif  // INCLUDE_ESMI_OOB_TSI_H_
