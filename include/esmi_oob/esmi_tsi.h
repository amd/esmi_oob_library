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

/** \file esmi_tsi.h
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

/**
 * @brief Register encode the temperature to increase in 0.125
 * In decimal portion one increase in byte is equivalent to 0.125
 */
#define TEMP_ENC 0.125

/**
 * @brief Bitfield values to be set for SBTSI confirwr register
 */
typedef enum {
	ARA_MASK = 0x2,
	READORDER_MASK = 0x20,
	RUNSTOP_MASK = 0x40,
	ALERTMASK_MASK = 0x80
} sbtsi_config_write;
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
 *  @param[in] socket a socket index
 *  @param[inout] buffer a pointer to hold the cpu temperature
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 */
oob_status_t read_sbtsi_cpuinttemp(int socket, int8_t *buffer);
/**
 * @brief Status register is Read-only, volatile field
 * If SBTSI::AlertConfig[AlertCompEn] == 0 , the temperature alert is latched
 * high until the alert is read. If SBTSI::AlertConfig[AlertCompEn] == 1,
 * the alert is cleared when the temperature does not meet the threshold
 * conditions for temperature and number of samples.
 *
 * @param[in] socket a socket index
 * @param[inout] buffer a pointer to hold the cpu temperature
 *
 * @retval ::OOB_SUCCESS is returned upon successful call.
 * @retval None-zero is returned upon failure.
 */
oob_status_t read_sbtsi_status(int socket, int8_t *buffer);
/**
 * @brief The bits in this register are Read-only and can be written
 * by Writing to the corresponding bits in SBTSI::ConfigWr
 *
 * @param[in] socket a socket index
 * @param[inout] buffer a pointer to hold the cpu temperature
 *
 * @retval ::OOB_SUCCESS is returned upon successful call.
 * @retval None-zero is returned upon failure.
 */
oob_status_t read_sbtsi_config(int socket, int8_t *buffer);
/**
 * @brief This register value specifies the rate at which CPU temperature
 * is compared against the temperature thresholds to determine if an alert
 * event has occurred.
 *
 * @param[in] socket a socket index
 * @param[inout] buffer a pointer to hold the cpu temperature
 *
 * @retval ::OOB_SUCCESS is returned upon successful call.
 * @retval None-zero is returned upon failure.
 */
oob_status_t read_sbtsi_updaterate(int socket, int8_t *buffer);
/**
 * @brief This register value specifies the rate at which CPU temperature
 * is compared against the temperature thresholds to determine if an alert
 * event has occurred.
 *
 * @param[in] socket a socket index
 * @param[inout] buffer a pointer to hold the cpu temperature
 *
 * @retval ::OOB_SUCCESS is returned upon successful call.
 * @retval None-zero is returned upon failure.
 */
oob_status_t read_sbtsi_updateratehz(int socket, float *buffer);
/**
 * @brief This register value specifies the rate at which CPU temperature
 * is compared against the temperature thresholds to determine if an alert
 * event has occurred.
 *
 * @param[in] socket a socket index
 * @param[in] buffer: value to write in raw format
 *
 * @retval ::OOB_SUCCESS is returned upon successful call.
 * @retval None-zero is returned upon failure.
 */
oob_status_t write_sbtsi_updaterate(int socket, int8_t buffer);
/**
 * @brief This register value specifies the rate at which CPU temperature
 * is compared against the temperature thresholds to determine if an alert
 * event has occurred.
 *
 * @param[in] socket a socket index
 * @param[in] uprate value to write in raw format
 *
 * @retval ::OOB_SUCCESS is returned upon successful call.
 * @retval None-zero is returned upon failure.
 */
oob_status_t write_sbtsi_updateratehz(int socket, float uprate);
/**
 * @brief This value specifies the integer  portion of the high temperature
 * threshold.
 * The high temperature threshold specifies the CPU temperature that
 * causes ALERT_L to assert if the CPU temperature is greater than or
 * equal to the threshold.
 *
 * @param[in] socket a socket index
 * @param[inout] buffer a pointer to hold the cpu temperature
 *
 * @retval ::OOB_SUCCESS is returned upon successful call.
 * @retval None-zero is returned upon failure.
 */
oob_status_t read_sbtsi_hitempint(int socket, int8_t *buffer);
/**
 * @brief This value specifies the integer portion of the low temperature
 * threshold.
 * The low temperature threshold specifies the CPU temperature that causes
 * ALERT_L to assert if the CPU temperature is less than or equal to the
 * threshold
 *
 * @param[in] socket a socket index
 * @param[inout] buffer a pointer to hold the cpu temperature
 *
 * @retval ::OOB_SUCCESS is returned upon successful call.
 * @retval None-zero is returned upon failure.
 */
