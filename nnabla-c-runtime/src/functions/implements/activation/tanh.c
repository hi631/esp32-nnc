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

#include <nnablart/config.h>
#include <nnablart/functions.h>

#include "../../utilities/accessor.h"
#include "../../utilities/shape.h"
#include <assert.h>
#include <math.h>

#ifdef CONFIG_TANH

typedef struct {
  rt_variable_t *input;
  rt_variable_getter get_input;
  int input_size;
  rt_variable_t *output;
  rt_variable_setter set_output;
  int output_size;
} tanh_local_context_t;

rt_function_error_t exec_tanh_generic(rt_function_t *f);

// Tanh
rt_function_error_t allocate_tanh_local_context(rt_function_t *f) {
  if (f->num_of_inputs != 1) {
    return RT_FUNCTION_ERROR_INVALID_NUM_OF_INPUTS;
  }
  if (f->num_of_outputs != 1) {
    return RT_FUNCTION_ERROR_INVALID_NUM_OF_OUTPUTS;
  }
  tanh_local_context_t *c = rt_malloc_func(sizeof(tanh_local_context_t));
  if (c == 0) {
    return RT_FUNCTION_ERROR_MALLOC;
  }
  f->local_context = c;
  c->input = f->inputs[0];
  c->get_input = select_getter(c->input);
  c->input_size = calc_shape_size(f->inputs[0]->shape);
  c->output = f->outputs[0];
  c->set_output = select_setter(c->output);
  c->output_size = calc_shape_size(f->outputs[0]->shape);
  if (c->input_size != c->output_size) {
    return RT_FUNCTION_ERROR_INVALID_SHAPE;
  }
  if (c->input->type == NN_DATA_TYPE_FLOAT &&
      c->output->type == NN_DATA_TYPE_FLOAT) {
#ifdef CONFIG_TANH_FLOAT32
    f->exec_func = exec_tanh;
#endif /* CONFIG_TANH_FLOAT32 */
  } else {
#ifdef CONFIG_TANH_GENERIC
    f->exec_func = exec_tanh_generic;
#endif /* CONFIG_TANH_GENERIC */
  }
  return RT_FUNCTION_ERROR_NOERROR;
}

rt_function_error_t free_tanh_local_context(rt_function_t *f) {
  return RT_FUNCTION_ERROR_NOERROR;
}

#ifdef CONFIG_TANH_FLOAT32
rt_function_error_t exec_tanh(rt_function_t *f) {
  tanh_local_context_t *c = (tanh_local_context_t *)(f->local_context);
  float *x = (float *)(c->input->data);
  float *y = (float *)(c->output->data);

  int i; // Iterator
  for (i = 0; i < c->input_size; i++) {
    y[i] = tanhf(x[i]);
  }
  return RT_FUNCTION_ERROR_NOERROR;
}
#endif /* CONFIG_TANH_FLOAT32 */

#ifdef CONFIG_TANH_GENERIC
rt_function_error_t exec_tanh_generic(rt_function_t *f) {
  tanh_local_context_t *c = (tanh_local_context_t *)(f->local_context);

  int i; // Iterator
  for (i = 0; i < c->input_size; i++) {
    float x = c->get_input(c->input, i);
    c->set_output(c->output, i, tanhf(x));
  }
  return RT_FUNCTION_ERROR_NOERROR;
}
#endif /* CONFIG_TANH_GENERIC */

#endif /* CONFIG_TANH */
