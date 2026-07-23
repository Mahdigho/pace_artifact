#include "include/app.h"
#include "include/model.h"

// NOTE: Update these empirically based on the application and your capacitor
// configuration

#if CAP_CONFIG == CAP_CONFIG_330uF
__nv uint16_t slim_lookup_rt[WIDTH_SIZE] = {660, 450, 345, 234,
                                            191, 160, 130, 112};
__nv uint16_t ee_lookup_rt[MAX_EXIT] = {270, 162, 112};
#elif CAP_CONFIG == CAP_CONFIG_1mF
#if TUNED_LATE
#if LATENCY == LATENCY1
__nv uint16_t slim_lookup_rt[WIDTH_SIZE] = {10000, 630, 479, 305, 4, 3, 2, 1};
__nv uint16_t ee_lookup_rt[MAX_EXIT] = {10000, 3, 1};
#elif LATENCY == LATENCY2
__nv uint16_t slim_lookup_rt[WIDTH_SIZE] = {10000, 900, 700, 460,
                                            389,   305, 267, 1};
__nv uint16_t ee_lookup_rt[MAX_EXIT] = {10000, 310, 1};
#elif LATENCY == LATENCY3
#if GREEDY_LOOKUP
__nv uint16_t slim_lookup_rt[WIDTH_SIZE] = {10000, 1150, 840, 600,
                                            505,   420,  360, 306};
__nv uint16_t ee_lookup_rt[MAX_EXIT] = {10000, 420, 306};
#else
__nv uint16_t slim_lookup_rt[WIDTH_SIZE] = {10000, 1130, 810, 580,
                                            485,   390,  335, 280};
__nv uint16_t ee_lookup_rt[MAX_EXIT] = {10000, 400, 280};
#endif
#elif LATENCY == LATENCY4
__nv uint16_t slim_lookup_rt[WIDTH_SIZE] = {10000, 1400, 1130, 775,
                                            633,   536,  465,  400};
__nv uint16_t ee_lookup_rt[MAX_EXIT] = {10000, 535, 400};
#elif LATENCY == LATENCY5
__nv uint16_t slim_lookup_rt[WIDTH_SIZE] = {10000, 1800, 1410, 870,
                                            760,   650,  540,  471};
__nv uint16_t ee_lookup_rt[MAX_EXIT] = {10000, 660, 471};
#endif
#else

// 1.0 mF
__nv uint16_t ee_lookup_rt[MAX_EXIT] = {10000, 400, 290};
__nv uint16_t slexnet_lookup_rt[SLUM_CONFIG_SIZE] = {
    10000, 1900, 1600, 1150, 800, 560, 470, 400, 340, 290};
// 1.3 mF
// __nv uint16_t ee_lookup_rt[MAX_EXIT] = {10000, 510, 370};
// __nv uint16_t slexnet_lookup_rt[SLUM_CONFIG_SIZE] = {
//     10000, 2500, 2050, 1470, 1020, 700, 590, 510, 420, 370,
// } ;
#endif
#elif CAP_CONFIG == CAP_CONFIG_1mFBig
#if GREEDY_LOOKUP
__nv uint16_t slim_lookup_rt[WIDTH_SIZE] = {10000, 3387, 2500, 1790,
                                            1540,  1200, 1000, 890};
__nv uint16_t ee_lookup_rt[MAX_EXIT] = {10000, 1200, 890};
#else
__nv uint16_t slim_lookup_rt[WIDTH_SIZE] = {10000, 3200, 2350, 1700,
                                            1400,  1102, 940,  820};
__nv uint16_t ee_lookup_rt[MAX_EXIT] = {10000, 1102, 820};
#endif
#endif

// PACE can measure these empyrically during warmup
// But we also measured them offline to double check
__nv uint16_t ee_uptime[MAX_EXIT] = {
    3795,
    6160,
    8390,
};

__nv uint16_t slim_switch_uptime[WIDTH_SIZE] = {
    1465, 2095, 2790, 4210, 5135, 6180, 7205, 8390,
};

__nv uint16_t early_slim_uptimes[WIDTH_SIZE][MAX_EXIT] = {
    {805, 1175, 1475},  {1090, 1645, 2130}, {1415, 2165, 2810},
    {2030, 3190, 4250}, {2415, 3845, 5185}, {2865, 4600, 6210},
    {3310, 5335, 7250}, {3795, 6180, 8440}};

__nv slumber_config slumber_configs[SLUM_CONFIG_SIZE] = {
    {.uptime = 8440, .exit = 3, .width = 10},
    {.uptime = 7250, .exit = 3, .width = 9},
    {.uptime = 6210, .exit = 3, .width = 8},
    {.uptime = 5185, .exit = 3, .width = 7},
    {.uptime = 4250, .exit = 3, .width = 6},
    {.uptime = 2810, .exit = 3, .width = 5},
    {.uptime = 2130, .exit = 3, .width = 4},
    {.uptime = 1475, .exit = 3, .width = 3},
    {.uptime = 1175, .exit = 2, .width = 3},
    {.uptime = 805, .exit = 1, .width = 3},
};
