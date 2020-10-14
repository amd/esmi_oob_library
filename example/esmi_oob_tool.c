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

#include <ctype.h>
#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <esmi_oob/esmi_common.h>
#include <esmi_oob/esmi_cpuid_msr.h>
#include <esmi_oob/esmi_mailbox.h>
#include <esmi_oob/esmi_rmi.h>
#include <esmi_oob/esmi_tsi.h>


#define RED "\x1b[31m"
#define RESET "\x1b[0m"

#define ARGS_MAX 64

#define APML_SLEEP 10000


void show_smi_parameters(uint32_t, uint32_t);
void show_smi_message(void);
void show_smi_end_message(void);
static int flag;

oob_status_t apml_get_sockpower(uint32_t i2c_bus, uint32_t i2c_addr)
{
	oob_status_t ret;
	uint32_t power = 0;

	printf("---------------------------------------------");
	ret = read_socket_power(i2c_bus, i2c_addr, &power);
	if (ret != OOB_SUCCESS) {
		printf("Failed: to get i2c_addr[0x%x] power, Err[%d]: %s\n",
			i2c_addr, ret, esmi_get_err_msg(ret));
		return ret;
	}
	printf("\n| Power (Watts)\t\t |");
	printf(" %-17.3f|", (double)power/1000);

	/* Get the PowerLimit for a given i2c_bus, i2c_addr index */
	ret = read_socket_power_limit(i2c_bus, i2c_addr, &power);
	if (ret != OOB_SUCCESS) {
		printf("Failed: to get i2c_addr[0x%x] powerlimit,Err[%d]:%s\n",
			i2c_addr, ret, esmi_get_err_msg(ret));
		return ret;
	}
	printf("\n| PowerLimit (Watts)\t |");
	printf(" %-17.3f|", (double)power/1000);

	/* Get the maxpower for a given i2c_bus, i2c_addr index */
	ret = read_max_socket_power_limit(i2c_bus, i2c_addr, &power);
	if (ret != OOB_SUCCESS) {
		printf("Failed: to get i2c_addr[0x%x] maxpower, Err[%d]: %s\n",
			i2c_addr, ret, esmi_get_err_msg(ret));
		return ret;
	}
	printf("\n| PowerLimitMax (Watts)\t |");
	printf(" %-17.3f|", (double)power/1000);
	printf("\n---------------------------------------------\n");

	return OOB_SUCCESS;
}

oob_status_t apml_get_socktdp(uint32_t i2c_bus, uint32_t i2c_addr)
{
	oob_status_t ret;
	uint32_t buffer = 0;

	ret = read_tdp(i2c_bus, i2c_addr, &buffer);
	if (ret != OOB_SUCCESS) {
		printf("Failed: to get i2c_addr[0x%x] tdp, Err[%d]: %s\n",
			i2c_addr, ret, esmi_get_err_msg(ret));
		return ret;
	}
	printf("i2c_addr[0x%x]/TDP: %16.03f Watts\n", i2c_addr,
		(double)buffer/1000);

	/* Get min tdp value for a given i2c_bus, i2c_addr */
	ret = read_min_tdp(i2c_bus, i2c_addr, &buffer);
	if (ret != OOB_SUCCESS) {
		printf("Failed: to get i2c_addr[0x%x] min tdp, Err[%d]: %s\n",
			i2c_addr, ret, esmi_get_err_msg(ret));
		return ret;
	}
	printf("i2c_addr[0x%x]/Min_TDP: %12.03f Watts\n", i2c_addr,
		(double)buffer/1000);

	/* Get max tdp value for a given i2c_bus, i2c_addr */
	ret = read_max_tdp(i2c_bus, i2c_addr, &buffer);
	if (ret != OOB_SUCCESS) {
		printf("Failed: to get i2c_addr[0x%x] max_tdp, Err[%d]: %s\n",
			i2c_addr, ret, esmi_get_err_msg(ret));
		return ret;
	}
	printf("i2c_addr[0x%x]/Max_TDP: %12.03f Watts\n\n", i2c_addr,
		(double)buffer/1000);
	return OOB_SUCCESS;
}

oob_status_t apml_setpower_limit(uint32_t i2c_bus, uint32_t i2c_addr,
				 uint32_t power)
{
	oob_status_t ret;
	uint32_t max_power = 0;

	ret = read_max_socket_power_limit(i2c_bus, i2c_addr, &max_power);
	if ((ret == OOB_SUCCESS) && (power > max_power)) {
		printf("Input power is not within accepted limit,\n"
			"So value set to default max %.3f Watts\n",
			(double)max_power/1000);
		power = max_power;
	}
	ret = write_socket_power_limit(i2c_bus, i2c_addr, power);
	if (ret != OOB_SUCCESS) {
		printf("Failed:to set i2c_addr[0x%x] power_limit,Err[%d]:%s\n",
			i2c_addr, ret, esmi_get_err_msg(ret));
		return ret;
	}
	printf("\nSet i2c_addr[0x%x]/power_limit : %16.03f Watts successfully\n",
		i2c_addr, (double)power/1000);
	return OOB_SUCCESS;
}

oob_status_t apml_get_cclk_freqlimit(uint32_t i2c_bus, uint32_t i2c_addr)
{
	oob_status_t ret;
	uint32_t buffer = 0;

	ret = read_cclk_freq_limit(i2c_bus, i2c_addr, &buffer);
	if (ret != OOB_SUCCESS) {
		printf("Failed:to get i2c_addr[0x%x] cclk_freqlimit,Err[%d]:%s\n",
			i2c_addr, ret, esmi_get_err_msg(ret));
		return ret;
	}
	printf("i2c_addr[0x%x]/cclk_freqlimit: %u MHz\n\n", i2c_addr, buffer);
	return OOB_SUCCESS;
}

