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

#include <assert.h>
#include <string.h>

#include <nnablart/network.h>
#include <nnablart/runtime.h>

#include "runtime_internal.h"

void *(*rt_variable_malloc_func)(size_t size) = malloc;
void (*rt_variable_free_func)(void *ptr) = free;

void *(*rt_malloc_func)(size_t size) = malloc;
void (*rt_free_func)(void *ptr) = free;

rt_return_value_t rt_allocate_context(rt_context_pointer *context) {
  rt_context_t *c = rt_malloc_func(sizeof(rt_context_t));
  memset(c, 0, sizeof(rt_context_t));
  if (c == 0) {
    return RT_RET_ERROR_ALLOCATE_CONTEXT;
  }
  c->callbacks = 0;
  c->num_of_callbacks = 0;
  *context = c;
  return RT_RET_NOERROR;
}

void rt_set_variable_malloc(void *(*user_malloc)(size_t size)) {
  if (user_malloc == 0) {
    rt_variable_malloc_func = malloc;
  } else {
    rt_variable_malloc_func = user_malloc;
  }
}

void rt_set_variable_free(void (*user_free)(void *ptr)) {
  if (user_free == 0) {
    rt_variable_free_func = free;
  } else {
    rt_variable_free_func = user_free;
  }
}

void rt_set_malloc(void *(*user_malloc)(size_t size)) {
  if (user_malloc == 0) {
    rt_malloc_func = malloc;
  } else {
    rt_malloc_func = user_malloc;
  }
}

void rt_set_free(void (*user_free)(void *ptr)) {
  if (user_free == 0) {
    rt_free_func = free;
  } else {
    rt_free_func = user_free;
  }
}

rt_return_value_t rt_add_callback(
    rt_context_pointer context, nn_function_type_t type,
    rt_return_value_t (*allocate_local_context)(nn_network_t *net, void *f)) {

  rt_context_t *c = context;

  rt_function_callback_t *callbacks;
  callbacks = rt_malloc_func((c->num_of_callbacks + 1) *
                             sizeof(rt_function_callback_t));
  if (callbacks == 0) {
    return RT_RET_ERROR_ALLOCATE_CALLBACK_BUFFER;
  }

  if (c->callbacks == 0) {
    c->callbacks = callbacks;
  } else {
    memcpy(callbacks, c->callbacks,
           c->num_of_callbacks * sizeof(rt_function_callback_t));
    rt_free_func(c->callbacks);
    c->callbacks = callbacks;
  }

  (c->callbacks + c->num_of_callbacks)->type = type;
  (c->callbacks + c->num_of_callbacks)->allocate_local_context =
      allocate_local_context;

  c->num_of_callbacks += 1;
  return RT_RET_NOERROR;
}

