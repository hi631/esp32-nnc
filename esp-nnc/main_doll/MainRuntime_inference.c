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

// *WARNING*
// THIS FILE IS AUTO-GENERATED BY CODE GENERATOR.
// PLEASE DO NOT EDIT THIS FILE BY HAND!

#include "MainRuntime_inference.h"

#include <nnablart/functions.h>
#include <string.h>

typedef struct {
    void *buffer_pool[2];
    void *param_pool[10];
    rt_buffer_allocate_type_t variable_buffers_allocate_type[21];

    // Variables
    rt_variable_t v0; ///< Input
    int v0_shape[4];
    rt_variable_t v1; ///< Affine/affine/W
    int v1_shape[2];
    rt_variable_t v2; ///< Affine/affine/b
    int v2_shape[1];
    rt_variable_t v3; ///< Affine_2/affine/W
    int v3_shape[2];
    rt_variable_t v4; ///< Affine_2/affine/b
    int v4_shape[1];
    rt_variable_t v5; ///< Affine_3/affine/W
    int v5_shape[2];
    rt_variable_t v6; ///< Affine_3/affine/b
    int v6_shape[1];
    rt_variable_t v7; ///< Affine_4/affine/W
    int v7_shape[2];
    rt_variable_t v8; ///< Affine_4/affine/b
    int v8_shape[1];
    rt_variable_t v9; ///< Affine_5/affine/W
    int v9_shape[2];
    rt_variable_t v10; ///< Affine_5/affine/b
    int v10_shape[1];
    rt_variable_t v11; ///< Affine
    int v11_shape[2];
    rt_variable_t v12; ///< Tanh
    int v12_shape[2];
    rt_variable_t v13; ///< Affine_2
    int v13_shape[2];
    rt_variable_t v14; ///< Tanh_2
    int v14_shape[2];
    rt_variable_t v15; ///< Affine_3
    int v15_shape[2];
    rt_variable_t v16; ///< Tanh_3
    int v16_shape[2];
    rt_variable_t v17; ///< Affine_4
    int v17_shape[2];
    rt_variable_t v18; ///< Tanh_4
    int v18_shape[2];
    rt_variable_t v19; ///< Affine_5
    int v19_shape[2];
    rt_variable_t v20; ///< Sigmoid
    int v20_shape[2];

    // Functions
    rt_function_t f0; ///< Affine
    rt_variable_t* f0_inputs[3];
    rt_variable_t* f0_outputs[1];
    affine_local_context_t *f0_local_context;
    rt_function_t f1; ///< Tanh
    rt_variable_t* f1_inputs[1];
    rt_variable_t* f1_outputs[1];
    rt_function_t f2; ///< Affine
    rt_variable_t* f2_inputs[3];
    rt_variable_t* f2_outputs[1];
    affine_local_context_t *f2_local_context;
    rt_function_t f3; ///< Tanh
    rt_variable_t* f3_inputs[1];
    rt_variable_t* f3_outputs[1];
    rt_function_t f4; ///< Affine
    rt_variable_t* f4_inputs[3];
    rt_variable_t* f4_outputs[1];
    affine_local_context_t *f4_local_context;
    rt_function_t f5; ///< Tanh
    rt_variable_t* f5_inputs[1];
    rt_variable_t* f5_outputs[1];
    rt_function_t f6; ///< Affine
    rt_variable_t* f6_inputs[3];
    rt_variable_t* f6_outputs[1];
    affine_local_context_t *f6_local_context;
    rt_function_t f7; ///< Tanh
    rt_variable_t* f7_inputs[1];
    rt_variable_t* f7_outputs[1];
    rt_function_t f8; ///< Affine
    rt_variable_t* f8_inputs[3];
    rt_variable_t* f8_outputs[1];
    affine_local_context_t *f8_local_context;
    rt_function_t f9; ///< Sigmoid
    rt_variable_t* f9_inputs[1];
    rt_variable_t* f9_outputs[1];
} nnablart_mainruntime_local_context_t;

int actual_buf_sizes[2] = {
    256,
    784,
};

void *(*rt_variable_malloc_func)(size_t size) = malloc;
void (*rt_variable_free_func)(void *ptr) = free;

void *(*rt_malloc_func)(size_t size) = malloc;
void (*rt_free_func)(void *ptr) = free;


