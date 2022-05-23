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
#ifndef INCLUDE_APML_MAILBOX_H_
#define INCLUDE_APML_MAILBOX_H_

#include "apml_err.h"
#include "stdbool.h"

#define BIT(N) (1 << N)		//!< Perform left shift operation by N bits //
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0])) //!< Returns the array size //

/** \file esmi_mailbox.h
 *  Header file for the Mailbox messages supported by APML library.
 *  All required function, structure, enum, etc. definitions should be defined
 *  in this file.
 *
 *  @details  This header file contains the following:
 *  APIs prototype of the Mailbox messages exported by the APML library.
 *  Description of the API, arguments and return values.
 *  The Error codes returned by the API.
 */

/**
 * @brief Mailbox message types defined in the APML library
 */
typedef enum {
	READ_PACKAGE_POWER_CONSUMPTION  = 0x1,
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
	READ_NBIO_ERROR_LOGGING_REGISTER = 0x11,
	READ_IOD_BIST = 0x13,
	READ_CCD_BIST_RESULT,
	READ_CCX_BIST_RESULT,
	READ_DDR_BANDWIDTH = 0x18,
	WRITE_BMC_REPORT_DIMM_POWER = 0X40,
	WRITE_BMC_REPORT_DIMM_THERMAL_SENSOR,
	READ_BMC_RAS_PCIE_CONFIG_ACCESS,
	READ_BMC_RAS_MCA_VALIDITY_CHECK,
	READ_BMC_RAS_MCA_MSR_DUMP,
	READ_BMC_RAS_FCH_RESET_REASON,
	READ_DIMM_TEMP_RANGE_AND_REFRESH_RATE,
	READ_DIMM_POWER_CONSUMPTION,
	READ_DIMM_THERMAL_SENSOR,
	READ_PWR_CURRENT_ACTIVE_FREQ_LIMIT_SOCKET,
	READ_PWR_CURRENT_ACTIVE_FREQ_LIMIT_CORE,
	READ_PWR_SVI_TELEMETRY_ALL_RAILS,
	READ_SOCKET_FREQ_RANGE,
	READ_CURRENT_IO_BANDWIDTH,
	READ_CURRENT_XGMI_BANDWIDTH,
	WRITE_GMI3_LINK_WIDTH_RANGE,
	WRITE_XGMI_LINK_WIDTH_RANGE,
	WRITE_APB_DISABLE,
	WRITE_APB_ENABLE,
	READ_CURRENT_DFPSTATE_FREQUENCY,
	WRITE_LCLK_DPM_LEVEL_RANGE,
	READ_BMC_RAPL_UNITS,
	READ_BMC_RAPL_CORE_LO_COUNTER,
	READ_BMC_RAPL_CORE_HI_COUNTER,
	READ_BMC_RAPL_PKG_COUNTER,
	READ_BMC_CPU_BASE_FREQUENCY,
	READ_BMC_CONTROL_PCIE_GEN5_RATE,
	READ_RAS_LAST_TRANSACTION_ADDRESS = 0X5C,
	WRITE_PWR_EFFICIENCY_MODE,
	WRITE_DF_PSTATE_RANGE,
	READ_LCLK_DPM_LEVEL_RANGE
} esb_mailbox_commmands;

/**
 * @brief APML IO Bandwidth Encoding defined in the APML library
 */
typedef enum {
	AGG_BW = BIT(0),
	RD_BW = BIT(1),
	WR_BW = BIT(2)
} apml_io_bw_encoding;

/**
 * @brief APML IO LINK ID Encoding defined in the APML library
 */
typedef enum {
	P0 = BIT(0),
	P1 = BIT(1),
	P2 = BIT(2),
	P3 = BIT(3),
	G0 = BIT(4),
	G1 = BIT(5),
	G2 = BIT(6),
	G3 = BIT(7)
} apml_link_id_encoding;

/**
 * @brief DIMM power(mW), update rate(ms) and dimm address
 */
