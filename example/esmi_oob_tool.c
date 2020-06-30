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

void show_smi_parameters(void);
void show_smi_message(void);
void show_smi_end_message(void);
static int flag;

oob_status_t apml_get_sockpower(uint32_t sock_id)
{
	oob_status_t ret;
	uint32_t power = 0;

	ret = read_socket_power(sock_id, &power);
	if (ret != OOB_SUCCESS) {
		printf("Failed: to get socket[%d] power, Err[%d]: %s\n",
			sock_id, ret, esmi_get_err_msg(ret));
		return ret;
	}
	printf("socket[%d]/power: %16.03f Watts\n",
		sock_id, (double)power/1000);

	/* Get the PowerLimit for a given socket index */
	ret = read_socket_power_limit(sock_id, &power);
	if (ret != OOB_SUCCESS) {
		printf("Failed: to get socket[%d] powerlimit, Err[%d]: %s\n",
			sock_id, ret, esmi_get_err_msg(ret));
		return ret;
	}
	printf("socket[%d]/powerlimit: %12.03f Watts\n",
		sock_id, (double)power/1000);

	/* Get the maxpower for a given socket index */
	ret = read_max_socket_power_limit(sock_id, &power);
	if (ret != OOB_SUCCESS) {
		printf("Failed: to get socket[%d] maxpower, Err[%d]: %s\n",
			sock_id, ret, esmi_get_err_msg(ret));
		return ret;
	}
	printf("socket[%d]/max_power_limit: %7.03f Watts\n\n",
		sock_id, (double)power/1000);
	return OOB_SUCCESS;
}

oob_status_t apml_get_socktdp(uint32_t socket)
{
	oob_status_t ret;
	uint32_t buffer = 0;

	ret = read_tdp(socket, &buffer);
	if (ret != OOB_SUCCESS) {
		printf("Failed: to get socket[%d] tdp, Err[%d]: %s\n",
			socket, ret, esmi_get_err_msg(ret));
		return ret;
	}
	printf("socket[%d]/TDP: %16.03f Watts\n", socket,
		(double)buffer/1000);

	/* Get min tdp value for a given socket */
	ret = read_min_tdp(socket, &buffer);
	if (ret != OOB_SUCCESS) {
		printf("Failed: to get socket[%d] min tdp, Err[%d]: %s\n",
			socket, ret, esmi_get_err_msg(ret));
		return ret;
	}
	printf("socket[%d]/Min_TDP: %12.03f Watts\n", socket,
		(double)buffer/1000);

	/* Get max tdp value for a given socket */
	ret = read_max_tdp(socket, &buffer);
	if (ret != OOB_SUCCESS) {
		printf("Failed: to get socket[%d] max_tdp, Err[%d]: %s\n",
			socket, ret, esmi_get_err_msg(ret));
		return ret;
	}
	printf("socket[%d]/Max_TDP: %12.03f Watts\n\n", socket,
		(double)buffer/1000);
	return OOB_SUCCESS;
}

oob_status_t apml_setpower_limit(uint32_t sock_id, uint32_t power)
{
	oob_status_t ret;
	uint32_t max_power = 0;

	ret = read_max_socket_power_limit(sock_id, &max_power);
	if ((ret == OOB_SUCCESS) && (power > max_power)) {
		printf("Input power is more than max limit,"
			" So It set's to default max %.3f Watts\n",
			(double)max_power/1000);
		power = max_power;
	}
	ret = write_socket_power_limit(sock_id, power);
	if (ret != OOB_SUCCESS) {
		printf("Failed: to set socket[%d] power_limit, Err[%d]: %s\n",
			sock_id, ret, esmi_get_err_msg(ret));
		return ret;
	}
	printf("Set socket[%d]/power_limit : %16.03f Watts successfully\n",
		sock_id, (double)power/1000);
	return OOB_SUCCESS;
}

oob_status_t apml_get_cclk_freqlimit(uint32_t socket)
{
	oob_status_t ret;
	uint32_t buffer = 0;

	ret = read_cclk_freq_limit(socket, &buffer);
	if (ret != OOB_SUCCESS) {
		printf("Failed: to get socket[%d] cclk_freqlimit, Err[%d]:%s\n",
			socket, ret, esmi_get_err_msg(ret));
		return ret;
	}
	printf("socket[%d]/cclk_freqlimit: %u MHz\n\n", socket, buffer);
	return OOB_SUCCESS;
}

