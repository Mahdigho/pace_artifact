EXEC = vww

OBJECTS =
OBJECTS += app.o reporting.o uptime.o init.o
OBJECTS += inf.o model.o inf_approach.o model_stats.o
OBJECTS += runtime/clock.o runtime/energy.o runtime/_checkpoint_state.o runtime/_checkpoint_cpu.o runtime/_checkpoint_lea.o runtime/runtime.o runtime/scheduler.o
# OBJECTS += clock.o
OBJECTS += dnn/util.o dnn/fc.o dnn/lea.o dnn/add.o dnn/conv_mac.o dnn/pw_mac.o dnn/dsc_mac.o

OBJECTS += weights/inputs.o
OBJECTS += weights/b0.o
OBJECTS += weights/b2_dsc.o weights/b2_pw.o
OBJECTS += weights/b3_dsc.o weights/b3_pw.o
OBJECTS += weights/b4_dsc.o weights/b4_pw.o
OBJECTS += weights/b5_dsc.o weights/b5_pw.o
OBJECTS += weights/b6_dsc.o weights/b6_pw.o
OBJECTS += weights/b7_dsc.o weights/b7_pw.o
OBJECTS += weights/b8_dsc.o weights/b8_pw.o
OBJECTS += weights/b9_dsc.o weights/b9_pw.o
OBJECTS += weights/b10_dsc.o weights/b10_pw.o
OBJECTS += weights/b11_dsc.o weights/b11_pw.o
OBJECTS += weights/b12_dsc.o weights/b12_pw.o
OBJECTS += weights/b13_dsc.o weights/b13_pw.o
OBJECTS += weights/b14_dsc.o weights/b14_pw.o
OBJECTS += weights/out0_dsc.o weights/out0_pw.o
OBJECTS += weights/out1_dsc.o weights/out1_pw.o
OBJECTS += weights/linear1.o weights/linear0.o weights/linear2.o


DEPS = libfixedAbstract:gcc libmatAbstract:gcc
DEPS += libdsp:gcc
DEPS += libmspprintf:gcc libmspio:gcc libmsp:gcc
DEPS += librng:gcc

export LIBMSP_CLK_DCOFREQ = 16
export LIBMSP_CLK_MCLK_DIV = 1
export LIBMSP_CLK_SMCLK_DIV = 1

export LIBUARTIO0_INT_ENABLE = 1
export LIBMSPPRINTF_PRINT_PORT = 0
# printing
export LIBMAT_CONSOLE = 1

# Hopefully won't have to use these.
export LIBFIXED_BITWIDTH = 16
# export LIBFIXED_FRAC_BITWIDTH = 15

override CFLAGS += -DSLEEPTIME_POWER_CONT=1

include tools/maker/Makefile

