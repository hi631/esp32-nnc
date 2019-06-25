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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "MainRuntime_inference.h"
#include "MainRuntime_parameters.h"

#ifndef _WIN32
extern void spiffs_init();
extern void spiffs_listdir();
#endif

int main(int argc, char* argv[])
{
	char buf[256];
	char *datafn[8] = { "0_1.bin","1_1.bin","2_1.bin","3_1.bin", "4_1.bin","4_2.bin","9_1.bin","9_2.bin" };
#ifdef _WIN32
	char *dirn = "data\\";
#else
	spiffs_init();
	spiffs_listdir("/spiffs");
	char *dirn = "/spiffs/";
#endif

	for (int i = 0; i < 8; i++) {

		// Allocate and initialize context
		void *context = nnablart_mainruntime_allocate_context(MainRuntime_parameters);

		// Input files.
		float *idbf = nnablart_mainruntime_input_buffer(context, 0); // Input.Data.Buffer
		strcpy(buf, dirn); strcat(buf, datafn[i]);
		FILE* input0 = fopen(buf, "rb");
#ifdef _WIN32
		assert(input0);
#else
		if (!input0) { printf("File.NotFound\n"); while (1); }
#endif
		int input_read_size0 = fread(idbf, sizeof(float), NNABLART_MAINRUNTIME_INPUT0_SIZE, input0);
		assert(input_read_size0 == NNABLART_MAINRUNTIME_INPUT0_SIZE);
		fclose(input0);

		// Exec inference
		nnablart_mainruntime_inference(context);

		// Display Output
		float *odbf = nnablart_mainruntime_output_buffer(context, 0); // Output>data.buffer
		printf("%s ", datafn[i]);
		float maxd = -99; int maxp = 0;
		for (int j = 0; j < 10; j++) {
			printf(" %+3.3f", odbf[j]);
			if (odbf[j] > maxd) { maxd = odbf[j]; maxp = j; }
		}
		printf(" ( %d )\n", maxp);


		// free all context
		nnablart_mainruntime_free_context(context);
	}
	return 0;
}