oob_status_t apml_get_sockc0_residency(uint32_t socket)
{
	oob_status_t ret;
	uint32_t buffer = 0;

	ret = read_socket_c0_residency(socket, &buffer);
	if (ret != OOB_SUCCESS) {
		printf("Failed: to get socket[%d] c0_residency, Err[%d]: %s\n",
			socket, ret, esmi_get_err_msg(ret));
		return ret;
	}
	printf("socket[%d]/c0_residency: %u%%\n\n", socket, buffer);
	return OOB_SUCCESS;
}

oob_status_t get_boostlimit(uint32_t socket, uint32_t core_id)
{
	oob_status_t ret;
	uint32_t buffer = 0;

	ret = read_esb_boost_limit(socket, core_id, &buffer);
	if (ret != OOB_SUCCESS) {
		printf("Failed: to get socket[%d]/core[%d] apml_boostlimit,"
			" Err[%d]: %s\n", socket, core_id, ret,
			esmi_get_err_msg(ret));
		return ret;
	}
	printf("socket[%d]/core[%d] apml_boostlimit: '%u MHz'\n\n",
		socket, core_id, buffer);

	usleep(1000);
	/* Get the Bios boostlimit for a given socket index */
	ret = read_bios_boost_fmax(socket, core_id, &buffer);
	if (ret != OOB_SUCCESS) {
		printf("Failed: to get socket[%d]/core[%d] bios_boostlimit, "
			"Err[%d]: %s\n", socket, core_id, ret,
			esmi_get_err_msg(ret));
		return ret;
	}
	printf("socket[%d]/core[%d] bios_boostlimit: '%u MHz'\n\n",
		socket, core_id, buffer);
	return OOB_SUCCESS;
}

oob_status_t set_apml_boostlimit(uint32_t socket, uint32_t core_id,
				 uint32_t boostlimit)
{
	oob_status_t ret;
	uint32_t blimit = 0;

	ret = write_esb_boost_limit(socket, core_id, boostlimit);
	if (ret != OOB_SUCCESS) {
		printf("Failed: to set socket[%d]/core[%d] apml_boostlimit "
			"Err[%d]: %s\n", socket, core_id, ret,
			esmi_get_err_msg(ret));
		return ret;
	}
	usleep(1000);
	ret = read_esb_boost_limit(socket, core_id, &blimit);
	if (ret == OOB_SUCCESS) {
		if (blimit < boostlimit) {
			printf("Set to max boost limit: %u MHz\n", blimit);
		} else if (blimit > boostlimit) {
			printf("Set to min boost limit: %u MHz\n", blimit);
		}
	}
	printf("socket[%d]/core[%d] apml_boostlimit set successfully\n",
		socket, core_id);
	return OOB_SUCCESS;
}

oob_status_t set_apml_socket_boostlimit(uint32_t socket, uint32_t boostlimit)
{
	oob_status_t ret;
	uint32_t blimit = 0;

	ret = write_esb_boost_limit_allcores(socket, boostlimit);
	if (ret != OOB_SUCCESS) {
		printf("Failed: to set apml_boostlimit for all cores"
			"Err[%d]: %s\n", ret, esmi_get_err_msg(ret));
		return ret;
	}
	usleep(1000);
	ret = read_esb_boost_limit(socket, 0, &blimit);
	if (ret == OOB_SUCCESS) {
		if (blimit < boostlimit) {
			printf("Set to max boost limit: %u MHz\n", blimit);
		} else if (blimit > boostlimit) {
			printf("Set to min boost limit: %u MHz\n", blimit);
		}
	}
	printf("apml_boostlimit for all cores set successfully\n");
	return OOB_SUCCESS;
}

oob_status_t set_and_verify_dram_throttle(uint32_t socket, uint32_t dram_thr)
{
	oob_status_t ret;
	uint32_t limit = 0;

	ret = write_dram_throttle(socket, dram_thr);
	if (ret != OOB_SUCCESS) {
		printf("Failed: to set DRAM throttle, "
			"Err[%d]: %s\n", ret, esmi_get_err_msg(ret));
		return ret;
	}
	usleep(1000);
	ret = read_dram_throttle(socket, &limit);
	if (ret == OOB_SUCCESS) {
		if (dram_thr != limit) {
			return OOB_TRY_AGAIN;
		}
	}
	printf("Set and Verify Success\n");
	return OOB_SUCCESS;
}

oob_status_t set_and_verify_apml_socket_uprate(uint32_t socket, float uprate)
{
	oob_status_t ret;
	float rduprate;

	ret = write_sbtsi_updateratehz(socket, uprate);
	if (ret != OOB_SUCCESS) {
		printf("Failed: to set Update rate for a socket"
			"Err[%d]: %s\n", ret, esmi_get_err_msg(ret));
		return ret;
	}
	usleep(1000);
	if (read_sbtsi_updateratehz(socket, &rduprate) == 0) {
		if (uprate != rduprate) {
			return OOB_TRY_AGAIN;
		} else {
			printf("Set and verify Success\n");
		}
	}
	return OOB_SUCCESS;
}

