#ifndef __LIBDNN_DNN_H
#define __LIBDNN_DNN_H
#include <libmatAbstract/mat.h>
#include <libmsp/macro_basics.h>
#include <stdint.h>

typedef struct {
  uint8_t left;
  uint8_t right;
  uint8_t top;
  uint8_t bottom;
} pad_t;

typedef struct {
  uint8_t cols;
  uint8_t rows;
} stride_t;

void dnn_init();
void linear(mat_t *src, mat_t *dst, mat_t *weights, mat_t *bias);

void avg_pool(mat_t *src, mat_t *dst, stride_t strides, int16_t mult);
void max_pool(mat_t *src, mat_t *dst, stride_t strides);

void relu(mat_t *src, mat_t *dst, fixed max);
void inplace_relu(mat_t *src, fixed max);

void mat_add(mat_t *sr0, mat_t *sr1, mat_t *dst);
void mat_move(mat_t *src, mat_t *dst);

void pad(mat_t *src, fixed *dst, pad_t padding);

void convNN(mat_t *src, mat_t *dst, mat_t *flt, mat_t *bias, mat_t *scale,
            pad_t padding, stride_t strides);

void dscNN(mat_t *src, mat_t *dst, mat_t *flt, mat_t *bias, mat_t *scale,
           pad_t padding, stride_t strides);

void pwNN(mat_t *src, mat_t *dst, mat_t *flt, mat_t *bias, mat_t *scale,
          stride_t strides);

#endif // !__LIBDNN_DNN_H