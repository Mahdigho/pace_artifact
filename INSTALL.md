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
4. Update the `pace/Makefile.env` file with the appropriate FET name and device index so the makefile knows which port to flash.
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

Read `testbed/USAGE.md` for further details on how to use the testbed.

#### Flashing the itsy bitsy

We need to flash the itsy bitsy m0 with necessary information about controlling the MCP4261. First, make sure your itsy bitsy has circuit python enabled. Next, take the files from `testbed/itsy_code/` and drop them into the CIRCUITPY drive via your file explorer.

#### Wiring up the MCP4261

Similar to prior work, we use a rheostat to adjust the resistance, and thus the power, of the intermittent node, effectively simulating energy harvesting power. We have included in the `mcp_wiring.jpeg` image to outline the necessary physical connections between the MCP4261 and the itsy bitsy on a bread board.

#### Finalizing the Test Bed

The testbed has 4 components:

1. The analog discover 3, which provides power.
2. The P2110-EVB board, which harvests the power.
3. The MSP430 board which uses the power.
4. The MCP4261 and itsy bitsy which modulates the power.

Follow these steps to setup wire connections between all 4.

1. 1, 2, and 3 need to share the same ground.
2. Connect the positive power supply output pin of the discover 3 to connections on row 6 of the breadboard as shown in `mcp_wiriing.jpeg`. Connect the ground pin adjacent to the power supply pin to your shared ground.
3. Connect the P2110-EVB boards ground pin to the shared ground. Connect the capacitor's positive side to the connections on row 5 of the breadboard as shown in `mcp_wiring.jpeg`.

Let's test and make sure the testbed works. Connect the itsy bitsy and analog discovery to your computer. Toggle the LED switch on the P2110-EVB board to VCC. Run `python3 testbed/power_management/trace_play.py`. Provide the tty port to the itsy bitsy and select constant traces to be played. Start the first trace. The red LED on the P2110-EVB board should start blinking. This means that the test bed can supply power to the capacitor. Stop the trace and start the next few ones in sucession. The LED should blink slower and slower as the traces effectively provide less and less power. This means that the test bed can modulate power.

4. Connect the testbed to the MSP430. Connect 3.3V VCC pin on the MSP430 board to the output VCC of the P2110-EVB board and the ground to your shared ground. Connect the comparator and ADC pin defined in `pace/apps/{APPLICATION}/src/runtime/include/defs.h` to the capacitor's positive side on the P2110 EVB board.

#### Customizing the Capacitance

You can solder other capacitors on the P2110-EVB board to change the capacitance (besides the default ones) or

## Running PACE and Recreating the Experiments

### Flashing PACE

After setting up the testbed, follow these steps to flash PACE.

1. Ensure that the pins are connected as outlined above.
2. Ensure that the connection jumpers between pins on the MSP430 side of the eval board and on board debug probe are connected. Keep the report pin
3. Connect the MSP430 to your computer. Keep the report pin high (described in `pace/USAGE.md`) while you're doing this; it prevents the application from accidentally running while the board is getting flashed.
4. Run `make apps/cifar10/bld/gcc/prog` (or another application as outlined in `pace/USAGE.md`).
5. Once the application is flashed, disconnect the USA and the connection jumpers mentioned in step 2. The debug probe will siphon energy off of the capacitor if not done and lead to bugs.
6. Pull the report pin low.
7. Run `python3 testbed/power_management/trace_play.py` to play your desired scripts.
8. The application should start.
   1. It first flashes the red LED, then runs a warmup inference.
   2. The application runs the selected experiment the requested number of times.
   3. Once done, it starts flashing the green LED until a hard reset is done.
   4. To loop the application state back to step 8.1, you need to hard reset the application. You can do this by either pressing the RESET button on the board, by removing the power entirely, or stopping the trace being played.

### Running Applications in the Paper

Most PACE experiments can be recreated easily by setting appropriate latency and inference configurations in the `include/app.h` file for each respective application and running the desired power trace.

### Reading the analytics

Once done with your experiment,

1. Disconnect the MSP430 board from the testbed.
2. Pull the report pin high.
3. Reconnect the connection jumpers.
4. Connect the MSP430 board to the USB port.
5. Start your Serial monitoring tool with baudrate 19200 to see the collected statistics and analytics.
6. You can press the reset button to have the analytics start from the beginning in case you missed some of the earlier lines.
