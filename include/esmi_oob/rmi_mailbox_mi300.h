/*
 * University of Illinois/NCSA Open Source License
 *
 * Copyright (c) 2023, Advanced Micro Devices, Inc.
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
 #ifndef INCLUDE_RMI_MI300_H_
 #define INCLUDE_RMI_MI300_H_

 #include "apml_err.h"

/** \file rmi_mailbox_mi300.h
 * Header file for the MI300 mailbox messages supported by APML library.
 *  All required function, structure, enum, etc. definitions should be defined
 *  in this file.
 *
 *  @details  This header file contains the following:
 *  APIs prototype of the Mailbox messages for MI300 exported by the APML library.
 *  Description of the API, arguments and return values.
 *  The Error codes returned by the API.
 */

/**
 * @brief Mailbox message types defined in the APML library
 */
typedef enum {
	SET_MAX_GFX_CORE_CLOCK = 0x81,
	SET_MIN_GFX_CORE_CLOCK,
	SET_MAX_PSTATE,
	GET_PSTATES,
	SET_XGMI_PSTATE = 0x86,
	UNSET_XGMI_PSTATE,
	GET_XGMI_PSTATES,
	GET_XCC_IDLE_RESIDENCY,
	GET_ENERGY_ACCUMULATOR = 0x90,
	GET_RAS_ALARMS,
	GET_PM_ALARMS,
	GET_PSN,
	GET_LINK_INFO,
	GET_ABS_MAX_GFX_FREQ = 0x96,
	GET_SVI_TELEMETRY_BY_RAIL,
	GET_ACT_GFX_FREQ_CAP_SELECTED = 0x9c,
	GET_DIE_HOT_SPOT_INFO = 0xA0,
	GET_MEM_HOT_SPOT_INFO,
	GET_STATUS = 0xA4,
	GET_MAX_MEM_BW_UTILIZATION = 0XB0,
	GET_HBM_THROTTLE,
	SET_HBM_THROTTLE,
	GET_HBM_STACK_TEMP,
	GET_GFX_CLK_FREQ_LIMITS,
	GET_FCLK_FREQ_LIMITS,
	GET_SOCKETS_IN_SYSTEM,
	GET_BIST_RESULTS = 0xBC,
	QUERY_STATISTICS,
	CLEAR_STATISTICS
} esb_mi300_mailbox_commmands;

/**
 * @brief APML range type used by GFX core clock frequency.
 * Max, MIN are the values. Min is 0 and Max is 1.
 */
enum range_type {
	MIN = 0,
	MAX
};

/**
 * @brief APML gfx_domain_type. Absolute maximum gfx and current gfx.
 * ABS_MAX_GFX value is 0 and CUR_GFX is 1.
 */
enum gfx_domain_type {
	ABS_MAX_GFX = 0,
	CUR_GFX
};

/**
 * @brief APML clock frequency type. GFX_CLK or F_CLK
 * GFX_CLK value is 0 and F_CLK value is 1.
 */
enum clk_type {
	GFX_CLK = 0,
	F_CLK
};

/**
 * @brief Structure for Max DDR/HBM bandwidth and utilization.
 * It contains max bandwidth(16 bit data) in GBps, current utilization
 * bandwidth(Read+Write)(16 bit data) in GBps.
 */
struct max_mem_bw {
	uint16_t max_bw;		//!< Max Bandwidth (16 bit data)
	uint16_t utilized_bw;		//!< Utilized Bandwidth  (16 bit data)
};

/**
 * @brief struct containing port and slave address.
 */
struct svi_port_domain {
	uint8_t port : 2;               //!< SVI port
	uint8_t slave_addr : 3;         //!< slave address
};

/**
 * @brief struct containing max frequency and min frequencey limit
 */
struct freq_limits {
	uint16_t max;                   //!< Max clock frequency
	uint16_t min;                   // Min clock frequency
};

/*
 * @brief struct containing memory clock and fabric clock
 * pstate mappings.
 */
struct mclk_fclk_pstates {
	uint16_t mem_clk;               //!< memory clock frequency in MHz
	uint16_t f_clk;                 //!< fabric clock frequnecy in MHz
};

/*
 * @brief struct containing statistics parameter of interest and output control
 * pstate mappings.
 */
struct statistics {
	uint16_t stat_param;		//!< statistics parameter of interest
	uint16_t output_control;	//!< Output control
};

/*
 * @brief struct containing xgmi speed rate in MHZ and link width in units
 * of Gpbs. If link_width[0] = 1 then XGMI link X2 is supported. If
 * link_width[1] = 1 then XGMI link X4 is supported. If Link_width[2] = 1
 * then XGMI link X4 is supported and similarly if link_width[3] = 1
 * then XGMI link X8 is supported.
 */