rt_return_value_t rt_initialize_context(rt_context_pointer context,
                                        nn_network_t *n) {
  rt_context_t *c = context;
  int i, j; // Iterator

  //////////////////////////////////////////////////////////////////////////////
  // Binary format version check
  if (n->version < NN_BINARY_FORMAT_MINIMUM_VERSION ||
      n->version > NN_BINARY_FORMAT_VERSION) {
    return RT_RET_ERROR_VERSION_UNMATCH;
  }

  //////////////////////////////////////////////////////////////////////////////
  // Buffer list
  c->num_of_buffers = n->buffers.size;
  c->buffers =
      rt_malloc_func(sizeof(rt_variable_buffer_context_t) * c->num_of_buffers);

  if (c->buffers == 0) {
    return RT_RET_ERROR_ALLOCATE_CONTEXT;
  }
  for (i = 0; i < c->num_of_buffers; i++) {
    c->buffers[i].allocate_type = RT_BUFFER_ALLOCATE_TYPE_INITIAL;
  }

  //////////////////////////////////////////////////////////////////////////////
  // Inputs
  rt_list_t inputs = create_rt_list_from_nn_list(n, n->inputs);
  c->num_of_inputs = inputs.size;
  c->input_variable_ids = rt_malloc_func(sizeof(int *) * c->num_of_inputs);
  for (i = 0; i < c->num_of_inputs; i++) {
    c->input_variable_ids[i] = inputs.data[i];
  }

  //////////////////////////////////////////////////////////////////////////////
  // Outputs
  rt_list_t outputs = create_rt_list_from_nn_list(n, n->outputs);
  c->num_of_outputs = outputs.size;
  c->output_variable_ids = rt_malloc_func(sizeof(int *) * c->num_of_outputs);
  for (i = 0; i < c->num_of_outputs; i++) {
    c->output_variable_ids[i] = outputs.data[i];
  }

  //////////////////////////////////////////////////////////////////////////////
  // Allocate buffers
  int *list = (int *)NN_GET(n, n->buffers.list);
  for (i = 0; i < c->num_of_buffers; i++) {
    if (c->buffers[i].allocate_type == RT_BUFFER_ALLOCATE_TYPE_INITIAL) {
      c->buffers[i].allocate_type = RT_BUFFER_ALLOCATE_TYPE_MALLOC;
      if (n->version == 2) {
        c->buffers[i].buffer =
            rt_variable_malloc_func(*(list + i) * sizeof(float));
        memset(c->buffers[i].buffer, 0, *(list + i) * sizeof(float));
      } else {
        c->buffers[i].buffer = rt_variable_malloc_func(*(list + i));
        memset(c->buffers[i].buffer, 0, *(list + i));
      }
      if (c->buffers[i].buffer == 0) {
        return RT_RET_ERROR_ALLOCATE_CONTEXT;
      }
    }
  }

  //////////////////////////////////////////////////////////////////////////////
  // Variables
  c->num_of_variables = n->variables.size;
  c->variables = rt_malloc_func(sizeof(rt_variable_t) * c->num_of_variables);
  if (c->variables == 0) {
    return RT_RET_ERROR_ALLOCATE_CONTEXT;
  }

  list = (int *)NN_GET(n, n->variables.list);
  for (i = 0; i < c->num_of_variables; i++) {
    nn_variable_t *var = (nn_variable_t *)(NN_GET(n, *(list + i)));
    c->variables[i].shape = create_rt_list_from_nn_list(n, var->shape);
    c->variables[i].type = var->type;
    c->variables[i].fp_pos = var->fp_pos;

    if (var->type == NN_DATA_TYPE_INT8 || var->type == NN_DATA_TYPE_INT16) {
      c->variables[i].coefficient = (1.0 / (1 << var->fp_pos));
    } else {
      c->variables[i].coefficient = 0;
    }

    if (var->data_index < 0) {
      int index = (-1 * var->data_index) - 1;
      if (index >= c->num_of_buffers) {
        return RT_RET_ERROR_INVALID_BUFFER_INDEX;
      }
      c->variables[i].data = c->buffers[index].buffer;
    } else {
      c->variables[i].data = NN_GET(n, var->data_index);
    }
  }

  //////////////////////////////////////////////////////////////////////////////
  // Functions
  c->num_of_functions = n->functions.size;
  c->functions =
      rt_malloc_func(sizeof(rt_function_context_t) * c->num_of_functions);
  list = (int *)NN_GET(n, n->functions.list);
  for (i = 0; i < c->num_of_functions; i++) {
    nn_function_t *func = (nn_function_t *)(NN_GET(n, *(list + i)));
    c->functions[i] = allocate_function_io(n, c, func);

    int callback_registered_flag = 0;
    if (func->impl <= NN_END_OF_USER_DEFINED_FUNCTION_IMPLEMENT) {
      for (j = 0; j < c->num_of_callbacks; j++) {
        if ((c->callbacks + j)->type == func->type) {
          rt_return_value_t ret =
              (c->callbacks +
               j)->allocate_local_context(n, (void *)(&(c->functions[i])));
          if (ret == RT_RET_FUNCTION_MATCH) {
            callback_registered_flag = 1;
            break;
          }
        }
      }
    }
    if (!callback_registered_flag) {
      allocate_function_context(n, func, c->functions + i);
    }
  }

  c->network = n;

  return RT_RET_NOERROR;
}

