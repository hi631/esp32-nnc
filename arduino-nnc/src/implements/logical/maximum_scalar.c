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

#include "../../utilities/shape.h"
#include "../arithmetic/arithmetic.h"
#include "../../nnablart/config.h"
#include "../../nnablart/functions.h"

#ifdef CONFIG_MAXIMUMSCALAR

rt_function_error_t exec_maximum_scalar_generic(rt_function_t *f);

// MaximumScalar
rt_function_error_t allocate_maximum_scalar_local_context(rt_function_t *f) {
  if (f->num_of_inputs != 1) {
    return RT_FUNCTION_ERROR_INVALID_NUM_OF_INPUTS;
  }
  if (f->num_of_outputs != 1) {
    return RT_FUNCTION_ERROR_INVALID_NUM_OF_OUTPUTS;
  }
  int input_size = calc_shape_size(f->inputs[0]->shape);
  int output_size = calc_shape_size(f->outputs[0]->shape);
  if (input_size != output_size) {
    return RT_FUNCTION_ERROR_INVALID_SHAPE;
  }
  if (f->inputs[0]->type == NN_DATA_TYPE_FLOAT &&
      f->outputs[0]->type == NN_DATA_TYPE_FLOAT) {
#ifdef CONFIG_MAXIMUMSCALAR_FLOAT32
    f->exec_func = exec_maximum_scalar;
#endif /* CONFIG_MAXIMUMSCALAR_FLOAT32 */
  } else {
#ifdef CONFIG_MAXIMUMSCALAR_GENERIC
    f->exec_func = exec_maximum_scalar_generic;
#endif /* CONFIG_MAXIMUMSCALAR_GENERIC */
  }
  return RT_FUNCTION_ERROR_NOERROR;
}

rt_function_error_t free_maximum_scalar_local_context(rt_function_t *f) {
  return RT_FUNCTION_ERROR_NOERROR;
}

#ifdef CONFIG_MAXIMUMSCALAR_FLOAT32
rt_function_error_t exec_maximum_scalar(rt_function_t *f) {
  maximum_scalar_local_context_t *context =
      (maximum_scalar_local_context_t *)(f->local_context);
  calc_scalar(f, context->val, select_max);
  return RT_FUNCTION_ERROR_NOERROR;
}
#endif /* CONFIG_MAXIMUMSCALAR_FLOAT32 */

#ifdef CONFIG_MAXIMUMSCALAR_GENERIC
rt_function_error_t exec_maximum_scalar_generic(rt_function_t *f) {
  maximum_scalar_local_context_t *context =
      (maximum_scalar_local_context_t *)(f->local_context);
  calc_scalar_generic(f, context->val, select_max);
  return RT_FUNCTION_ERROR_NOERROR;
}
#endif /* CONFIG_MAXIMUMSCALAR_GENERIC */

#endif /* CONFIG_MAXIMUMSCALAR */