struct dimm_power {
	uint16_t power : 15;		//!< Dimm power consumption
	uint16_t update_rate : 9;	//!< update rate in ms
	uint8_t dimm_addr;		//!< Dimm address
};

/**
 * @brief DIMM thermal sensor (degree C), update rate
 * and dimm address
 */
struct dimm_thermal {
	uint16_t sensor : 11;		//!< Dimm thermal sensor
	uint16_t update_rate : 9;	//!< update rate in ms
	uint8_t dimm_addr;		//!< Dimm address
};

/**
 * @brief DIMM temperature range and refresh rate, temperature update flag
 */
struct temp_refresh_rate {
	uint8_t range : 3;		//!< temp refresh rate (3 bit data)
	uint8_t ref_rate : 1;		//!< temp update flag (1 bit data)
};

/**
 * @brief PCI address information .PCI address includes 4 bit segment,
 * 12 bit aligned offset, 8 bit bus, 5 bit device info and 3 bit function
 */
struct pci_address {
	uint8_t func : 3;	//!< function (3 bit data)
	uint8_t device : 5;	//!< device info (5 bit data)
	uint8_t bus;		//!< bus (8 bit data)
	uint16_t offset : 12;	//!< offset address (12 bit data)
	uint8_t segment : 4;	//!< segment (4 bit data)
};

/**
 * @brief Max and min LCK DPM level on a given NBIO ID.
 * Valid Max and min DPM level values are 0 - 1.
 */
struct dpm_level {
	uint8_t max_dpm_level;		//!< Max LCLK DPM level [0 - 1]
	uint8_t min_dpm_level;		//!< Min LCLK DPM level [ 0 - 1]
};

/**
 * @brief Max and Min Link frequency clock (LCLK) DPM level on a socket.
 * 8 bit NBIO ID, dpm_level struct containing 8 bit max DPM level,
 * 8 bit min DPM level
 */
struct lclk_dpm_level_range {
	uint8_t nbio_id;		//!< NBIOD id (8 bit data [0 - 3])
	struct dpm_level dpm;           //!< struct with max dpm, min dpm levels
};

/**
 * @brief NBIO quadrant(8 bit data) and NBIO register offset(24 bit) data.
 */
struct nbio_err_log {
	uint8_t quadrant;	//!<< NBIO quadrant data
	uint32_t offset : 24;	//!<< NBIO register offset (24 bit data)
};

/**
 * @brief Structure for Max DDR bandwidth and utilization.
 * It contains max bandwidth(12 bit data) in GBps, current utilization
 * bandwidth(12 bit data) in GBps, current utilized bandwidth( 8 bit data)
 * in percentage.
 */
struct max_ddr_bw {
	uint16_t max_bw : 12;		//!< Max Bandwidth (12 bit data)
	uint16_t utilized_bw : 12;	//!< Utilized Bandwidth  (12 bit data)
	uint8_t utilized_pct;		//!< Utliized Bandwidth percentage
};

/**
 * @brief MCA bank information.It contains 16 bit Index for MCA Bank
 * and 16 bit offset.
 */
struct mca_bank {
	uint16_t offset;	//!< Offset with in MCA Bank
	uint16_t index;		//!< Index of MCA Bank
};

/**
 * @brief APML LINK ID and Bandwidth type Information.It contains
 * APML LINK ID Encoding. Valid Link ID encodings are 1(P0), 2(P1),
 * 4(P2), 8(P3), 16(G0), 32(G1), 64(G2), 128(G3). Valid APML IO Bandwidth
 *  types 1(Aggregate_BW), 2 (Read BW), 4 (Write BW).
 */
struct link_id_bw_type {
	apml_io_bw_encoding bw_type;	//!< Bandwidth Type Information [1, 2, 4]
	apml_link_id_encoding link_id;	//!< Link ID [1,2,4,8,16,32,64,128]
};

/**
 * @brief DF P-state frequency.It includes mem clock(16 bit data) frequency
 * (DRAM memory clock), data fabric clock (12 bit data), UMC clock divider
 * (UMC) (1 bit data).
 */
