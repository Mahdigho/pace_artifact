# Installation Guide

Once the software requirements in the `REQUIREMENTS.md` file are met, follow these instructions to setup PACE and the testbed. We use the `install.sh` file to install the required dependencies and binaries for Pace locally in a virtual environment. However, some aspects of the setting up the testbed require alternative software or physical actions.

> [!NOTE]
> Make sure initialize all submodules with `git submodule update --init --recursive`.


> [!NOTE]
> The installation script install everything locally in a python virtual environment, so you'll have to manually activate the environment to use PACE and the testbed.

## PACE

### Installation

To install and setup pace, simply run `bash install.sh` in the root directory.

> [!NOTE]
> There is no need for any root access/privilege for the building PACE components. However, to flash or communicate with various devices on Linux, you will need to access the tty port, which will require you to add your user to the `dialout` group. A simple method would be to run `sudo usermod -a -G dialout $USER`, but it requires sudo access.

### Testing the installation (without the need for the TestBed)

1. Activate your virtual environment (run `. ./.artifact_venv/bin/activate` in the root directory)..
2. Connect the MSP430 board to your device; two tty devices should pop up in your `/dev/tty{FET_PREFIX}{DEVICE_IDX}` (where `FET_PREFIX` and `DEVICE_IDX` depend on your device). The port with the lower index is for flashing, the second one is for communication (first one should generally have a lower number).
3. Make sure you have read and write access to these ports (as outlined previously).
4. Update the `paace/Makefile.env` file with the appropriate FET name and device index so the makefile knows which port to flash.
5. Change your directory to the `pace` folder.
6. Run `make apps/blink/bld/gcc/all` to build the blinking lights application (should automatically build all the necessary steps).
7. Run `make apps/blink/bld/gcc/prog` to flash the MSP430 (this step can also build the application if you skipped the last one).
8. At this point, your MSP430 lights (the green and red one) should blink alternately.
9. Use your Serial monitoring tool to read the output on the second tty device. The baudrate is 19200; the MSP430 should be counting its blinks up.

### Usage

Refer to `pace/Usage.md` for detailed usage information.

## Testbed

The test bed is composed of several hardware components and a software interface.

### Software Installation

All required software will be installed via the `bash install.sh` script.

### Hardware setup

#### Flashing the itsy bitsy

We need to flash the itsy bitsy m0 with necessary information about controlling the MCP4261. First, make sure your itsy bitsy has circuit python enabled. Next, take the files from `testbed/itsy_code/` and drop them into the CIRCUITPY drive via your file explorer.

#### Wiring up the MCP4261

Similar to prior work, we use a rheostat to adjust the resistance, and thus the power, of the intermittent node, effectively simulating energy harvesting power. We have included in the `mcp_wiring.jpeg` image to outline the necessary physical connections between the MCP4261 and the itsy bitsy.

#### TODO

### Flashing PACE

To build and flash PACE as is, setup the pins as described above and simply run `make apps/cifar10/bld/gcc/prog` (or another application) in the pace subdirectory. Keep the report pin high (described in `pace/USAGE.md` while you're doing this; it prevents the application from accidentally running while the board is getting flashed. Once done, and disconnect the USB from the board and connect the MSP430 board to the test bed as described. Once done, start the power scripts as described above.

### Running PACE

### Reading the analytics

Once done with your experiment, disconnect the MSP430 board from the testbed, put the reset pin high, and connect it to the USB port. Your Serial monitoring tool should output the collected statistics.