void* nnablart_mainruntime_allocate_context(void** params)
{
    nnablart_mainruntime_local_context_t* c = malloc(sizeof(nnablart_mainruntime_local_context_t));
    // Variable buffer
    for (int i = 0; i < 2; i++) {
        c->buffer_pool[i] = malloc(sizeof(float) * actual_buf_sizes[i]);
        memset(c->buffer_pool[i], 0, sizeof(float) * actual_buf_sizes[i]);
    }
    if(params) {
        c->variable_buffers_allocate_type[1] = RT_BUFFER_ALLOCATE_TYPE_ALLOCATED;
        c->param_pool[0] = *params++;
        c->variable_buffers_allocate_type[2] = RT_BUFFER_ALLOCATE_TYPE_ALLOCATED;
        c->param_pool[1] = *params++;
        c->variable_buffers_allocate_type[3] = RT_BUFFER_ALLOCATE_TYPE_ALLOCATED;
        c->param_pool[2] = *params++;
        c->variable_buffers_allocate_type[4] = RT_BUFFER_ALLOCATE_TYPE_ALLOCATED;
        c->param_pool[3] = *params++;
        c->variable_buffers_allocate_type[5] = RT_BUFFER_ALLOCATE_TYPE_ALLOCATED;
        c->param_pool[4] = *params++;
        c->variable_buffers_allocate_type[6] = RT_BUFFER_ALLOCATE_TYPE_ALLOCATED;
        c->param_pool[5] = *params++;
        c->variable_buffers_allocate_type[7] = RT_BUFFER_ALLOCATE_TYPE_ALLOCATED;
        c->param_pool[6] = *params++;
        c->variable_buffers_allocate_type[8] = RT_BUFFER_ALLOCATE_TYPE_ALLOCATED;
        c->param_pool[7] = *params++;
        c->variable_buffers_allocate_type[9] = RT_BUFFER_ALLOCATE_TYPE_ALLOCATED;
        c->param_pool[8] = *params++;
        c->variable_buffers_allocate_type[10] = RT_BUFFER_ALLOCATE_TYPE_ALLOCATED;
        c->param_pool[9] = *params++;
    } else {
        c->variable_buffers_allocate_type[1] = RT_BUFFER_ALLOCATE_TYPE_MALLOC;
        c->param_pool[0] = *params++;
        c->param_pool[0] = malloc(sizeof(float) * 200704);
        c->variable_buffers_allocate_type[2] = RT_BUFFER_ALLOCATE_TYPE_MALLOC;
        c->param_pool[1] = *params++;
        c->param_pool[1] = malloc(sizeof(float) * 256);
        c->variable_buffers_allocate_type[3] = RT_BUFFER_ALLOCATE_TYPE_MALLOC;
        c->param_pool[2] = *params++;
        c->param_pool[2] = malloc(sizeof(float) * 32768);
        c->variable_buffers_allocate_type[4] = RT_BUFFER_ALLOCATE_TYPE_MALLOC;
        c->param_pool[3] = *params++;
        c->param_pool[3] = malloc(sizeof(float) * 128);
        c->variable_buffers_allocate_type[5] = RT_BUFFER_ALLOCATE_TYPE_MALLOC;
        c->param_pool[4] = *params++;
        c->param_pool[4] = malloc(sizeof(float) * 8192);
        c->variable_buffers_allocate_type[6] = RT_BUFFER_ALLOCATE_TYPE_MALLOC;
        c->param_pool[5] = *params++;
        c->param_pool[5] = malloc(sizeof(float) * 64);
        c->variable_buffers_allocate_type[7] = RT_BUFFER_ALLOCATE_TYPE_MALLOC;
        c->param_pool[6] = *params++;
        c->param_pool[6] = malloc(sizeof(float) * 640);
        c->variable_buffers_allocate_type[8] = RT_BUFFER_ALLOCATE_TYPE_MALLOC;
        c->param_pool[7] = *params++;
        c->param_pool[7] = malloc(sizeof(float) * 10);
        c->variable_buffers_allocate_type[9] = RT_BUFFER_ALLOCATE_TYPE_MALLOC;
        c->param_pool[8] = *params++;
        c->param_pool[8] = malloc(sizeof(float) * 100);
        c->variable_buffers_allocate_type[10] = RT_BUFFER_ALLOCATE_TYPE_MALLOC;
        c->param_pool[9] = *params++;
        c->param_pool[9] = malloc(sizeof(float) * 10);
    }

    // Variables
    // Input
    (c->v0).type = NN_DATA_TYPE_FLOAT;
    (c->v0).shape.size = 4;
    c->v0_shape[0] = 1;
    c->v0_shape[1] = 1;
    c->v0_shape[2] = 28;
    c->v0_shape[3] = 28;
    (c->v0).shape.data = c->v0_shape;
    (c->v0).data = c->buffer_pool[1];
    // Affine/affine/W
    (c->v1).type = NN_DATA_TYPE_FLOAT;
    (c->v1).shape.size = 2;
    c->v1_shape[0] = 784;
    c->v1_shape[1] = 256;
    (c->v1).shape.data = c->v1_shape;
    (c->v1).data = c->param_pool[0];
    // Affine/affine/b
    (c->v2).type = NN_DATA_TYPE_FLOAT;
    (c->v2).shape.size = 1;
    c->v2_shape[0] = 256;
    (c->v2).shape.data = c->v2_shape;
    (c->v2).data = c->param_pool[1];
    // Affine_2/affine/W
    (c->v3).type = NN_DATA_TYPE_FLOAT;
    (c->v3).shape.size = 2;
    c->v3_shape[0] = 256;
    c->v3_shape[1] = 128;
    (c->v3).shape.data = c->v3_shape;
    (c->v3).data = c->param_pool[2];
    // Affine_2/affine/b
    (c->v4).type = NN_DATA_TYPE_FLOAT;
    (c->v4).shape.size = 1;
    c->v4_shape[0] = 128;
    (c->v4).shape.data = c->v4_shape;
    (c->v4).data = c->param_pool[3];
    // Affine_3/affine/W
    (c->v5).type = NN_DATA_TYPE_FLOAT;
    (c->v5).shape.size = 2;
    c->v5_shape[0] = 128;
    c->v5_shape[1] = 64;
    (c->v5).shape.data = c->v5_shape;
    (c->v5).data = c->param_pool[4];
    // Affine_3/affine/b
    (c->v6).type = NN_DATA_TYPE_FLOAT;
    (c->v6).shape.size = 1;
    c->v6_shape[0] = 64;
    (c->v6).shape.data = c->v6_shape;
    (c->v6).data = c->param_pool[5];
    // Affine_4/affine/W
    (c->v7).type = NN_DATA_TYPE_FLOAT;
    (c->v7).shape.size = 2;
    c->v7_shape[0] = 64;
    c->v7_shape[1] = 10;
    (c->v7).shape.data = c->v7_shape;
    (c->v7).data = c->param_pool[6];
    // Affine_4/affine/b
    (c->v8).type = NN_DATA_TYPE_FLOAT;
    (c->v8).shape.size = 1;
    c->v8_shape[0] = 10;
    (c->v8).shape.data = c->v8_shape;
    (c->v8).data = c->param_pool[7];
    // Affine_5/affine/W
    (c->v9).type = NN_DATA_TYPE_FLOAT;
    (c->v9).shape.size = 2;
    c->v9_shape[0] = 10;
    c->v9_shape[1] = 10;
    (c->v9).shape.data = c->v9_shape;
    (c->v9).data = c->param_pool[8];
    // Affine_5/affine/b
    (c->v10).type = NN_DATA_TYPE_FLOAT;
    (c->v10).shape.size = 1;
    c->v10_shape[0] = 10;
    (c->v10).shape.data = c->v10_shape;
    (c->v10).data = c->param_pool[9];
    // Affine
    (c->v11).type = NN_DATA_TYPE_FLOAT;
    (c->v11).shape.size = 2;
    c->v11_shape[0] = 1;
    c->v11_shape[1] = 256;
    (c->v11).shape.data = c->v11_shape;
    (c->v11).data = c->buffer_pool[0];
    // Tanh
    (c->v12).type = NN_DATA_TYPE_FLOAT;
    (c->v12).shape.size = 2;
    c->v12_shape[0] = 1;
    c->v12_shape[1] = 256;
    (c->v12).shape.data = c->v12_shape;
    (c->v12).data = c->buffer_pool[1];
    // Affine_2
    (c->v13).type = NN_DATA_TYPE_FLOAT;
    (c->v13).shape.size = 2;
    c->v13_shape[0] = 1;
    c->v13_shape[1] = 128;
    (c->v13).shape.data = c->v13_shape;
    (c->v13).data = c->buffer_pool[0];
    // Tanh_2
    (c->v14).type = NN_DATA_TYPE_FLOAT;
    (c->v14).shape.size = 2;
    c->v14_shape[0] = 1;
    c->v14_shape[1] = 128;
    (c->v14).shape.data = c->v14_shape;
    (c->v14).data = c->buffer_pool[1];
    // Affine_3
    (c->v15).type = NN_DATA_TYPE_FLOAT;
    (c->v15).shape.size = 2;
    c->v15_shape[0] = 1;
    c->v15_shape[1] = 64;
    (c->v15).shape.data = c->v15_shape;
    (c->v15).data = c->buffer_pool[0];
    // Tanh_3
    (c->v16).type = NN_DATA_TYPE_FLOAT;
    (c->v16).shape.size = 2;
    c->v16_shape[0] = 1;
    c->v16_shape[1] = 64;
    (c->v16).shape.data = c->v16_shape;
    (c->v16).data = c->buffer_pool[1];
    // Affine_4
    (c->v17).type = NN_DATA_TYPE_FLOAT;
    (c->v17).shape.size = 2;
    c->v17_shape[0] = 1;
    c->v17_shape[1] = 10;
    (c->v17).shape.data = c->v17_shape;
    (c->v17).data = c->buffer_pool[0];
    // Tanh_4
    (c->v18).type = NN_DATA_TYPE_FLOAT;
    (c->v18).shape.size = 2;
    c->v18_shape[0] = 1;
    c->v18_shape[1] = 10;
    (c->v18).shape.data = c->v18_shape;
    (c->v18).data = c->buffer_pool[1];
    // Affine_5
    (c->v19).type = NN_DATA_TYPE_FLOAT;
    (c->v19).shape.size = 2;
    c->v19_shape[0] = 1;
    c->v19_shape[1] = 10;
    (c->v19).shape.data = c->v19_shape;
    (c->v19).data = c->buffer_pool[0];
    // Sigmoid
    (c->v20).type = NN_DATA_TYPE_FLOAT;
    (c->v20).shape.size = 2;
    c->v20_shape[0] = 1;
    c->v20_shape[1] = 10;
    (c->v20).shape.data = c->v20_shape;
    (c->v20).data = c->buffer_pool[1];

    // Functions
    // Affine
    c->f0_local_context = malloc(sizeof(affine_local_context_t));
    (c->f0).num_of_inputs = 3;
    (c->f0_inputs)[0] = &(c->v0);
    (c->f0_inputs)[1] = &(c->v1);
    (c->f0_inputs)[2] = &(c->v2);
    (c->f0).inputs = c->f0_inputs;
    (c->f0).num_of_outputs = 1;
    (c->f0_outputs)[0] = &(c->v11);
    (c->f0).outputs = c->f0_outputs;
    (c->f0).local_context = c->f0_local_context;
    (c->f0_local_context)->base_axis = 1;
    allocate_affine_local_context(&(c->f0));
    // Tanh
    (c->f1).num_of_inputs = 1;
    (c->f1_inputs)[0] = &(c->v11);
    (c->f1).inputs = c->f1_inputs;
    (c->f1).num_of_outputs = 1;
    (c->f1_outputs)[0] = &(c->v12);
    (c->f1).outputs = c->f1_outputs;
    (c->f1).local_context = 0;
    allocate_tanh_local_context(&(c->f1));
    // Affine
    c->f2_local_context = malloc(sizeof(affine_local_context_t));
    (c->f2).num_of_inputs = 3;
    (c->f2_inputs)[0] = &(c->v12);
    (c->f2_inputs)[1] = &(c->v3);
    (c->f2_inputs)[2] = &(c->v4);
    (c->f2).inputs = c->f2_inputs;
    (c->f2).num_of_outputs = 1;
    (c->f2_outputs)[0] = &(c->v13);
    (c->f2).outputs = c->f2_outputs;
    (c->f2).local_context = c->f2_local_context;
    (c->f2_local_context)->base_axis = 1;
    allocate_affine_local_context(&(c->f2));
    // Tanh
    (c->f3).num_of_inputs = 1;
    (c->f3_inputs)[0] = &(c->v13);
    (c->f3).inputs = c->f3_inputs;
    (c->f3).num_of_outputs = 1;
    (c->f3_outputs)[0] = &(c->v14);
    (c->f3).outputs = c->f3_outputs;
    (c->f3).local_context = 0;
    allocate_tanh_local_context(&(c->f3));
    // Affine
    c->f4_local_context = malloc(sizeof(affine_local_context_t));
    (c->f4).num_of_inputs = 3;
    (c->f4_inputs)[0] = &(c->v14);
    (c->f4_inputs)[1] = &(c->v5);
    (c->f4_inputs)[2] = &(c->v6);
    (c->f4).inputs = c->f4_inputs;
    (c->f4).num_of_outputs = 1;
    (c->f4_outputs)[0] = &(c->v15);
    (c->f4).outputs = c->f4_outputs;
    (c->f4).local_context = c->f4_local_context;
    (c->f4_local_context)->base_axis = 1;
    allocate_affine_local_context(&(c->f4));
    // Tanh
    (c->f5).num_of_inputs = 1;
    (c->f5_inputs)[0] = &(c->v15);
    (c->f5).inputs = c->f5_inputs;
    (c->f5).num_of_outputs = 1;
    (c->f5_outputs)[0] = &(c->v16);
    (c->f5).outputs = c->f5_outputs;
    (c->f5).local_context = 0;
    allocate_tanh_local_context(&(c->f5));
    // Affine
    c->f6_local_context = malloc(sizeof(affine_local_context_t));
    (c->f6).num_of_inputs = 3;
    (c->f6_inputs)[0] = &(c->v16);
    (c->f6_inputs)[1] = &(c->v7);
    (c->f6_inputs)[2] = &(c->v8);
    (c->f6).inputs = c->f6_inputs;
    (c->f6).num_of_outputs = 1;
    (c->f6_outputs)[0] = &(c->v17);
    (c->f6).outputs = c->f6_outputs;
    (c->f6).local_context = c->f6_local_context;
    (c->f6_local_context)->base_axis = 1;
    allocate_affine_local_context(&(c->f6));
    // Tanh
    (c->f7).num_of_inputs = 1;
    (c->f7_inputs)[0] = &(c->v17);
    (c->f7).inputs = c->f7_inputs;
    (c->f7).num_of_outputs = 1;
    (c->f7_outputs)[0] = &(c->v18);
    (c->f7).outputs = c->f7_outputs;
    (c->f7).local_context = 0;
    allocate_tanh_local_context(&(c->f7));
    // Affine
    c->f8_local_context = malloc(sizeof(affine_local_context_t));
    (c->f8).num_of_inputs = 3;
    (c->f8_inputs)[0] = &(c->v18);
    (c->f8_inputs)[1] = &(c->v9);
    (c->f8_inputs)[2] = &(c->v10);
    (c->f8).inputs = c->f8_inputs;
    (c->f8).num_of_outputs = 1;
    (c->f8_outputs)[0] = &(c->v19);
    (c->f8).outputs = c->f8_outputs;
    (c->f8).local_context = c->f8_local_context;
    (c->f8_local_context)->base_axis = 1;
    allocate_affine_local_context(&(c->f8));
    // Sigmoid
    (c->f9).num_of_inputs = 1;
    (c->f9_inputs)[0] = &(c->v19);
    (c->f9).inputs = c->f9_inputs;
    (c->f9).num_of_outputs = 1;
    (c->f9_outputs)[0] = &(c->v20);
    (c->f9).outputs = c->f9_outputs;
    (c->f9).local_context = 0;
    allocate_sigmoid_local_context(&(c->f9));
    return (void*)c;
}

