#include "include/app.h"
#include "include/model.h"

// NOTE: Update these empirically based on the application and your capacitor
// configuration

#if CAP_CONFIG == CAP_CONFIG_330uF
__nv uint16_t slim_lookup_rt[WIDTH_SIZE] = {500, 360, 275, 225, 170, 150, 124};
__nv uint16_t ee_lookup_rt[MAX_EXIT] = {640, 249, 124};
#elif CAP_CONFIG == CAP_CONFIG_1mF
#if TUNED_LATE
#if LATENCY == LATENCY1
__nv uint16_t slim_lookup_rt[WIDTH_SIZE] = {1, 1, 1, 1, 1, 1};
__nv uint16_t ee_lookup_rt[MAX_EXIT] = {1, 1, 1};
#elif LATENCY == LATENCY2
__nv uint16_t slim_lookup_rt[WIDTH_SIZE] = {1, 1, 1, 1, 1, 1};
__nv uint16_t ee_lookup_rt[MAX_EXIT] = {1, 1, 1};
#elif LATENCY == LATENCY3
#if GREEDY_LOOKUP
__nv uint16_t slim_lookup_rt[WIDTH_SIZE] = {1, 1, 1, 1, 1, 1};
__nv uint16_t ee_lookup_rt[MAX_EXIT] = {1, 1, 1};
#else
__nv uint16_t slim_lookup_rt[WIDTH_SIZE] = {1380, 930, 690, 430, 370, 310};
__nv uint16_t ee_lookup_rt[MAX_EXIT] = {10000, 630, 310};
#endif
#elif LATENCY == LATENCY4
__nv uint16_t slim_lookup_rt[WIDTH_SIZE] = {1, 1, 1, 1, 1, 1};
__nv uint16_t ee_lookup_rt[MAX_EXIT] = {1, 1, 1};
#elif LATENCY == LATENCY5
__nv uint16_t slim_lookup_rt[WIDTH_SIZE] = {1, 1, 1, 1, 1, 1};
__nv uint16_t ee_lookup_rt[MAX_EXIT] = {1, 1, 1};
#endif
#else
__nv uint16_t slim_lookup_rt[WIDTH_SIZE] = {1380, 930, 690, 430, 370, 310};
__nv uint16_t ee_lookup_rt[MAX_EXIT] = {10000, 540, 260};
__nv uint16_t slexnet_lookup_rt[SLUM_CONFIG_SIZE] = {
    10000, 4000, 2500, 2000, 1600, 1160, 970,
    740,   640,  560,  510,  370,  320,  260};
#endif
#elif CAP_CONFIG == CAP_CONFIG_1mFBig
#if GREEDY_LOOKUP
__nv uint16_t slim_lookup_rt[WIDTH_SIZE] = {1, 1, 1, 1, 1, 1};
__nv uint16_t ee_lookup_rt[MAX_EXIT] = {1, 1, 1};
#else
__nv uint16_t slim_lookup_rt[WIDTH_SIZE] = {1, 1, 1, 1, 1, 1};
__nv uint16_t ee_lookup_rt[MAX_EXIT] = {1, 1, 1};
#endif
#endif

// PACE can measure these empyrically during warmup
// But we also measured them offline to double check
__nv uint16_t ee_uptime[MAX_EXIT] = {2160, 5235, 10620};

__nv uint16_t slim_switch_uptime[WIDTH_SIZE] = {
    2390, 3460, 4730, 5820, 7650, 8865, 10620,
};

__nv uint16_t early_slim_uptimes[WIDTH_SIZE][MAX_EXIT] = {
    {770, 1420, 2400},   {870, 1860, 3470},  {1230, 2570, 4750},
    {1350, 3045, 5850},  {1700, 3985, 7690}, {1820, 4530, 8910},
    {2160, 5275, 10670},
};

__nv slumber_config slumber_configs[SLUM_CONFIG_SIZE] = {
    {.uptime = 10650, .exit = 3, .width = 8}, // 82
    {.uptime = 8890, .exit = 3, .width = 7},  // 81
    {.uptime = 7670, .exit = 3, .width = 6},  // 80
    {.uptime = 5835, .exit = 3, .width = 5},  // 79
    {.uptime = 5245, .exit = 2, .width = 8},  // 765
    {.uptime = 4490, .exit = 2, .width = 7},  // 758
    // {.uptime = 4750, .exit = 3, .width = 4}, // 754
    {.uptime = 3970, .exit = 2, .width = 6}, // 745
    {.uptime = 3025, .exit = 2, .width = 5}, // 739
    // {.uptime = 3470, .exit = 3, .width = 3}, // 737
    {.uptime = 2400, .exit = 3, .width = 2}, // 712
    {.uptime = 1860, .exit = 2, .width = 3}, // 678
    {.uptime = 1420, .exit = 2, .width = 2}, // 659
    {.uptime = 1230, .exit = 1, .width = 4}, // 659
    {.uptime = 870, .exit = 1, .width = 3},  // 659
    {.uptime = 770, .exit = 1, .width = 2},  // 659
};