struct xgmi_speed_rate_n_width {
	uint16_t speed_rate;		//!< Speed rate
	uint8_t link_width : 4;		//!< XGMI link width
};

/**
 * @brief APML alarms type. RAS_ALARMS and PM_ALARMS. RAS is 0
 * and PM is 1.
 */
enum alarms_type {
	RAS = 0,
	PM
};

/**
 * @brief RAS alarm status.
 */
static char *ras_alarm_status[4] = {"RAS INIT FAILURE",
				    "mGPU FALL BACK TO sGPU",
				    "WAFL INIT ERROR",
				    "XGMI INIT ERRPR"};
/**
 * @brief PM alarm status.
 */
static char *pm_alarm_status[4] = {"VRHOT", "DIE OVER TEMP",
				   "HBM OVER TEMP", "PWRBRK"};

/**
 *  @brief Set maximum/minimum gfx core clock frequency.
 *
 *  @details This function sets user provied frequency as MAX GFX/MIN GFX core
 *  clock frequency in MHZ based on enumeration type #range_type.
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[in] freq_type enumeration type #range_type containing "MIN" = 0
 *  or "MAX" = 1.
 *
 *  @param[in] freq frequency in MHZ.
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval Non-zero is returned upon failure.
 *
 */
oob_status_t set_gfx_core_clock(uint8_t soc_num, enum range_type freq_type,
				uint32_t freq);

/**
 *  @brief Set maximum mem and fclck pstate.
 *
 *  @details This function sets the maximum memory and fabric clock power state.
 *  Mappings from memory and fabric clock pstate to MEMCLK/FCLK frequency can be
 *  found by issuing GetPstates command.
 *
 *  In case if the in-band has also set the maximum Pstate, then lower of the
 *  limits is used.
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[in] pstate maximum pstate.Valid pstate range is 0 - 3.
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval Non-zero is returned upon failure.
 *
 */
oob_status_t set_mclk_fclk_max_pstate(uint8_t soc_num, uint32_t pstate);

/**
 *  @brief Get memory and fabric clock power state mappings
 *
 *  @details This function returns the memory and fabric
 *  clock power state mappings. Returns MEMCLK/FCLK frequency
 *  in units of 1MHz for the available clock power states (Pstates).
 *  Each MEMCLK/FCLK frequency pair is returned independently
 *  for each pstate.
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[in] pstate index of the pstate.
 *
 *  @param[out] pstate struct containing mem_clk and
 *  f_clk frequency in MHz.
 *
 *  @param[out] mem_clk memory clk frequency in MHZ.
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval Non-zero is returned upon failure.
 *
 */
oob_status_t get_mclk_fclk_pstates(uint8_t soc_num, uint8_t pstate_ind,
				   struct mclk_fclk_pstates *pstate);

/**
 *  @brief Sets the XGMI Pstate
 *
 *  @details This function sets the specified XGMI Pstate. This disables all
 *  active XGMI Pstate management although XGMI power down modes will still
 *  be supported. Only 2 XGMI states are supported (0/1).
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[in] pstate power state. valid values are 0 - 1.
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval Non-zero is returned upon failure.
 *
 */
oob_status_t set_xgmi_pstate(uint8_t soc_num, uint32_t pstate);

/**
 *  @brief Resets the XGMI Pstate
 *
 *  @details This function resets the XGMI Pstate specified in the
 *  SetXgmiPstate, causing XGMI link speed/width to be actively managed
 *  by the GPU.
 *
 *  @param[in] soc_num Socket index.
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval Non-zero is returned upon failure.
 *
 */
oob_status_t unset_xgmi_pstate(uint8_t soc_num);

/**
 *  @brief Read XGMI power state mappings
 *
 *  @details This function reads the XGMI power state mappings.Reads the
 *  supported XGMI link speeds and widths available to the SetXgmiPstate
 *  message.Link speeds reported in units of 1Gpbs.
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[in] pstate_ind xgmi pstate index for speed rate.
 *
 *  @param[out] xgmi_pstate struct xgmi_speed_rate_n_width containing
 *  speed rate in MHz and link width
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval Non-zero is returned upon failure.
 *
 */
oob_status_t get_xgmi_pstates(uint8_t soc_num, uint8_t pstate_ind,
			      struct xgmi_speed_rate_n_width *xgmi_pstate);

/**
 *  @brief Read xcc idle residency percentage
 *
 *  @details This function will provide the average xcc idle residency across
 *  all GFX cores in the socket.100% specifies that all enabled GFX cores
 *  in the socket are running in idle.
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[out] gfx_cores_idle_res idle residency in percentage
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval Non-zero is returned upon failure.
 *
 */
