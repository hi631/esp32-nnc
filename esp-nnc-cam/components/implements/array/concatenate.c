// Copyright (c) 2018 Sony Corporation. All Rights Reserved.
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
#include "../../utilities/accessor.h"
#include "../../utilities/shape.h"
#include <nnablart/config.h>
#include <nnablart/functions.h>

#ifdef CONFIG_CONCATENATE

typedef struct {
  int outer_size;
  int inner_total_size;
  rt_list_t *in_shape;
} concatenate_private_t;

static inline int calc_size(rt_list_t shape, int axis) {
  int size = 1;
  for (int i = axis; i < shape.size; i++) {
    size *= shape.data[i];
  }
  return size;
}

rt_function_error_t exec_concatenate_generic(rt_function_t *f);

// Concatenate
rt_function_error_t allocate_concatenate_local_context(rt_function_t *f) {
  concatenate_local_context_t *c =
      (concatenate_local_context_t *)(f->local_context);

#ifdef CONFIG_CONCATENATE_FLOAT32
  f->exec_func = exec_concatenate;
#endif /* CONFIG_CONCATENATE_FLOAT32 */

#ifdef CONFIG_CONCATENATE_GENERIC
  for (int i = 0; i < f->num_of_inputs; i++) {
    if (f->inputs[i]->type != NN_DATA_TYPE_FLOAT) {
      f->exec_func = exec_concatenate_generic;
    }
  }
  if (f->outputs[0]->type != NN_DATA_TYPE_FLOAT) {
    f->exec_func = exec_concatenate_generic;
  }
#endif /* CONFIG_CONCATENATE_GENERIC */

  concatenate_private_t *p =
      (concatenate_private_t *)rt_malloc_func(sizeof(concatenate_private_t));
  if (p == 0) {
    return RT_FUNCTION_ERROR_MALLOC;
  }

  p->inner_total_size = 0;
  p->in_shape =
      (rt_list_t *)rt_malloc_func(sizeof(rt_list_t) * f->num_of_inputs);
  for (int i = 0; i < f->num_of_inputs; i++) {
    p->in_shape[i] = clone_list(f->inputs[i]->shape);
    const int inner_size = calc_size(p->in_shape[i], c->axis);
    p->inner_total_size += inner_size;

    if (i >= 1) {
      p->in_shape[0].data[c->axis] += p->in_shape[i].data[c->axis];
    }
  }
  p->outer_size =
      calc_shape_size(p->in_shape[0]) / calc_size(p->in_shape[0], c->axis);

  ((concatenate_local_context_t *)(f->local_context))->data = (void *)p;
  return RT_FUNCTION_ERROR_NOERROR;
}

rt_function_error_t free_concatenate_local_context(rt_function_t *f) {
  concatenate_private_t *p =
      (concatenate_private_t
           *)(((concatenate_local_context_t *)(f->local_context))->data);
  for (int i = 0; i < f->num_of_inputs; i++) {
    free_list(p->in_shape[i]);
  }
  rt_free_func(p->in_shape);
  rt_free_func(p);
  return RT_FUNCTION_ERROR_NOERROR;
}

#ifdef CONFIG_CONCATENATE_FLOAT32
rt_function_error_t exec_concatenate(rt_function_t *f) {
  concatenate_local_context_t *c =
      (concatenate_local_context_t *)(f->local_context);
  concatenate_private_t *p = (concatenate_private_t *)(c->data);

  float *y = (float *)(f->outputs[0]->data);
  int inner_offset = 0;
  for (int i = 0; i < f->num_of_inputs; i++) {
    const float *x = (float *)(f->inputs[i]->data);
    const int inner_size = calc_size(f->inputs[i]->shape, c->axis);
    for (int j = 0; j < p->outer_size; ++j) {
      for (int k = 0; k < inner_size; k++) {
        y[j * p->inner_total_size + inner_offset + k] = x[j * inner_size + k];
      }
    }
    inner_offset += inner_size;
  }
  return RT_FUNCTION_ERROR_NOERROR;
}
#endif /* CONFIG_CONCATENATE_FLOAT32 */

#ifdef CONFIG_CONCATENATE_GENERIC
rt_function_error_t exec_concatenate_generic(rt_function_t *f) {
  concatenate_local_context_t *c =
      (concatenate_local_context_t *)(f->local_context);
  concatenate_private_t *p = (concatenate_private_t *)(c->data);
  rt_variable_t *output = f->outputs[0];
  rt_variable_setter set_output = select_setter(output);

  int inner_offset = 0;
  for (int i = 0; i < f->num_of_inputs; i++) {
    rt_variable_t *input = f->inputs[i];
    rt_variable_getter get_input = select_getter(input);
    const int inner_size = calc_size(p->in_shape[i], c->axis);
    for (int j = 0; j < p->outer_size; ++j) {
      for (int k = 0; k < inner_size; k++) {
        const float x = get_input(input, j * inner_size + k);
        set_output(output, j * p->inner_total_size + inner_offset + k, x);
      }
    }
    inner_offset += inner_size;
  }
  return RT_FUNCTION_ERROR_NOERROR;
}
#endif /* CONFIG_CONCATENATE_GENERIC */

#endif /* CONFIG_CONCATENATE */
