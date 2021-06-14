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
#ifndef INCLUDE_ESMI_OOB_MAILBOX_H_
#define INCLUDE_ESMI_OOB_MAILBOX_H_

#include "esmi_common.h"

/** \file esmi_mailbox.h
 *  Header file for the Mailbox messages supported by E-SMI OOB library.
 *  All required function, structure, enum, etc. definitions should be defined
 *  in this file.
 *
 *  @details  This header file contains the following:
 *  APIs prototype of the Mailbox messages exported by the E-SMI OOB library.
 *  Description of the API, arguments and return values.
 *  The Error codes returned by the API.
 */

/**
 * @brief Mailbox message types defined in the E-SMI OOB library
 */
typedef enum {
	READ_PACKAGE_POWER_CONSUMPTION	= 0x1,
	WRITE_PACKAGE_POWER_LIMIT,
	READ_PACKAGE_POWER_LIMIT,
	READ_MAX_PACKAGE_POWER_LIMIT,
	READ_TDP,
	READ_MAX_cTDP,
	READ_MIN_cTDP,
	READ_BIOS_BOOST_Fmax,
	READ_APML_BOOST_LIMIT,
	WRITE_APML_BOOST_LIMIT,
	WRITE_APML_BOOST_LIMIT_ALLCORES,
	READ_DRAM_THROTTLE,
	WRITE_DRAM_THROTTLE,
	READ_PROCHOT_STATUS,
	READ_PROCHOT_RESIDENCY,
	READ_VDDIO_MEM_POWER,
	READ_NBIO_ERROR_LOGGING_REGISTER,
	READ_IOD_BIST = 0x13,
	READ_CCD_BIST_RESULT,
	READ_CCX_BIST_RESULT,
	READ_PACKAGE_CCLK_FREQ_LIMIT,
	READ_PACKAGE_C0_RESIDENCY,
	READ_DDR_BANDWIDTH
} esb_mailbox_commmands;

/*****************************************************************************/

/** @defgroup MailboxMsg SB-RMI Mailbox Service
 *  Below functions to support SB-RMI Mailbox messages to read,
 *  write, 'write and read' operations for a given socket.
 *  @{
 */
/** @defgroup PowerQuer Power Monitor
 *  Below functions provide interfaces to get the current power usage and
 *  Power Limits for a given socket.
 *  @{
 */

/**
 *  @brief Get the power consumption of the socket with provided
 *  i2c_bus and i2c_addr.
 *
 *  @details Given a i2c_bus and i2c_addr and a pointer to a uint32_t
 *  @p buffer, this function will get the current power consumption
 *  (in watts) to the uint32_t pointed to by @p buffer.
 *
 *  @param[in] i2c_bus is the Bus connected to the socket
 *
 *  @param[in] i2c_addr is the 7-bit socket address
 *
 *  @param[inout] buffer a pointer to uint32_t value of power
 *  consumption
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t read_socket_power(uint32_t i2c_bus, uint32_t i2c_addr,
			       uint32_t *buffer);

/**
 *  @brief Get the current power cap/limit value for a given socket.
 *
 *  @details This function will return the valid power cap @p buffer for a given
 *  socket, this value will be used for the system to limit the power.
 *
 *  @param[in] i2c_bus is the Bus connected to the socket
 *
 *  @param[in] i2c_addr is the 7-bit socket address
 *
 *  @param[inout] buffer a pointer to a uint32_t that indicates the valid
 *  possible power cap/limit, in watts
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t read_socket_power_limit(uint32_t i2c_bus, uint32_t i2c_addr,
				     uint32_t *buffer);

/**
 *  @brief Get the maximum value that can be assigned as a power cap/limit for
 *         a given socket.
 *
 *  @details This function will return the maximum possible valid power cap/limit
 *
 *  @param[in] i2c_bus is the Bus connected to the socket
 *
 *  @param[in] i2c_addr is the 7-bit socket address
 *
 *  @param[out] buffer a pointer to a uint32_t that indicates the maximum
 *                possible power cap/limit, in watts
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t read_max_socket_power_limit(uint32_t i2c_bus, uint32_t i2c_addr,
					 uint32_t *buffer);
/** @} */  // end of PowerQuer
/*****************************************************************************/

/** @defgroup PowerCont Power Control
 *  This function provides a way to control Power Limit.
 *  @{
 */

