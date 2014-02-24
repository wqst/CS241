/** @file parser.c*/
#define _GNU_SOURCE
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "queue.h"
#include "rule.h"

/** Boolean datatype*/
typedef enum { False=0, True=1} boolean;


/**
 * Creates dynamically sized array of tokens from string.
 *
 * @param line String containing tokens.
 * @return Malloc()'ed array of tokens, NULL terminated.
 */
static char **parse_line(char *line)
{
	char **tokens = malloc( (strlen(line)+1) * sizeof(char*));
	int tokIdx = 0;
	tokens[tokIdx] = strtok(line, " ");
	while(tokens[tokIdx] != NULL){
		tokIdx++;
		tokens[tokIdx] = strtok(NULL, " ");
	}
	return tokens;
}


/**
 * Connects queue_iterate interface to free.
 *
 * @param arg Address to be freed.
 * @param trash Do NOT use.
 * @return Void.
 */
static void free_adapter(void *arg, void *trash){
	free(arg);
}


/**
 * Connects queue_iterate interface to free with rule_destroy call.
 *
 * @param arg Address to be freed.
 * @param trash do NOT use.
 * @return Void.
 */
static void rule_free_adapter(void *arg, void *trash){
	rule_t *rule = arg;
	queue_iterate(rule->deps, free_adapter, 0);
	queue_iterate(rule->commands, free_adapter, 0);
	
	free(rule->target);

	rule_destroy(arg);
	free(arg);
}


/**
 * Compares validRules' dependency list against curRule's target.
 * Simulates connectivity of DAG
 *
 * @param curRule Rule with target to check.
 * @param validRules Queue of already accepted rules.
 * @return Boolean based on whether match was made.
 */
static boolean findMatch(rule_t *curRule, queue_t *validRules){
	int valIdx;
	for(valIdx=0; valIdx < queue_size(validRules); valIdx++){
		rule_t *curValRule = queue_at(validRules, valIdx);
		int depIdx; 
		for(depIdx=0; depIdx < queue_size(curValRule->deps); depIdx++){
			char *dep = queue_at(curValRule->deps, depIdx);
			if(strcmp(curRule->target, dep) == 0){
				return True;
			}
		}
	}
	return False;
}


/**
 * Removes all rules which are not descendants of simulated DAG rooted at targets.
 *
 * @param rules Queue of rules which is read and modified.
 * @param targets List of targets which act as roots.
 * @return Void.
 */
static void filterOnTargets(queue_t **rules, char **targets){
	queue_t *validRules = malloc(sizeof(queue_t));
	queue_init(validRules);
	int idx;
	
	//initialize validRules with targets
	for(idx=0; idx < queue_size(*rules);idx++){
		rule_t *curRule = queue_at(*rules, idx);
		int tarIdx;
		for(tarIdx=0; targets[tarIdx] != NULL; tarIdx++){
			if(strcmp(curRule->target, targets[tarIdx]) == 0){
				queue_enqueue(validRules, curRule);
				queue_remove_at(*rules, idx);
				idx--;
				break;
			}
		}
	}

	//repeated linear search for new valid targets
	int prevSize = 0;
	while(prevSize != queue_size(validRules)){
		prevSize = queue_size(validRules);

		for(idx=0; idx < queue_size(*rules); idx++){
			rule_t *curRule = queue_at(*rules, idx);
			if(findMatch(curRule, validRules)){
				queue_enqueue(validRules, curRule);
				queue_remove_at(*rules, idx);
				idx--;
			}
		}
	}
	
	//cleanup old rules
	queue_iterate(*rules, rule_free_adapter, 0);
	queue_destroy(*rules);
	free(*rules);

	//assign new queue
	*rules = validRules;
}


/**
 * Calls appropriate callback functions for a given rule.
 *
 * @param rules Queue of rules.
 * @param notify_target Callback function for targets.
 * @param notify_dep Callback function for dependencies.
 * @param notify_cmd Callback function for commands.
 * @return Void.
 */
static void notifyRules
(
	queue_t *rules,
	void (*notify_target)(char *),
	void (*notify_dep)(char *,char *),
	void (*notify_cmd)(char *, char *)
)
{
	int ruleIdx, i;
	
	for(ruleIdx=0; ruleIdx < queue_size(rules); ruleIdx++){

		rule_t *curRule = queue_at(rules, ruleIdx);
		notify_target(curRule->target);

		for(i=0; i < queue_size(curRule->deps); i++){
			char *dep = queue_at(curRule->deps, i);
			notify_dep(curRule->target, dep);
		}
		
		for(i=0; i < queue_size(curRule->commands); i++){
			char *cmd = queue_at(curRule->commands, i);
			notify_cmd(curRule->target, cmd);
		}
	}
}


