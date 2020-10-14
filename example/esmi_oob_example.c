/*
 * University of Illinois/NCSA Open Source License
 *
 * Copyright (c) 2020, Advanced Micro Devices, Inc.
 * All rights reserved.
 *
 * Developed by:
 *
 *			 AMD Research and AMD Software Development
 *
 *			 Advanced Micro Devices, Inc.
 *
 *			 www.amd.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal with the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 *  - Redistributions of source code must retain the above copyright notice,
 *this list of conditions and the following disclaimers.
 *  - Redistributions in binary form must reproduce the above copyright
 *notice, this list of conditions and the following disclaimers in
 *the documentation and/or other materials provided with the distribution.
 *  - Neither the names of <Name of Development Group, Name of Institution>,
 *nor the names of its contributors may be used to endorse or promote
 *products derived from this Software without specific prior written
 *permission.
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

#define ARGS_MAX 64
#define APML_SLEEP 100

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
	uint32_t power, power_cap, power_max;
	uint32_t i2c_bus, i2c_addr;
	char *end;
	int ret;

        if (getuid() != 0) {
                rerun_sudo(argc, argv);
        }
	if (argc < 3) {
		printf("Provide i2c_bus number and i2c_addr as arguments\n"
			"Usage: %s <i2c_bus> <i2c_addr>\n"
			"Where:  i2c_bus : 0 or 1\n"
			"\ti2c_addr : 0x3c for Socket0 and 0x38 for Socket1\n",
			argv[0]);
		return 0;
	}
	i2c_bus = atoi(argv[1]);
	i2c_addr = strtoul(argv[2], &end, 16);
	if (*end || !*argv[2]) {
		printf("Require a valid i2c_address in Hexa\n");
		return 0;
	}
	power = 0;
	power_cap = 0;
	power_max = 0;
	ret = read_socket_power(i2c_bus, i2c_addr, &power);
	if (ret != OOB_SUCCESS) {
		printf("Failed: to get i2c_addr[0x%x] power, Err[%d]: %s\n",
			i2c_addr, ret, esmi_get_err_msg(ret));
		return ret;
	}
	printf("-----------------------------------------\n");
	printf("Power(Watts) \t\t |");
	printf(" %.03f \t|", (double)power/1000);

	usleep(APML_SLEEP);
	ret = read_socket_power_limit(i2c_bus, i2c_addr, &power_cap);
	if (ret != OOB_SUCCESS) {
		printf("Failed: to get i2c_addr[0x%x] powerlimit,Err[%d]:%s\n",
			i2c_addr, ret, esmi_get_err_msg(ret));
		return ret;
	}
	printf("\nPowerLimit(Watts)\t | %.03f \t|", (double)power_cap/1000);

	usleep(APML_SLEEP);
	ret = read_max_socket_power_limit(i2c_bus, i2c_addr, &power_max);
	if (ret != OOB_SUCCESS) {
		printf("Failed: to get i2c_addr[0x%x] maxpower, Err[%d]: %s\n",
			i2c_addr, ret, esmi_get_err_msg(ret));
		return ret;
	}
	printf("\nPowerLimitMax(Watts) \t | %.03f \t|\n", (double)power_max/1000);
	printf("-----------------------------------------\n");
	/* Normal program termination */
	return 0;
}