oob_status_t apml_get_sockc0_residency(uint32_t i2c_bus, uint32_t i2c_addr)
{
	oob_status_t ret;
	uint32_t buffer = 0;

	ret = read_socket_c0_residency(i2c_bus, i2c_addr, &buffer);
	if (ret != OOB_SUCCESS) {
		printf("Failed:to get i2c_addr[0x%x] c0_residency, Err[%d]:%s\n",
			i2c_addr, ret, esmi_get_err_msg(ret));
		return ret;
	}
	printf("i2c_addr[0x%x]/c0_residency: %u%%\n\n", i2c_addr, buffer);
	return OOB_SUCCESS;
}

void apml_get_ddr_bandwidth(uint32_t i2c_bus, uint32_t i2c_addr)
{
	uint32_t max_bw, utilized_bw, utilized_pct;
	oob_status_t ret;

	ret = read_ddr_bandwidth(i2c_bus, i2c_addr, &max_bw, &utilized_bw,
				 &utilized_pct);
	if (ret != OOB_SUCCESS) {
		printf("Failed:to get i2c_addr[0x%x] DDR Bandwidth, Err[%d]:%s\n",
			i2c_addr, ret, esmi_get_err_msg(ret));
		return;
	}
	printf("---------------------------------------------");
	printf("\n| DDR Max BW (GB/s)\t |");
	printf(" %-17d|", max_bw);
	printf("\n| DDR Utilized BW (GB/s) |");
	printf(" %-17d|", utilized_bw);
	printf("\n| DDR Utilized Percent(%%)|");
	printf(" %-17d|", utilized_pct);
	printf("\n---------------------------------------------\n");
}

oob_status_t get_boostlimit(uint32_t i2c_bus, uint32_t i2c_addr,
			    uint32_t core_id)
{
	oob_status_t ret;
	uint32_t buffer = 0;

	ret = read_esb_boost_limit(i2c_bus, i2c_addr, core_id, &buffer);
	if (ret != OOB_SUCCESS) {
		printf("Failed: to get i2c_addr[0x%x]/core[%d] apml_boostlimit,"
			" Err[%d]: %s\n", i2c_addr, core_id, ret,
			esmi_get_err_msg(ret));
		return ret;
	}
	printf("i2c_addr[0x%x]/core[%d] apml_boostlimit: '%u MHz'\n\n",
		i2c_addr, core_id, buffer);

	usleep(APML_SLEEP);
	/* Get the Bios boostlimit for a given i2c_bus, i2c_addr index */
	ret = read_bios_boost_fmax(i2c_bus, i2c_addr, core_id, &buffer);
	if (ret != OOB_SUCCESS) {
		printf("Failed:to get i2c_addr[0x%x]/core[%d] bios_boostlimit, "
			"Err[%d]: %s\n", i2c_addr, core_id, ret,
			esmi_get_err_msg(ret));
		return ret;
	}
	printf("i2c_addr[0x%x]/core[%d] bios_boostlimit: '%u MHz'\n\n",
		i2c_addr, core_id, buffer);
	return OOB_SUCCESS;
}

oob_status_t set_apml_boostlimit(uint32_t i2c_bus, uint32_t i2c_addr,
				 uint32_t core_id, uint32_t boostlimit)
{
	oob_status_t ret;
	uint32_t blimit = 0;

	ret = write_esb_boost_limit(i2c_bus, i2c_addr, core_id, boostlimit);
	if (ret != OOB_SUCCESS) {
		printf("Failed: to set i2c_addr[0x%x]/core[%d] apml_boostlimit "
			"Err[%d]: %s\n", i2c_addr, core_id, ret,
			esmi_get_err_msg(ret));
		return ret;
	}
	usleep(APML_SLEEP);
	ret = read_esb_boost_limit(i2c_bus, i2c_addr, core_id, &blimit);
	if (ret == OOB_SUCCESS) {
		if (blimit < boostlimit)
			printf("Set to max boost limit: %u MHz\n", blimit);
		else if (blimit > boostlimit)
			printf("Set to min boost limit: %u MHz\n", blimit);
	}
	printf("i2c_addr[0x%x]/core[%d] apml_boostlimit set successfully\n",
		i2c_addr, core_id);
	return OOB_SUCCESS;
}

oob_status_t set_apml_socket_boostlimit(uint32_t i2c_bus, uint32_t i2c_addr,
					uint32_t boostlimit)
{
	oob_status_t ret;

	ret = write_esb_boost_limit_allcores(i2c_bus, i2c_addr, boostlimit);
	if (ret != OOB_SUCCESS) {
		printf("Failed: to set apml_boostlimit for all cores Err[%d]: %s\n",
			ret, esmi_get_err_msg(ret));
		return ret;
	}
	printf("apml_boostlimit for all cores set successfully\n");
	return OOB_SUCCESS;
}

oob_status_t set_and_verify_dram_throttle(uint32_t i2c_bus, uint32_t i2c_addr,
					  uint32_t dram_thr)
{
	oob_status_t ret;
	uint32_t limit = 0;

	ret = write_dram_throttle(i2c_bus, i2c_addr, dram_thr);
	if (ret != OOB_SUCCESS) {
		printf("Failed: to set DRAM throttle, Err[%d]: %s\n",
			ret, esmi_get_err_msg(ret));
		return ret;
	}
	usleep(APML_SLEEP);
	ret = read_dram_throttle(i2c_bus, i2c_addr, &limit);
	if (ret == OOB_SUCCESS) {
		if (limit < dram_thr)
			printf("Set to max dram throttle: %u %%\n", limit);
		else if (limit > dram_thr)
			printf("Set to min dram throttle: %u %%\n", limit);
	}
	printf("Set and Verify Success %u %%\n", limit);
	return OOB_SUCCESS;
}