oob_status_t read_sbtsi_lotempint(int socket, int8_t *buffer);
/**
 * @brief This register provides write access to SBTSI::Config
 *
 * @param[in] socket a socket index
 * @param[inout] buffer a pointer to hold the cpu temperature
 *
 * @retval ::OOB_SUCCESS is returned upon successful call.
 * @retval None-zero is returned upon failure.
 */
oob_status_t read_sbtsi_configwrite(int socket, int8_t *buffer);
/**
 * @brief The value returns the decimal portion of the CPU temperature
 *
 * @param[in] socket a socket index
 * @param[inout] buffer a pointer to hold the cpu temperature
 *
 * @retval ::OOB_SUCCESS is returned upon successful call.
 * @retval None-zero is returned upon failure.
 */
oob_status_t read_sbtsi_cputempdecimal(int socket, uint8_t *buffer);
/**
 * @brief SBTSI::CpuTempOffInt and SBTSI::CpuTempOffDec combine to specify
 * the CPU temperature offset
 *
 * @param[in] socket a socket index
 * @param[inout] buffer a pointer to hold the cpu temperature
 *
 * @retval ::OOB_SUCCESS is returned upon successful call.
 * @retval None-zero is returned upon failure.
 */
oob_status_t read_sbtsi_cputempoffsethibyte(int socket, uint8_t *buffer);
/**
 * @brief This value specifies the decimal/fractional portion of the
 * CPU temperature offset added to Tctl to calculate the CPU temperature.
 *
 * @param[in] socket a socket index
 * @param[inout] buffer a pointer to hold the cpu temperature
 *
 * @retval ::OOB_SUCCESS is returned upon successful call.
 * @retval None-zero is returned upon failure.
 */
oob_status_t read_sbtsi_cputempoffsetdecimal(int socket, uint8_t *buffer);
/**
 * @brief This value specifies the decimal portion of the high temperature
 * threshold.
 *
 * @param[in] socket a socket index
 * @param[inout] buffer a pointer to hold the cpu temperature
 *
 * @retval ::OOB_SUCCESS is returned upon successful call.
 * @retval None-zero is returned upon failure.
 */
oob_status_t read_sbtsi_hitempdecimal(int socket, uint8_t *buffer);
/**
 * @brief value specifies the decimal portion of the low temperature
 * threshold.
 *
 * @param[in] socket a socket index
 * @param[inout] buffer a pointer to hold the cpu temperature
 *
 * @retval ::OOB_SUCCESS is returned upon successful call.
 * @retval None-zero is returned upon failure.
 */
oob_status_t read_sbtsi_lotempdecimal(int socket, uint8_t *buffer);
/**
 * @brief value specifies 0=SMBus defined timeout support disabled.
 * 1=SMBus defined timeout support enabled. SMBus timeout enable.
 * If SB-RMI is in use, SMBus timeouts should
 * be enabled or disabled in a consistent manner on both interfaces.
 * SMBus defined timeouts are not disabled for SB-RMI when this bit is set
 * to 0.
 *
 * @param[in] socket a socket index
 * @param[inout] buffer a pointer to hold the cpu temperature
 *
 * @retval ::OOB_SUCCESS is returned upon successful call.
 * @retval None-zero is returned upon failure.
 */
oob_status_t read_sbtsi_timeoutconfig(int socket, uint8_t *buffer);
/**
 * @brief Specifies the number of consecutive CPU temperature
 * samples for which a temperature alert condition needs to remain valid
 * before the corresponding alert bit is set.
 *
 * @param[in] socket a socket index
 * @param[inout] buffer a pointer to hold the cpu temperature
 *
 * @retval ::OOB_SUCCESS is returned upon successful call.
 * @retval None-zero is returned upon failure.
 */
oob_status_t read_sbtsi_alertthreshold(int socket, int8_t *buffer);
/**
 * @brief Status register is Read-only, volatile field
 * If SBTSI::AlertConfig[AlertCompEn] == 0 , the temperature alert is latched
 * high until the alert is read. If SBTSI::AlertConfig[AlertCompEn] == 1,
 * the alert is cleared when the temperature does not meet the threshold
 * conditions for temperature and number of samples.
 *
 * @param[in] socket a socket index
 * @param[inout] buffer a pointer to hold the cpu temperature
 *
 * @retval ::OOB_SUCCESS is returned upon successful call.
 * @retval None-zero is returned upon failure.
 */
oob_status_t read_sbtsi_alertconfig(int socket, int8_t *buffer);
/**
 * @brief Returns the AMD manufacture ID
 *
 * @param[in] socket a socket index
 * @param[inout] buffer a pointer to hold the cpu temperature
 *
 * @retval ::OOB_SUCCESS is returned upon successful call.
 * @retval None-zero is returned upon failure.
 */
