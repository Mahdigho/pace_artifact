# Installation Requirements

While our work is mainly a software contribution, the artifact and the underlying testbed rely on specific executables, libraries, and hardware. We use this file to outline necessary requirements.

We do not provide a docker file as tty ports used to communicate with and flash hardware devices are not available in all platforms.

## Software

### PLATFORM

Linux or MacOS (Windows is currently not supported).

### Libraries

- libboost (version 1.85) required by msp debug stack (for macos, we assume this is installed by brew; for linux we assume this is install in 'usr/lib/x86_64-linux/gnu/'. If this is not the case for your platform, please change the `MSP_BOOS_DIR` variable on line 37 or 40 of the `install.sh` script).

### Executables/Programs

- A terminal to run commands.
- bash for the installation script.
- [uv](https://docs.astral.sh/uv/) for creating virtual environment for both the testbed and pace.
- GNU autotools (autoconf, automake, libtool) for building dependencies.
- cMake for building dependencies.
- GNU stow.
- Serial monitoring tool with variable baudrate for observing MSP430 outputs.