static oob_status_t set_high_temp_threshold(uint32_t socket, float temp)
{
	oob_status_t ret;
	int temp_int;
	float temp_dec;
	if (temp < 0 || temp > 70) {
		printf("Invalid temp, please mention temp between 0 and 70\n");
		return OOB_INVALID_INPUT;
	}
	temp_int = temp;
	temp_dec = (temp - temp_int);

	ret = sbtsi_set_hightemp_threshold(socket, temp_int, temp_dec);
	if (ret != OOB_SUCCESS) {
		printf("Failed: to set Higher Temp threshold limit"
			"Err[%d]: %s\n", ret, esmi_get_err_msg(ret));
		return ret;
	}
	printf("Set Success\n");
	return OOB_SUCCESS;
}

static oob_status_t set_low_temp_threshold(uint32_t socket, float temp)
{
	oob_status_t ret;
	int temp_int;
	float temp_dec;
	if (temp < 0 || temp > 70) {
		printf("Invalid temp, please mention temp between 0 and 70\n");
		return OOB_INVALID_INPUT;
	}
	temp_int = temp;
	temp_dec = (temp - temp_int);

	ret = sbtsi_set_lowtemp_threshold(socket, temp_int, temp_dec);
	if (ret != OOB_SUCCESS) {
		printf("Failed: to set Lower Temp threshold limit"
			"Err[%d]: %s\n", ret, esmi_get_err_msg(ret));
		return ret;
	}
	printf("Set Success\n");
	return OOB_SUCCESS;
}

static oob_status_t set_temp_offset(uint32_t socket, float temp)
{
	oob_status_t ret;

	ret = write_sbtsi_cputempoffset(socket, temp);
	if (ret != OOB_SUCCESS) {
		printf("Failed: to set Temp offset, "
			"Err[%d]: %s\n", ret, esmi_get_err_msg(ret));
		return ret;
	}
	printf("Set CPU temp offset success\n");
	return OOB_SUCCESS;
}

static oob_status_t set_timeout_config(uint32_t socket, int value)
{
	oob_status_t ret;

	printf("VALUE: %d", value);
	if (value < 0 || value > 1) {
		printf("Invalid Value. Argument = 0 : disable timeout \
				Argument = 1 : enable timeout\n");
		return OOB_INVALID_INPUT;
	}
	ret = sbtsi_set_timeout_config(socket, value);
	if (ret != OOB_SUCCESS) {
		printf("Failed: to set timeout config, "
			"Err[%d]: %s\n", ret, esmi_get_err_msg(ret));
		return ret;
	}
	printf("Set timeout config success\n");
	return OOB_SUCCESS;
}

static oob_status_t set_alert_threshold(uint32_t socket, int value)
{
	oob_status_t ret;

	if (value < 1 || value > 8) {
		printf("Value => number of sample, value = [1, 8]\n");
		return OOB_INVALID_INPUT;
	}
	ret = sbtsi_set_alert_threshold(socket, value);
	if (ret != OOB_SUCCESS) {
		printf("Failed: to set alert threshold sample, "
			"Err[%d]: %s\n", ret, esmi_get_err_msg(ret));
		return ret;
	}
	printf("Set alert threshold success\n");
	return OOB_SUCCESS;
}

static oob_status_t set_alert_config(uint32_t socket, int value)
{
	oob_status_t ret;

	if (value < 0 || value > 1) {
		printf("Invalid Value. Argument = 0 : Alert config disable \
				Argument = 1 : Alert config enable\n");
		return OOB_INVALID_INPUT;
	}

	ret = sbtsi_set_alert_config(socket, value);
	if (ret != OOB_SUCCESS) {
		printf("Failed: to set alert config, "
			"Err[%d]: %s\n", ret, esmi_get_err_msg(ret));
		return ret;
	}
	printf("Set alert config success\n");
	return OOB_SUCCESS;
}