oob_status_t set_and_verify_apml_socket_uprate(uint32_t i2c_bus,
					       uint32_t i2c_addr, float uprate)
{
	oob_status_t ret;
	float rduprate;

	ret = write_sbtsi_updaterate(i2c_bus, i2c_addr, uprate);
	if (ret != OOB_SUCCESS) {
		printf("Failed: to set Update rate for i2c_addr Err[%d]: %s\n",
			ret, esmi_get_err_msg(ret));
		return ret;
	}
	usleep(APML_SLEEP);

	if (read_sbtsi_updaterate(i2c_bus, i2c_addr, &rduprate) == 0) {
		if (uprate != rduprate)
			return OOB_TRY_AGAIN;
		printf("Set and verify Success %f\n", rduprate);
	}

	return OOB_SUCCESS;
}

static oob_status_t set_high_temp_threshold(uint32_t i2c_bus, uint32_t i2c_addr,
					    float temp)
{
	oob_status_t ret;
	int temp_int;
	float temp_dec;

	ret = sbtsi_set_hitemp_threshold(i2c_bus, i2c_addr, temp);
	if (ret != OOB_SUCCESS) {
		printf("Failed: to set Higher Temp threshold limit Err[%d]: %s\n",
			ret, esmi_get_err_msg(ret));
		return ret;
	}
	printf("Set Success\n");
	return OOB_SUCCESS;
}

static oob_status_t set_low_temp_threshold(uint32_t i2c_bus, uint32_t i2c_addr,
					   float temp)
{
	oob_status_t ret;
	int temp_int;
	float temp_dec;

	if (temp < 0 || temp > 70) {
		printf("Invalid temp, please mention temp between 0 and 70\n");
		return OOB_INVALID_INPUT;
	}

	ret = sbtsi_set_lotemp_threshold(i2c_bus, i2c_addr, temp);
	if (ret != OOB_SUCCESS) {
		printf("Failed: to set Lower Temp threshold limit Err[%d]: %s\n",
			ret, esmi_get_err_msg(ret));
		return ret;
	}
	printf("Set Success\n");
	return OOB_SUCCESS;
}

static oob_status_t set_temp_offset(uint32_t i2c_bus, uint32_t i2c_addr,
				    float temp)
{
	oob_status_t ret;

	ret = write_sbtsi_cputempoffset(i2c_bus, i2c_addr, temp);
	if (ret != OOB_SUCCESS) {
		printf("Failed: to set Temp offset, Err[%d]: %s\n",
			ret, esmi_get_err_msg(ret));
		return ret;
	}
	printf("Set CPU temp offset success\n");
	return OOB_SUCCESS;
}

static oob_status_t set_timeout_config(uint32_t i2c_bus, uint32_t i2c_addr,
				       int value)
{
	oob_status_t ret;

	ret = sbtsi_set_timeout_config(i2c_bus, i2c_addr, value);
	if (ret != OOB_SUCCESS) {
		printf("Failed: to set timeout config, Err[%d]: %s\n",
			ret, esmi_get_err_msg(ret));
		return ret;
	}
	printf("Set timeout config success\n");
	return OOB_SUCCESS;
}

static oob_status_t set_alert_threshold(uint32_t i2c_bus, uint32_t i2c_addr,
					int value)
{
	oob_status_t ret;

	ret = sbtsi_set_alert_threshold(i2c_bus, i2c_addr, value);
	if (ret != OOB_SUCCESS) {
		printf("Failed: to set alert threshold sample, Err[%d]: %s\n",
			ret, esmi_get_err_msg(ret));
		return ret;
	}
	printf("Set alert threshold success\n");
	return OOB_SUCCESS;
}

static oob_status_t set_alert_config(uint32_t i2c_bus, uint32_t i2c_addr,
				     int value)
{
	oob_status_t ret;

	ret = sbtsi_set_alert_config(i2c_bus, i2c_addr, value);
	if (ret != OOB_SUCCESS) {
		printf("Failed: to set alert config, Err[%d]: %s\n",
			ret, esmi_get_err_msg(ret));
		return ret;
	}
	printf("Set alert config success\n");
	return OOB_SUCCESS;
}

static oob_status_t set_tsi_config(uint32_t i2c_bus, uint32_t i2c_addr,
				   int value, const char check)
{
	oob_status_t ret;

	switch (check) {
	case 'k':
		ret = sbtsi_set_configwr(i2c_bus, i2c_addr, value,
				       ALERTMASK_MASK);
		if (ret != OOB_SUCCESS) {
			printf("Failed: to set tsi config alert_mask, "
				"Err[%d]: %s\n", ret,
				esmi_get_err_msg(ret));
			return ret;
		}
		printf("ALERT_L pin %s\n", value ? "Disabled" : "Enabled");
		break;
	case 'm':
		ret = sbtsi_set_configwr(i2c_bus, i2c_addr, value,
					   RUNSTOP_MASK);
		if (ret != OOB_SUCCESS) {
			printf("Failed: to set tsi config runstop_mask, "
				"Err[%d]: %s\n", ret,
				esmi_get_err_msg(ret));
			return ret;
		}
		printf("runstop bit %s\n", value ? "Comparisions Disabled"
		       : "Comparisions Enabled");
		break;
	case 'n':
		ret = sbtsi_set_configwr(i2c_bus, i2c_addr, value,
					   READORDER_MASK);
		if (ret != OOB_SUCCESS) {
			printf("Failed: to set tsi config readorder_mask, "
				"Err[%d]: %s\n", ret,
				esmi_get_err_msg(ret));
			return ret;
		}
		printf("Atomic read bit %s\n", value ? "Decimal Latches Integer" :
			"Integer Latches Decimal");
		break;
	case 'o':
		ret = sbtsi_set_configwr(i2c_bus, i2c_addr, value,
					   ARA_MASK);
		if (ret != OOB_SUCCESS) {
			printf("Failed: to set tsi config ara_mask, "
				"Err[%d]: %s\n", ret,
				esmi_get_err_msg(ret));
			return ret;
		}
		printf("ARA Diable bit %s\n", value ? "Disabled" : "Enabled");
	}
	return OOB_SUCCESS;
}

