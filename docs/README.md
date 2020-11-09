
# EPYC™ System Management Interface Out-of-band (E-SMI-OOB) Library

The EPYC™ System Management Interface Out-of-band Library or E-SMI-OOB library, is part of the EPYC™ System Management Out-of-band software stack. It is a C library for Linux that provides a user space interface to monitor and control the CPU's Systems Management features.

# Important note about Versioning and Backward Compatibility
The E-SMI-OOB library is currently under development, and therefore subject to change at the API level. The intention is to keep the API as stable as possible while in development, but in some cases we may need to break backwards compatibility in order to achieve future stability and usability. Following Semantic Versioning rules, while the E-SMI-OOB library is in a high state of change, the major version will remain 0, and achieving backward compatibility may not be possible.

Once new development has leveled off, the major version will become greater than 0, and backward compatibility will be enforced between major versions.

# Building E-SMI-OOB

#### Additional Required software for building
In order to build the E-SMI-OOB library, the following components are required. Note that the software versions listed are what is being used in development. Earlier versions are not guaranteed to work:
* CMake (v3.5.0)
* latex (pdfTeX 3.14159265-2.6-1.40.18)
* i2c-tools, libi2c-dev

#### Dowloading the source
The source code for E-SMI library is available on [Github](https://github.com/amd/esmi_oob_library).

#### Directory stucture of the source
Once the E-SMI-OOB library source has been cloned to a local Linux machine, the directory structure of source is as below:
* `$ docs/` Contains Doxygen configuration files and Library descriptions
* `$ example/` Contains esmi_oob_tool and esmi_oob_ex based on the E-SMI-OOB library
* `$ include/esmi_oob` Contains the header files used by the E-SMI-OOB library
* `$ src/esmi_oob` Contains library E-SMI-OOB source

#### Building the library is achieved by following the typical CMake build sequence for native build, as follows.
##### ```$ mkdir -p build```
##### ```$ mkdir -p install```
##### ```$ cd build```
##### ```$ cmake -DCMAKE_INSTALL_PREFIX=${PWD}/install <location of root of E-SMI-OOB library CMakeLists.txt>```
##### ```$ make```
The built library will appear in the `build` folder.

#### Cross compile the library for Target systems

Before installing the cross compiler verfiy the target architecture
##### ```$ uname -m```

Eg: To cross compile for ARM32 processor:
##### ```$ sudo apt-get install gcc-arm-linux-gnueabihf```

Eg: To cross compile for AARCH64 processor: use 
##### ```$ sudo apt-get install gcc-aarch64-linux-gnu```

The ESMI_OOB Library depends on the libi2c-dev library, libi2c-dev package needs to be installed.
##### ```$sudo apt-get install libi2c-dev```
Compilation steps
##### ```$ mkdir -p build```
##### ```$ cd build```
##### ```$ cmake -DCMAKE_TOOLCHAIN_FILE=../cross-[arch..].cmake <location of root of E-SMI-OOB library CMakeLists.txt>```
##### ```$ make```
The built library will appear in the `build` folder.
Copy the required binaries and the dynamic linked library to target board(BMC).
##### ```$ scp libesmi_oob64.so.0 root@10.x.x.x:/usr/lib```
##### ```$ scp esmi_oob_tool root@10.x.x.x:/usr/bin```

NOTE: For cross compilation, cross-$ARCH.cmake file is provided for below Architectures:
 - armhf
 - aarch64

#### Disclaimer
 - User may not be able to use this library when the i2c addresses are reserved, this is observed when TSI driver is loaded
 - Input arguments like i2c address and bus number passed by the user are not validated. It might result in unreliable system behavior

#### Building the Documentation
The documentation PDF file can be built with the following steps (continued from the steps above):
##### ```$ make doc```
The reference manual (ESMI_OOB_Manual.pdf), release notes (ESMI_OOB_Release_Notes.pdf) upon a successful build.

# Usage Basics
## Device Indices
Many of the functions in the library take I2C Bus and 7-bit address as index.

# Hello E-SMI-OOB
Below is a simple "Hello World" type program that displays power of required socket.

```
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <esmi_oob/esmi_common.h>
#include <esmi_oob/esmi_mailbox.h>
#include <esmi_oob/esmi_rmi.h>

int main(int argc, char **argv) {
	uint32_t power_avg = 0;
	uint32_t i2c_bus, i2c_addr;
	char *end;

	i2c_bus = atoi(argv[1]);
	i2c_addr = strtoul(argv[2], &end, 16);
	if (*end || !*argv[2]) {
		printf("Require a valid i2c_address in Hexa\n");
		return 0;
	}

	read_socket_power(i2c_bus, i2c_addr, &power_avg);
	printf(" Avg:%.03f, ", (double)power_avg/1000);

	return 0;
}
```
# Usage
## Tool Usage
E-SMI tool is a C program based on the E-SMI Out-of-band Library, the executable "esmi_oob_tool" will be generated in the build/ folder.
This tool provides options to Monitor and Control System Management functionality.

In execution platform, user can cross-verfiy "i2c-dev" module is loaded or not, if not follow the below step:
##### ```$ lsmod | grep i2c-dev```
If not loaded, load the module as below
##### ```$ insmod /lib/modules/`uname -r`/kernel/drivers/i2c/i2c-dev.ko``` or
##### ```$ modprobe i2c-dev.ko```
Check I2C addresses are enumerated as below, if not i2c connection is at fault.
Pass the I2C bus number connected to socket for RMI or TSI
```
$ i2cdetect -y 1
     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
00:          -- -- -- -- -- -- -- -- -- 0c -- -- --
10: 10 -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
20: 20 -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
30: -- -- -- -- -- -- -- -- -- -- -- -- 3c -- -- --
40: -- -- -- -- -- -- -- -- -- -- -- -- 4c -- -- --
50: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
60: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
70: -- -- -- -- -- -- -- --
```
For 2p targets, additional I2C addresses are enumerated as:
```
$ i2cdetect -y 1
     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
00:          -- -- -- -- -- -- -- -- -- 0c -- -- --
10: 10 -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
20: 20 -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
30: -- -- -- -- -- -- -- -- 38 -- -- -- 3c -- -- --
40: -- -- -- -- -- -- -- -- 48 -- -- -- 4c -- -- --
50: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
60: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
70: -- -- -- -- -- -- -- --
```
Below is a sample usage to dump the functionality, with default core/socket available.
```
$ ./esmi_oob_tool

=============== APML System Management Interface ===============

Usage: ./esmi_oob_tool <i2c_bus> <i2c_addr>
Where:  i2c_bus : 0 or 1
        Eg,  i2c_addr : SB-RMI addresses:       0x3c for Socket0 and 0x38 for Socket1
                        SB-TSI addresses:       0x4c for Socket0 and 0x48 for Socket1

====================== End of APML SMI Log =====================
```

For detailed and up to date usage information, we recommend consulting the help:

For convenience purposes, following is the output from the -h or --help flag:
```
$ ./esmi_oob_tool --help

=============== APML System Management Interface ===============

Usage: ./esmi_oob_tool [Option<s>] SOURCES
Option<s>:
< MAILBOX COMMANDS >:
  -p, (--showpower)               [I2C_BUS][I2C_ADDR]                    Get Power for a given socket in Watts
  -t, (--showtdp)                 [I2C_BUS][I2C_ADDR]                    Get TDP for a given socket in Watts
  -s, (--setpowerlimit)           [I2C_BUS][I2C_ADDR][POWER]             Set powerlimit for a given socket in mWatts
  -c, (--showcclkfreqlimit)       [I2C_BUS][I2C_ADDR]                    Get cclk freqlimit for a given socket in MHz
  -r, (--showc0residency)         [I2C_BUS][I2C_ADDR]                    Show c0_residency for a given socket
  -b, (--showboostlimit)          [I2C_BUS][I2C_ADDR][THREAD]            Get APML and BIOS boostlimit for a given core index in MHz
  -d, (--setapmlboostlimit)       [I2C_BUS][I2C_ADDR][THREAD][BOOSTLIMIT]Set APML boostlimit for a given core in MHz
  -a, (--setapmlsocketboostlimit) [I2C_BUS][I2C_ADDR][BOOSTLIMIT]        Set APML boostlimit for all cores in a socket in MHz
  --showddrbandwidth              [I2C_BUS][I2C_ADDR]                    Show DDR Bandwidth of a system
  --set_and_verify_dramthrottle   [I2C_BUS][I2C_ADDR][0 to 80%]          Set DRAM THROTTLE for a given socket
< SB-RMI COMMANDS >:
  --showrmicommandregisters       [I2C_BUS][I2C_ADDR]                    Get values of SB-RMI reg commands for a given socket
< SB-TSI COMMANDS >:
  --showtsicommandregisters       [I2C_BUS][I2C_ADDR]                    Get values of SB-TSI reg commands for a given socket
  --set_verify_updaterate         [I2C_BUS][I2C_ADDR][Hz]                Set APML Frequency Update rate for a given socket
  --sethightempthreshold          [I2C_BUS][I2C_ADDR][TEMP(°C)]          Set APML High Temp Threshold
  --setlowtempthreshold           [I2C_BUS][I2C_ADDR][TEMP(°C)]          Set APML Low Temp Threshold
  --settempoffset                 [I2C_BUS][I2C_ADDR][VALUE]             Set APML CPU Temp Offset, VALUE = [-CPU_TEMP(°C), 127 °C]
  --settimeoutconfig              [I2C_BUS][I2C_ADDR][VALUE]             Set/Reset APML CPU timeout config, VALUE = 0 or 1
  --setalertthreshold             [I2C_BUS][I2C_ADDR][VALUE]             Set APML CPU alert threshold sample, VALUE = 1 to 8
  --setalertconfig                [I2C_BUS][I2C_ADDR][VALUE]             Set/Reset APML CPU alert config, VALUE = 0 or 1
  --setalertmask                  [I2C_BUS][I2C_ADDR][VALUE]             Set/Reset APML CPU alert mask, VALUE = 0 or 1
  --setrunstop                    [I2C_BUS][I2C_ADDR][VALUE]             Set/Reset APML CPU runstop, VALUE = 0 or 1
  --setreadorder                  [I2C_BUS][I2C_ADDR][VALUE]             Set/Reset APML CPU read order, VALUE = 0 or 1
  --setara                        [I2C_BUS][I2C_ADDR][VALUE]             Set/Reset APML CPU ARA, VALUE = 0 or 1
  -h, (--help)                                                           Show this help message

====================== End of APML SMI Log =====================

```
Below is a sample usage to get the individual library functionality API's.
User can pass arguments either any of the ways "./esmi_oob_tool -p [bus_num] [7 bit adress]" or "./esmi_oob_tool --showpower [bus_num] [7 bit address]"
```
	1. $ ./esmi_oob_tool -p 1 0x3c

		=============== APML System Management Interface ===============

		---------------------------------------------
		| Power (Watts)          | 52.729           |
		| PowerLimit (Watts)     | 225.000          |
		| PowerLimitMax (Watts)  | 240.000          |
		---------------------------------------------

		====================== End of APML SMI Log =====================

	2. $ ./esmi_oob_tool --setpowerlimit 1 0x3c 200000

		=============== APML System Management Interface ===============

		Set i2c_addr[0x3c]/power_limit :          200.000 Watts successfully

		====================== End of APML SMI Log =====================

	3. $ ./esmi_oob_tool --showtsicommandregisters 1 0x4c

	       =============== APML System Management Interface ===============

	       ----------------------------------------------------------------

                               *** SB-TSI REGISTER SUMMARY ***
	       ----------------------------------------------------------------
	        _CPUTEMP                | 40.250 _C
		_HIGH_THRESHOLD_TEMP    | 70.000 _C
		_LOW_THRESHOLD_TEMP     | 0.000 _C
		_TSI_UPDATERATE         | 16.000 Hz
		_THRESHOLD_SAMPLE       | 1
		_TEMP_OFFSET            | 0.000 _C
		_STATUS                 | No Temp Alert
		_CONFIG                 |
		        ALERT_L pin     | Enabled
		        Runstop         | Comparison Enabled
		        Atomic Rd order | Integer latches Decimal
		        ARA response    | Enabled
		_TIMEOUT_CONFIG         | Enabled
		_TSI_ALERT_CONFIG       | Disabled
		_TSI_MANUFACTURE_ID     | 0
		_TSI_REVISION           | 0x4
		---------------------------------------------------------------

		====================== End of APML SMI Log ====================
```