rt_return_value_t rt_free_context(rt_context_pointer *context) {
  rt_context_t *c = *context;

  int i; // Iterator

  // Buffers
  for (i = 0; i < c->num_of_buffers; i++) {
    if (c->buffers[i].allocate_type == RT_BUFFER_ALLOCATE_TYPE_MALLOC) {
      rt_variable_free_func(c->buffers[i].buffer);
    }
  }
  rt_free_func(c->buffers);

  // Variables
  rt_free_func(c->variables);

  // Functions
  for (i = 0; i < c->num_of_functions; i++) {
    rt_free_func(c->functions[i].func.inputs);
    rt_free_func(c->functions[i].func.outputs);

    c->functions[i].func.free_local_context_func(&(c->functions[i].func));
    if (c->functions[i].func.local_context != 0) {
      rt_free_func(c->functions[i].func.local_context);
      c->functions[i].func.local_context = NULL;
    }
  }
  rt_free_func(c->functions);

  rt_free_func(c->input_variable_ids);
  rt_free_func(c->output_variable_ids);

  // Callback
  if (c->callbacks) {
    rt_free_func(c->callbacks);
  }

  rt_free_func(*context);
  return RT_RET_NOERROR;
}

int rt_num_of_input(rt_context_pointer context) {
  return ((rt_context_t *)context)->num_of_inputs;
}

int rt_input_size(rt_context_pointer context, size_t index) {
  int i; // Iterator

  rt_context_t *c = context;
  rt_variable_t v = c->variables[c->input_variable_ids[index]];

  int size = 1;
  for (i = 0; i < v.shape.size; i++) {
    size *= v.shape.data[i];
  }

  return size;
}

int rt_input_dimension(rt_context_pointer context, size_t index) {
  rt_context_t *c = context;
  return c->variables[c->input_variable_ids[index]].shape.size;
}

int rt_input_shape(rt_context_pointer context, size_t index,
                   size_t shape_index) {
  rt_context_t *c = context;
  return c->variables[c->input_variable_ids[index]].shape.data[shape_index];
}

void *rt_input_buffer(rt_context_pointer context, size_t index) {
  rt_context_t *c = context;
  return c->variables[c->input_variable_ids[index]].data;
}

int rt_num_of_output(rt_context_pointer context) {
  return ((rt_context_t *)context)->num_of_outputs;
}

int rt_output_size(rt_context_pointer context, size_t index) {
  int i; // Iterator

  rt_context_t *c = context;
  rt_variable_t v = c->variables[c->output_variable_ids[index]];

  int size = 1;
  for (i = 0; i < v.shape.size; i++) {
    size *= v.shape.data[i];
  }

  return size;
}

int rt_output_dimension(rt_context_pointer context, size_t index) {
  rt_context_t *c = context;
  return c->variables[c->output_variable_ids[index]].shape.size;
}

int rt_output_shape(rt_context_pointer context, size_t index,
                    size_t shape_index) {
  rt_context_t *c = context;
  return c->variables[c->output_variable_ids[index]].shape.data[shape_index];
}

void *rt_output_buffer(rt_context_pointer context, size_t index) {
  rt_context_t *c = context;
  return c->variables[c->output_variable_ids[index]].data;
}

nn_variable_t *rt_input_variable(rt_context_pointer context, size_t index) {
  rt_context_t *c = context;
  const nn_network_t *n = c->network;
  int *list = (int *)NN_GET(n, n->variables.list);
  int i = c->input_variable_ids[index];
  return (nn_variable_t *)(NN_GET(n, *(list + i)));
}

nn_variable_t *rt_output_variable(rt_context_pointer context, size_t index) {
  rt_context_t *c = context;
  const nn_network_t *n = c->network;
  int *list = (int *)NN_GET(n, n->variables.list);
  int i = c->output_variable_ids[index];
  return (nn_variable_t *)(NN_GET(n, *(list + i)));
}

rt_return_value_t rt_forward(rt_context_pointer context) {
  int i; // Iterator
  rt_context_t *c = context;

  for (i = 0; i < c->num_of_functions; i++) {
    c->functions[i].func.exec_func(&(c->functions[i].func));
  }

  return RT_RET_NOERROR;
}

const char *const rt_c_runtime_version(void) { return NN_C_RUNTIME_VERSION; }

const int rt_nnb_version(void) { return NN_BINARY_FORMAT_VERSION; }

const char *const rt_nnb_revision(void) { return NN_BINARY_FORMAT_REVISION; }