oob_status_t get_apml_rmi_access(uint32_t i2c_bus, uint32_t i2c_addr)
{
	int8_t buf;
	oob_status_t ret;

	printf("------------------------------------------------------------"
		"----\n");
	printf("\n\t\t *** SB-RMI REGISTER SUMMARY ***\n");
	printf("------------------------------------------------------------"
		"----\n");
	ret = read_sbrmi_revision(i2c_bus, i2c_addr, &buf);
	if (ret != 0) {
		printf("Err[%d]:%s\n", ret, esmi_get_err_msg(ret));
		return ret;
	}
	printf("_RMI_REVISION		| %#4x\n", buf);

	usleep(APML_SLEEP);
	if (read_sbrmi_control(i2c_bus, i2c_addr, &buf) == 0)
	printf("_RMI_CONTROL		| %#4x\n", buf);

	usleep(APML_SLEEP);
	if (read_sbrmi_status(i2c_bus, i2c_addr, &buf) == 0)
	printf("_RMI_STATUS		| %#4x\n", buf);

	usleep(APML_SLEEP);
	if (read_sbrmi_readsize(i2c_bus, i2c_addr, &buf) == 0)
	printf("_RMI_READSIZE		| %#4x\n", buf);

	usleep(APML_SLEEP);
	if (read_sbrmi_threadenablestatus(i2c_bus, i2c_addr, &buf) == 0)
	printf("_RMI_THREADENABLESTATUS	| %#4x\n", (uint8_t)buf);

	usleep(APML_SLEEP);
	if (read_sbrmi_swinterrupt(i2c_bus, i2c_addr, &buf) == 0)
	printf("_RMI_SWINTERRUPT	| %#4x\n", buf);

	usleep(APML_SLEEP);
	if (read_sbrmi_threadnumber(i2c_bus, i2c_addr, &buf) == 0)
	printf("_RMI_THREADNUMEBER	| %#4x\n", (uint8_t)buf);
	printf("------------------------------------------------------------"
		"----\n");
}

oob_status_t get_apml_tsi_register_descriptions(uint32_t i2c_bus,
						uint32_t i2c_addr)
{
	uint8_t lowalert, hialert;
	uint8_t al_mask, run_stop, read_ord, ara;
	uint8_t timeout;
	int8_t intr;
	float dec;
	float temp_value[3];
	float uprate;
	uint8_t id, buf;
	oob_status_t ret;

	usleep(APML_SLEEP);
	ret = sbtsi_get_cputemp(i2c_bus, i2c_addr, &temp_value[0]);
	if (ret)
		return ret;

	usleep(APML_SLEEP);
	ret = sbtsi_get_hitemp_threshold(i2c_bus, i2c_addr, &temp_value[1]);
	if (ret)
		return ret;

	usleep(APML_SLEEP);
	ret = sbtsi_get_lotemp_threshold(i2c_bus, i2c_addr, &temp_value[2]);
	if (ret)
		return ret;

	usleep(APML_SLEEP);
	ret = read_sbtsi_updaterate(i2c_bus, i2c_addr, &uprate);
	if (ret)
		return ret;
	printf("\n\t\t *** SB-TSI REGISTER SUMMARY ***\n");
	printf("------------------------------------------------------------"
		"----\n");
	printf("_CPUTEMP\t\t| %.3f °C \n", temp_value[0]);
	printf("_HIGH_THRESHOLD_TEMP\t| %.3f °C \n", temp_value[1]);
	printf("_LOW_THRESHOLD_TEMP \t| %.3f °C \n", temp_value[2]);
	printf("_TSI_UPDATERATE\t\t| %.3f Hz\n", uprate);

	usleep(APML_SLEEP);
	ret = read_sbtsi_alertthreshold(i2c_bus, i2c_addr, &buf);
	if (ret)
		return ret;
	printf("_THRESHOLD_SAMPLE\t| %d\n", buf);

	usleep(APML_SLEEP);
	ret = read_sbtsi_cputempoffset(i2c_bus, i2c_addr, &dec);
	if (ret)
		return ret;
	printf("_TEMP_OFFSET\t\t| %.3f °C \n", dec);

	usleep(APML_SLEEP);
	ret = sbtsi_get_temp_status(i2c_bus, i2c_addr, &lowalert, &hialert);
	if (ret)
		return ret;
	printf("_STATUS\t\t\t| ");
	if (lowalert)
		printf("CPU Temp Low Alert\n");
	else if (hialert)
		printf("CPU Temp Hi Alert\n");
	else
		printf("No Temp Alert\n");

	usleep(APML_SLEEP);
	ret = sbtsi_get_config(i2c_bus, i2c_addr, &al_mask, &run_stop,
			       &read_ord, &ara);
	if (ret)
		return ret;
	printf("_CONFIG\t\t\t| \n");
	printf("\tALERT_L pin\t| %s\n", al_mask ? "Disabled" : "Enabled");
	printf("\tRunstop\t\t| %s\n", run_stop ? "Comparison Disabled" :
		"Comparison Enabled");
	printf("\tAtomic Rd order\t| %s\n", read_ord ? "Decimal Latches Integer" :
		"Integer latches Decimal");
	printf("\tARA response\t| %s\n", ara ? "Disabled" : "Enabled");

	usleep(APML_SLEEP);
	ret = sbtsi_get_timeout(i2c_bus, i2c_addr, &timeout);
	if (ret)
		return ret;
	printf("_TIMEOUT_CONFIG\t\t| %s\n", timeout ? "Enabled" : "Disabled");

	usleep(APML_SLEEP);
	ret = read_sbtsi_alertconfig(i2c_bus, i2c_addr, &buf);
	if (ret)
		return ret;
	printf("_TSI_ALERT_CONFIG\t| %s\n", buf ? "Enabled" : "Disabled");

	usleep(APML_SLEEP);
	ret = read_sbtsi_manufid(i2c_bus, i2c_addr, &id);
	if (ret)
		return ret;
	printf("_TSI_MANUFACTURE_ID\t| %#x\n", id);

	usleep(APML_SLEEP);
	ret = read_sbtsi_revision(i2c_bus, i2c_addr, &id);
	if (ret)
		return ret;
	printf("_TSI_REVISION\t\t| %#x\n", id);

	return OOB_SUCCESS;
}

