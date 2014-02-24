/** @file rule.h */
#ifndef __RULE_H__
#define __RULE_H__

#include "queue.h"
/**
 * Rule Data Structure
 */
typedef struct{
	char *target; ///<Name of target
	queue_t *deps; ///<List of dependencies
	queue_t *commands; ///<List of commands
} rule_t;

void rule_init(rule_t *rule);
void rule_destroy(rule_t *rule);

#endif