oob_status_t get_xcc_idle_residency(uint8_t soc_num, uint32_t *gfx_cores_idle_res);

/**
 *  @brief Read energy accumulator with time stamp
 *
 *  @details This function will read 64 bits energy accumulator
 *  and the 56 bit time stamp.
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[out] energy accumulator  2^ 16 J.
 *
 *  @param[out] time_stamp time stamp (units:10ns).
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval Non-zero is returned upon failure.
 *
 */
oob_status_t get_energy_accum_with_timestamp(uint8_t soc_num, uint64_t *energy,
					     uint64_t *time_stamp);

/**
 *  @brief Read RAS/PM alarm status based on enumeration type #alarms_type
 *
 *  @details This function provides RAS alarm status if the enumeration type
 *  #alarms_type is RAS = 0.
 *  If buffer value is 1 the status is RAS init failure. If the buffer
 *  value is 2 status is mGPU fallback to sGPU.If the buffer value is 4
 *  status is WAFL initialization error and if the buffer is 8 then status
 *  is XGMI initialization error.
 *
 *  If the enumeration type #alarms_type is PM = 1 then it will retrieve
 *  PM alarm status .
 *  If buffer value  is 1 the status is VRHOT. If the buffer
 *  value is 2 status is die over temp. If the buffer value is 4 status is
 *  HBM over temp and if the buffer is 8 then status is PWRBRK.
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[in] type enumeration type #alarms_type. RAS = 0 or PM = 1.
 *
 *  @param[out] buffer returns PSP fw return data.
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval Non-zero is returned upon failure.
 *
 */
oob_status_t get_alarms(uint8_t soc_num, enum alarms_type type,
			uint32_t *buffer);

/**
 *  @brief Reads public serial number (PSN).
 *
 *  @details This function will return 64 bit public serial number (PSN)
 *  unique to each die.
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[in] die_index core/die index.
 *
 *  @param[out] buffer returns 64 bit unique public serial number.
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval Non-zero is returned upon failure.
 *
 */
oob_status_t get_psn(uint8_t soc_num, uint32_t die_index, uint64_t *buffer);

/**
 *  @brief Read link Info
 *
 *  @details This function will read link info. Function will
 *  read the module ID and link config reflecting strapping pins.
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[out] link_config link configuration.
 *
 *  @param[out] module_id module ID.
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval Non-zero is returned upon failure.
 *
 */
oob_status_t get_link_info(uint8_t soc_num, uint8_t *link_config,
			   uint8_t *module_id);

/**
 *  @brief Read maximum allowed/Current selected  GFX engine frequency
 *  based on enumeration type #gfx_domain_type.
 *
 *  @details This function will read maximum allowed GFX engine frequency
 *  if the enumeration type #gfx_domain_type is "ABS_MAX_GFX" = 0.
 *
 *  This function will also read current selected GFX frequency if the
 *  enumeration type #gfx_domain_type is "CUR_GFX" = 1. This reflects minimum
 *  of all frequency caps selected via in-band or out-ofband controls.
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[in] type enumberation type #gfx_domain_type.
 *  values are "ABS_MAX_GFX" = 0 and "CUR_GFX".
 *
 *  @param[out] freq frequency in MHZ.
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval Non-zero is returned upon failure.
 *
 */
oob_status_t get_gfx_freq(uint8_t soc_num, enum gfx_domain_type type,
			  uint16_t *freq);

/**
 *  @brief Read SVI based telemetry for individual rails
 *
 *  @details This function will read SVI based telemetry for
 *  individual rails.
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[in] port struct svi_telemetry_domain containing port
 *  and slave address.
 *
 *  @param[out] pow power in milliwatts.
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval Non-zero is returned upon failure.
 *
 */
oob_status_t get_svi_rail_telemetry(uint8_t soc_num,
				    struct svi_port_domain port,
				    uint32_t *pow);

/**
 *  @brief Reads local ID of the hottest die and its temperature.
 *
 *  @details This function will read local ID of the hottest die and its
 *  corresponding die temperature. Measured in every 1 ms and
 *  the most recently measured temperature in °C is reported.
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[out] die_id Hottest die ID.
 *
 *  @param[out] temp Die hot spot temperature in °C.
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval Non-zero is returned upon failure.
 *
 */
oob_status_t get_die_hotspot_info(uint8_t soc_num, uint8_t *die_id,
				  uint16_t *temp);

