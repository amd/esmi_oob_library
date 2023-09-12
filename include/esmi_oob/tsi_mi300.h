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
#ifndef INCLUDE_TSI_MI300_H_
#define INCLUDE_TSI_MI300_H_

#include "apml_err.h"

/** \file tsi_mi300.h
 *  Header file for the APML library for SB-TSI functionality access for MI300.
 *  All required function, structure, enum, etc. definitions should be defined
 *  in this file for SB-TSI Register accessing.
 *
 *  @details  This header file contains the following:
 *  APIs prototype of the APIs exported by the APML library.
 *  Description of the API, arguments and return values.
 *  The Error codes returned by the API.
 */

/**
 * @brief SB-TSI(Side-Band Temperature Sensor Interface) commands register
 * access.
 */

typedef enum {
	SBTSI_HBM_HITEMPINT_LIMIT	= 0x40,
	SBTSI_HBM_HITEMPDEC_LIMIT	= 0x44,
	SBTSI_HBM_LOTEMPINT_LIMIT	= 0x48,
	SBTSI_HBM_LOTEMPDEC_LIMIT	= 0x4C,
	SBTSI_MAX_HBMTEMPINT		= 0x50,
	SBTSI_MAX_HBMTEMPDEC		= 0x54,
	SBTSI_HBMTEMPINT		= 0x5C,
	SBTSI_HBMTEMPDEC		= 0x60,
} sbtsi_mi300_registers;

/**
 *  @brief Get high hbm temperature integer threshold in °C
 *
 *  @details This function will read high hbm temperature interger threshold
 *  in °C.
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[out] buffer a pointer to hold the high hbm temperature integer
 *  threshold in °C.
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *
 *  @retval Non-zero is returned upon failure.
 */
oob_status_t read_sbtsi_hbm_hi_temp_int_th(uint8_t soc_num, uint8_t *buffer);

/**
 *  @brief Set high hbm temperature integer and decimal threshold in °C.
 *
 *  @details This function will set high hbm temperature interger and decimal
 *  threshold in °C.
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[in] hi_temp_th high hbm temperature threshold containing integer and
 *  decimal part in °C.
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *
 *  @retval Non-zero is returned upon failure.
 */
oob_status_t write_sbtsi_hbm_hi_temp_th(uint8_t soc_num, float hi_temp_th);

/**
 *  @brief Get high hbm temperature decimal threshold
 *
 *  @details This function will read high hbm temperature decimal threshold
 *  in °C.
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[out] buffer a pointer to hold the high hbm temperature decimal
 *  threshold in °C.
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *
 *  @retval Non-zero is returned upon failure.
 */
oob_status_t read_sbtsi_hbm_hi_temp_dec_th(uint8_t soc_num, float *buffer);

/**
 *  @brief Get high hbm temperature threshold
 *
 *  @details This function will read high hbm temperature threshold
 *  in °C.
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[out] buffer a pointer to hold the high hbm temperature in °C
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *
 *  @retval Non-zero is returned upon failure.
 */
oob_status_t read_sbtsi_hbm_hi_temp_th(uint8_t soc_num, float *buffer);

/**
 *  @brief Get low hbm temperature integer threshold
 *
 *  @details This function will read low hbm temperature interger threshold
 *  in °C.
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[out] buffer a pointer to hold the low hbm temperature integer
 *  threshold in °C.
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *
 *  @retval Non-zero is returned upon failure.
 */
oob_status_t read_sbtsi_hbm_lo_temp_int_th(uint8_t soc_num, uint8_t *buffer);

/**
 *  @brief Get low hbm temperature decimal threshold
 *
 *  @details This function will low high hbm temperature decimal threshold
 *  in °C.
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[out] buffer a pointer to hold the low hbm temperature decimal
 *  threshold in °C.
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *
 *  @retval Non-zero is returned upon failure.
 */
oob_status_t read_sbtsi_hbm_lo_temp_dec_th(uint8_t soc_num, float *buffer);

/**
 *  @brief Set low hbm temperature threshold
 *
 *  @details This function will set low hbm temperature threshold
 *  in °C.
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[in] temp_th low hbm temperature threshold in °C.
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *
 *  @retval Non-zero is returned upon failure.
 */
oob_status_t write_sbtsi_hbm_lo_temp_th(uint8_t soc_num, float temp_th);
/**
 *  @brief Get max hbm integer temperature
 *
 *  @details This function will read max hbm interger temperature in °C
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[out] buffer a pointer to hold the max hbm integer temperature in °C
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *
 *  @retval Non-zero is returned upon failure.
 */
oob_status_t read_sbtsi_max_hbm_temp_int(uint8_t soc_num, uint8_t *buffer);

/**
 *  @brief Get max hbm decimal temperature
 *
 *  @details This function will read max hbm decimal temperature in °C
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[out] buffer a pointer to hold the max hbm decimal temperature
 *  in °C.
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *
 *  @retval Non-zero is returned upon failure.
 */
oob_status_t read_sbtsi_max_hbm_temp_dec(uint8_t soc_num, float *buffer);

/**
 *  @brief Get hbm integer temperature
 *
 *  @details This function will read hbm integer temperature in °C.
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[out] buffer a pointer to hold the hbm integer temperature in °C
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *
 *  @retval Non-zero is returned upon failure.
 */
oob_status_t read_sbtsi_hbm_temp_int(uint8_t soc_num, uint8_t *buffer);

/**
 *  @brief Get hbm decimal temperature
 *
 *  @details This function will read hbm decimal temperature in °C.
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[out] buffer a pointer to hold the hbm decimal temperature in °C
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *
 *  @retval Non-zero is returned upon failure.
 */
oob_status_t read_sbtsi_hbm_temp_dec(uint8_t soc_num, float *buffer);

/**
 *  @brief Get hbm low temperature threshold
 *
 *  @details This function will read hbm low threshold temperature in °C.
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[out] buffer a pointer to hold the low hbm temperature threshold
 *  in °C.
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *
 *  @retval Non-zero is returned upon failure.
 */
oob_status_t read_sbtsi_hbm_lo_temp_th(uint8_t soc_num, float *buffer);

/**
 *  @brief Get hbm maximum temperature
 *
 *  @details This function will read maximum hbm temperature in °C.
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[out] buffer a pointer to hold the max hbm temperature in °C
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *
 *  @retval Non-zero is returned upon failure.
 */
oob_status_t read_sbtsi_max_hbm_temp(uint8_t soc_num, float *buffer);

/**
 *  @brief Get hbm temperature
 *
 *  @details This function will read maximum hbm temperature in °C.
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[out] buffer a pointer to hold the hbm temperature in °C
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *
 *  @retval Non-zero is returned upon failure.
 */
oob_status_t read_sbtsi_hbm_temp(uint8_t soc_num, float *buffer);

/**
 *  @brief Get hbm alert threshold
 *
 *  @details This function will read hbm alert threshold.
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[out] samples hbm threshold samples
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *
 *  @retval Non-zero is returned upon failure.
 */
oob_status_t read_sbtsi_hbm_alertthreshold(uint8_t soc_num, uint8_t *samples);

/**
 *  @brief Set hbm alert samples
 *
 *  @details This function will set hbm alert samples.
 *
 *  @param[in] soc_num Socket index.
 *
 *  @param[in] samples hbm threshold samples
 *
 *  @retval ::OOB_SUCCESS is returned upon successful call.
 *
 *  @retval Non-zero is returned upon failure.
 */
oob_status_t sbtsi_set_hbm_alert_threshold(uint8_t soc_num,
					   uint8_t samples);
#endif  // INCLUDE_TSI_MI300_H_
