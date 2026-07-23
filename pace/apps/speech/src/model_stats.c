#include "include/app.h"
#include "include/model.h"

__nv uint16_t exit1_switch[WIDTH_SIZE] = {
    470, 725, 1020, 1290, 1640, 2010, 2430,
};

__nv uint16_t exit_main_switch[WIDTH_SIZE] = {
    1190, 1960, 2850, 3660, 4760, 5990, 7350,
};

// NOTE: Update these empirically based on the application and your capacitor
// configuration

#if CAP_CONFIG == CAP_CONFIG_330uF
__nv uint16_t slim_lookup_rt[WIDTH_SIZE] = {690, 450, 322, 249, 195, 157, 123};
__nv uint16_t ee_lookup_rt[MAX_EXIT] = {365, 123};
#elif CAP_CONFIG == CAP_CONFIG_1mF
#if TUNED_LATE
#if LATENCY == LATENCY1
__nv uint16_t slim_lookup_rt[WIDTH_SIZE] = {1, 1, 1, 1, 1, 1, 1};
__nv uint16_t ee_lookup_rt[MAX_EXIT] = {1, 1};
#elif LATENCY == LATENCY2
__nv uint16_t slim_lookup_rt[WIDTH_SIZE] = {1, 1, 1, 1, 1, 1, 1};
__nv uint16_t ee_lookup_rt[MAX_EXIT] = {1, 1};
#elif LATENCY == LATENCY3
#if GREEDY_LOOKUP
__nv uint16_t slim_lookup_rt[WIDTH_SIZE] = {1, 1, 1, 1, 1, 1, 1};
__nv uint16_t ee_lookup_rt[MAX_EXIT] = {1, 1};
#else
__nv uint16_t slim_lookup_rt[WIDTH_SIZE] = {10000, 1120, 805, 620,
                                            480,   380,  325};
__nv uint16_t ee_lookup_rt[MAX_EXIT] = {10000, 325};
#endif
#elif LATENCY == LATENCY4
__nv uint16_t slim_lookup_rt[WIDTH_SIZE] = {1, 1, 1, 1, 1, 1, 1};
__nv uint16_t ee_lookup_rt[MAX_EXIT] = {1, 1};
#elif LATENCY == LATENCY5
__nv uint16_t slim_lookup_rt[WIDTH_SIZE] = {1, 1, 1, 1, 1, 1, 1};
__nv uint16_t ee_lookup_rt[MAX_EXIT] = {1, 1};
#endif
#else
__nv uint16_t ee_lookup_rt[MAX_EXIT] = {10000, 240};
__nv uint16_t slexnet_lookup_rt[SLUM_CONFIG_SIZE] = {
    10000, 2500, 1700, 1500, 1000, 640, 520, 400, 310, 240};
#endif
#elif CAP_CONFIG == CAP_CONFIG_1mFBig
#if GREEDY_LOOKUP
__nv uint16_t slim_lookup_rt[WIDTH_SIZE] = {1, 1, 1, 1, 1, 1, 1};
__nv uint16_t ee_lookup_rt[MAX_EXIT] = {1, 1};
#else
__nv uint16_t slim_lookup_rt[WIDTH_SIZE] = {1, 1, 1, 1, 1, 1, 1};
__nv uint16_t ee_lookup_rt[MAX_EXIT] = {1, 1};
#endif
#endif

// PACE can measure these empyrically during warmup
// But we also measured them offline to double check
__nv uint16_t ee_uptime[MAX_EXIT] = {2420, 7330};

__nv uint16_t slim_switch_uptime[WIDTH_SIZE] = {
    1185, 1950, 2835, 3650, 4745, 5970, 7310,
};

__nv uint16_t early_slim_uptimes[WIDTH_SIZE][MAX_EXIT] = {
    {470, 1190},  {735, 1960},  {1020, 2850}, {1295, 3660},
    {1640, 4755}, {2020, 5985}, {2420, 7330},
};

__nv slumber_config slumber_configs[SLUM_CONFIG_SIZE] = {
    {.uptime = 7330, .exit = 2, .width = 8},
    {.uptime = 5985, .exit = 2, .width = 7},
    {.uptime = 4755, .exit = 2, .width = 6},
    {.uptime = 3660, .exit = 2, .width = 5},
    {.uptime = 2850, .exit = 2, .width = 4},
    {.uptime = 1960, .exit = 2, .width = 3},
    {.uptime = 1190, .exit = 2, .width = 2},
    {.uptime = 1020, .exit = 1, .width = 4},
    {.uptime = 735, .exit = 1, .width = 3},
    {.uptime = 470, .exit = 1, .width = 2},
};