/**
 * Parses makeFileName based on GNU 'make' utility.
 *
 * @param makeFileName Path to valid makefile.
 * @param run_targets Null-terminated list of targets as listed on command-line.
 * @param parsed_new_target Function pointer to callback function for new target.
 * @param parsed_new_dependency Function pointer to callback function for new dependency.
 * @param parsed_new_command Function pointer to callback function for new command.
 * @return Void.
 */
void parser_parse_makefile
(
	const char *makeFileName,
	char **run_targets,
	void (*parsed_new_target)(char *target),
	void (*parsed_new_dependency)(char *target, char *dependency),
	void (*parsed_new_command)(char *target, char *command)
)
{
	boolean run_targetsMalloced = False;
	FILE *f = fopen(makeFileName, "r");
	assert(f != NULL);

	queue_t *rules = malloc(sizeof(queue_t));
	queue_init(rules);
	queue_t curRules;
	queue_init(&curRules);

	char *lineBuf = NULL;
	size_t bytes;
	size_t len;
	while((len = getline(&lineBuf, &bytes, f)) != -1){
	 
		if(len && lineBuf[len-1] == '\n'){
			lineBuf[--len] = '\0';
			if(len && lineBuf[len-1] == '\r')	
				lineBuf[--len] = '\0';
		}

		if(isalnum(lineBuf[0])){
			char *depLine = strstr(lineBuf, ":");
			assert(depLine != NULL);
			depLine[0] = '\0';
			depLine = depLine + 1;
			
			//insert prev Rule to queue
			int ruleIdx;
			for(ruleIdx=0; ruleIdx < queue_size(&curRules); ruleIdx++){
				queue_enqueue(rules, queue_at(&curRules,ruleIdx));
			}
			queue_destroy(&curRules);
			queue_init(&curRules);

			char **targets = parse_line(lineBuf);
			int tarIdx;
			for(tarIdx=0; targets[tarIdx] != NULL; tarIdx++){
				rule_t *newRule = malloc(sizeof(rule_t));
				rule_init(newRule);

				char *target = strdup(targets[tarIdx]);
				newRule->target = target;

				queue_enqueue(&curRules, newRule);
			}

			if(run_targets == NULL || run_targets[0] == NULL){
				run_targetsMalloced = True;
				run_targets = calloc(sizeof(char *), tarIdx + 1);
				int rtIdx;
				for(rtIdx=0; rtIdx < tarIdx; rtIdx++)
					run_targets[rtIdx] = strdup(targets[rtIdx]);
			}
			free(targets);

			char **deps = parse_line(depLine);
			int depIdx;
			for(depIdx=0; deps[depIdx] != NULL; depIdx++){
				int ruleIdx;
				for(ruleIdx=0; ruleIdx < queue_size(&curRules); ruleIdx++){
					rule_t *curRule = queue_at(&curRules, ruleIdx);
					char *dep = strdup(deps[depIdx]);
					queue_enqueue(curRule->deps, dep);
				}
			}
			free(deps);
		}
		else if(lineBuf[0] == '\t')
		{
			assert(queue_size(&curRules) != 0); //command without target
			int idx;
			for(idx=0; idx < queue_size(&curRules); idx++){
				rule_t *curRule = queue_at(&curRules, idx);
				char *command = strdup(lineBuf + 1);
				queue_enqueue(curRule->commands, command);
			}

		}else
		{
			;//ignore line
		}
	}
	int ruleIdx;
	for(ruleIdx=0; ruleIdx < queue_size(&curRules); ruleIdx++){
		queue_enqueue(rules, queue_at(&curRules, ruleIdx));
	}
	queue_destroy(&curRules); //No longer need curRules

	filterOnTargets(&rules, run_targets);

	//send callbacks
	notifyRules(rules, parsed_new_target, parsed_new_dependency, parsed_new_command);	

	//cleanup
	queue_iterate(rules, rule_free_adapter, 0);
	queue_destroy(rules);

	if(run_targetsMalloced){
		int tarIdx;
		for(tarIdx=0; run_targets[tarIdx] != NULL; tarIdx++)
			free(run_targets[tarIdx]);
		free(run_targets);
	}

	free(lineBuf);
	free(rules);

	fclose(f);
}
