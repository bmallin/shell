/** shell.h
 *
 * Author: Benjamin Mallin
 *
 * Header file for shell.c. 
 *
 * This file defines constants used in the program
 * as well as function declarations for shell.c.
 */

#ifndef SHELL_SHELL_H
#define SHELL_SHELL_H

/* Defines */
/* Input Buffer Config */
#define SHELL_INPUT_BUFFER_SIZE 1024
#define SHELL_INPUT_BUFFER_GROWTH 2
#define SHELL_INPUT_TOKEN_CHARS " \t\r\n\a"

/* Shell Config */
#define SHELL_NAME "shell"
#define SHELL_PROMPT SHELL_NAME "> "
#define SHELL_EXECUTE_BACKGROUND_TOKEN '&'

/* Error Messages */
#define SHELL_ERROR_BUFFER_ALLOC "Shell Error: Could not allocate input buffer.\n"
#define SHELL_ERROR_TOKEN_ALLOC "Shell Error: Could not allocate token buffer.\n"

/* Function Declarations */
void shell_repl(void);
char* shell_read_input(void);
char** shell_tokenize_input(char* string);
int shell_execute(char** arguments, int background);

/* Helper Functions */
void* shell_not_null(void* buffer, const char* message);
int shell_is_background(char* string);

#endif //SHELL_SHELL_H