static oob_status_t set_tsi_config(uint32_t socket, int value, const char check)
{
	oob_status_t ret;

	if (value < 0 || value > 1) {
		printf("Invalid Value. Argument = 0 : Alert config disable \
				Argument = 1 : Alert config enable\n");
		return OOB_INVALID_INPUT;
	}
	switch(check) {
		case 'k':
			ret = sbtsi_set_tsi_config(socket, value, ALERTMASK_MASK);
			if (ret != OOB_SUCCESS) {
				printf("Failed: to set tsi config alert_mask, "
					"Err[%d]: %s\n", ret, esmi_get_err_msg(ret));
				return ret;
			}
			break;
		case 'm':
			ret = sbtsi_set_tsi_config(socket, value, RUNSTOP_MASK);
			if (ret != OOB_SUCCESS) {
				printf("Failed: to set tsi config runstop_mask, "
					"Err[%d]: %s\n", ret, esmi_get_err_msg(ret));
				return ret;
			}
			break;
		case 'n':
			ret = sbtsi_set_tsi_config(socket, value, READORDER_MASK);
			if (ret != OOB_SUCCESS) {
				printf("Failed: to set tsi config radorder_mask, "
					"Err[%d]: %s\n", ret, esmi_get_err_msg(ret));
				return ret;
			}
			break;
		case 'o':
			ret = sbtsi_set_tsi_config(socket, value, ARA_MASK);
			if (ret != OOB_SUCCESS) {
				printf("Failed: to set tsi config ara_mask, "
					"Err[%d]: %s\n", ret, esmi_get_err_msg(ret));
				return ret;
			}
	}
	printf("Set TSI config bit success\n");
	return OOB_SUCCESS;
}

oob_status_t get_apml_rmi_access(uint32_t socket)
{
	int8_t buf;
	oob_status_t ret;

	printf("_FUNCTION		| Socket |    Value Units |\n");
	ret = read_sbrmi_revision(socket, &buf);
	if (ret != 0) {
		printf("Err[%d]:%s\n", ret, esmi_get_err_msg(ret));
		return ret;
	}
	printf("_RMI_REVISION		| %6d | %14d |\n", socket, buf);

	usleep(1000);
	if (read_sbrmi_control(socket, &buf) == 0)
	printf("_RMI_CONTROL		| %6d | %#14x |\n", socket, buf);

	usleep(1000);
	if (read_sbrmi_status(socket, &buf) == 0)
	printf("_RMI_STATUS		| %6d | %#14x |\n", socket, buf);

	usleep(1000);
	if (read_sbrmi_readsize(socket, &buf) == 0)
	printf("_RMI_READSIZE		| %6d | %#14x |\n", socket, buf);

	usleep(1000);
	if (read_sbrmi_threadenablestatus(socket, &buf) == 0)
	printf("_RMI_THREADENABLESTATUS	| %6d | %#14x |\n", socket,
		(uint8_t)buf);

	usleep(1000);
	if (read_sbrmi_swinterrupt(socket, &buf) == 0)
	printf("_RMI_SWINTERRUPT	| %6d | %#14x |\n", socket,
		buf);

	usleep(1000);
	if (read_sbrmi_threadnumber(socket, &buf) == 0)
	printf("_RMI_THREADNUMEBER	| %6d | %#14x |\n", socket,
		(uint8_t)buf);
}

