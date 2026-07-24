# Requirements

While our work is mainly a software contribution, the artifact and the underlying testbed rely on specific executables, libraries, and hardware. This file outlines the necessary requirements for each component of our work/artifact. For exact use case and operation of PACE or the testbed, refer to the respective folders. Other prerequisites for both PACE and the testbed will be installed by the installation script as outlined in the `INSTALL.md` file.

We do not provide a docker file as tty ports used to communicate and flash hardware devices are not available in all platforms.

## PACE

The following requirements are necessary to build and flash PACE onto the targeted microcontroller.

### Software

#### PLATFORM

Linux or MacOS (Windows is currently not supported).

#### Libraries

- libboost (version 1.85) required by msp debug stack (for macos, we assume this is installed by brew; for linux we assume this is install in 'usr/lib/x86_64-linux/gnu/'. If this is not the case for your platform, please change the `MSP_BOOS_DIR` variable on line 37 or 40 of the `install.sh` script).

#### Executables/Programs

- A terminal to run commands.
- bash for the installation script.
- [uv](https://docs.astral.sh/uv/) for creating virtual environment for both the testbed and pace.
- GNU autotools (autoconf, automake, libtool) for building dependencies.
- cMake for building dependencies.
- GNU stow.
- Serial communication monitoring tool with variable baudrate for observing MSP430 outputs.

### Hardware

- A MSP-EXP430FR5994 board (or ideally two as described in the PACE `INSTALL.md`), refered to generally as the MSP430.
- A computer with USB ports to flash and communicate with the MSP430 (via serial communication).

## TestBed

The following hardware is required to create PACE's testbed

- A computer with 2 serial capable USB ports to control the Aanalog Discovery 3 and the itsy bitsy M0.
- Analog Discover 3 for the programmable power supply.
- MCP426 dual Digital POT, 10k resistance option, for modulating the resistance.
- Itsy bitsy M0 (or any circuit python capable MCU with SPI and serial communication) to control the MCP.
- P2110-EVB kit as the energy harvester.
- Additional 0.1mF tantalum capacitors to recreate the capacitor decay and tolerance experiments.
- Wires for connections.
