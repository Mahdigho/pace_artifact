# PACE

This is the artifact repository for oue work titled "PACE: Deployment-Time Scheduling Policy for Deadline-Aware, Adaptive Intermittent Inference".

It consists of necessary information on the setup and usage of both the PACE runtime and its test bed. The structure of this repository is as follows,

1. Documentation files,
   1. `REQUIREMENTS.md` lists the needed software and hardware components.
   2. `INSTALL.md` outlines how to setup the software and hardware.
   3. `STATUS.md` outlines the status of the artifact.
   4. `LICENSE.md`.
   5. `PACE_ACCEPTED.pdf`, a copy of the accepted paper.
   6. `pace/USAGE.md` outlines how to use and customize the PACE runtime.
   7. `testbed/USAGE.md` outlines how to use and customize the test bed.
   8. This file which provides a general structure along with information about running pace and recreating the experiments after the installation and usage files have been completed.
2. The PACE runtime and compilation environment in the `pace` subdirectory. Refer to the `pace/USAGE.md` to see further details.
3. The testbed scripts and source files in the `testbed` subdirectory.

## Running PACE and Recreating the Experiments

### Flashing PACE

After setting up the testbed, follow these steps to flash PACE.

1. Ensure that the pins are connected as outlined in the `INSTALL.md` file.
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