oob_status_t get_apml_tsi_register_descriptions(uint32_t socket)
{
	uint8_t lowalert, hialert;
	uint8_t al_mask, run_stop, read_ord, ara;
	uint8_t timeout;
	int8_t intr;
	float dec;
	float temp_value;
	float uprate;
	uint8_t id, buf;

	if (sbtsi_get_cputemp(socket, &temp_value) == 0) {
		printf("_CPUTEMP\t\t| %.3f °C \n", temp_value);
	}

	usleep(1000);
	if (sbtsi_get_htemp_threshold(socket, &intr, &dec) == 0) {
		temp_value = intr + dec;
		printf("_HIGH_THRESHOLD_TEMP\t| %.3f °C \n", temp_value);
	}

	usleep(1000);
	if (sbtsi_get_ltemp_threshold(socket, &intr, &dec) == 0) {
		temp_value = intr + dec;
		printf("_LOW_THRESHOLD_TEMP \t| %.3f °C \n", temp_value);
	}

	usleep(1000);
	if (read_sbtsi_updateratehz(socket, &uprate) == 0)
		printf("_TSI_UPDATERATE\t\t| %.3f Hz\n", uprate);

	usleep(1000);
	if (read_sbtsi_alertthreshold(socket, &buf) == 0)
		printf("_THRESHOLD_SAMPLE\t| %d\n", buf);

	usleep(1000);
	if (read_sbtsi_cputempoffset(socket, &dec) == 0) {
		printf("_TEMP_OFFSET\t\t| %.3f °C \n", dec);
	}

	usleep(1000);
	if (sbtsi_get_temp_status(socket, &lowalert, &hialert) == 0) {
		printf("_STATUS\t\t\t| ");
		if (lowalert) {
			printf("CPU Temp Low Alert\n");
		} else if (hialert) {
			printf("CPU Temp Hi Alert\n");
		} else {
			printf("No Temp Alert\n");
		}
	}

	usleep(1000);
	if (sbtsi_get_config(socket, &al_mask, &run_stop,
			     &read_ord, &ara) == 0) {
		printf("_CONFIG\t\t\t| \n");
		if (al_mask) {
			printf("\tALERT_L pin\t| Disabled\n");
		} else {
			printf("\tALERT_L pin\t| Enabled\n");
		}
		if(run_stop) {
			printf("\tRunstop\t\t| Disabled\n");
		} else {
			printf("\tRunstop\t\t| Enabled\n");
		}
		if (read_ord) {
			printf("\tAtomic rd order\t| Enabled\n");
		} else {
			printf("\tAtomic rd order\t| Disabled\n");
		}
		if (ara) {
			printf("\tARA response\t| Disabled\n");
		} else {
			printf("\tARA response\t| Enabled\n");
		}
	}

	usleep(1000);
	if (sbtsi_get_timeout(socket, &timeout) == 0) {
		printf("_TIMEOUT_CONFIG\t\t| ");
		if (timeout) {
			printf("Enabled\n");
		} else {
			printf("Disabled\n");
		}
	}

	usleep(1000);
	if (read_sbtsi_alertconfig(socket, &buf) == 0) {
		if (buf == 0)
			printf("_TSI_ALERT_CONFIG\t| Disabled\n");
		else
			printf("_TSI_ALERT_CONFIG\t| Enabled\n");
	}

	usleep(1000);
	if (read_sbtsi_manufid(socket, &id) == 0)
		printf("_TSI_MANUFACTURE_ID\t| %#x\n", id);

	usleep(1000);
	if (read_sbtsi_revision(socket, &id) == 0)
		printf("_TSI_REVISION\t\t| %#x\n", id);

}

oob_status_t get_apml_tsi_access(uint32_t socket)
{

	printf("\t\t *** SB-TSI UPDATE ***  \t\t \n");
	printf("Socket:%d\n", socket);
	printf("------------------------------------------------------------"
		"--------------------\n");
	get_apml_tsi_register_descriptions(socket);
	printf("-----------------------------------------------------------"
		"--------------------\n");
}

static void show_usage(char *exe_name) {
	printf("Usage: %s [Option<s>] SOURCES\n"
	"Option<s>:\n"
	"< MAILBOX COMMANDS >:\n"
	"\t-p, (--showpower)\t [SOCKET]\t\t\tGet Power for a given"
	" socket in Watt\n"
	"\t-t, (--showtdp)\t\t [SOCKET]\t\t\tGet TDP for a given"
	" socket in Watt\n"
	"\t-s, (--setpowerlimit)\t [SOCKET][POWER]\t\tSet powerlimit for a"
	" given socket in mWatt\n"
	"\t-c, (--showcclkfreqlimit)[SOCKET]\t\t\tGet cclk freqlimit for a"
	" given socket in MHz\n"
	"\t-r, (--showc0residency)\t [SOCKET]\t\t\tShow socket c0_residency"
	" given socket\n"
	"\t-b, (--showboostlimit)   [SOCKET][THREAD]\t\tGet APML and BIOS"
	" boostlimit for a given socket and core index in MHz\n"
	"\t-d, (--setapmlboostlimit)[SOCKET][THREAD][BOOSTLIMIT]   Set "
	"APML boostlimit for a given socket and core in MHz\n"
	"\t-a, (--setapmlsocketboostlimit)  [SOCKET][BOOSTLIMIT]   Set "
	"APML boostlimit for all cores in a socket in MHz\n"
	"\t--set_and_verify_dramthrottle    [SOCKET][0 to 80%%]     Set "
	"DRAM THROTTLE for a given socket\n"
	"< SB-RMI COMMANDS >:\n"
	"\t--showrmicommandregisters [SOCKET]\t\t\tGet the values of different"
	" commands of SB-RMI registers for a given socket\n"
	"< SB-TSI COMMANDS >:\n"
	"\t--showtsicommandregisters [SOCKET]\t\t\tGet the values of different"
	" commands of SB-TSI registers for a given socket\n"
	"\t--set_verify_updaterate	  [SOCKET][Hz]\t\t\tSet "
	"APML Frequency Update rate for a socket\n"
	"\t--sethightempthreshold	  [SOCKET][TEMP(°C)]\t\tSet "
	"APML High Temp Threshold\n"
	"\t--setlowtempthreshold	  [SOCKET][TEMP(°C)]\t\tSet "
	"APML Low Temp Threshold\n"
	"\t--settempoffset\t	  [SOCKET][VALUE]\t\tSet "
	"APML CPU Temp Offset, VALUE = [-CPU_TEMP(°C), 127 °C]\n"
	"\t--settimeoutconfig	  [SOCKET][VALUE]\t\tSet/Reset "
	"APML CPU timeout config, VALUE = 0 or 1\n"
	"\t--setalertthreshold	  [SOCKET][VALUE]\t\tSet "
	"APML CPU alert threshold sample, VALUE = 1 to 8\n"
	"\t--setalertconfig	  [SOCKET][VALUE]\t\tSet/Reset "
	"APML CPU alert config, VALUE = 0 or 1\n"
	"\t--setalertmask\t	  [SOCKET][VALUE]\t\tSet/Reset "
	"APML CPU alert mask, VALUE = 0 or 1\n"
	"\t--setrunstop\t	  [SOCKET][VALUE]\t\tSet/Reset "
	"APML CPU runstop, VALUE = 0 or 1\n"
	"\t--setreadorder\t	  [SOCKET][VALUE]\t\tSet/Reset "
	"APML CPU read order, VALUE = 0 or 1\n"
	"\t--setara\t	  [SOCKET][VALUE]\t\tSet/Reset "
	"APML CPU ARA, VALUE = 0 or 1\n"
	"\t-h, (--help)\t\t\t\t\t\tShow this help message\n", exe_name);
}