oob_status_t get_apml_tsi_access(uint32_t i2c_bus, uint32_t i2c_addr)
{
	oob_status_t ret;

	printf("------------------------------------------------------------"
		"----\n");
	ret = get_apml_tsi_register_descriptions(i2c_bus, i2c_addr);
	if (ret)
		printf("Failed: TSI Err[%d]: %s\n", ret, esmi_get_err_msg(ret));
	printf("-----------------------------------------------------------"
		"----\n");
	return ret;
}

static void show_usage(char *exe_name) {
	printf("Usage: %s [Option<s>] SOURCES\n"
	"Option<s>:\n"
	"< MAILBOX COMMANDS >:\n"
	"  -p, (--showpower)\t\t  [I2C_BUS][I2C_ADDR]\t\t\t Get Power for a "
	"given socket in Watts\n"
	"  -t, (--showtdp)\t\t  [I2C_BUS][I2C_ADDR]\t\t\t Get TDP for a given"
	" socket in Watts\n"
	"  -s, (--setpowerlimit)\t\t  [I2C_BUS][I2C_ADDR][POWER]\t\t Set "
	"powerlimit for a given socket in mWatts\n"
	"  -c, (--showcclkfreqlimit)\t  [I2C_BUS][I2C_ADDR]\t\t\t Get "
	"cclk freqlimit for a given socket in MHz\n"
	"  -r, (--showc0residency)\t  [I2C_BUS][I2C_ADDR]\t\t\t Show "
	"c0_residency for a given socket\n"
	"  -b, (--showboostlimit)\t  [I2C_BUS][I2C_ADDR][THREAD]\t\t Get "
	"APML and BIOS boostlimit for a given core index in MHz\n"
	"  -d, (--setapmlboostlimit)\t  [I2C_BUS][I2C_ADDR][THREAD][BOOSTLIMIT]Set"
	" APML boostlimit for a given core in MHz\n"
	"  -a, (--setapmlsocketboostlimit) [I2C_BUS][I2C_ADDR][BOOSTLIMIT]\t Set"
	" APML boostlimit for all cores in a socket in MHz\n"
	"  --showddrbandwidth\t\t  [I2C_BUS][I2C_ADDR]\t\t\t Show "
	"DDR Bandwidth of a system\n"
	"  --set_and_verify_dramthrottle   [I2C_BUS][I2C_ADDR][0 to 80%%]\t\t Set"
	" DRAM THROTTLE for a given socket\n"
	"< SB-RMI COMMANDS >:\n"
	"  --showrmicommandregisters\t  [I2C_BUS][I2C_ADDR]\t\t\t Get values of"
	" SB-RMI reg commands for a given socket\n"
	"< SB-TSI COMMANDS >:\n"
	"  --showtsicommandregisters\t  [I2C_BUS][I2C_ADDR]\t\t\t Get values of"
	" SB-TSI reg commands for a given socket\n"
	"  --set_verify_updaterate	  [I2C_BUS][I2C_ADDR][Hz]\t\t Set "
	"APML Frequency Update rate for a given socket\n"
	"  --sethightempthreshold	  [I2C_BUS][I2C_ADDR][TEMP(°C)]\t\t Set "
	"APML High Temp Threshold\n"
	"  --setlowtempthreshold\t	  [I2C_BUS][I2C_ADDR][TEMP(°C)]\t\t Set "
	"APML Low Temp Threshold\n"
	"  --settempoffset\t	  [I2C_BUS][I2C_ADDR][VALUE]\t\t Set "
	"APML CPU Temp Offset, VALUE = [-CPU_TEMP(°C), 127 °C]\n"
	"  --settimeoutconfig\t	  [I2C_BUS][I2C_ADDR][VALUE]\t\t Set/Reset "
	"APML CPU timeout config, VALUE = 0 or 1\n"
	"  --setalertthreshold\t\t  [I2C_BUS][I2C_ADDR][VALUE]\t\t Set "
	"APML CPU alert threshold sample, VALUE = 1 to 8\n"
	"  --setalertconfig\t	  [I2C_BUS][I2C_ADDR][VALUE]\t\t Set/Reset "
	"APML CPU alert config, VALUE = 0 or 1\n"
	"  --setalertmask\t	  [I2C_BUS][I2C_ADDR][VALUE]\t\t Set/Reset "
	"APML CPU alert mask, VALUE = 0 or 1\n"
	"  --setrunstop\t\t	  [I2C_BUS][I2C_ADDR][VALUE]\t\t Set/Reset "
	"APML CPU runstop, VALUE = 0 or 1\n"
	"  --setreadorder\t	  [I2C_BUS][I2C_ADDR][VALUE]\t\t Set/Reset "
	"APML CPU read order, VALUE = 0 or 1\n"
	"  --setara\t\t	  [I2C_BUS][I2C_ADDR][VALUE]\t\t Set/Reset "
	"APML CPU ARA, VALUE = 0 or 1\n"
	"  -h, (--help)\t\t\t\t\t\t\t\t Show this help message\n", exe_name);
}