oob_status_t read_sbtsi_manufid(int socket, int8_t *buffer);
/**
 * @brief Specifies the SBI temperature sensor interface revision
 *
 * @param[in] socket a socket index
 * @param[inout] buffer a pointer to hold the cpu temperature
 *
 * @retval ::OOB_SUCCESS is returned upon successful call.
 * @retval None-zero is returned upon failure.
 */
oob_status_t read_sbtsi_revision(int socket, int8_t *buffer);

/* Extra API's for bit parsing */
/**
 * @brief CPU temperature value
 * The CPU temperature is calculated by adding SBTSI::CpuTempInt
 * and SBTSI::CpuTempDec combine to return the CPU temperature.
 *
 * @param[in] socket a socket index
 * @param[inout] temp_value: temperature of the CPU
 *
 * @retval ::OOB_SUCCESS is returned upon successful call.
 * @retval None-zero is returned upon failure.
 */
oob_status_t sbtsi_get_cputemp(int socket, float *temp_value);
/**
 * @brief Status register is Read-only, volatile field
 * If SBTSI::AlertConfig[AlertCompEn] == 0 , the temperature alert is latched
 * high until the alert is read. If SBTSI::AlertConfig[AlertCompEn] == 1,
 * the alert is cleared when the temperature does not meet the threshold
 * conditions for temperature and number of samples.
 *
 * @param[in] socket a socket index
 * @param[inout] loalert: 1=> CPU temp is less than or equal to low temperature
 * 	     threshold for consecutive samples
 * @param[inout] hialert: 1=> CPU temp is greater than or equal to high
 * 		temperature threshold for consecutive samples
 *
 * @retval ::OOB_SUCCESS is returned upon successful call.
 * @retval None-zero is returned upon failure.
 */
oob_status_t sbtsi_get_temp_status(int socket, uint8_t *loalert,
				   uint8_t *hialert);
/**
 * @brief The bits in this register are Read-only and can be written
 * by Writing to the corresponding bits in SBTSI::ConfigWr
 *
 * @param[in] socket a socket index
 * @param[inout] al_mask: 0=> ALERT_L pin enabled. 1=> ALERT_L pin disabled
 * 	     and does not assert.
 * @param[inout] run_stop: 0=> Updates to CpuTempInt and CpuTempDec and
 * 		alert comparisons are enabled. 1=> Updates are disabled and
 * 		alert comparisons are disabled.
 * @param[inout] read_ord: 0=> Reading CpuTempInt causes the satate of
 * 		CpuTempDec to be latched. 1=> Reading CpuTempInt causes
 * 		the satate of CpuTempDec to be latched.
 * @param[inout] ara: 1=> ARA response disabled.
 *
 * @retval ::OOB_SUCCESS is returned upon successful call.
 * @retval None-zero is returned upon failure.
 */
oob_status_t sbtsi_get_config(int socket, uint8_t *al_mask,
			      uint8_t *run_stop, uint8_t *read_ord,
			      uint8_t *ara);
/**
 * @brief The bits in this register are defined sbtsi_config_write and can
 * be written by writing to the corresponding bits in SBTSI::ConfigWr
 *
 * NOTE: Currently testing is not done for this API.
 *
 * @param[in] socket a socket index
 * @param[in] value: value to update 0 or 1
 * @param[in] check: which bit need to update
 *
 * @retval ::OOB_SUCCESS is returned upon successful call.
 * @retval None-zero is returned upon failure.
 */
oob_status_t sbtsi_set_tsi_config(int socket, int value, int check);
/**
 * @brief To verify if timeout support enabled or disabled
 *
 * @param[in] socket a socket index
 * @param[inout] timeout: 0=>SMBus defined timeout support disabled.
 * 	     1=SMBus defined timeout support enabled. SMBus timeout enable.
 * 	     If SB-RMI is in use, SMBus timeouts should
 * 	     be enabled or disabled in a consistent manner on both interfaces.
 * 	     SMBus defined timeouts are not disabled for SB-RMI when this bit
 * 	     is set to 0.
 *
 * @retval ::OOB_SUCCESS is returned upon successful call.
 * @retval None-zero is returned upon failure.
 */
oob_status_t sbtsi_get_timeout(int socket, uint8_t *timeout);
/**
 * @brief To enable/disable timeout support
 *
 * @param[in] socket a socket index
 * @param[in] value:  0=>SMBus defined timeout support disabled.
 *	    1=>SMBus defined timeout support enabled. SMBus timeout enable.
 *	    If SB-RMI is in use, SMBus timeouts should
 *	    be enabled or disabled in a consistent manner on both interfaces.
 *	    SMBus defined timeouts are not disabled for SB-RMI when
 *	    this bit is set to 0.
 *
 * @retval ::OOB_SUCCESS is returned upon successful call.
 * @retval None-zero is returned upon failure.
 */
