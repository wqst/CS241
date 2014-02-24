/*
 * Machine Problem #1
 * Hacker Version Benchmark -- This file does not test your functionality but only benchmark your dictionary performance
 * CS 241 Fall2013
 */


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "libdictionary/libdictionary.h"

#define MAX_ENTRIES 1000000

int main()
{
	/**
 	 * Set the start time
 	 */ 
	clock_t start, end;
	double time_spent;
	start = clock();

	/*
	 * Initialize the dictionary data structure.
	 */
	dictionary_t dictionary;
	dictionary_init(&dictionary);

	int i=0;
	char** key_values = (char**)malloc(MAX_ENTRIES*sizeof(char*));
	for(i=0; i<MAX_ENTRIES; i++){
	    key_values[i] = (char*)malloc(50*sizeof(char));
	    sprintf(key_values[i], "%d: %d", i, i); 
	    if(dictionary_parse(&dictionary, key_values[i])!=0)
	    {
		printf("NOT_OK...THE ITEM SHOULD NOT ALREADY EXIST IN DICTIONARY\n");
		return 1;
	    } 
	}

        for(i=0; i<MAX_ENTRIES; i++){
	    char tmp[30];
            sprintf(tmp, "%d", i);
            if(strcmp(tmp, dictionary_get(&dictionary, tmp))!=0)
	    {
		printf("NOT_OK...CANNOT GET EXISTING ITEM IN DICTIONARY\n");
	        return 1;
	    }
        }

        for(i=0; i<MAX_ENTRIES; i++){
            if(dictionary_parse(&dictionary, key_values[i])==0)
            {
                printf("NOT_OK...THE ITEM SHOULD ALREADY EXIST IN DICTIONARY\n");
                return 1;
            } 
        }

	for(i=0; i<MAX_ENTRIES; i++){
            char tmp[30];
            sprintf(tmp, "%d", i);
	    if(dictionary_remove(&dictionary, tmp)!=0){
                printf("NOT_OK...THE ITEM TO REMOVE SHOULD IN DICTIONARY\n");
                return 1;
	    }
	}

	/*
	 * Free up the memory used by the dictionary and close the file.
	 */
	dictionary_destroy(&dictionary);

	for(i=0; i<MAX_ENTRIES; i++)
	    free(key_values[i]);
	free(key_values);

	/**
 	 * Set the end time
 	 */ 
        end = clock();
	time_spent = (double)(end - start) / CLOCKS_PER_SEC;
	printf("The running time for your dictionary is: %f seconds\n", time_spent);

	return 0;
}

