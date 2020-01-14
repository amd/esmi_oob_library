
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
The source code for E-SMI-OOB is available esmi-oob-library.tar.gz in the current directory.
 Note: This will be hosted on a GitHub later.

#### Directory stucture of the source
Once the E-SMI-OOB library source has been cloned to a local Linux machine, the directory structure of source is as below:
* `$ docs/` Contains Doxygen configuration files and Library descriptions
* `$ example/` Contains esmi_oob_tool and esmi_oob_ex based on the E-SMI-OOB library
* `$ include/esmi_oob` Contains the header files used by the E-SMI-OOB library
* `$ src/esmi_oob` Contains library E-SMI-OOB source

Building the library is achieved by following the typical CMake build sequence, as follows.
##### ```$ mkdir -p build```
##### ```$ cd build```
##### ```$ cmake <location of root of E-SMI-OOB library CMakeLists.txt>```
##### ```$ make```
The built library will appear in the `build` folder.

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
#include <esmi_oob/common.h>
#include <esmi_oob/mailbox.h>

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