oob_status_t sbtsi_set_timeout_config(int socket, int value);
/**
 * @brief This value set the high temperature threshold.
 * The high temperature threshold specifies the CPU temperature that
 * causes ALERT_L to assert if the CPU temperature is greater than or
 * equal to the threshold.
 *
 * @param[in] socket a socket index
 * @param[in] temp_int: Specifies the integer part of threshold
 * @param[in] temp_dec: Specifies the decimal part of threshold
 *
 * @retval ::OOB_SUCCESS is returned upon successful call.
 * @retval None-zero is returned upon failure.
 */
oob_status_t sbtsi_set_hightemp_threshold(int socket, int temp_int,
					  float temp_dec);
/**
 * @brief This value set the low temperature threshold.
 * The low temperature threshold specifies the CPU temperature that
 * causes ALERT_L to assert if the CPU temperature is less than or
 * equal to the threshold.
 *
 * @param[in] socket a socket index
 * @param[in] temp_int: Specifies the integer part of threshold
 * @param[in] temp_dec: Specifies the decimal part of threshold
 *
 * @retval ::OOB_SUCCESS is returned upon successful call.
 * @retval None-zero is returned upon failure.
 */
oob_status_t sbtsi_set_lowtemp_threshold(int socket, int temp_int,
					 float temp_dec);
/**
 * @brief This value specifies the high temperature threshold.
 * The high temperature threshold specifies the CPU temperature that
 * causes ALERT_L to assert if the CPU temperature is greater than or
 * equal to the threshold.
 *
 * @param[in] socket a socket index
 * @param[inout] integer: Specifies the integer part of threshold
 * @param[inout] decimal: Specifies the decimal part of threshold
 *
 * @retval ::OOB_SUCCESS is returned upon successful call.
 * @retval None-zero is returned upon failure.
 */
oob_status_t sbtsi_get_htemp_threshold(int socket, int8_t *integer,
				       float *decimal);
/**
 * @brief This value specifies the low temperature threshold.
 * The low temperature threshold specifies the CPU temperature that
 * causes ALERT_L to assert if the CPU temperature is less than or
 * equal to the threshold.
 *
 * @param[in] socket a socket index
 * @param[inout] integer: Specifies the integer part of threshold
 * @param[inout] decimal: Specifies the decimal part of threshold
 *
 * @retval ::OOB_SUCCESS is returned upon successful call.
 * @retval None-zero is returned upon failure.
 */
oob_status_t sbtsi_get_ltemp_threshold(int socket, int8_t *integer,
				       float *decimal);
/**
 * @brief SBTSI::CpuTempOffInt and SBTSI::CpuTempOffDec combine to specify
 * the CPU temperature offset
 *
 * @param[in] socket a socket index
 * @param[inout] temp_offset to get the offset value for temperature
 *
 * @retval ::OOB_SUCCESS is returned upon successful call.
 * @retval None-zero is returned upon failure.
 */
oob_status_t read_sbtsi_cputempoffset(int socket, float *temp_offset);
/**
 * @brief SBTSI::CpuTempOffInt and SBTSI::CpuTempOffDec combine to set
 * the CPU temperature offset
 *
 * @param[in] socket a socket index
 * @param[in] temp_offset to set the offset value for temperature
 *
 * @retval ::OOB_SUCCESS is returned upon successful call.
 * @retval None-zero is returned upon failure.
 */
oob_status_t write_sbtsi_cputempoffset(uint32_t socket, float temp_offset);
/**
 * @brief Specifies the number of consecutive CPU temperature
 * samples for which a temperature alert condition needs to remain valid
 * before the corresponding alert bit is set.
 *
 * @param[in] socket a socket index
 * @param[in] value: Number of samples
 * 	0h: 1 sample
 *	6h-1h: (value + 1) sample
 *	7h: 8 sample
 *
 * @retval ::OOB_SUCCESS is returned upon successful call.
 * @retval None-zero is returned upon failure.
 */
oob_status_t sbtsi_set_alert_threshold(int socket, int value);
/**
 * @brief Alert comparator mode enable
 *
 * @param[in] socket a socket index
 * @param[in] value 0=> SBTSI::Status[TempHighAlert] &
 * 	  SBTSI::Status[TempLowAlert] are read-clear.
 * 	  1=>  SBTSI::Status[TempHighAlert] &
 * 	  SBTSI::Status[TempLowAlert] are read-only. ARA response
 * 	  disabled.
 *
 * @retval ::OOB_SUCCESS is returned upon successful call.
 * @retval None-zero is returned upon failure.
 */
oob_status_t sbtsi_set_alert_config(int socket, int value);
/** @} */  // end of SB-TSI Register access
/*****************************************************************************/

#endif  // INCLUDE_ESMI_OOB_TSI_H_