/**
 *  @brief Set the power cap/limit value for a given socket.
 *
 *  @details This function will set the power cap/limit
 *
 *  @param[in] i2c_bus is the Bus connected to the socket
 *
 *  @param[in] i2c_addr is the 7-bit socket address
 *
 *  @param[in] limit uint32_t that indicates the desired power cap/limit,
 *  in milliwatts
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t write_socket_power_limit(uint32_t i2c_bus, uint32_t i2c_addr,
				      uint32_t limit);

/** @} */  // end of PowerCont

/*****************************************************************************/

/** @defgroup PerfQuer Performance (Boost limit) Monitor
 *  This function provides the current boostlimit value for a given core.
 *  @{
 */

/**
 *  @brief Get the Out-of-band boostlimit value for a given core
 *
 *  @details This function will return the core's current Out-of-band
 *  boost limit
 *  @p buffer for a particular @p value
 *
 *  @param[in] i2c_bus is the Bus connected to the socket
 *
 *  @param[in] i2c_addr is the 7-bit socket address
 *
 *  @param[in] value a cpu index
 *
 *  @param[inout] buffer pointer to a uint32_t that indicates the
 *  possible boost limit value
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t read_esb_boost_limit(uint32_t i2c_bus, uint32_t i2c_addr,
				  uint32_t value,
				  uint32_t *buffer);

/**
 *  @brief Get the In-band maximum boostlimit value for a given core
 *
 *  @details This function will return the core's current maximum In-band
 *  boost limit @p buffer for a particular @p value is cpu_ind
 *
 *  @param[in] i2c_bus is the Bus connected to the socket
 *
 *  @param[in] i2c_addr is the 7-bit socket address
 *
 *  @param[in] value is a cpu index
 *
 *  @param[inout] buffer a pointer to a uint32_t that indicates the
 *  maximum boost limit value set via In-band
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t read_bios_boost_fmax(uint32_t i2c_bus, uint32_t i2c_addr,
				  uint32_t value,
				  uint32_t *buffer);
/** @} */  // end of PerfQuer

/*****************************************************************************/

/** @defgroup PerfCont Out-of-band Performance (Boost limit) Control
 *  Below functions provide ways to control the Out-of-band Boost limit values.
 *  @{
 */

/**
 *  @brief Set the Out-of-band boostlimit value for a given core
 *
 *  @details This function will set the boostlimit to the provided value @p
 *  limit for a given cpu.
 *  NOTE: Currently the limit is setting for all the cores instead of a
 *  particular cpu. Testing in Progress.
 *
 *  @param[in] i2c_bus is the Bus connected to the socket
 *
 *  @param[in] i2c_addr is the 7-bit socket address
 *
 *  @param[in] cpu_ind a cpu index is a given core to set the boostlimit
 *
 *  @param[in] limit a uint32_t that indicates the desired Out-of-band
 *  boostlimit value of a given core
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t write_esb_boost_limit(uint32_t i2c_bus, uint32_t i2c_addr,
				   int cpu_ind, uint32_t limit);

/**
 *  @brief Set the boostlimit value for the whole socket (whole system).
 *
 *  @details This function will set the boostlimit to the provided value @p
 *  boostlimit for the socket.
 *
 *  @param[in] i2c_bus is the Bus connected to the socket
 *
 *  @param[in] i2c_addr is the 7-bit socket address
 *
 *  @param[in] limit a uint32_t that indicates the desired boostlimit
 *  value of the socket
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t write_esb_boost_limit_allcores(uint32_t i2c_bus,
					    uint32_t i2c_addr, uint32_t limit);

/** @} */  // end of PerfCont

/*****************************************************************************/

/** @defgroup TdpQuer Current, Min, Max TDP
 *  Below functions provide interfaces to get the current, Min and Max TDP,
 *  Prochot and Prochot Residency for a given socket.
 *  @{
 */

