# Usage information

We briefly outline how the testbed scripts can be used.

Ensure that the wires are connected as outline in `INSTALL.md` and that both the itsy bitsy and Discover 3 are connected to a computer. Then, simply running `python3 testbed/power_management/trace_play.py` setups the traces uses in the paper. You will need to provide the itsy bitsy tty port and whether you want constant or volatile traces. The script will then walk you through the traces, which run as long as needed (they loop over at some point).

The default traces in the script were used in the paper; edit the `wiper_vals` value in the `trace_play.py` script.
