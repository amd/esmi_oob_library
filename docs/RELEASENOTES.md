# Advanced Platform Management Link (APML) Library
# (formerly known as EPYC™ System Management Interface (E-SMI) Out-of-band Library)

Thank you for using AMD APML Library.

# Changes Notes


## Highlights of major release v2.0
* Rename ESMI_OOB library to APML Library
* Rework the library to use APML modules (apml_sbrmi and apml_sbtsi)
    - This helps us acheive better locking across the protocols
    - APML modules takes care of the probing the APML devices and provding interfaces
* Add features supported on Family 19h Model 10h-1Fh
    - Support APML over I2C and APML over I3C
    - Handle thread count >127 per socket
    - CPUID and MSR read over I3C

## Highlights of minor release v1.1

* Single command to create doxygen based PDF document
* Improved the esmi tool

## Highlights of major release v1.0
APIs to Monitor and control the following feature
* Power
    * Get current power consumed
    * Get and set cap/limit
    * Get max power cap/limit
* Performance
    * Get/Set boostlimit
    * Get DDR bandwidth
    * Set DRAM throttle
* Temeprature
    * Get CPU temperature
    * Set High/Low temperature threshold
    * Set Temp offset
    * Set alert threshold sample & alert config
    * Set readorder

# Supported Processors
* Family 17h model 31h
* Family 19h model 0h~0Fh & 10h~1Fh

# Supported BMCs
Compile this library for any BMC running linux. Please use the [APML Library/tool Support](https://github.com/amd/esmi_oob_library/issues) to provide your feedback.

# Supported Operating Systems
APML Library is tested on OpenBMC for Aspeed and RPI based BMCs with the following "System requirements"

# System Requirements
## Hardware requirements
BMC with I2C/I3C controller as master, I2C/I3C master adapter channel (SCL and SDA lines) connected to the "Supported Processors"

## Software requirements

In order to build the APML library, the following components are required. Note that the software versions listed are what is being used in development. Earlier versions are not guaranteed to work:

### Compilation requirements
* CMake (v3.5.0)
* APML library upto v1.1 depends on libi2c-dev
* Doxygen (1.8.13)
* latex (pdfTeX 3.14159265-2.6-1.40.18)

# Dependencies
APML library upto v1.1 depends on libi2c-dev
Later versions depends on the apml_modules (hosted github.com/amd/apml_modules)

# Support
To provide your feedback, bug reports, support and feature requests.

Please use [APML Library Support](https://github.com/amd/esmi_oob_library/issues).