/**
 *  @brief Get the Thermal Design Power limit TDP of the socket with provided
 *  socket index.
 *
 *  @details Given a socket and a pointer to a uint32_t @p buffer, this function
 *  will get the current TDP (in milliwatts)
 *
 *  @param[in] i2c_bus is the Bus connected to the socket
 *
 *  @param[in] i2c_addr is the 7-bit socket address
 *
 *  @param[inout] buffer a pointer to uint32_t to which the Current TDP value
 *  will be copied
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t read_tdp(uint32_t i2c_bus, uint32_t i2c_addr, uint32_t *buffer);

/**
 *  @brief Get the Maximum Thermal Design Power limit TDP of the socket with
 *  provided socket index.
 *
 *  @details Given a socket and a pointer, this function will get the Maximum
 *  TDP (watts)
 *
 *  @param[in] i2c_bus is the Bus connected to the socket
 *
 *  @param[in] i2c_addr is the 7-bit socket address
 *
 *  @param[inout] buffer a pointer to uint32_t to which the Maximum TDP value
 *  will be copied
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t read_max_tdp(uint32_t i2c_bus, uint32_t i2c_addr,
			  uint32_t *buffer);

/**
 *  @brief Get the Minimum Thermal Design Power limit TDP of the socket
 *
 *  @details Given a socket and a pointer to a uint32_t, this function will
 *  get the Minimum  TDP (watts)
 *
 *  @param[in] i2c_bus is the Bus connected to the socket
 *
 *  @param[in] i2c_addr is the 7-bit socket address
 *
 *  @param[inout] buffer a pointer to uint32_t to which the Minimum TDP value
 *  will be copied
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t read_min_tdp(uint32_t i2c_bus, uint32_t i2c_addr,
			  uint32_t *buffer);

/** @} */  // end of TdpQuer

/*****************************************************************************/

/** @defgroup ProchotQuer Prochot
 *  Below functions provide interfaces to get Prochot and Prochot Residency
 *  for a given socket.
 *  @{
 */
/**
 *  @brief Get the Prochot Status of the socket with provided socket index.
 *
 *  @details Given a socket and a pointer to a uint32_t,
 *  this function will get the Prochot status as active/1 or
 *  inactive/0
 *
 *  @param[in] i2c_bus is the Bus connected to the socket
 *
 *  @param[in] i2c_addr is the 7-bit socket address
 *
 *  @param[inout] buffer a pointer to uint32_t to which the Prochot status
 *  will be copied
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t read_prochot_status(uint32_t i2c_bus, uint32_t i2c_addr,
				 uint32_t *buffer);

/**
 *  @brief Get the Prochot Residency (since the boot time or last
 *  read of Prochot Residency) of the socket.
 *
 *  @details Given a socket and a pointer to a uint32_t,
 *  this function will get the Prochot residency as a percentage
 *
 *  @param[in] i2c_bus is the Bus connected to the socket
 *
 *  @param[in] i2c_addr is the 7-bit socket address
 *
 *  @param[inout] buffer a pointer to uint32_t to which the Prochot residency
 *  will be copied
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t read_prochot_residency(uint32_t i2c_bus, uint32_t i2c_addr,
				    uint32_t *buffer);

/** @} */  // end of ProchotQuer

/****************************************************************************/
/** @defgroup dramQuer Dram and other features Query
 *  @{
*/

