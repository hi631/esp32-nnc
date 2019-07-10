// Copyright (c) 2017 Sony Corporation. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "shape.h"

void pos_to_shape(rt_list_t out, rt_list_t shape, int pos) {
  int i = 0, o = 1;
  for (i = shape.size - 1; i >= 0; i--) {
    out.data[i] = (pos / o) % shape.data[i];
    o *= shape.data[i];
  }
}

int shape_to_pos(rt_list_t shape, rt_list_t pos) {
  int ret = 0;
  int i, o = 1;
  for (i = shape.size - 1; i >= 0; i--) {
    ret += o * pos.data[i];
    o *= shape.data[i];
  }
  return ret;
}

static inline int int_product(const int *data, int begin, int end) {
  int prod = 1;
  const int *const END = data + end;
  const int *it = data + begin;
  while (it != END) {
    prod *= *it;
    it++;
  }
  return prod;
}

int calc_shape_size(rt_list_t shape) {
  return int_product(shape.data, 0, shape.size);
}

int find_num_in_shape(rt_list_t shape, int num) {
  int i;
  for (i = 0; i < shape.size; i++) {
    if (shape.data[i] == num) {
      return i;
    }
  }
  return -1;
}

int shape_product_of(const rt_variable_t *v, unsigned shape_begin,
                     unsigned shape_end) {
  return int_product(v->shape.data, shape_begin, shape_end);
}

rt_list_t calc_contiguous_strides(rt_list_t shape) {
  if (!shape.size) {
    return allocate_list(0);
  }
  rt_list_t strides = allocate_list(shape.size);
  int i;
  for (i = 0; i < shape.size; ++i) {
    strides.data[i] = 1;
  }
  for (i = strides.size - 2; i >= 0; --i) {
    strides.data[i] *= strides.data[i + 1] * shape.data[i + 1];
  }
  return strides;
}
