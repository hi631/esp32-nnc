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

#ifndef H_RUNTIME_H_
#define H_RUNTIME_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @defgroup Runtime Runtime APIs
/// @brief Calculate feed forward neural network with @ref Network
///
/// - Sequence to use Runtime API.
///   - Initialization
/// @li Initialize runtime context with @ref rt_initialize_context()
/// @li Allocate I/O buffers in size get from @ref rt_num_of_input(), @ref
/// rt_input_size(), @ref rt_num_of_output() and @ref rt_output_size()
///   - Diagram
/// @startuml
/// skinparam monochrome true
/// hide footbox
///
/// UserApplication -> Runtime: "rt_allocate_context(&context)"
/// activate Runtime
/// Runtime --> UserApplication: "(Context)"
/// deactivate Runtime
///
/// loop
///   UserApplication -> Runtime: "rt_add_callback"
///   activate Runtime
///   Runtime --> UserApplication:
///   deactivate Runtime
///  end
///
/// UserApplication -> Runtime: "rt_initialize_context(context, network)"
/// activate Runtime
/// Runtime --> UserApplication:
/// deactivate Runtime
///
/// UserApplication -> Runtime: "rt_num_of_input(context)"
/// activate Runtime
/// Runtime --> UserApplication: num_of_input
/// deactivate Runtime
///
/// loop
///   UserApplication -> Runtime: "rt_input_size(context, index)"
///   activate Runtime
///   Runtime --> UserApplication: input_size[index]
///   deactivate Runtime
///   UserApplication -> Runtime: "rt_input_buffer(context, index)"
///   activate Runtime
///   Runtime --> UserApplication: input_buffer[index]
///   deactivate Runtime
/// end
///
/// UserApplication -> Runtime: "rt_num_of_output(context)"
/// activate Runtime
/// Runtime --> UserApplication: num_of_output
/// deactivate Runtime
///
/// loop
///   UserApplication -> Runtime: "rt_output_size(context, index)"
///   activate Runtime
///   Runtime --> UserApplication: output_size[index]
///   deactivate Runtime
///   UserApplication -> Runtime: "rt_output_buffer(context, index)"
///   activate Runtime
///   Runtime --> UserApplication: output_buffer[index]
///   deactivate Runtime
/// end
///
/// UserApplication -> UserApplication: Prepare to use buffer.
/// activate UserApplication
/// deactivate UserApplication
///
/// @enduml
///   - Inference
/// @li Prepare input data and buffer to output.
/// @li @ref rt_forward()
/// @li Use output data in output buffer.
/// @startuml
/// skinparam monochrome true
/// hide footbox
///
/// UserApplication -> UserApplication: Copy input data into input buffer
/// activate UserApplication
/// deactivate UserApplication
///
/// UserApplication -> Runtime: rt_forward(context)
/// activate Runtime
/// Runtime -> Runtime: (Execute feed forward neural network)
/// activate Runtime
/// deactivate Runtime
/// Runtime --> UserApplication: (Output stored into output_buffer)
/// deactivate Runtime
/// @enduml
///
/// @ref Runtime provides following functions.
/// - @ref rt_allocate_context()
/// - @ref rt_initialize_context()
/// - @ref rt_free_context()
/// - @ref rt_num_of_input()
/// - @ref rt_input_size()
/// - @ref rt_input_dimension()
/// - @ref rt_input_shape()
/// - @ref rt_num_of_output()
/// - @ref rt_output_size()
/// - @ref rt_output_dimension()
/// - @ref rt_output_shape()
/// - @ref rt_forward()
///
/// @{

/// @file
/// @brief @ref Runtime API Header

#include <nnablart/functions.h>

/// @brief Return values in @ref Runtime.
typedef enum {
  RT_RET_ERROR_VERSION_UNMATCH = -899,   ///< 899
  RT_RET_ERROR_ALLOCATE_CONTEXT,         ///< 898
  RT_RET_ERROR_INITIALIZE_CONTEXT_TWICE, ///< 897
  RT_RET_ERROR_ALLOCATE_CALLBACK_BUFFER, ///< 896
  RT_RET_ERROR_INVALID_BUFFER_INDEX,     ///< 895
  RT_RET_ERROR_INIT_VARIABLE,            ///< 894
  RT_RET_ERROR_UNKNOWN_FUNCTION,         ///< 893
  RT_RET_ERROR_NO_MATCHING_FUNCTION,     ///< 892
  RT_RET_NOERROR = 0,                    ///< 0
  RT_RET_FUNCTION_MATCH,                 ///< 1
  RT_RET_FUNCTION_DONT_MATCH,            ///< 2
  RT_RET_END_OF_VALUES
} rt_return_value_t;

typedef void *rt_context_pointer;

/// @brief Create runtime context.
/// In this function only allocates runtime context.
/// You must initialize context with @ref rt_initialize_context
/// @param[out] context Pointer to created context. It must be freed by @ref
/// rt_free_context()
/// @return @ref rt_return_value_t
rt_return_value_t rt_allocate_context(rt_context_pointer *context);

