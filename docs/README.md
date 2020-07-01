
# EPYC™ System Management Interface Out-of-band (E-SMI-OOB) Library

The EPYC™ System Management Interface Out-of-band Library or E-SMI-OOB library, is part of the EPYC™ System Management Out-of-band software stack. It is a C library for Linux that provides a user space interface to monitor and control the CPU's Systems Management features.

# Important note about Versioning and Backward Compatibility
The E-SMI-OOB library is currently under development, and therefore subject to change at the API level. The intention is to keep the API as stable as possible while in development, but in some cases we may need to break backwards compatibility in order to achieve future stability and usability. Following Semantic Versioning rules, while the E-SMI-OOB library is in a high state of change, the major version will remain 0, and achieving backward compatibility may not be possible.

Once new development has leveled off, the major version will become greater than 0, and backward compatibility will be enforced between major versions.

# Building E-SMI-OOB

#### Additional Required software for building
In order to build the E-SMI-OOB library, the following components are required. Note that the software versions listed are what is being used in development. Earlier versions are not guaranteed to work:
* CMake (v3.5.0)
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

#### Cross compile the library for armhf
Install cross-compiler for ubuntu as follows
##### ```$ sudo apt-get install gcc-arm-linux-gnueabihf```
Install libi2c-dev for armhf
Compilation steps
##### ```$ mkdir -p build```
##### ```$ cd build```
##### ```$ cmake -DCMAKE_TOOLCHAIN_FILE=../cross-arm-linux-gnueabihf.cmake <location of root of E-SMI-OOB library CMakeLists.txt>```
##### ```$ make```
The built library will appear in the `build` folder.
Copy the required binaries and the dynamic linked library to raspberrypi(required) board.
##### ```$ scp libesmi_oob64.so.0 rpi@10.x.x.x:/usr/lib```
##### ```$ scp esmi_oob_tool rpi@10.x.x.x:/usr/bin```

#### Building the Documentation
The documentation PDF file can be built with the following steps (continued from the steps above):
##### ```$ make doc```
##### ```$ cd latex```
##### ```$ make```
The reference manual, `refman.pdf` will be in the `latex` directory and `refman.rtf` will be in the `rtf` directory upon a successful build.

# Usage Basics
## Device Indices
Many of the functions in the library take a "core or socket index". The core or socket index is a number greater than or equal to 0, and less than the number of cores or sockets on the system.

# Hello E-SMI-OOB
The only required E-SMI-OOB call for any program that wants to use E-SMI-OOB is the `esmi_oob_init()` call. This call initializes some internal data structures that will be used by subsequent E-SMI-OOB calls.

When E-SMI-OOB is no longer being used, `esmi_oob_exit()` should be called. This provides a way to do any releasing of resources that E-SMI-OOB may have held. In many cases, this may have no effect, but may be necessary in future versions of the library.

Below is a simple "Hello World" type program that displays the Core energy of detected cores.

```
#include <stdio.h>
#include <stdint.h>
#include <esmi_oob/esmi_common.h>
#include <esmi_oob/esmi_mailbox.h>

int main() {
	oob_status_t ret;
	int package = 0, buffer = 0;

	ret = esmi_oob_init(1);
	if (ret != OOB_SUCCESS) {
		printf("I2CDEV INIT FAILED. Err[%d]\n", ret);
		return ret;
	}
	if (read_min_tdp(package, &buffer) < 0)
		goto x;
	printf("package[%d] Min TDP: %lx\n", package, buffer);

x:
	esmi_oob_exit();
	return ret;
}
```
# Usage
## Tool Usage
E-SMI tool is a C program based on the E-SMI Out-of-band Library, the executable "esmi_oob_tool" will be generated in the build/ folder.
This tool provides options to Monitor and Control System Management functionality.

Below is a sample usage to dump the functionality, with default core/socket available.
```
esmi_oob_library/build> ./esmi_oob_tool
=============== APML System Management Interface ===============

--------------------------------------------------------------------------------
			SOCKET 0
--------------------------------------------------------------------------------
*** SB-RMI Mailbox Service Access ***
_POWER	(Watts)		| Avg : 56.068,  Limit : 200.000,  Max : 240.000
_TDP	(Watts)		| Avg : 225.000,  Minim : 225.000,  Max : 240.000
_CCLK_FREQ_LIMIT (MHz)	| 3200
_C0_RESIDENCY (in %)	| 0%
_BOOST_LIMIT (MHz)	| BIOS: 3200,  APML: 3200
_DRAM_THROTTLE	(in %)	| 50
_PROCHOT STATUS		| NOT_PROCHOT
_PROCHOT RESIDENCY (MHz)| 0
_VDDIOMem_POWER (mWatts)| 36460
_NBIO_Error_Logging_Reg	| 0
_IOD_Bist_RESULT	| Bist pass
_CCD_Bist_RESULT	| Bist pass
_CCX_Bist_RESULT	| 0

*** SB-TSI UPDATE ***
_CPUTEMP		| 20.625 °C
_HIGH_THRESHOLD_TEMP	| 70.000 °C
_LOW_THRESHOLD_TEMP 	| 2.125 °C
_TSI_UPDATERATE		| 0.125 Hz
_THRESHOLD_SAMPLE	| 1
_TEMP_OFFSET		| -21.125 °C
_STATUS			| No Temp Alert
_CONFIG			|
	ALERT_L pin	| Enabled
	Runstop		| Enabled
	Atomic rd order	| Disabled
	ARA response	| Enabled
_TIMEOUT_CONFIG		| Enabled
_TSI_ALERT_CONFIG	| Enabled
_TSI_MANUFACTURE_ID	| 0
_TSI_REVISION		| 0x4
--------------------------------------------------------------------------------
Try './esmi_oob_tool --help' for more information.

====================== End of APML SMI Log =====================
```