int nnablart_mainruntime_free_context(void* context)
{
    nnablart_mainruntime_local_context_t* c = (nnablart_mainruntime_local_context_t*)context;

    for (int i = 0; i < 2; i++) {
        free(c->buffer_pool[i]);
    }
    if(c->variable_buffers_allocate_type[1] == RT_BUFFER_ALLOCATE_TYPE_MALLOC) {
        free(c->param_pool[0]);
    }
    if(c->variable_buffers_allocate_type[2] == RT_BUFFER_ALLOCATE_TYPE_MALLOC) {
        free(c->param_pool[1]);
    }
    if(c->variable_buffers_allocate_type[3] == RT_BUFFER_ALLOCATE_TYPE_MALLOC) {
        free(c->param_pool[2]);
    }
    if(c->variable_buffers_allocate_type[4] == RT_BUFFER_ALLOCATE_TYPE_MALLOC) {
        free(c->param_pool[3]);
    }
    if(c->variable_buffers_allocate_type[5] == RT_BUFFER_ALLOCATE_TYPE_MALLOC) {
        free(c->param_pool[4]);
    }
    if(c->variable_buffers_allocate_type[6] == RT_BUFFER_ALLOCATE_TYPE_MALLOC) {
        free(c->param_pool[5]);
    }
    if(c->variable_buffers_allocate_type[7] == RT_BUFFER_ALLOCATE_TYPE_MALLOC) {
        free(c->param_pool[6]);
    }
    if(c->variable_buffers_allocate_type[8] == RT_BUFFER_ALLOCATE_TYPE_MALLOC) {
        free(c->param_pool[7]);
    }
    if(c->variable_buffers_allocate_type[9] == RT_BUFFER_ALLOCATE_TYPE_MALLOC) {
        free(c->param_pool[8]);
    }
    if(c->variable_buffers_allocate_type[10] == RT_BUFFER_ALLOCATE_TYPE_MALLOC) {
        free(c->param_pool[9]);
    }
    free_affine_local_context(&(c->f0));
    if (c->f0.local_context != 0) {
        rt_free_func(c->f0.local_context);
        c->f0.local_context = 0;
    }
    free_tanh_local_context(&(c->f1));
    if (c->f1.local_context != 0) {
        rt_free_func(c->f1.local_context);
        c->f1.local_context = 0;
    }
    free_affine_local_context(&(c->f2));
    if (c->f2.local_context != 0) {
        rt_free_func(c->f2.local_context);
        c->f2.local_context = 0;
    }
    free_tanh_local_context(&(c->f3));
    if (c->f3.local_context != 0) {
        rt_free_func(c->f3.local_context);
        c->f3.local_context = 0;
    }
    free_affine_local_context(&(c->f4));
    if (c->f4.local_context != 0) {
        rt_free_func(c->f4.local_context);
        c->f4.local_context = 0;
    }
    free_tanh_local_context(&(c->f5));
    if (c->f5.local_context != 0) {
        rt_free_func(c->f5.local_context);
        c->f5.local_context = 0;
    }
    free_affine_local_context(&(c->f6));
    if (c->f6.local_context != 0) {
        rt_free_func(c->f6.local_context);
        c->f6.local_context = 0;
    }
    free_tanh_local_context(&(c->f7));
    if (c->f7.local_context != 0) {
        rt_free_func(c->f7.local_context);
        c->f7.local_context = 0;
    }
    free_affine_local_context(&(c->f8));
    if (c->f8.local_context != 0) {
        rt_free_func(c->f8.local_context);
        c->f8.local_context = 0;
    }
    free_sigmoid_local_context(&(c->f9));
    if (c->f9.local_context != 0) {
        rt_free_func(c->f9.local_context);
        c->f9.local_context = 0;
    }  
    free(context);
    return NN_ERROR_CODE_NOERROR;
}