/**
 *  @brief Read Dram Throttle will always read the lowest percentage value.
 *
 *  @details This function will read dram throttle.
 *
 *  @param[in] i2c_bus is the Bus connected to the socket
 *
 *  @param[in] i2c_addr is the 7-bit socket address
 *
 *  @param[out] buffer is to read the dram throttle in % (0 - 100).
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t read_dram_throttle(uint32_t i2c_bus, uint32_t i2c_addr,
				uint32_t *buffer);

/**
 *  @brief Set Dram Throttle value in terms of  percentage.
 *
 *  @details This function will set the dram throttle of the provided value
 *  limit for the given socket.
 *
 *  @param[in] i2c_bus is the Bus connected to the socket
 *
 *  @param[in] i2c_addr is the 7-bit socket address
 *
 *  @param[in] limit that indicates the desired limit as per SSP PPR write can be
 *  between 0 to 80% to for a given socket
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t write_dram_throttle(uint32_t i2c_bus, uint32_t i2c_addr,
				 uint32_t limit);

/**
 *  @brief Read VDDIOMem Power returns the estimated VDDIOMem power consumed
 *  within the socket.
 *
 *  @details This function will read VDDIOMem Power for the given socket
 *
 *  @param[in] i2c_bus is the Bus connected to the socket
 *
 *  @param[in] i2c_addr is the 7-bit socket address
 *
 *  @param[out] buffer is to read VDDIOMem Power.
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t read_vddio_mem_power(uint32_t i2c_bus, uint32_t i2c_addr,
				  uint32_t *buffer);

/**
 *  @brief Read NBIO Error Logging Register
 *
 *  @details Given a socket, quadrant and register offset as @p input,
 *  this function will read NBIOErrorLoggingRegister.
 *
 *  @param[in] i2c_bus is the Bus connected to the socket
 *
 *  @param[in] i2c_addr is the 7-bit socket address
 *
 *  @param[in] quadrant value is Quadrant[31:24] from NBIO register
 *
 *  @param[in] offset value is register offset[23:0] from NBIO register
 *
 *  @param[out] buffer is to read NBIOErrorLoggingRegiter(register value).
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t
read_nbio_error_logging_register(uint32_t i2c_bus, uint32_t i2c_addr,
				 uint8_t quadrant, uint32_t offset,
				 uint32_t *buffer);

/**
 *  @brief Read IOD Bist status.
 *
 *  @details This function will read IOD Bist result for the given socket.
 *
 *  @param[in] i2c_bus is the Bus connected to the socket
 *
 *  @param[in] i2c_addr is the 7-bit socket address
 *
 *  @param[out] buffer is to read IODBistResult (0=Bist pass, 1= Bist fail).
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t read_iod_bist(uint32_t i2c_bus, uint32_t i2c_addr,
			   uint32_t *buffer);

/**
 *  @brief Read CCD Bist status. Results are read for each CCD present in the
 *  system.
 *
 *  @details Given a socket bus number and address, Logical CCD instance
 *  number as @p input, this function will read CCDBistResult.
 *
 *  @param[in] i2c_bus is the Bus connected to the socket
 *
 *  @param[in] i2c_addr is the 7-bit socket address
 *
 *  @param[in] input is a Logical CCD instance number.
 *
 *  @param[out] buffer is to read CCDBistResult (0 = Bist pass, 1 = Bist fail)
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t read_ccd_bist_result(uint32_t i2c_bus, uint32_t i2c_addr,
				  uint32_t input, uint32_t *buffer);

/**
 *  @brief Read CPU Core Complex Bist result. results are read for each Logical
 *  CCX instance number and returns a value which is the concatenation of L3
 *  pass status and all cores in the complex(n:0).
 *
 *  @details Given a socket bus number, address, Logical CCX instance number 
 *  as @p input, this function will read CCXBistResult.
 *
 *  @param[in] i2c_bus is the Bus connected to the socket
 *
 *  @param[in] i2c_addr is the 7-bit socket address
 *
 *  @param[in] value is a Logical CCX instance number.
 *
 *  @param[out] buffer is to read CCXBistResult (L3pass, Core[n:0]Pass)
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t read_ccx_bist_result(uint32_t i2c_bus, uint32_t i2c_addr,
				  uint32_t value, uint32_t *buffer);

/**
 *  @brief Provides the socket's CPU core clock (CCLK) frequency limit from
 *  the most restrictive infrastructure limit at the time of the request.
 *
 *  @details This function will read Frequency for the given socket
 *
 *  @param[in] i2c_bus is the Bus connected to the socket
 *
 *  @param[in] i2c_addr is the 7-bit socket address
 *
 *  @param[out] buffer is to read freequency[MHz]
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t read_cclk_freq_limit(uint32_t i2c_bus, uint32_t i2c_addr,
				  uint32_t *buffer);

/**
 *  @brief Provides the average C0 residency across all cores in the socket.
 *  100% specifies that all enabled cores in the socket are runningin C0.
 *
 *  @details This function will read Socket C0 residency[%] for the given socket.
 *
 *  @param[in] i2c_bus is the Bus connected to the socket
 *
 *  @param[in] i2c_addr is the 7-bit socket address
 *
 *  @param[out] buffer is to read Socket C0 residency[%].
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t read_socket_c0_residency(uint32_t i2c_bus, uint32_t i2c_addr,
				      uint32_t *buffer);

/**
 *  @brief Get the Theoretical maximum DDR Bandwidth of the system in GB/s,
 *  Current utilized DDR Bandwidth (Read + Write) in GB/s and
 *  Current utilized DDR Bandwidth as a percentage of theoretical maximum.
 *
 *  @param[in] i2c_bus is the Bus connected to the socket
 *
 *  @param[in] i2c_addr is the 7-bit socket address
 *
 *  @param[out] max_bw is the maxium DDR Bandwidth in GB/s
 *
 *  @param[out] utilized_bw is the utilized DDR Bandwidth in GB/s
 *
 *  @param[out] utilized_pct is the utilized DDR Bandwidth in %.
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t read_ddr_bandwidth(uint32_t i2c_bus,
				uint32_t i2c_addr,
				uint32_t *max_bw,
				uint32_t *utilized_bw,
				uint32_t *utilized_pct);

/** @} */

/** @} */  // end of MailboxMsg
/****************************************************************************/

#endif  // INCLUDE_ESMI_OOB_MAILBOX_H_
