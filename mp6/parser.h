/** @file parser.h */
#ifndef __PARSER_H
#define __PARSER_H

void parser_parse_makefile
(
	const char *makeFileName,
	char **targets,
	void (*parsed_new_target) (char *target),
	void (*parsed_new_dependency) (char *target, char *dependency),
	void (*parsed_new_command) (char *target, char *command)
);

#endif
