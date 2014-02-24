/** @file rule.c */
#include <assert.h>
#include <stdlib.h>

#include "queue.h"
#include "rule.h"

/**
 * Initializes Rule data structure.
 *
 * @param rule A pointer to the rule data structure.
 * @return Void.
 */
void rule_init(rule_t *rule){
	assert( rule != NULL);
	rule->target = NULL;

	rule->deps = malloc(sizeof(queue_t));
	queue_init(rule->deps);
	
	rule->commands = malloc(sizeof(queue_t));
	queue_init(rule->commands);
}

/**
 * Destroys Rule data structure.
 *
 * @param rule A pointer to the rule data structure.
 * @return Void.
 */
void rule_destroy(rule_t *rule){
	queue_destroy(rule->deps);
	free(rule->deps);

	queue_destroy(rule->commands);
	free(rule->commands);
}