void show_smi_message(void)
{
	printf("\n=============== APML System Management Interface ===============\n\n");
}

void show_smi_end_message(void)
{
	printf("\n====================== End of APML SMI Log =====================\n");
}

void show_smi_parameters(void)
{
	oob_status_t ret;
	static int i;
	uint8_t quadrant;
	uint32_t reg_offset;
	uint32_t power_avg, power_cap, power_max;
	uint32_t tdp_avg, tdp_min, tdp_max;
	uint32_t cclk, residency;
	uint32_t bios_boost, esb_boost;
	uint32_t dram_thr, prochot, prochot_res;
	uint32_t vddio, nbio, iod, ccd, ccx;
	float uprat;

	usleep(1000);
	for (i = 0; i < 2; i++) {
	printf("---------------------------------------------------------------"
		"-----------------\n");
	printf("\t\t\tSOCKET %d\n", i);
	printf("---------------------------------------------------------------"
		"-----------------\n");
	printf("*** SB-RMI Mailbox Service Access ***\n");
	printf("_POWER	(Watts)\t\t|");
	if (read_socket_power(i, &power_avg) == 0) {
		printf(" Avg : %.03f, ", (double)power_avg/1000);
	}

	usleep(1000);
	if (read_socket_power_limit(i, &power_cap) == 0) {
		printf(" Limit : %.03f, ", (double)power_cap/1000);
	}

	usleep(1000);
	if (read_max_socket_power_limit(i, &power_max) == 0) {
		printf(" Max : %.03f \n", (double)power_max/1000);
	}

	usleep(1000);
	printf("_TDP	(Watts)\t\t|");
	if (read_tdp(i, &tdp_avg) == 0) {
		printf(" Avg : %.03f, ", (double)tdp_avg/1000);
	}

	usleep(1000);
	if (read_min_tdp(i, &tdp_min) == 0) {
		printf(" Minim : %.03f, ", (double)tdp_min/1000);
	}

	usleep(1000);
	if (read_max_tdp(i, &tdp_max) == 0) {
		printf(" Max : %.03f\n", (double)tdp_max/1000);
	}

	usleep(1000);
	printf("_CCLK_FREQ_LIMIT (MHz)\t|");
	if (read_cclk_freq_limit(i, &cclk) == 0) {
		printf(" %u\n", cclk);
	}

	usleep(1000);
	printf("_C0_RESIDENCY (in %%)\t|");
	if (read_socket_c0_residency(i, &residency) == 0) {
		printf(" %u%%\n", residency);
	}

	usleep(1000);
	printf("_BOOST_LIMIT (MHz)\t|");
	if (read_bios_boost_fmax(i, 0x2, &bios_boost) == 0) {
		printf(" BIOS: %u, ", bios_boost);
	}

	usleep(1000);
	if (read_esb_boost_limit(i, 0x30, &esb_boost) == 0) {
		printf(" APML: %u\n", esb_boost);
	}

	usleep(1000);
	printf("_DRAM_THROTTLE	(in %%) \t|");
	if (read_dram_throttle(i, &dram_thr) == 0) {
		printf(" %u\n", dram_thr);
	}

	usleep(1000);
	printf("_PROCHOT STATUS\t\t|");
	if (read_prochot_status(i, &prochot) == 0) {
		printf(" %s\n", prochot ? "PROCHOT" : "NOT_PROCHOT");
	}

	usleep(1000);
	printf("_PROCHOT RESIDENCY (MHz)|");
	if (read_prochot_residency(i, &prochot_res) == 0) {
		printf(" %u\n", prochot_res);
	}

	usleep(1000);
	printf("_VDDIOMem_POWER (mWatts)|");
	if (read_vddio_mem_power(i, &vddio) == 0) {
		printf(" %u\n", vddio);
	}

	usleep(1000);
	printf("_NBIO_Error_Logging_Reg\t|");
	quadrant = 0x03;
	reg_offset = 0x20;
	if (read_nbio_error_logging_register(i, quadrant,
					     reg_offset, &nbio) == 0) {
		printf(" %u\n", nbio);
	}
	usleep(1000);
	printf("_IOD_Bist_RESULT\t|");
	if (read_iod_bist(i, &iod) == 0) {
		printf(" %s\n", iod ? "Bist fail" : "Bist pass");
	}

	usleep(1000);
	printf("_CCD_Bist_RESULT\t|");
	if (read_ccd_bist_result(i, 0x2, &ccd) == 0) {
		printf(" %s\n", ccd ? "Bist fail" : "Bist pass");
	}

	usleep(1000);
	printf("_CCX_Bist_RESULT\t|");
	if (read_ccx_bist_result(i, 0x2, &ccx) == 0) {
		printf(" %u\n",ccx);
	}
	printf("\n*** SB-TSI UPDATE ***\n");
	get_apml_tsi_register_descriptions(i);
	}
	printf("---------------------------------------------------------------"
		"-----------------\n");
}

