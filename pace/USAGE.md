# Usage information

This artifact contains the three applications present in paper. Each application follows very similliar setup, so we will outline only 1. The others can be flashed and experimented with by replacing `cifar10` with `vww` or `speech`. Please follow the installation instructions in the `INSTALL.md` file in the parent directory first and make sure your test bed is set up properly.

## Application structure

Given an example application `cifar10`, the source files are located in `pace/apps/cifar10/src`. All paths in the remainder of this section are relative to the source files.

1. `dnn` and `weights` are application specific. You won't need to change these unless you want to update the weights or change the model; refer to the Lupe paper for how this can be done (Lupe: Integrating the Top-down Approach with DNN Execution on Ultra-Low-Power Devices). You would have to also update the relevant application Makefile to include your files.
2. `runtime` includes the behind the scenes checkpointing and other auxiliary support functions. There are two import aspects here,
   1. First, the `runtime/include/defs.h` file needs to be customized so the runtime knows which pins are used for reading capacitor voltage and min/max capacitor voltage so the comparator needs to know when to put the board to sleep and when to wake it up. Follow the instructions in the file to set this up for your application or to use the default.
   2. The runtime exposes a simple API for application (as listed in `runtime/include/runtime.h`). The file has some information about the API, but we provide more details here. We list the use case here,
      - `pin_init` for pin initialization; these need to happen every restart and should be fast.
      - `other_init` initialize other aspects of your code; these need to happen every restart and can be slow, but not too slow such that it uses all the energy.
      - `application` your application that you want to run intermittently. Note that the application only runs when voltage conditions are met (based on the voltage definitions in `runtime/include/defs.h`).
      - `report` although we run things intermittently, we also want a way for book keeping and analytics that would be too expensive to simply communication. The report pin and the report function accomplish this. If the report pin (defined in `runtime/include/defs.h`) is high, the runtime instead executes the report function; otherwise the application is run. This is run irrespective of min/max voltage and is the ideal place to print out analytics and other book keeping aspects. We use this report function to gather info on specific exit statistics and a host of other information used in our paper (eg. average latency, misses, etc. etc.).
      - `wakeup_sequence` called when the devices wakes up, i.e. meets max voltage. Should be quick code.
      - `low_energy_sequence` called when the device is about to sleep. Should be quick code.
   3. The runtime exposes a timer via the `get_ticks` function; each second takes 8192 ticks.
3. The application code:
   1. The `include/app.h` lets you control which specific experiment you want to run (i.e. Zygarde, pace-EE, pace-SLEX) and allows you to set your latency target. NOTE: latency targets are in ticks and currently cannot exceed 2^16 (about 8 s) due to variable limitations (`uint16_t`).
   2. The `include/uptime.h` allows you to control the various variables in our estimation algorithm such as the lookup table size, the momentum of the EWMA, the T_BIAS, etc. etc.
   3. `app.c` is our application. It first flashes the red LED, then runs a warmup. Afterwords, it runs the selected experiment the requested number of times. Once done, it starts flashing the green LED until a hard reset is done. You can do this by either pressing the RESET button on the board or removing the power entirely.
   4. `inf.c` is the dnn execution engine. The base engine only runs a model but the slumber engine supports depth replanning.
   5. `inf_approach.c` includes the various baselines used to run experiments.
   6. `init.c` includes initialization and per recharge bookkeeping functionality required by our estimation algorithm.
   7. `model_stats.c` houses the various model specific stats (e.g. runtimes). It also includes the eperceptive style lookup tables.
   8. `reporting.c` houses the majority of the bookkeeping and analytics logic which can be reported if the report pin is high.
   9. `uptime.c` maintains the latency estimation logic.

## Customizing the script

1. To change the model, you'll have to update the `dnn` and `weights` directories.
2. To change how the board interacts with the testbed, change values in `runtime/include/defs.h`.
3. To change the wakeup and sleep voltages, change values `runtime/include/defs.h`.
4. To experiment with various baselines and latencies, look at `include/app.h`. If running with ePerceptive style lookup tables, make sure it's calibrated for your capacitor and latency by changing the values in `model_stats.c`
5. To change how the estimation algorithm works and learns, take a look at `uptime.c` and `include/uptime.h`.

## Calibrating ePerceptive style lookup tables

This is very tedious to do, but it's doable. First, setup the testbed to circle through a wide range of non-volatile traces. Then essentially binary search the optimal ePerceptive style lookup table values: set a bunch of values, run 2-5 inferences per power trace, and check the report to see if your chosen values failed or succeeded for the observed recharge times. Adjust as needed until you get the optimal values. It's very tedious. Alternatively, you can run PACE and see what it does for a given recharge time, but that assumes you trust the PACE is working optimally. We used the first method to avoid any bias in the experiments.
