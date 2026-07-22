EXEC = blink

OBJECTS = main.o

DEPS = libmsptimer:gcc libmspprintf:gcc libmspio:gcc libmsp:gcc

export LIBMSP_CLK_DCOFREQ = 1
export LIBMSP_CLK_MCLK_DIV = 2
export LIBUARTIO0_INT_ENABLE = 1
export LIBMSPPRINTF_PRINT_PORT = 0

include tools/maker/Makefile