float* nnablart_mainruntime_input_buffer(void* context, int index)
{
    nnablart_mainruntime_local_context_t* c = (nnablart_mainruntime_local_context_t*)context;
    switch(index) {
        case 0: return (c->v0).data;
    }
    return 0;
}

float* nnablart_mainruntime_output_buffer(void* context, int index)
{
    nnablart_mainruntime_local_context_t* c = (nnablart_mainruntime_local_context_t*)context;
    switch(index) {
        case 0: return (c->v20).data;
    }

    return 0;
}
float* nnablart_mainruntime_param_buffer(void* context, int index)
{
    nnablart_mainruntime_local_context_t* c = (nnablart_mainruntime_local_context_t*)context;
    switch(index) {
        case 0: return (c->v1).data;
        case 1: return (c->v2).data;
        case 2: return (c->v3).data;
        case 3: return (c->v4).data;
        case 4: return (c->v5).data;
        case 5: return (c->v6).data;
        case 6: return (c->v7).data;
        case 7: return (c->v8).data;
        case 8: return (c->v9).data;
        case 9: return (c->v10).data;
    }
    return 0;
}
int nnablart_mainruntime_inference(void* context)
{
    nnablart_mainruntime_local_context_t* c = (nnablart_mainruntime_local_context_t*)context;
    exec_affine(&(c->f0));
    exec_tanh(&(c->f1));
    exec_affine(&(c->f2));
    exec_tanh(&(c->f3));
    exec_affine(&(c->f4));
    exec_tanh(&(c->f5));
    exec_affine(&(c->f6));
    exec_tanh(&(c->f7));
    exec_affine(&(c->f8));
    exec_sigmoid(&(c->f9));
    return NN_ERROR_CODE_NOERROR;
}