struct pstate_freq {
	uint16_t mem_clk;	//!< DRAM Memory clock Frequency (MHz)(12 bit)
	uint16_t fclk : 12;	//!< Data fabric clock (MHz)(12 bit data)
	uint8_t uclk : 1;	//!< UMC clock divider (1 bit data)
};

/**
 * @brief frequency limit source names
 */
static char * const freqlimitsrcnames[] = {
	"cHTC-Active",
	"PROCHOT",
	"TDC limit",
	"PPT Limit",
	"OPN Max",
	"Reliability Limit",
	"APML Agent",
	"HSMP Agent"
};

/**
 * @brief energy status multiplier
 * value is 1/2^ESU where ESU is
 * [12:8] bit of the mailbox command 0x55h.
 */
extern float esu_multiplier;

/*****************************************************************************/

/** @defgroup MailboxMsg SB-RMI Mailbox Service
 *  write, 'write and read' operations for a given socket.
 *  @{
 */
/** @defgroup PowerQuer Power Monitor
 *  Below functions provide interfaces to get the current power usage and
 *  Power Limits for a given socket.
 *  @{
 */

/**
 *  @brief Get the power consumption of the socket.
 *
 *  @details Given socket number and a pointer to a uint32_t
 *  @p buffer, this function will get the current power consumption
 *  (in watts) to the uint32_t pointed to by @p buffer.
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[inout] buffer a pointer to uint32_t value of power
 *  consumption
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t read_socket_power(uint8_t soc_num, uint32_t *buffer);

/**
 *  @brief Get the current power cap/limit value for a given socket.
 *
 *  @details This function will return the valid power cap @p buffer for a given
 *  socket, this value will be used for the system to limit the power.
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[inout] buffer a pointer to a uint32_t that indicates the valid
 *  possible power cap/limit, in watts
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t read_socket_power_limit(uint8_t soc_num, uint32_t *buffer);

/**
 *  @brief Get the maximum value that can be assigned as a power cap/limit for
 *         a given socket.
 *
 *  @details This function will return the maximum possible valid power cap/limit
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[out] buffer a pointer to a uint32_t that indicates the maximum
 *                possible power cap/limit, in watts
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t read_max_socket_power_limit(uint8_t soc_num, uint32_t *buffer);

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
 *  @param[in] soc_num Socket index.
 *
 *  @param[in] limit uint32_t that indicates the desired power cap/limit,
 *  in milliwatts
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t write_socket_power_limit(uint8_t soc_num, uint32_t limit);

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
 *  @param[in] soc_num Socket index.
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
oob_status_t read_esb_boost_limit(uint8_t soc_num, uint32_t value,
				  uint32_t *buffer);

/**
 *  @brief Get the In-band maximum boostlimit value for a given core
 *
 *  @details This function will return the core's current maximum In-band
 *  boost limit @p buffer for a particular @p value is cpu_ind
 *
 *  @param[in] soc_num Socket index.
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
oob_status_t read_bios_boost_fmax(uint8_t soc_num,
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
 *  @param[in] soc_num Socket index.
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
oob_status_t write_esb_boost_limit(uint8_t soc_num,
				   uint32_t cpu_ind, uint32_t limit);

/**
 *  @brief Set the boostlimit value for the whole socket (whole system).
 *
 *  @details This function will set the boostlimit to the provided value @p
 *  boostlimit for the socket.
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[in] limit a uint32_t that indicates the desired boostlimit
 *  value of the socket
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t write_esb_boost_limit_allcores(uint8_t soc_num, uint32_t limit);

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
 *  @param[in] soc_num Socket index.
 *
 *  @param[inout] buffer a pointer to uint32_t to which the Current TDP value
 *  will be copied
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t read_tdp(uint8_t soc_num, uint32_t *buffer);

/**
 *  @brief Get the Maximum Thermal Design Power limit TDP of the socket with
 *  provided socket index.
 *
 *  @details Given a socket and a pointer, this function will get the Maximum
 *  TDP (watts)
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[inout] buffer a pointer to uint32_t to which the Maximum TDP value
 *  will be copied
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t read_max_tdp(uint8_t soc_num, uint32_t *buffer);

/**
 *  @brief Get the Minimum Thermal Design Power limit TDP of the socket
 *
 *  @details Given a socket and a pointer to a uint32_t, this function will
 *  get the Minimum  TDP (watts)
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[inout] buffer a pointer to uint32_t to which the Minimum TDP value
 *  will be copied
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t read_min_tdp(uint8_t soc_num, uint32_t *buffer);

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
 *  @param[in] soc_num Socket index.
 *
 *  @param[inout] buffer a pointer to uint32_t to which the Prochot status
 *  will be copied
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t read_prochot_status(uint8_t soc_num, uint32_t *buffer);

/**
 *  @brief Get the Prochot Residency (since the boot time or last
 *  read of Prochot Residency) of the socket.
 *
 *  @details Given a socket and a pointer to a uint32_t,
 *  this function will get the Prochot residency as a percentage
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[inout] buffer a pointer to float to which the Prochot residency
 *  will be copied
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t read_prochot_residency(uint8_t soc_num, float *buffer);

/** @} */  // end of ProchotQuer

