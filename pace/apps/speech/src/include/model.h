#ifndef __SLIMRE_MODEL_H
#define __SLIMRE_MODEL_H
#include <libmsp/mspbase.h>
#include <stdint.h>

#define WIDTH_SIZE 7
#define MAX_EXIT 2
#define MIN_WIDTH 2

#define LAYER_LENGHT 11

extern __nv uint16_t ee_lookup_rt[MAX_EXIT];
extern __nv uint16_t ee_uptime[MAX_EXIT];

extern __nv uint16_t slim_lookup_rt[WIDTH_SIZE];
extern __nv uint16_t slim_switch_uptime[WIDTH_SIZE];

extern __nv uint16_t early_slim_uptimes[WIDTH_SIZE][MAX_EXIT];

typedef struct {
  uint16_t uptime;
  uint16_t exit;
  uint16_t width;
} slumber_config;

#define SLUM_CONFIG_SIZE (WIDTH_SIZE + 3)
extern __nv slumber_config slumber_configs[SLUM_CONFIG_SIZE];
extern __nv uint16_t slexnet_lookup_rt[SLUM_CONFIG_SIZE];

#endif