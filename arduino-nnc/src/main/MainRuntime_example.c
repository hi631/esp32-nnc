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

//#define FINP
//#define IDF

#ifndef FINP
#include "01_iris_flower_dataset_validation_delo.h"
#endif
#ifndef _WIN32
extern void spiffs_init();
extern void spiffs_listdir();
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "MainRuntime_inference.h"
#include "MainRuntime_parameters.h"

int main(int argc, char* argv[])
{
	char buf[256];
	float label;

#ifdef FINP
#ifdef _WIN32
	FILE *fp = fopen("c_source\\01_iris_flower_dataset_validation_delo.csv", "r");
	if (fp == NULL) printf("File not.found");
	fgets(buf, 256, fp);
	while (fgets(buf, 256, fp)) {
#elseif IDF
	spiffs_init();
    spiffs_listdir("/spiffs");
	FILE* fp = fopen("/spiffs/iris_flower.csv", "r");
	if (fp == NULL) { printf("open.error"); while (1); }
	fgets(buf, 256, fp);
	while (fgets(buf, 256, fp)) {
#else
    FILE* fp; int rc = os_fopen("/iris_flower.csv", "r");
    if(rc == 0) { printf("File.Open.Error\n"); while(1); }
    os_fgets( buf, fp);
    while(os_fgets( buf, fp) != 0){
#endif
#else
	for (int i = 0; i < 30; i++) {
		strcpy(buf, csvdt[i + 1]);
#endif

		// Allocate and initialize context
		void *context = nnablart_mainruntime_allocate_context(MainRuntime_parameters);

		float *idbf = nnablart_mainruntime_input_buffer(context, 0); // Input.Data.Buffer

		sscanf(buf, "%f,%f,%f,%f,%f", &idbf[0], &idbf[1], &idbf[2], &idbf[3], &label);
		for (int j = 0; j<4; j++) printf(" %2.1f", idbf[j]); printf(" ( %1.0f ) ", label);

		// Exec inference
		nnablart_mainruntime_inference(context);

		// output check
		float *odbf = nnablart_mainruntime_output_buffer(context, 0); // Output>data.buffer
		float maxd = -99; int maxp = 0;
		for (int j = 0; j < 3; j++) {
			printf(" %+.3f", odbf[j]);
			if (odbf[j] > maxd) { maxd = odbf[j]; maxp = j; }
		}
		printf(" ( %d )\n", maxp);
		// free all context
		nnablart_mainruntime_free_context(context);

	}
#ifdef FINP
#ifdef _WIN32 
	fclose(fp);
#elseif IDF
	fclose(fp);
#else
	os_fclose(fp);
#endif
#endif
	return 0;
	}