/****************************************************************************/
/** @defgroup dramQuer Dram and other features Query
 *  @{
 */

/**
 *  @brief Read Dram Throttle will always read the highest percentage value.
 *
 *  @details This function will always read the highest percentage value
 *  as represented by PROCHOT throttle or write dram throttle.
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[out] buffer is to read the dram throttle in % (0 - 100).
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t read_dram_throttle(uint8_t soc_num, uint32_t *buffer);

/**
 *  @brief Set Dram Throttle value in terms of  percentage.
 *
 *  @details This function will set the dram throttle of the provided value
 *  limit for the given socket.
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[in] limit that indicates the desired limit as per SSP PPR write can be
 *  between 0 to 80% to for a given socket
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t write_dram_throttle(uint8_t soc_num, uint32_t limit);

/**
 *  @brief Read NBIO Error Logging Register
 *
 *  @details Given a socket, quadrant and register offset as @p input,
 *  this function will read NBIOErrorLoggingRegister.
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[in] nbio nbio_err_log Struct containing nbio quadrant and offset.
 *
 *  @param[out] buffer is to read NBIOErrorLoggingRegiter(register value).
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t
read_nbio_error_logging_register(uint8_t soc_num,
				 struct nbio_err_log nbio,
				 uint32_t *buffer);

/**
 *  @brief Read IOD Bist status.
 *
 *  @details This function will read IOD Bist result for the given socket.
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[out] buffer is to read IODBistResult
 *  0 = Bist pass,
 *  1 = Bist fail
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t read_iod_bist(uint8_t soc_num, uint32_t *buffer);

/**
 *  @brief Read CCD Bist status. Results are read for each CCD present in the
 *  system.
 *
 *  @details Given a socket bus number and address, Logical CCD instance
 *  number as @p input, this function will read CCDBistResult.
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[in] input is a Logical CCD instance number.
 *
 *  @param[out] buffer is to read CCDBistResult
 *  0 = Bist pass,
 *  1 = Bist fail
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t read_ccd_bist_result(uint8_t soc_num, uint32_t input,
				  uint32_t *buffer);

/**
 *  @brief Read CPU Core Complex Bist result. results are read for each Logical
 *  CCX instance number and returns a value which is the concatenation of L3
 *  pass status and all cores in the complex(n:0).
 *
 *  @details Given a socket bus number, address, Logical CCX instance number
 *  as @p input, this function will read CCXBistResult.
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[in] value is a Logical CCX instance number.
 *
 *  @param[out] buffer is to read CCXBistResult (L3pass, Core[n:0]Pass)
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t read_ccx_bist_result(uint8_t soc_num, uint32_t value,
				  uint32_t *buffer);

/**
 *  @brief Get the Theoretical maximum DDR Bandwidth of the system in GB/s,
 *  Current utilized DDR Bandwidth (Read + Write) in GB/s and
 *  Current utilized DDR Bandwidth as a percentage of theoretical maximum.
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[out] max_ddr max_ddr_bw struct containing max bandwidth,
 *  utilized bandwidth and utilized bandwidth percentage.
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t read_ddr_bandwidth(uint8_t soc_num,
				struct max_ddr_bw *max_ddr);
/** @} */

