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

#include <assert.h>
#include <math.h>
#include "../../nnablart/config.h"
#include "../../nnablart/functions.h"

#ifdef CONFIG_CELU

typedef struct {
  rt_variable_t *input;
  rt_variable_getter get_input;
  int input_size;
  rt_variable_t *output;
  rt_variable_setter set_output;
  int output_size;
  rt_list_t in_shape;
} celu_private_t;

rt_function_error_t exec_celu_generic(rt_function_t *f);

// CELU
rt_function_error_t allocate_celu_local_context(rt_function_t *f) {
  if (f->num_of_inputs != 1) {
    return RT_FUNCTION_ERROR_INVALID_NUM_OF_INPUTS;
  }
  if (f->num_of_outputs != 1) {
    return RT_FUNCTION_ERROR_INVALID_NUM_OF_OUTPUTS;
  }

  celu_private_t *p = (celu_private_t *)rt_malloc_func(sizeof(celu_private_t));
  if (p == 0) {
    return RT_FUNCTION_ERROR_MALLOC;
  }

  ((celu_local_context_t *)f->local_context)->data = p;
  p->input = f->inputs[0];
  p->get_input = select_getter(p->input);
  p->input_size = calc_shape_size(f->inputs[0]->shape);
  p->output = f->outputs[0];
  p->set_output = select_setter(p->output);
  p->output_size = calc_shape_size(f->outputs[0]->shape);
  p->in_shape = clone_list(f->inputs[0]->shape);

  if (p->input_size * 2 != p->output_size) {
    free_list(p->in_shape);
    rt_free_func(p);
    return RT_FUNCTION_ERROR_INVALID_SHAPE;
  }
  if (p->input->type == NN_DATA_TYPE_FLOAT &&
      p->output->type == NN_DATA_TYPE_FLOAT) {
#ifdef CONFIG_CELU_FLOAT32
    f->exec_func = exec_celu;
#endif /* CONFIG_CELU_FLOAT32 */
  } else {
#ifdef CONFIG_CELU_GENERIC
    f->exec_func = exec_celu_generic;
#endif /* CONFIG_CELU_GENERIC */
  }
  return RT_FUNCTION_ERROR_NOERROR;
}

rt_function_error_t free_celu_local_context(rt_function_t *f) {
  celu_local_context_t *c = (celu_local_context_t *)(f->local_context);
  celu_private_t *p = (celu_private_t *)(c->data);
  free_list(p->in_shape);
  rt_free_func(c->data);
  return RT_FUNCTION_ERROR_NOERROR;
}

#ifdef CONFIG_CELU_FLOAT32
/**
 * https://arxiv.org/pdf/1603.05201.pdf
 * Definition 2.1. CReLU activation, denoted by ρc : R →R2
,* is defined as follows: ∀x ∈ R, ρc(x) = ([x]+, [?x]+).
 */
rt_function_error_t exec_celu(rt_function_t *f) {
  celu_local_context_t *c = (celu_local_context_t *)(f->local_context);
  celu_private_t *p = (celu_private_t *)(c->data);
  int i, j, s0 = 1, s1;

  for (i = c->axis; i < p->in_shape.size; ++i) {
    s0 *= p->in_shape.data[i];
  }
  s1 = p->input_size / s0;

  for (i = 0; i < s1; ++i) {
    for (j = 0; j < s0; ++j) {
      float x = *((float *)(p->input->data) + i * s0 + j);
      float *y = (float *)(p->output->data);
      *(y + i * s0 * 2 + j) = x > 0.0f ? x : c->alpha * (expf(x) - 1.0f);
      *(y + i * s0 * 2 + s0 + j) = x < 0.0f ? -x : c->alpha * (expf(-x) - 1.0f);
    }
  }
  return RT_FUNCTION_ERROR_NOERROR;
}
#endif /* CONFIG_CELU_FLOAT32 */

#ifdef CONFIG_CELU_GENERIC
rt_function_error_t exec_celu_generic(rt_function_t *f) {
  celu_local_context_t *c = (celu_local_context_t *)(f->local_context);
  celu_private_t *p = (celu_private_t *)(c->data);
  int i, j, s0 = 1, s1;

  for (i = c->axis; i < p->in_shape.size; ++i) {
    s0 *= p->in_shape.data[i];
  }
  s1 = p->input_size / s0;

  for (i = 0; i < s1; ++i) {
    for (j = 0; j < s0; ++j) {
      float x = p->get_input(p->input, i * s0 + j);
      float value = x > 0.0f ? x : c->alpha * (expf(x) - 1.0f);
      p->set_output(p->output, i * s0 * 2 + j, value);
      value = x < 0.0f ? -x : c->alpha * (expf(-x) - 1.0f);
      p->set_output(p->output, i * s0 * 2 + s0 + j, value);
    }
  }
  return RT_FUNCTION_ERROR_NOERROR;
}
#endif /* CONFIG_CELU_GENERIC */

#endif /* CONFIG_CELU */