/// @brief Add callback function to runtime context.
/// @param[in] context
/// @param[in] type
/// @param[in] allocate_local_context
/// @return @ref rt_return_value_t
rt_return_value_t rt_add_callback(
    rt_context_pointer context, nn_function_type_t type,
    rt_return_value_t (*allocate_local_context)(nn_network_t *net,
                                                void *function_context));

/// @brief Initialize runtime context with parsing @ref nn_network_t.
/// Initialize all functions in context and prepare forward calculation.
///
///
/// Callback selection rule.
/// @startuml
/// skinparam monochrome true
/// start
///
/// if (impl <= NN_END_OF_SYSTEM_DEFINED_FUNCTION_IMPLEMENT) then (yes)
///   if (Found callback->type is same as func->type) then (yes)
///     :Call user defined allocator;
///     if (return value == RT_RET_FUNCTION_MATCH) then (yes)
///       stop
///     else (no)
///       :Call default allocator;
///       stop
///     endif
///   else (no)
///     :Call default allocator;
///     stop
///   endif
/// else (no)
///   :Call default allocator;
///   stop
/// endif
///
/// @enduml
///
/// @param[out] context Pointer to created context. It must be freed by @ref
/// rt_free_context()
/// @param[in] network
/// @return @ref rt_return_value_t
rt_return_value_t rt_initialize_context(rt_context_pointer context,
                                        nn_network_t *network);

/// @brief Free context.
/// @param[in] context
/// @return @ref rt_return_value_t
rt_return_value_t rt_free_context(rt_context_pointer *context);

/// @brief Get number of network inputs.
/// @param[in] context
/// @return Number of inputs
int rt_num_of_input(rt_context_pointer context);

/// @brief Size of input data at index
/// @param[in] context
/// @param[in] index
/// @return Size of data
int rt_input_size(rt_context_pointer context, size_t index);

/// @brief Get dimension of input data at index
/// @param[in] context
/// @param[in] index
/// @return Dimension size
int rt_input_dimension(rt_context_pointer context, size_t index);

/// @brief Shape size at shape_index.
/// @param[in] context
/// @param[in] index
/// @param[in] shape_index
/// @return Size
int rt_input_shape(rt_context_pointer context, size_t index,
                   size_t shape_index);

/// @brief Get input buffer at index
/// NOTE: This function may use for debug purpose.
/// @param[in] context
/// @param[in] index
/// @return pointer
void *rt_input_buffer(rt_context_pointer context, size_t index);

/// @brief Get number of network outputs.
/// @param[in] context
/// @return Number of outputs
int rt_num_of_output(rt_context_pointer context);

/// @brief Size of output data at index
/// @param[in] context
/// @param[in] index
/// @return Size of data
int rt_output_size(rt_context_pointer context, size_t index);

/// @brief Get dimension of output data at index
/// @param[in] context
/// @param[in] index
/// @return Dimension size
int rt_output_dimension(rt_context_pointer context, size_t index);

/// @brief Shape size at shape_index.
/// @param[in] context
/// @param[in] index
/// @param[in] shape_index
/// @return Size
int rt_output_shape(rt_context_pointer context, size_t index,
                    size_t shape_index);

/// @brief Get output buffer at index
/// NOTE: This function may use for debug purpose.
/// @param[in] context
/// @param[in] index
/// @return pointer
void *rt_output_buffer(rt_context_pointer context, size_t index);

/// @brief Get input variable description at index
/// This function obtains variable description such as data type,
/// floating point position(e.g.0.5 means fp=1), and so on.
/// NOTE: This function may use for debug purpose.
/// @param[in] context
/// @param[in] index
/// @return pointer to variable description.
nn_variable_t *rt_input_variable(rt_context_pointer context, size_t index);

/// @brief Get output variable description at index
/// This function obtains variable description such as data type,
/// floating point position(e.g.0.5 means fp=1), and so on.
/// NOTE: This function may use for debug purpose.
/// @param[in] context
/// @param[in] index
/// @return pointer to variable description.
nn_variable_t *rt_output_variable(rt_context_pointer context, size_t index);

/// @brief Execute feed forward calculation.
/// @param[in] context
/// @return @ref rt_return_value_t
rt_return_value_t rt_forward(rt_context_pointer context);

/// @brief user set variable malloc func.
/// @param[in] user_malloc
void rt_set_variable_malloc(void *(*user_malloc)(size_t size));

/// @brief user set variable free func.
/// @param[in] user_free
void rt_set_variable_free(void (*user_free)(void *ptr));

/// @brief user set malloc func.
/// @param[in] user_malloc
void rt_set_malloc(void *(*user_malloc)(size_t size));

/// @brief user set free func.
/// @param[in] user_free
void rt_set_free(void (*user_free)(void *ptr));

/// @brief return c runtime version string
const char *const rt_c_runtime_version(void);

/// @brief return nnb version number
const int rt_nnb_version(void);

/// @brief return nnb revision
const char *const rt_nnb_revision(void);

/// @}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // H_RUNTIME_H_