/** @} */

/**
 *  @brief Set DIMM Power consumption in mwatts.
 *
 *  @details This function will set DIMM Power consumption periodically
 *  by BMC at specified update rate (10 ms or less) when bmc owns the
 *  SPD side-band bus.
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[in] dp_info dimm_power Struct with power(mw), updaterate(ms) & dimm address
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t write_bmc_report_dimm_power(uint8_t soc_num,
					 struct dimm_power dp_info);

/**
 *  @brief Set DIMM thermal Sensor in degree Celcius.
 *
 *  @details This function will set DIMM thermal sensor (in degree celcius)
 *  periodically by BMC at specified update rate (10 ms or less) when bmc owns
 *  the SPD side-band bus.
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[in] dt_info struct with temp(ºC), updaterate(ms) & dimm address
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t write_bmc_report_dimm_thermal_sensor(uint8_t soc_num,
						  struct dimm_thermal dt_info);

/**
 *  @brief Read BMC RAS PCIE config access.
 *
 *  @details This function will read the 32 bit BMC RAS
 *  extended PCI config space.
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[in] pci_addr pci_address structure with fucntion(3 bit),
 *   device(4 bit) bus(8 bit), offset(12 bit), segment(4 bit).
 *   SEGMENT:0 BUS 0:DEVICE 18 and SEGMENT:0 BUS 0:DEVICE 19 are inaccessable.
 *
 *  @param[out] out_buf 32 bit data from offset in PCI config space.
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t read_bmc_ras_pcie_config_access(uint8_t soc_num,
					     struct pci_address pci_addr,
					     uint32_t *out_buf);

/**
 *  @brief Read number of MCA banks with valid status after a fatal error.
 *
 *  @details This function returns the number of MCA banks with valid status
 *  after a fatal error.
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[out] bytes_per_mca returns bytes per mca.
 *
 *  @param[out] mca_banks number of mca banks.
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t read_bmc_ras_mca_validity_check(uint8_t soc_num,
					     uint16_t *bytes_per_mca,
					     uint16_t *mca_banks);

/**
 *  @brief Read data from mca bank reported by bmc ras mca validity check.
 *
 *  @details This function returns the data from mca bank reported by bmc
 *  ras mca validity check.
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[in] mca_dump mca_bank Struct containing offset, index of MCA bank.
 *
 *  @param[out] out_buf 32 bit data from offset in mca bank.
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t read_bmc_ras_mca_msr_dump(uint8_t soc_num,
				       struct mca_bank mca_dump,
				       uint32_t *out_buf);

/**
 *  @brief Read FCH reason code from the previous reset.
 *
 *  @details This function reads the FCH reason code from the previous
 *  reset.
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[in] input integer for id of FCH register.
 *
 *  @param[out] out_buf Data from FCH register.
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t read_bmc_ras_fch_reset_reason(uint8_t soc_num,
					   uint32_t input,
					   uint32_t *out_buf);

/**
 *  @brief Read DIMM temperature range and refresh rate.
 *
 *  @details This function returns the per DIMM temperature range and
 *  refresh rate from the MR4 register, per JEDEC spec.
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[in] dimm_addr Encoded address of the dimm.
 *
 *  @param[out] rate temp_refresh_rate structure with refresh rate(1 bit)
 *  and range(3 bit). 
 *  refresh rate: 0 = 1X, 1 = 2X.
 *  Temperature range: 001b = 1X, 101b = 2X.
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t read_dimm_temp_range_and_refresh_rate(uint8_t soc_num,
						   uint32_t dimm_addr,
						   struct temp_refresh_rate *rate);

/**
 *  @brief Read DIMM power consumption.
 *
 *  @details This function returns the DIMM power consumption
 *  when bmc does not own the SPD side band bus.
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[in] dimm_addr Encoded address of the dimm.
 *
 *  @param[out] dimm_pow struct dimm_power contains
 *  updaterate(ms): Time since last update (0-511ms).
 *	0 means last update was < 1ms, and 511 means update was >= 511ms
 *  power consumption(mw): power consumed (0 - 32767 mW)
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t read_dimm_power_consumption(uint8_t soc_num,
					 uint32_t dimm_addr,
					 struct dimm_power *dimm_pow);

/**
 *  @brief Read DIMM thermal sensor.
 *
 *  @details This function returns the DIMM thermal sensor
 *  (2 sensors per DIMM) when bmc does not own the SPD side band bus.
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[in] dimm_addr Encoded address of the dimm.
 *
 *  @param[out] dimm_temp struct dimm_thermal struct contains
 *  updaterate(ms): Time since last update (0-511ms).
 *	0 means last update was < 1ms, and 511 means update was >= 511ms
 *  temperature (Degrees C): Temperature (-256 - 255.75 degree C)
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t read_dimm_thermal_sensor(uint8_t soc_num,
				      uint32_t dimm_addr,
				      struct dimm_thermal *dimm_temp);

/**
 *  @brief Read current active frequency limit per socket.
 *
 *  @details This function returns the current active frequency limit
 *  per socket.
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[out] freq Frequency (MHz).
 *
 *  @param[out] source_type Source of limit.
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t read_pwr_current_active_freq_limit_socket(uint8_t soc_num,
						       uint16_t *freq,
						       char **source_type);

/**
 *  @brief Read current active frequency limit set per core.
 *
 *  @details This function returns the current active frequency limit
 *  per core.
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[in] core_id index.
 *
 *  @param[out] base_freq Frequency (MHz).
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t read_pwr_current_active_freq_limit_core(uint8_t soc_num,
						     uint32_t core_id,
						     uint16_t *base_freq);

/**
 *  @brief Read SVR based telemtry for all rails.
 *
 *  @details This function returns the SVR based telemetry (power and update rate)
 *  for all rails.
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[out] power SVI-based Telemetry for all rails(mW)
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t read_pwr_svi_telemetry_all_rails(uint8_t soc_num,
					      uint32_t *power);

/**
 *  @brief Read socket frequency range.
 *
 *  @details This function returns the fmax and fmin frequency
 *  per socket.
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[out] fmax maximum frequency (MHz).
 *
 *  @param[out] fmin minimum frequency (MHz).
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t read_socket_freq_range(uint8_t soc_num,
				    uint16_t *fmax,
				    uint16_t *fmin);

/**
 *  @brief Read current bandwidth on IO Link.
 *
 *  @details This function returns the current IO bandwidth.
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[in] link link_id_bw_type struct containing bandwidth type and
 *  Link ID encoding
 *  bandwidth type:
 *	001b Aggregate BW
 *	Other Reserved
 *  APML Link ID Encoding:
 *	00000001b: P0
 *	00000010b: P1
 *	00000100b: P2
 *	00001000b: P3
 *	00010000b: G0
 *	00100000b: G1
 *	01000000b: G2
 *	10000000b: G3
 *
 *  @param[out] io_bw io bandwidth (Mbps).
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t read_current_io_bandwidth(uint8_t soc_num,
				       struct link_id_bw_type link,
				       uint32_t *io_bw);

/**
 *  @brief Read current bandwidth on xGMI Link.
 *
 *  @details This function returns the current xGMI bandwidth.
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[in] link link_id_bw_type struct containing link id and
 *  bandwidth type info.
 *  Valid BW type are
 *	001b Aggregate BW
 *	010b Read BW
 *	100b Write BW
 *	Other Reserved
 *  APML Link ID Encoding:
 *	00000001b: P0
 *	00000010b: P1
 *	00000100b: P2
 *	00001000b: P3
 *	00010000b: G0
 *	00100000b: G1
 *	01000000b: G2
 *	10000000b: G3
 *
 *  @param[out] xgmi_bw io bandwidth (Mbps).
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t read_current_xgmi_bandwidth(uint8_t soc_num,
					 struct link_id_bw_type link,
					 uint32_t *xgmi_bw);

/**
 *  @brief Set the max and min width of GMI3 link.
 *
 *  @details This function will set the max and min width of GMI3 Link.
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[in] min_link_width minimum link width.
 *  0 = Quarter width
 *  1 = Half width
 *  2 = full width
 *
 *  @param[in] max_link_width maximum link width.
 *  0 = Quarter width
 *  1 = Half width
 *  2 = full width
 *  NOTE: max value must be greater than or equal to min value.
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t write_gmi3_link_width_range(uint8_t soc_num,
					 uint8_t min_link_width,
					 uint8_t max_link_width);

/**
 *  @brief Set the max and min width of xGMI link.
 *
 *  @details This function will set the max and min width of xGMI Link.
 *  If this API is called from both the master and the slave sockets,
 *  then the largest width values from either calls are used.
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[in] min_link_width minimum link width.
 *  0 = X4
 *  1 = X8
 *  2 = X16
 *
 *  @param[in] max_link_width maximum link width.
 *  0 = X4
 *  1 = X8
 *  2 = X16
 *  NOTE: Max value must be greater than or equal to min value.
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t write_xgmi_link_width_range(uint8_t soc_num,
					 uint8_t min_link_width,
					 uint8_t max_link_width);

/**
 *  @brief Set the APBDisabled.
 *
 *  @details This function will set the APBDisabled by specifying the
 *  Data Fabric(DF) P-state. Messages APBEnable and APBDisable specify
 *  DF(Data Fabric) P-state behavior. DF P-states specify the frequency
 *  of clock domains from the CPU core boundary through to and including
 *  system memory, where 0 is the highest DF P-state and 2 is the lowest.
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[in] df_pstate data fabric p-state.
 *
 *  @param[out] prochot_asserted prochot asserted status.
 *  True  indicates asserted
 *  False indicates not-asserted.
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t write_apb_disable(uint8_t soc_num,
			       uint8_t df_pstate,
			       bool *prochot_asserted);

/**
 *  @brief Enable the DF p-state performance boost algorithm.
 *
 *  @details This function will enable the DF p-state performance
 *  boost algorithm.
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[out] prochot_asserted prochot asserted status. True indicates asserted
 *  and false indicates not-asserted.
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t write_apb_enable(uint8_t soc_num, bool *prochot_asserted);

/**
 *  @brief Read current DF p-state frequency .
 *
 *  @details This function returns the current DF p-state frequency.
 *  Returns the Fclck, DRAM memory clock(memclk),umc clock divider
 *  for the current socket DF P-state.
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[out] df_pstate struct pstate_freq contains
 *  DRAM memory clock(mem clk)
 *  data fabric clock (Fclk)
 *  UMC clock divider
 *  Uclk = 0 means divide by 1 else divide by 2.
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t read_current_dfpstate_frequency(uint8_t soc_num,
					     struct pstate_freq *df_pstate);
/**
 *  @brief Set the max and min LCK DPM Level on a given NBIO per socket.
 *
 *  @details This function will set the LCK DPM Level on a given NBIO per
 *  socket.The DPM Level is an encoding to represent the PCIE Link Frequency
 *  (LCLK) under a root complex (NBIO).
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[in] lclk lclk_dpm_level_range struct containing NBIOID (8 bit)
 *  Min dpm level (8 bit) and Max dpm level(8 bit). Valid NBIOID, min dpm
 *  level and max dpm level values are between 0 ~ 3.
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t write_lclk_dpm_level_range(uint8_t soc_num,
					struct lclk_dpm_level_range lclk);
/**
 *  @brief Read RAPL (Running Average Power Limit) Units.
 *
 *  @details This function returns the RAPL (Running Average Power Limit)
 *  Units. Energy information (in Joules) is based on the multiplier: 1/(2^ESU).
 *  Time information (in Seconds) is based on the multiplier: 1/(2^TU).
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[out] tu_value TU value.
 *
 *  @param[out] esu_value esu value.
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t read_bmc_rapl_units(uint8_t soc_num,
				 uint8_t  *tu_value,
				 uint8_t  *esu_value);

/**
 *  @brief Read RAPL base frequency per CPU socket.
 *
 *  @details This function returns the base frequency per CPU socket.
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[out] base_freq base frequency.
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t read_bmc_cpu_base_frequency(uint8_t soc_num,
					 uint16_t *base_freq);

/**
 *  @brief Control PCIe Rate on Gen5-Capable devices..
 *
 *  @details This function returns the PCIe rate.
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[in] rate PCIe gen rate. 0 indicates Auto-Detect
 *  BW and set link rate accordingly. 1 is for Limit at
 *  Gen4 Rate. 2 is for Limit at Gen5 rate.
 *
 *  @param[out] mode previous mode.
 *  0 for Auto-Detect,
 *  1 for Limit at Gen4 rate,
 *  2 for limit at Gen5 rate.
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t read_bmc_control_pcie_gen5_rate(uint8_t soc_num,
					     uint8_t rate,
					     uint8_t *mode);

/**
 *  @brief Read RAPL core energy counters.
 *
 *  @details This function returns the RAPL core energy counters.
 *
 *  @param[in] soc_num Soskcet index.
 *
 *  @param[in] core_id core id.
 *
 *  @param[out] energy_counters core energy.
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t read_rapl_core_energy_counters(uint8_t soc_num,
					    uint32_t core_id,
					    double *energy_counters);

/**
 *  @brief Read RAPL package energy counters.
 *
 *  @details This function returns the RAPL package energy counters.
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[out] energy_counters core energy.
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t read_rapl_pckg_energy_counters(uint8_t soc_num,
					    double *energy_counters);

/**
 *  @brief Read RAS last transaction address.
 *
 *  @details This function returns the last transaction address.
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[out] transaction_addr transaction address.
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval Non-zero is returned upon failure.
 *
 */
