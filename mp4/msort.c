/** @file msort.c */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _psort_t {
	int* start;
	size_t size;
} psort_t;

typedef struct _pmerge_t {
	int* array_1;
	size_t size_1;
	int* array_2;
	size_t size_2;
	char kill;
} pmerge_t;

int upround(float a) {
	int base = (int)a;
	if(a == (float)base)
		return base;
	return base + 1;
}

void print_array(int* array, size_t size) {
	size_t i;
	for(i = 0; i < size; i++)
		printf("%i\n", array[i]);
}

int compare(const void* arg1, const void* arg2) {
	const int* first = (const int*)arg1;
	const int* second = (const int*)arg2;
	return (*first > *second) - (*first < *second);
}

void* sorter(void* ptr) {
	psort_t* data = ptr;
	qsort(data->start, data->size, sizeof(int), compare);
	fprintf(stderr, "Sorted %ld elements.\n", data->size);
	return NULL;
}

void* merger(void* ptr) {
	pmerge_t* data = ptr;
	psort_t* out = malloc(sizeof(psort_t));

	int total_size = data->size_1 + data->size_2;
	int* output = malloc(total_size * sizeof(int));
	int a_pos = 0;
	int b_pos = 0;
	int duplicates = 0;
	while((a_pos + b_pos) < total_size) {

		if(data->size_2 == 0 || (size_t)b_pos == data->size_2) {	//catches empty second list
			output[a_pos + b_pos] = data->array_1[a_pos];
			a_pos++;
		}
		else if((size_t)a_pos == data->size_1) {
			output[a_pos + b_pos] = data->array_2[b_pos];
			b_pos++;
		}
		else if(data->array_1[a_pos] < data->array_2[b_pos]) {
			output[a_pos + b_pos] = data->array_1[a_pos];
			a_pos++;
		}
		else if(data->array_1[a_pos] > data->array_2[b_pos]) {
			output[a_pos + b_pos] = data->array_2[b_pos];
			b_pos++;
		}
		else {
			output[a_pos + b_pos] = data->array_1[a_pos];
			duplicates++;
			a_pos++;
		}
	}

	fprintf(stderr, "Merged %ld and %ld elements with %i duplicates.\n", data->size_1, data->size_2, duplicates);

	if(data->kill) {
		//printf("freeing arrays, size_1 is %ld, size_2 is %ld\n", data->size_1, data->size_2);
		free(data->array_1);
		if(data->size_2 != 0)
			free(data->array_2);
	}

	out->start = output;
	out->size = total_size;

	return out;
}

void verify(int* arr1, int* arr2, size_t size) {
	size_t i;
	for(i = 0; i < size; i++) {
		if(arr1[i] != arr2[i]) {
			printf("Verification failed at index %ld. Got %i and %i.\n", i, arr1[i], arr2[i]);
			return;
		}
	}
	printf("Verification passed.\n");
}

int main(int argc, char **argv)
{
	if(argc <= 1) {
		fprintf(stderr, "usage: %s [# threads]\n", argv[0]);
		return 1;
	}

	int num_threads = atoi(argv[1]);
	int* in_array = malloc(sizeof(int));
	size_t in_array_length = 0;

	char* line = NULL;
	size_t size;

	while(getline(&line, &size, stdin) != -1) {
		in_array_length++;
		in_array = realloc(in_array, in_array_length * sizeof(int));
		in_array[in_array_length - 1] = atoi(line);
	}

	//int* original_array = malloc(sizeof(int) * in_array_length);
	//memcpy(original_array, in_array, sizeof(int) * in_array_length);

	size_t seg_size;

	if(in_array_length % num_threads == 0) 
		seg_size = in_array_length / num_threads;
	else
		seg_size = (in_array_length / num_threads) + 1;

	size_t size_remaining = in_array_length;

	psort_t* datas = malloc(sizeof(psort_t) * num_threads);
	pthread_t* threads = malloc(sizeof(pthread_t) * num_threads);

	int i;
	for(i = 0; i < num_threads; i++) {
		datas[i].start = &in_array[i * seg_size];
		if(size_remaining < seg_size) {
			datas[i].size = size_remaining;
			size_remaining = 0;
		}
		else {
			datas[i].size = seg_size;
			size_remaining -= seg_size;
		}
		pthread_create(&threads[i], NULL, sorter, &datas[i]);
	}

	for(i = 0; i < num_threads; i++)
		pthread_join(threads[i], NULL);

	int merge_queue_length = upround((float)num_threads / 2.0);
	pmerge_t* to_merge = malloc(sizeof(pmerge_t) * merge_queue_length);
	pthread_t* merge_threads = malloc(sizeof(pthread_t) * merge_queue_length);
	int first_pass = 1;
	int used_size = num_threads;

	while(used_size > 1) {
		for(i = 0; i < used_size; i += 2) {
			to_merge[i / 2].array_1 = datas[i].start;
			to_merge[i / 2].size_1 = datas[i].size;
			
			if(i + 1 < used_size) {
				to_merge[i / 2].array_2 = datas[i + 1].start;
				to_merge[i / 2].size_2 = datas[i + 1].size;
			}
			else {
				to_merge[i / 2].array_2 = NULL;
				to_merge[i / 2].size_2 = 0;
				used_size++;
			}

			if(first_pass)
				to_merge[i / 2].kill = 0;
			else
				to_merge[i / 2].kill = 1;

			pthread_create(&merge_threads[i / 2], NULL, merger, &to_merge[i / 2]);
		}

		for(i = 0; i < used_size; i++) {
			
			psort_t* out;
			pthread_join(merge_threads[i], (void**)&out);
			datas[i] = *out;
			used_size--;
			free(out);
		}

		if(first_pass)
			first_pass = 0;
	}

	print_array(datas[0].start, in_array_length);


	//qsort(original_array, in_array_length, sizeof(int), compare);
	//verify(datas[0].start, original_array, in_array_length);

	if(num_threads != 1)
		free(datas[0].start);
		
	free(in_array);
	free(merge_threads);
	free(datas);
	free(threads);
	free(to_merge);
	free(line);


	return 0;
}