void show_smi_message(void)
{
	printf("\n=============== APML System Management Interface ===============\n\n");
}

void show_smi_end_message(void)
{
	printf("\n====================== End of APML SMI Log =====================\n");
}

oob_status_t show_apml_mailbox_cmds(uint32_t i2c_bus, uint32_t i2c_addr)
{
	oob_status_t ret;
	uint8_t quadrant;
	uint32_t reg_offset;
	uint32_t power_avg, power_cap, power_max;
	uint32_t tdp_avg, tdp_min, tdp_max;
	uint32_t cclk, residency;
	uint32_t max_bw, utilized_bw, utilized_pct;
	uint32_t bios_boost, esb_boost;
	uint32_t dram_thr, prochot, prochot_res;
	uint32_t vddio, nbio, iod, ccd, ccx;
	float uprat;

	printf("\n\t\t *** SB-RMI SUMMARY ***\n");
	printf("------------------------------------------------------------"
		"----\n");
	usleep(APML_SLEEP);
	ret = read_socket_power(i2c_bus, i2c_addr, &power_avg);
	if (ret)
		return ret;
	printf("| Power (Watts)\t\t |");
	printf(" %-17.3f", (double)power_avg/1000);

	usleep(APML_SLEEP);
	printf("\n| PowerLimit (Watts)\t |");
	ret = read_socket_power_limit(i2c_bus, i2c_addr, &power_cap);
	if (ret)
		return ret;
	printf(" %-17.3f", (double)power_cap/1000);

	usleep(APML_SLEEP);
	printf("\n| PowerLimitMax (Watts)\t |");
	ret = read_max_socket_power_limit(i2c_bus, i2c_addr, &power_max);
	if (ret)
		return ret;
	printf(" %-17.3f", (double)power_max/1000);

	usleep(APML_SLEEP);
	printf("\n| C0 Residency (%%)\t |");
	ret = read_socket_c0_residency(i2c_bus, i2c_addr, &residency);
	if (ret)
		return ret;
	printf(" %-17u", residency);

	usleep(APML_SLEEP);
	ret = read_ddr_bandwidth(i2c_bus, i2c_addr, &max_bw, &utilized_bw,
				 &utilized_pct);
	if (ret)
		return ret;
	printf("\n| DDR Max BW (GB/s)\t |");
	printf(" %-17d", max_bw);
	printf("\n| DDR Utilized BW (GB/s) |");
	printf(" %-17d", utilized_bw);
	printf("\n| DDR Utilized Percent(%%)|");
	printf(" %-17d", utilized_pct);

	usleep(APML_SLEEP);
	printf("\n| BIOS boostlimit(MHz)\t |");
	ret = read_bios_boost_fmax(i2c_bus, i2c_addr, 0x0, &bios_boost);
	if (ret)
		return ret;
	printf(" %-17u", bios_boost);

	usleep(APML_SLEEP);
	printf("\n| APML boostlimit(MHz)\t |");
	ret = read_esb_boost_limit(i2c_bus, i2c_addr, 0x30, &esb_boost);
	if (ret)
		return ret;
	printf(" %-17u\n", esb_boost);

	return OOB_SUCCESS;
}

void show_smi_parameters(uint32_t i2c_bus, uint32_t i2c_addr)
{
	oob_status_t mret, tret;

	tret = get_apml_tsi_register_descriptions(i2c_bus, i2c_addr);

	if (tret)
		mret = show_apml_mailbox_cmds(i2c_bus, i2c_addr);

	if (mret && tret) {
		printf("\nInvalid I2C Bus or Address\n");
		printf("Failed: For Mailbox Err[%d]: %s\n", mret, esmi_get_err_msg(mret));
		printf("Failed: For TSI Err[%d]: %s\n", tret, esmi_get_err_msg(tret));
	}
	printf("------------------------------------------------------------"
		"----\n");
}

/*
 * returns 0 if the given string is a number, else 1
 */
static int is_string_number(char *str)
{
	int i;

	for (i = 0; str[i] != '\0'; i++) {
		if (str[i] != '.' && ((str[i] < '0') || (str[i] > '9')))
			return 1;
	}
	return OOB_SUCCESS;
}

/*
 * Parse command line parameters and set data for program.
 * @param argc number of command line parameters
 * @param argv list of command line parameters
 */