oob_status_t read_ras_last_transaction_address(uint8_t soc_num,
					       uint64_t *transaction_addr);

/**
 *  @brief Write power efficiency profile policy.
 *
 *  @details This function writes power efficiency mode
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[in] mode power efficiency mode.
 *  0 indicates High Performance Mode
 *  1 indicates Power Efficiency Mode.
 *  2 indicates I/O Performance Mode.
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t write_pwr_efficiency_mode(uint8_t soc_num,
				       uint8_t mode);

/**
 *  @brief Write df pstate range
 *
 *  @details This function writes df pstate range
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[in] max_pstate value.Max value must be
 *  less than or equal to min value. Valid values
 *  are 0 - 2.
 *
 *  @param[in] min_pstate value. Valid values are
 *  from 0 - 2.
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t write_df_pstate_range(uint8_t soc_num,
				   uint8_t max_pstate,
				   uint8_t min_pstate);

/**
 *  @brief Read LCLK Max and Min DPM level range.
 *
 *  @details This function returns the LCLK Max and Min DPM level range.
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[in] nbio_id nbio for a socket.
 *
 *  @param[out] dpm struct dpm level containing max and min dpm levels.
 *  Valid max and min dpm levels are from 0 - 1.
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval None-zero is returned upon failure.
 *
 */
oob_status_t read_lclk_dpm_level_range(uint8_t soc_num,
				       uint8_t nbio_id,
				       struct dpm_level *dpm);

/** @} */  // end of MailboxMsg
/****************************************************************************/

#endif  // INCLUDE_APML_MAILBOX_H_
