#ifndef __SLIMRE_INF_H
#define __SLIMRE_INF_H
#include "../runtime/include/runtime.h"
#include "libfixedAbstract/fixed.h"
#include "libmatAbstract/mat.h"
#include "libmsp/nv.h"
#include "model.h"
#include <stdbool.h>
#include <stdint.h>

typedef void (*nn_fn)(mat_t *, mat_t *, uint16_t width);
typedef struct {
  nn_fn fn;
  mat_t *s;
  mat_t *d;
  bool rd;
  nn_fn ee;
  uint16_t v;
} op_t;

extern __ro_hinv op_t dnn_layers[LAYER_LENGHT];

typedef struct {
  // filled for inf records
  uint32_t needed_up;
  uint32_t est_rt;
  uint32_t est_up;
  uint32_t est_late;
  uint16_t est_R;
  uint16_t i_time;
  uint16_t i_energy;
  uint8_t made_it;
  // runtime measurements
  uint8_t took_R;
  uint32_t took_rt;
  uint32_t took_up;
  uint32_t took_ckpt;
  // filled for inf and used by it
  uint8_t width;
  uint8_t exit_goal;
  uint8_t anytime;
  uint32_t start_t;
  uint32_t deadline;
  // filled by inf operation
  uint32_t exit_t;
  uint32_t end_t;
  uint8_t last_layer;
  uint8_t exit_achieved;
} result_t;

typedef struct {
  uint32_t max_end;
  uint32_t last_exit_end;
  uint16_t last_exit;
  uint16_t reduced;
  uint16_t last_layer;
} inf_ret_t;

extern __ro_hinv mat_t mat_out;
extern __nv bool simple_inf_happening;
extern __nv uint32_t inference_up;
extern __nv uint32_t inference_rt;
extern __nv uint32_t inference_ckpt;
extern __nv uint32_t prev_start_ticks;

#define UPDATE_WIDTH(mat, mult, width, reduct)                                 \
  {                                                                            \
    (mat)->dims[1] = (mult) * (width);                                         \
    (mat)->strides[0] = (mat)->strides[1] * (mat)->dims[1];                    \
  }

static inline void start_inf(uint32_t start) {
  inference_rt = 0;
  inference_up = 0;
  inference_ckpt = 0;
  prev_start_ticks = start;
  simple_inf_happening = true;
}

static inline void end_inf() {
  simple_inf_happening = false;
  inference_up += get_ticks() - prev_start_ticks;
}

void base_engine(op_t *layers, uint16_t length, result_t *inf);
void slumber_engine(op_t *layers, uint16_t length, result_t *inf,
                    uint16_t *latencies, bool adapt);

static inline void run_model(result_t *inf) {
  base_engine(dnn_layers, LAYER_LENGHT, inf);
}

static inline void run_slumber(result_t *inf, uint16_t *latencies, bool adapt) {
  slumber_engine(dnn_layers, LAYER_LENGHT, inf, latencies, adapt);
}

static inline void warmup_run(result_t *inf) {
  start_inf(inf->start_t);
  run_model(inf);
  end_inf();
}
#endif