/*
 * returns 0 if the given string is a number, else 1
 */
static int is_string_number(char *str)
{
	int i;
	for (i = 0; str[i] != '\0'; i++) {
		if ((str[i] < '0') || (str[i] > '9')) {
			return 1;
		}
	}
	return OOB_SUCCESS;
}

/**
Parse command line parameters and set data for program.
@param argc number of command line parameters
@param argv list of command line parameters
*/
oob_status_t parseesb_args(int argc,char **argv)
{
	oob_status_t ret;
	int i;
	int opt = 0; /* option character */
	uint32_t socket = 0, power = 0;
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
		{"showboostlimit",	required_argument,	0,	'b'},
		{"setapmlboostlimit",	required_argument,	0,	'd'},
		{"setapmlsocketboostlimit", required_argument,	0,	'a'},
		{"set_and_verify_dramthrottle", required_argument, 0,   'l'},
		{"showrmicommandregisters", required_argument,	&flag,	1},
		{"showtsicommandregisters", required_argument,	&flag,	2},
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

	/* Is someone trying without passing anything ? */
	if (argc <= 1) {
		show_smi_parameters();
		printf(RED "Try `%s --help' for more information." RESET"\n",
							argv[0]);
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
		if (is_string_number(optarg)) {
			printf("Option '-%c' require a valid numeric value"
				" as an argument\n\n", opt);
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
	    opt == 'j') {
		// make sure optind is valid  ... or another option
		if (optind >= argc || *argv[optind] == '-') {
			printf("\nOption '-%c' require TWO or more arguments"
				"\n\n", opt);
			show_usage(argv[0]);
			return OOB_SUCCESS;
		}
		if(is_string_number(argv[optind])) {
			printf("Option '-%c' require 2nd argument as valid"
				" numeric value\n\n", opt);
			show_usage(argv[0]);
			return OOB_SUCCESS;
		}
	}
	if (opt == 'u' || opt == 'x'|| opt == 'v' || opt == 'w') {
		if (optind >= argc) {
			printf("\nOption '-%c' require TWO or more arguments"
				"\n\n", opt);
			show_usage(argv[0]);
			return OOB_SUCCESS;
		}
	}
	if (opt == 'd') {
		if ((optind + 1) >= argc || *argv[optind + 1] == '-') {
			printf("\nOption '-%c' require THREE arguments"
				" <socket> <thread> <set_value>\n\n", opt);
			show_usage(argv[0]);
			return OOB_SUCCESS;
		}
		if(is_string_number(argv[optind + 1])) {
			printf("Option '-%c' require 3rd argument as valid"
				" numeric value\n\n", opt);
			show_usage(argv[0]);
			return OOB_SUCCESS;
		}
	}
	switch (opt) {
		case 0:
			socket = atoi(optarg);
			if (*(long_options[long_index].flag) == 1) {
				get_apml_rmi_access(socket);
			} else if (*(long_options[long_index].flag) == 2) {
				get_apml_tsi_access(socket);
			}
			break;
		case 'p' :
			/* Get the power consumption for a given socket */
			socket = atoi(optarg);
			apml_get_sockpower(socket);
			break;
		case 't' :
			/* Get tdp value for a given socket */
			socket = atoi(optarg);
			apml_get_socktdp(socket);
			break;
		case 's':
			socket = atoi(optarg);
			power = atoi(argv[optind++]);
			apml_setpower_limit(socket, power);
			break;
		case 'c' :
			/* Get the cclk freq limit for a given socket index */
			socket = atoi(optarg);
			apml_get_cclk_freqlimit(socket);
			break;
		case 'r' :
			/* Get the c0 residency for a given socket index */
			socket = atoi(optarg);
			apml_get_sockc0_residency(socket);
			break;
		case 'b' :
			/* Get the apml boostlimit for a given socket
			 * and thread index */
			socket = atoi(optarg);
			thread_ind = atoi(argv[optind++]);
			get_boostlimit(socket, thread_ind);
			break;
		case 'd':
			socket = atoi(optarg);
			thread_ind = atoi(argv[optind++]);
			boostlimit = atoi(argv[optind++]);
			set_apml_boostlimit(socket, thread_ind, boostlimit);
			break;
		case 'a':
			socket = atoi(optarg);
			boostlimit = atoi(argv[optind++]);
			set_apml_socket_boostlimit(socket, boostlimit);
			break;
		case 'l':
			socket = atoi(optarg);
			dram_thr = atoi(argv[optind++]);
			set_and_verify_dram_throttle(socket, dram_thr);
			break;
		case 'u':
			socket = atoi(optarg);
			uprate = atof(argv[optind++]);
			set_and_verify_apml_socket_uprate(socket, uprate);
			break;
		case 'v':
			socket = atoi(optarg);
			temp = atof(argv[optind++]);
			set_high_temp_threshold(socket, temp);
			break;
		case 'w':
			socket = atoi(optarg);
			temp = atof(argv[optind++]);
			set_low_temp_threshold(socket, temp);
			break;
		case 'x':
			socket = atoi(optarg);
			temp = atof(argv[optind++]);
			set_temp_offset(socket, temp);
			break;
		case 'f':
			socket = atoi(optarg);
			value = atoi(argv[optind++]);
			set_timeout_config(socket, value);
			break;
		case 'g':
			socket = atoi(optarg);
			value = atoi(argv[optind++]);
			set_alert_threshold(socket, value);
			break;
		case 'j':
			socket = atoi(optarg);
			value = atoi(argv[optind++]);
			set_alert_config(socket, value);
			break;
		case 'k':
			socket = atoi(optarg);
			value = atoi(argv[optind++]);
			set_tsi_config(socket, value, 'k');
			break;
		case 'm':
			socket = atoi(optarg);
			value = atoi(argv[optind++]);
			set_tsi_config(socket, value, 'm');
			break;
		case 'n':
			socket = atoi(optarg);
			value = atoi(argv[optind++]);
			set_tsi_config(socket, value, 'n');
			break;
		case 'o':
			socket = atoi(optarg);
			value = atoi(argv[optind++]);
			set_tsi_config(socket, value, 'o');
			break;
		case 'h' :
			show_usage(argv[0]);
			return OOB_SUCCESS;
		case ':' :
			/* missing option argument */
			printf(RED "%s: option '-%c' requires an argument."
				RESET"\n\n", argv[0], optopt);
			break;
		case '?':
			if (isprint(optopt)) {
				printf(RED "Try `%s --help' for more"
				" information." RESET "\n", argv[0]);
			} else {
				printf ("Unknown option character `\\x%x'.\n",
					optopt);
			}
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
	for (i = 0; i < argc; i++) {
		args[i + 1] = argv[i];
	}
	args[i + 1] = NULL;
	execvp("sudo", args);
}

/**
Main program.
@param argc number of command line parameters
@param argv list of command line parameters
*/
int main(int argc, char **argv)
{
	oob_status_t ret;
	if (getuid() !=0) {
		rerun_sudo(argc, argv);
	}
	show_smi_message();
	/* i2c initialization */
	ret = esmi_oob_init(1);
	if (ret != OOB_SUCCESS) {
		printf(RED "I2CDEV INIT FAILED. Err[%d]: %s\n"
		       RESET, ret, esmi_get_err_msg(ret));
		return ret;
	}
	usleep(1000);
	/* Parse command arguments */
	parseesb_args(argc, argv);
	show_smi_end_message();
	esmi_oob_exit();

	return OOB_SUCCESS;
}
