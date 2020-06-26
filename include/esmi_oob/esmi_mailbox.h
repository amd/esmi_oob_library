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

/** \file mailbox.h
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
	READ_PACKAGE_C0_RESIDENCY
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
 *  @brief Get the average power consumption of the socket with provided
 *  socket index.
 *
 *  @details Given a socket index @p socket_ind and a pointer to a uint32t
 *  @p ppower, this function will get the current average power consumption
 *  (in milliwatts) to the uint32t pointed to by @p ppower.
 *
 *  @param[in] socket_ind a socket index
 *
 *  @param[inout] ppower a pointer to uint32t to which the average power
 *  consumption will get
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t read_socket_power(int socket_ind, uint32_t *ppower);

/**
 *  @brief Get the current power cap/limit value for a given socket.
 *
 *  @details This function will return the valid power cap @p pcap for a given
 *  socket @ socket_ind, this value will be used for the system to limit
 *  the power.
 *
 *  @param[in] socket_ind a socket index
 *
 *  @param[inout] pcap a pointer to a uint32t that indicates the valid
 *  possible power cap/limit, in milliwatts
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t read_socket_power_limit(int socket_ind, uint32_t *pcap);

/**
 *  @brief Get the maximum value that can be assigned as a power cap/limit for
 *         a given socket.
 *
 *  @details This function will return the maximum possible valid
 *  	     power cap/limit
 *  @p pmax from a @p socket_ind.
 *
 *  @param[in] socket_ind a socket index
 *
 *  @param[inout] pmax a pointer to a uint32t that indicates the maximum
 *                possible power cap/limit, in milliwatts
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t read_max_socket_power_limit(int socket_ind, uint32_t *pmax);
/** @} */  // end of PowerQuer
/*****************************************************************************/

/** @defgroup PowerCont Power Control
 *  This function provides a way to control Power Limit.
 *  @{
 */

/**
 *  @brief Set the power cap/limit value for a given socket.
 *
 *  @details This function will set the power cap/limit to the provided
 *  value @p cap.
 *
 *  @param[in] socket_ind a socket index
 *
 *  @param[in] limit uint32t that indicates the desired power cap/limit, in
 *  milliwatts
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t write_socket_power_limit(int socket_ind, uint32_t limit);

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
 *  @p pboostlimit for a particular @p cpu_ind
 *
 *  @param[in] socket_ind a socket index
 *
 *  @param[in] cpu_ind a cpu index
 *
 *  @param[inout] pboostlimit pointer to a uint32t that indicates the
 *  possible boost
 *  limit value
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t read_esb_boost_limit(int socket_ind, uint32_t cpu_ind, uint32_t *pboostlimit);

/**
 *  @brief Get the In-band maximum boostlimit value for a given core
 *
 *  @details This function will return the core's current maximum In-band
 *  boost limit @p buffer for a particular @p value is cpu_ind
 *
 *  @param[in] socket_ind a socket index
 *
 *  @param[in] value is a cpu index
 *
 *  @param[inout] buffer a pointer to a uint32t that indicates the
 *  maximum boost limit value set via In-band
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t read_bios_boost_fmax(int socket_ind, uint32_t value,
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
 *  limit for a given cpu via Out-of-band.
 *
 *  @param[in] socket_ind a socket index
 *
 *  @param[in] cpu_id a cpu index is a given core to set the boostlimit
 *
 *  @param[in] limit a uint32t that indicates the desired Out-of-band
 *  boostlimit value of a given core
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t write_esb_boost_limit(int socket_ind, int cpu_id, uint32_t limit);

/**
 *  @brief Set the boostlimit value for the whole socket (whole system).
 *
 *  @details This function will set the boostlimit to the provided value @p
 *  boostlimit for the whole socket.
 *
 *  @param[in] socket_ind for detecting i2c address
 *
 *  @param[in] limit a uint32t that indicates the desired boostlimit
 *  value of the socket
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t write_esb_boost_limit_allcores(int socket_ind, uint32_t limit);

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
 *  @details Given a socket index @p socket_ind and a pointer to a uint32_t
 *  @p ptdp, this function will get the current TDP (in milliwatts) to
 *  the uint32_t pointed to by @p ptdp.
 *
 *  @param[in] socket_ind a socket index
 *
 *  @param[inout] ptdp a pointer to uint32_t to which the Current TDP value
 *  will be copied
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t read_tdp(int socket_ind, uint32_t *ptdp);

/**
 *  @brief Get the Maximum Thermal Design Power limit TDP of the socket with
 *  provided socket index.
 *
 *  @details Given a socket index @p socket_ind and a pointer to a uint32_t
 *  @p ptdp, this function will get the Maximum TDP (in milliwatts) to
 *  the uint32_t pointed to by @p ptdp.
 *
 *  @param[in] socket_ind a socket index
 *
 *  @param[inout] ptdp a pointer to uint32_t to which the Maximum TDP value
 *  will be copied
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t read_max_tdp(int socket_ind, uint32_t *ptdp);

/**
 *  @brief Get the Minimum Thermal Design Power limit TDP of the socket with
 *  provided socket index.
 *
 *  @details Given a socket index @p socket_ind and a pointer to a uint32_t
 *  @p ptdp, this function will get the Minimum  TDP (in milliwatts) to
 *  the uint32_t pointed to by @p ptdp.
 *
 *  @param[in] socket_ind a socket index
 *
 *  @param[inout] ptdp a pointer to uint32_t to which the Minimum TDP value
 *  will be copied
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t read_min_tdp(int socket_ind, uint32_t *ptdp);

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
 *  @details Given a socket index @p socket_ind and a pointer to a uint32_t
 *  @p pstatus, this function will get the Prochot status as active/1 or
 *  inactive/0 to the bool pointed to by @p pstatus.
 *
 *  @param[in] socket_ind a socket index
 *
 *  @param[inout] pstatus a pointer to uint32_t to which the Prochot status
 *  will be copied
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t read_prochot_status(int socket_ind, uint32_t *pstatus);

/**
 *  @brief Get the Prochot Residency (since the boot time or last
 *  read of Prochot Residency) of the socket with provided socket index.
 *
 *  @details Given a socket index @p socket_ind and a pointer to a uint32_t
 *  @p presi, this function will get the Prochot residency as a percentage
 *  pointed to by @p presi.
 *
 *  @param[in] socket_ind a socket index
 *
 *  @param[inout] presi a pointer to uint32_t to which the Prochot residency
 *  will be copied
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t read_prochot_residency(int socket_ind, uint32_t *presi);

/** @} */  // end of ProchotQuer