oob_status_t parseesb_args(int argc, char **argv)
{
	oob_status_t ret;
	int i;
	int opt = 0; /* option character */
	uint32_t i2c_bus, i2c_addr;
	char *end;
	int power = 0;
	uint32_t boostlimit = 0, thread_ind = 0;
	uint32_t dram_thr;
	float uprate;
	float temp;
	int value;

	//Specifying the expected options
	static struct option long_options[] = {
		{"help",		no_argument,		0,	'h'},
		{"showpower",		required_argument,	0,	'p'},
		{"showtdp",		required_argument,	0,	't'},
		{"setpowerlimit",	required_argument,	0,	's'},
		{"showcclkfreqlimit",	required_argument,	0,	'c'},
		{"showc0residency",	required_argument,	0,	'r'},
		{"showddrbandwidth",	required_argument,	&flag,	 3 },
		{"showboostlimit",	required_argument,	0,	'b'},
		{"setapmlboostlimit",	required_argument,	0,	'd'},
		{"setapmlsocketboostlimit", required_argument,	0,	'a'},
		{"set_and_verify_dramthrottle", required_argument, 0,   'l'},
		{"showrmicommandregisters", required_argument,	&flag,   1 },
		{"showtsicommandregisters", required_argument,	&flag,   2 },
		{"set_verify_updaterate",   required_argument,	0,	'u'},
		{"sethightempthreshold", required_argument,	0,	'v'},
		{"setlowtempthreshold",	required_argument,	0,	'w'},
		{"settempoffset",	required_argument,	0,	'x'},
		{"settimeoutconfig",	required_argument,	0,	'f'},
		{"setalertthreshold",	required_argument,	0,	'g'},
		{"setalertconfig",	required_argument,	0,	'j'},
		{"setalertmask",	required_argument,	0,	'k'},
		{"setrunstop",		required_argument,	0,	'm'},
		{"setreadorder",	required_argument,	0,	'n'},
		{"setara",		required_argument,	0,	'o'},
		{0,			0,			0,	0},
	};

	int long_index = 0;
	char *helperstring = "+hp:t:s:c:r:b:d:a:u:v:w:x:f:g:j:k:m:n:o:";

	if (argc <= 1) {
		printf("Usage: %s <i2c_bus> <i2c_addr>\n"
			"Where:  i2c_bus : 0 or 1\n"
			"\tEg,  i2c_addr : SB-RMI addresses: "
			"\t0x3c for Socket0 and 0x38 for Socket1\n "
			"\t\t\tSB-TSI addresses: "
			"\t0x4c for Socket0 and 0x48 for Socket1\n", argv[0]);
		return 0;
	}
	/* Is someone trying without passing anything ? */
	if (argv[1][0] != '-') {
		if (is_string_number(argv[1]) || argv[2] == NULL) {
			printf("Usage: %s <i2c_bus> <i2c_addr>\n"
			"Where:  i2c_bus : 0 or 1\n"
			"\tEg,  i2c_addr : SB-RMI addresses: "
			"\t0x3c for Socket0 and 0x38 for Socket1\n "
			"\t\t\tSB-TSI addresses: "
			"\t0x4c for Socket0 and 0x48 for Socket1\n", argv[0]);
			return 0;
		}
		i2c_bus = atoi(argv[1]);
		i2c_addr = strtoul(argv[2], &end, 16);
		if (*end || !*argv[2]) {
			printf("Require a valid i2c_address in Hexa\n");
				show_usage(argv[0]);
			return OOB_SUCCESS;
		}
		show_smi_parameters(i2c_bus, i2c_addr);
		printf(RED "Try `%s --help' for more information." RESET"\n",
							argv[0]);
		return 0;
	}

	optind = 0;
	while ((opt = getopt_long(argc, argv, helperstring,
				  long_options, &long_index)) != -1) {
	if (opt == 'p' ||
	    opt == 't' ||
	    opt == 'c' ||
	    opt == 'r' ||
	    opt == 's' ||
	    opt == 'b' ||
	    opt == 'd' ||
	    opt == 'a' ||
	    opt == 'l' ||
	    opt == 'u' ||
	    opt == 'v' ||
	    opt == 'w' ||
	    opt == 'x' ||
	    opt == 'f' ||
	    opt == 'g' ||
	    opt == 'j' ||
	    opt == 'k' ||
	    opt == 'm' ||
	    opt == 'n' ||
	    opt == 'o' ||
	    opt == 0) {
		if (is_string_number(optarg) || argv[optind] == NULL) {
			printf("Option '-%c' requires valid i2c_bus, i2c_add\n",
				opt);
				show_usage(argv[0]);
			return OOB_SUCCESS;
		}
		i2c_bus = atoi(optarg);
		i2c_addr = strtoul(argv[optind], &end, 16);
		if (*end || !*argv[optind++]) {
			printf("Option '-%c' require a valid i2c_address"
				" \n\n", opt);
				show_usage(argv[0]);
			return OOB_SUCCESS;
		}
	}
	if (opt == 's' ||
	    opt == 'b' ||
	    opt == 'a' ||
	    opt == 'l' ||
	    opt == 'd' ||
	    opt == 'f' ||
	    opt == 'g' ||
	    opt == 'k' ||
	    opt == 'm' ||
	    opt == 'n' ||
	    opt == 'o' ||
	    opt == 'j' ||
	    opt == 'u' ||
	    opt == 'v' ||
	    opt == 'x' ||
	    opt == 'w') {
		// make sure optind is valid  ... or another option
		if ((optind) >= argc) {
			printf("\nOption '-%c' require THREE or more arguments"
				"\n\n", opt);
			show_usage(argv[0]);
			return OOB_SUCCESS;
		}
		if (argv[optind][0] == '-') {
			if (is_string_number(&argv[optind][1])) {
				printf("\nOption '-%c' require THREE or more arguments"
					"\n\n", opt);
				show_usage(argv[0]);
				return OOB_SUCCESS;
			}
		} else if (is_string_number(argv[optind])) {
			printf("Option '-%c' require 3rd argument as valid"
				" numeric value\n\n", opt);
			show_usage(argv[0]);
			return OOB_SUCCESS;
		}
	}
	if (opt == 'd') {
		if ((optind + 1) >= argc || *argv[optind + 1] == '-') {
			printf("\nOption '-%c' require FOUR arguments"
				" <i2c_bus, <i2c_addr> <thread> <set_value>\n",
				opt);
			show_usage(argv[0]);
			return OOB_SUCCESS;
		}
		if (is_string_number(argv[optind + 1])) {
			printf("Option '-%c' require 4th argument as valid"
				" numeric value\n\n", opt);
			show_usage(argv[0]);
			return OOB_SUCCESS;
		}
	}
	switch (opt) {
	case 0:
		if (*(long_options[long_index].flag) == 1)
			get_apml_rmi_access(i2c_bus, i2c_addr);
		else if (*(long_options[long_index].flag) == 2)
			get_apml_tsi_access(i2c_bus, i2c_addr);
		else if (*(long_options[long_index].flag) == 3)
			apml_get_ddr_bandwidth(i2c_bus, i2c_addr);
		break;
	case 'p':
		/* Get the power metrics for a given i2c_bus, addr */
		apml_get_sockpower(i2c_bus, i2c_addr);
		break;
	case 't':
		/* Get tdp value for a given i2c_bus, i2c_addr */
		apml_get_socktdp(i2c_bus, i2c_addr);
		break;
	case 's':
		power = atoi(argv[optind++]);
		apml_setpower_limit(i2c_bus, i2c_addr, power);
		break;
	case 'c':
		/* Get cclk freqlimit for a given i2c_bus, i2c_addr */
		apml_get_cclk_freqlimit(i2c_bus, i2c_addr);
		break;
	case 'r':
		/* Get c0_residency for a given i2c_bus, i2c_addr */
		apml_get_sockc0_residency(i2c_bus, i2c_addr);
		break;
	case 'b':
		/* Get apml boostlimit for a given i2c_bus, i2c_addr
		 * and thread index */
		thread_ind = atoi(argv[optind++]);
		get_boostlimit(i2c_bus, i2c_addr, thread_ind);
		break;
	case 'd':
		thread_ind = atoi(argv[optind++]);
		boostlimit = atoi(argv[optind++]);
		set_apml_boostlimit(i2c_bus, i2c_addr, thread_ind,
				    boostlimit);
		break;
	case 'a':
		boostlimit = atoi(argv[optind++]);
		set_apml_socket_boostlimit(i2c_bus, i2c_addr,
					   boostlimit);
		break;
	case 'l':
		dram_thr = atoi(argv[optind++]);
		set_and_verify_dram_throttle(i2c_bus, i2c_addr,
					     dram_thr);
		break;
	case 'u':
		uprate = atof(argv[optind++]);
		set_and_verify_apml_socket_uprate(i2c_bus, i2c_addr,
						  uprate);
		break;
	case 'v':
		temp = atof(argv[optind++]);
		set_high_temp_threshold(i2c_bus, i2c_addr, temp);
		break;
	case 'w':
		temp = atof(argv[optind++]);
		set_low_temp_threshold(i2c_bus, i2c_addr, temp);
		break;
	case 'x':
		temp = atof(argv[optind++]);
		set_temp_offset(i2c_bus, i2c_addr, temp);
		break;
	case 'f':
		value = atoi(argv[optind++]);
		set_timeout_config(i2c_bus, i2c_addr, value);
		break;
	case 'g':
		value = atoi(argv[optind++]);
		set_alert_threshold(i2c_bus, i2c_addr, value);
		break;
	case 'j':
		value = atoi(argv[optind++]);
		set_alert_config(i2c_bus, i2c_addr, value);
		break;
	case 'k':
		value = atoi(argv[optind++]);
		set_tsi_config(i2c_bus, i2c_addr, value, 'k');
		break;
	case 'm':
		value = atoi(argv[optind++]);
		set_tsi_config(i2c_bus, i2c_addr, value, 'm');
		break;
	case 'n':
		value = atoi(argv[optind++]);
		set_tsi_config(i2c_bus, i2c_addr, value, 'n');
		break;
	case 'o':
		value = atoi(argv[optind++]);
		set_tsi_config(i2c_bus, i2c_addr, value, 'o');
		break;
	case 'h':
		show_usage(argv[0]);
		return OOB_SUCCESS;
	case ':':
		/* missing option argument */
		printf(RED "%s: option '-%c' requires an argument."
			RESET"\n\n", argv[0], optopt);
		break;
	case '?':
		if (isprint(optopt))
			printf(RED "Try `%s --help' for more"
			" information." RESET "\n", argv[0]);
		else
			printf("Unknown option character `\\x%x'.\n",
				optopt);
		return OOB_SUCCESS;
	default:
		printf(RED "Try `%s --help' for more information."
					RESET "\n\n", argv[0]);
		return OOB_SUCCESS;
	} // end of Switch
	}

	for (i = optind; i < argc; i++) {
		printf(RED "\nExtra Non-option argument<s> passed : %s"
				RESET"\n", argv[i]);
		printf(RED "Try `%s --help' for more information."
				RESET"\n", argv[0]);
		return OOB_SUCCESS;
	}

	return OOB_SUCCESS;
}

static void rerun_sudo(int argc, char **argv)
{
	static char *args[ARGS_MAX];
	char sudostr[] = "sudo";
	int i;

	args[0] = sudostr;
	for (i = 0; i < argc; i++)
		args[i + 1] = argv[i];
	args[i + 1] = NULL;
	execvp("sudo", args);
}

/*
 * Main program.
 * @param argc number of command line parameters
 * @param argv list of command line parameters
 */
int main(int argc, char **argv)
{
	oob_status_t ret;
	uint32_t i2c_bus, i2c_addr;

	if (getuid() != 0) {
		rerun_sudo(argc, argv);
	}
	show_smi_message();

	/* Parse command arguments */
	parseesb_args(argc, argv);
	show_smi_end_message();

	return OOB_SUCCESS;
}
