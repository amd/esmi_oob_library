# EPYC™ System Management Interface (E-SMI) Out-of-band Library

Thank you for using AMD ESMI_OOB Library. Please use the [ESMI Out-of-band Support](https://github.com/amd/esmi_oob_library/issues) to provide your feedback.

# Changes Notes

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
* Family 19h model 0h~0Fh & 30h~3Fh

# Supported BMCs
Compile this library for any BMC running linux. Please use the [ESMI Out-of-band Support](https://github.com/amd/esmi_oob_library/issues) to provide your feedback.

# Supported Operating Systems
ESMI_OOB library should run any linux based operating systems with the following "System requirements"

# System Requirements
## Hardware requirements
BMC with I2c controller as master, i2c master adapter channel (SCL and SDA lines) connected to the "Supported Processors"

## Software requirements

In order to build the ESMI_OBB library, the following components are required. Note that the software versions listed are what is being used in development. Earlier versions are not guaranteed to work:
### Compilation requirements
* CMake (v3.5.0)

### Packages
* libi2c-dev

In order to build the latest documentation, the following are required:

* Doxygen (1.8.13)
* latex (pdfTeX 3.14159265-2.6-1.40.18)

# Dependencies
The ESMI_OOB Library depends on the libi2c-dev library.

# Support
Pilease use [ESMI Out-of-band Support](https://github.com/amd/esmi_oob_library/issues) for bug reports, support and feature requests.