/****************************************************************************/
/** @defgroup dramQuer Dram and other features Query
 *  @{
*/

/**
 *  @brief Read Dram Throttle will always read the lowest percentage value.
 *
 *  @details Given a @p socket_ind, this function will read dram throttle.
 *
 *  @param[in] socket_ind is a particular package in the system.
 *
 *  @param[out] buffer is to read the dram throttle in % (0 - 100).
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t read_dram_throttle(int socket_ind, uint32_t *buffer);

/**
 *  @brief Set Dram Throttle value in terms of  percentage.
 *
 *  @details This function will set the dram throttle of the provided value
 *  limit for the given socket.
 *
 *  @param[in] socket_ind is a given socket
 *
 *  @param[in] limit a uint32t that indicates the desired limit to write
 *  for a given socket
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t write_dram_throttle(int socket_ind, uint32_t limit);

/**
 *  @brief Read VDDIOMem Power returns the estimated VDDIOMem power consumed
 *  within the socket.
 *
 *  @details Given a @p socket_ind, this function will read VDDIOMem Power.
 *
 *  @param[in] socket_ind is a particular package in the system.
 *
 *  @param[out] buffer is to read VDDIOMem Power.
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t read_vddio_mem_power(int socket_ind, uint32_t *buffer);

/**
 *  @brief Read NBIO Error Logging Register
 *
 *  @details Given a @p socket_ind, quadrant and register offset as @p input,
 *  this function will read NBIOErrorLoggingRegister.
 *
 *  @param[in] socket_ind is a particular package in the system.
 *
 *  @param[in] input value is Quadrant[31:24] and register offset[23:0]
 *
 *  @param[out] buffer is to read NBIOErrorLoggingRegiter(register value).
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t read_nbio_error_logging_register(int socket_ind, uint8_t quadrant,
					      uint32_t offset,
					      uint32_t *buffer);

/**
 *  @brief Read IOD Bist status.
 *
 *  @details Given a @p socket_ind, this function will read IOD Bist result.
 *
 *  @param[in] socket_ind is a particular package in the system.
 *
 *  @param[out] buffer is to read IODBistResult (0=Bist pass, 1= Bist fail).
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t read_iod_bist(int socket_ind, uint32_t *buffer);

/**
 *  @brief Read CCD Bist status. Results are read for each CCD present in the
 *  system.
 *
 *  @details Given a @p socket_ind, Logical CCD instance number as @p input,
 *  this function will read CCDBistResult.
 *
 *  @param[in] socket_ind is a particular package in the system.
 *
 *  @param[in] input is a Logical CCD instance number.
 *
 *  @param[out] buffer is to read CCDBistResult (0 = Bist pass, 1 = Bist fail)
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t read_ccd_bist_result(int socket_ind, uint32_t input,
				  uint32_t *buffer);
/**
 *  @brief Read CPU Core Complex Bist result. results are read for each Logical
 *  CCX instance number and returns a value which is the concatenation of L3
 *  pass status and all cores in the complex(n:0).
 *
 *  @details Given a @p socket_ind, Logical CCX instance number as @p input,
 *  this function will read CCXBistResult.
 *
 *  @param[in] socket_ind is a particular package in the system.
 *
 *  @param[in] input is a Logical CCX instance number.
 *
 *  @param[out] buffer is to read CCXBistResult (L3pass, Core[n:0]Pass)
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */

oob_status_t read_ccx_bist_result(int socket_ind, uint32_t input,
				  uint32_t *buffer);

/**
 *  @brief Provides the socket's CPU core clock (CCLK) frequency limit from
 *  the most restrictive infrastructure limit at the time of the request.
 *
 *  @details Given a @p socket_ind, this function will read Frequency.
 *
 *  @param[in] socket_ind is a particular package in the system.
 *
 *  @param[out] buffer is to read freequency[MHz]
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t read_cclk_freq_limit(int socket_ind, uint32_t *buffer);

/**
 *  @brief Provides the average C0 residency across all cores in the socket.
 *  100% specifies that all enabled cores in the socket are runningin C0.
 *
 *  @details Given a @p socket_ind, this function will read Socket C0
 *  residency[%].
 *
 *  @param[in] socket_ind is a particular package in the system.
 *
 *  @param[out] buffer is to read Socket C0 residency[%].
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t read_socket_c0_residency(int socket_ind, uint32_t *buffer);
/** @} */

/** @} */  // end of MailboxMsg
/****************************************************************************/

#endif  // INCLUDE_ESMI_OOB_MAILBOX_H_