For detailed and up to date usage information, we recommend consulting the help:

For convenience purposes, following is the output from the -h flag:
```
esmi_oob_library/build> ./esmi_oob_tool --help
Usage: ./esmi_oob_tool [Option<s>] SOURCES
Option<s>:
< MAILBOX COMMANDS >:
	-p, (--showpower)	 [SOCKET]			Get Power for a given socket in Watt
	-t, (--showtdp)		 [SOCKET]			Get TDP for a given socket in Watt
	-s, (--setpowerlimit)	 [SOCKET][POWER]		Set powerlimit for a given socket in mWatt
	-c, (--showcclkfreqlimit)[SOCKET]			Get cclk freqlimit for a given socket in MHz
	-r, (--showc0residency)	 [SOCKET]			Show socket c0_residency given socket
	-b, (--showboostlimit)   [SOCKET][THREAD]		Get APML and BIOS boostlimit for a given socket and core index in MHz
	-d, (--setapmlboostlimit)[SOCKET][THREAD][BOOSTLIMIT]   Set APML boostlimit for a given socket and core in MHz
	-a, (--setapmlsocketboostlimit)  [SOCKET][BOOSTLIMIT]   Set APML boostlimit for all cores in a socket in MHz
	--set_and_verify_dramthrottle    [SOCKET][0 to 80%]     Set DRAM THROTTLE for a given socket
< SB-RMI COMMANDS >:
	--showrmicommandregisters [SOCKET]			Get the values of different commands of SB-RMI registers for a given socket
< SB-TSI COMMANDS >:
	--showtsicommandregisters [SOCKET]			Get the values of different commands of SB-TSI registers for a given socket
	--set_verify_updaterate	  [SOCKET][Hz]			Set APML Frequency Update rate for a socket
	--sethightempthreshold	  [SOCKET][TEMP(°C)]		Set APML High Temp Threshold
	--setlowtempthreshold	  [SOCKET][TEMP(°C)]		Set APML Low Temp Threshold
	--settempoffset		  [SOCKET][VALUE]		Set APML CPU Temp Offset, VALUE = [-CPU_TEMP(°C), 127 °C]
	--settimeoutconfig	  [SOCKET][VALUE]		Set/Reset APML CPU timeout config, VALUE = 0 or 1
	--setalertthreshold	  [SOCKET][VALUE]		Set APML CPU alert threshold sample, VALUE = 1 to 8
	--setalertconfig	  [SOCKET][VALUE]		Set/Reset APML CPU alert config, VALUE = 0 or 1
	--setalertmask		  [SOCKET][VALUE]		Set/Reset APML CPU alert mask, VALUE = 0 or 1
	--setrunstop		  [SOCKET][VALUE]		Set/Reset APML CPU runstop, VALUE = 0 or 1
	--setreadorder		  [SOCKET][VALUE]		Set/Reset APML CPU read order, VALUE = 0 or 1
	--setara		  [SOCKET][VALUE]		Set/Reset APML CPU ARA, VALUE = 0 or 1
	-h, (--help)						Show this help message
```

Below is a sample usage to get the individual library functionality API's.
We can pass arguments either any of the ways "./esmi_oob_tool -p 0" or "./esmi_oob_tool --showpower=0"
```
1.	esmi_oob_library/build> ./esmi_oob_tool -p 0
	=============== APML System Management Interface ===============

	socket[0]/power:           56.155 Watts
	socket[0]/powerlimit:      200.000 Watts
	socket[0]/max_power_limit: 240.000 Watts


	====================== End of APML SMI Log =====================

2.	esmi_oob_library/build> ./esmi_oob_tool --setpowerlimit 0 220000
	=============== APML System Management Interface ===============

	Set socket[0]/power_limit :          220.000 Watts successfully

	====================== End of APML SMI Log =====================

3.	esmi_oob_library/build> ./esmi_oob_tool --showtsicommandregisters 0
	=============== APML System Management Interface ===============

			 *** SB-TSI UPDATE ***
	Socket:0
	--------------------------------------------------------------------------------
	_CPUTEMP		| 20.625 °C
	_HIGH_THRESHOLD_TEMP	| 70.000 °C
	_LOW_THRESHOLD_TEMP 	| 2.125 °C
	_TSI_UPDATERATE		| 0.125 Hz
	_THRESHOLD_SAMPLE	| 1
	_TEMP_OFFSET		| -21.125 °C
	_STATUS			| No Temp Alert
	_CONFIG			|
		ALERT_L pin	| Enabled
		Runstop		| Enabled
		Atomic rd order	| Disabled
		ARA response	| Enabled
	_TIMEOUT_CONFIG		| Enabled
	_TSI_ALERT_CONFIG	| Enabled
	_TSI_MANUFACTURE_ID	| 0
	_TSI_REVISION		| 0x4
	-------------------------------------------------------------------------------

	====================== End of APML SMI Log =====================
```
