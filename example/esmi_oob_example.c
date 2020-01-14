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
	uint32_t power_avg, power_cap, power_max;
	int i;

        if (getuid() !=0) {
                rerun_sudo(argc, argv);
        }

	esmi_oob_init(1);

	usleep(1000);
	for (i = 0; i < 2; i++) {
	printf("\nSocket %d:\n", i);
	printf("------------------------------------------------------------\n");
	printf("_POWER(Watts) |");
	power_avg = 0;
	power_cap = 0;
	power_max = 0;
	if (read_socket_power(i, &power_avg) != 0) {
		continue;
	}
	printf(" Avg:%.03f, ", (double)power_avg/1000);

	usleep(1000);
	if (read_socket_power_limit(i, &power_cap) != 0) {
		continue;
	}
	printf(" Limit:%.03f, ", (double)power_cap/1000);

	usleep(1000);
	if (read_max_socket_power_limit(i, &power_max) != 0) {
		continue;
	}
	printf(" Max:%.03f \n", (double)power_max/1000);
	printf("----------------------------------------------------------\n");
	}
	esmi_oob_exit();
	/* Normal program termination */
	return(0);
}