/**
 *  @brief Reads local ID of the HBM stack and its temperature.
 *
 *  @details This function will read local ID of the HBM stack and its
 *  corresponding HBM stack temperature. Measured in every 1 ms and
 *  the most recently measured temperature is reported.
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[out] hbm_stack_id Local ID of the hottest HBM stack.
 *
 *  @param[out] hbm_temp temperature in units of 1 °C.
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval Non-zero is returned upon failure.
 *
 */
oob_status_t get_mem_hotspot_info(uint8_t soc_num, uint8_t *hbm_stack_id,
				  uint16_t *hbm_temp);

/**
 *  @brief Reads power management controller status
 *
 *  @details This function will read PM status in a bit vector
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[out] status reports running status of power management controller.
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval Non-zero is returned upon failure.
 *
 */
oob_status_t get_controller_status(uint8_t soc_num, bool *status);

/**
 *  @brief Reads max memory bandwidth utilization.
 *
 *  @details This function will provide theoretic.al maximum HBM/memory
 *  bandwidth of the system in GB/s, utilized bandwidth in GB/S.
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[out] bw struct max_mem_bw containing max bw,
 *  utilized b/w.
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval Non-zero is returned upon failure.
 *
 */
oob_status_t get_max_mem_bw_util(uint8_t soc_num,
				 struct max_mem_bw *bw);

/**
 *  @brief Reads HBM throttle.
 *
 *  @details This function will read HBM throttle in percentage.
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[out] mem_th hbm throttle in percentage (0 - 100%).
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval Non-zero is returned upon failure.
 *
 */
oob_status_t get_hbm_throttle(uint8_t soc_num, uint32_t *mem_th);

/**
 *  @brief writes HBM throttle.
 *
 *  @details This function will write HBM throttle.
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[in] mem_th hbm throttle in percentage (0 - 80%).
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval Non-zero is returned upon failure.
 *
 */
oob_status_t set_hbm_throttle(uint8_t soc_num, uint32_t mem_th);

/**
 *  @brief Reads hbm stack temperature.
 *
 *  @details This function will read particular hbm stack temperature.
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[in] index hbm stack index ( 0 - 7 for MI300).
 *
 *  @param[out] temp temperature in units of 1 °C.
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval Non-zero is returned upon failure.
 *
 */
oob_status_t get_hbm_temperature(uint8_t soc_num, uint32_t index,
				 uint16_t *temp);

/**
 *  @brief Reads GFX/F clk frequency limits based on enumeration
 *  type #clk_type .
 *
 *  @details This function will provide socket's GFX/F clk max and min
 *  frequnecy limits based on enumberation type #clk_type .
 *  The function reads GFX clk frequency limits if the enumberation
 *  type #clk_type is GFX_CLK = 0 else it will read F_CLK frequency
 *  limits.
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[in] type enumeration type #clk_type .
 *  Values are "GFX_CLK" = 0  or "F_CLK" = 1.
 *
 *  @param[out] limit struct freq_limits containing max and min GFX/F_clk
 *  frequency in MHZ.
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval Non-zero is returned upon failure.
 *
 */
oob_status_t get_clk_freq_limits(uint8_t soc_num, enum clk_type type,
				 struct freq_limits *limit);

/**
 *  @brief Reads number of sockets in system
 *
 *  @details This function will read number of sockets in system.
 *
 *  @param[out] sockets Numbers of sockets in system
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval Non-zero is returned upon failure.
 *
 */
oob_status_t get_sockets_in_system(uint32_t *sockets_count);

/**
 *  @brief Reads die level bist result status from package
 *
 *  @details This function will read die level bist result status
 *  from package.
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[in] die_id die level id.
 *
 *  @param[out] bist_result constituent bist results depending on
 *  MI300X/A/C configuration.
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval Non-zero is returned upon failure.
 *
 */
oob_status_t get_bist_results(uint8_t soc_num, uint8_t die_id,
			      uint32_t *bist_result);

/**
 *  @brief Reads statistics for a given parameter.
 *
 *  @details This function will read statistics for a given parameter
 *  since the last clear statistics command.
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[in] stat_param struct containing statistics parameter of interest
 *  and output control.
 *
 *  @param[out] Param parameter or timestamp HI/Lo value.
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval Non-zero is returned upon failure.
 *
 */
oob_status_t get_statistics(uint8_t soc_num, struct statistics stat,
			    uint32_t *param);

/**
 *  @brief Clears statistics.
 *
 *  @details This function will clear all stored query statistics timestamps
 *  and then resumes data collection or aggregation.
 *
 *  @param[in] soc_num Socket index.
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *  @retval Non-zero is returned upon failure.
 *
 */
oob_status_t clear_statistics(uint8_t soc_num);

/* @}
 */  // end of MailboxMsg
/****************************************************************************/

 #endif  // INCLUDE_RMI_MI300_H_
