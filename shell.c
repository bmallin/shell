/** shell.c
 *
 * Author: Benjamin Mallin
 *
 * This file contains the shell implementation.
 * The main bits are in shell_execute and shell_repl.
 * Other functions are for user I/O or are helper functions.
 */

#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define NDEBUG
#include "debug.h"

#include "shell.h"

/**
 * Ensure `buffer` is not NULL.
 *
 * If `buffer` is NULL, print `message` to stderr and exit
 * with status code 1.
 *
 */
void* shell_not_null(void* buffer, const char* message)
{
    if (buffer == NULL)
    {
        fprintf(stderr, "%s", message);
        exit(1);
    }

    return buffer;
}

/**
 * Determine if command should be run in the background.
 * 
 * Returns 1 if command should be run in the background
 * and 0 if it should not. 
 * 
 * Note: The `string` argument is modified if a & character
 *       is found at the end.
 */
int shell_is_background(char* string)
{
    size_t length = strlen(string);
    
    if (string[length - 1] == SHELL_EXECUTE_BACKGROUND_TOKEN)
    {
        string[length - 1] = '\0';
        return 1;
    }

    return 0;
}

/**
 * Read input from stdin.
 *
 * Reads characters one at a time from stdin, into a
 * dynamically expanding buffer.
 *
 * Initial buffer size is determined by SHELL_INPUT_BUFFER_SIZE.
 * Buffer growth rate is determined by SHELL_INPUT_BUFFER_GROWTH.
 *
 * Returns a null-terminated string containing the user's input.
 */
char* shell_read_input()
{
    size_t current_buffer_size = SHELL_INPUT_BUFFER_SIZE;
    char* input_buffer = calloc(
        current_buffer_size,
        sizeof(char) * current_buffer_size
    );

    // Ensure allocation did not fail...
    shell_not_null(input_buffer, SHELL_ERROR_BUFFER_ALLOC);

    size_t buffer_pos = 0;
    while(1)
    {
        int character = getchar();

        // if we see EOF or \n, we are done.
        if (character == EOF || character == '\n')
        {
            input_buffer[buffer_pos] = '\0';
            break;
        }
        else
        {
            input_buffer[buffer_pos] = (char)character;
        }

        buffer_pos++;

        // See if we need to expand the buffer.
        if (buffer_pos >= current_buffer_size)
        {
            current_buffer_size *= SHELL_INPUT_BUFFER_GROWTH;
            input_buffer = realloc(input_buffer, current_buffer_size);

            shell_not_null(input_buffer, SHELL_ERROR_BUFFER_ALLOC);
        }
    }

    return input_buffer;
}

/**
 * Tokenizes the `string` given. 
 * 
 * Returns a NULL-terminated array of tokens.
 */
char** shell_tokenize_input(char* string)
{    
    size_t current_buffer_size = SHELL_INPUT_BUFFER_SIZE;
    char** token_buffer = calloc(
        current_buffer_size,
        sizeof(char) * current_buffer_size
    );

    // ensure allocation did not fail...
    shell_not_null(token_buffer, SHELL_ERROR_TOKEN_ALLOC);

    // get first token.
    char* token = strtok(string, SHELL_INPUT_TOKEN_CHARS);

    size_t token_buffer_pos = 0;

    while (token != NULL)
    {
        token_buffer[token_buffer_pos] = token;
        token_buffer_pos++;

        // see if we need to grow the buffer.
        if (token_buffer_pos >= current_buffer_size)
        {
            current_buffer_size *= SHELL_INPUT_BUFFER_GROWTH;
            token_buffer = realloc(token_buffer, current_buffer_size);

            shell_not_null(token_buffer, SHELL_ERROR_BUFFER_ALLOC);
        }

        // get next token
        token = strtok(NULL, SHELL_INPUT_TOKEN_CHARS);
    }

    // ensure token buffer is NULL terminated.
    token_buffer[token_buffer_pos] = NULL;

    return token_buffer;
}

/**
 * Executes the given command.
 *
 * Always returns 0, unless the exit command is given.
 */
int shell_execute(char** arguments, int background)
{
    pid_t pid;
    int status;

    if (strcmp(arguments[0], "exit") == 0)
    {
        return 1;
    }
    else if (strcmp(arguments[0], "quit") == 0)
    {
        return 1;
    }

    pid = fork();
    if (pid < 0)
    {
        perror(SHELL_NAME);
    }
    else if (pid == 0)
    {
        int execvp_return = execvp(arguments[0], arguments);
        if (execvp_return == -1)
        {
            perror(SHELL_NAME);
        }
    }
    else
    {
        if (background)
        {
            return 0;
        }

        do
        {
            waitpid(pid, &status, WUNTRACED);
        }
        while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 0;
}

/**
 * Execute a read-evaluate-print loop.
 * 
 * Reads user input, tokenizes it, and attempts
 * to run it as a command.
 */
void shell_repl(void)
{
    int should_exit = 0;

    while (should_exit != 1)
    {
        printf("%s", SHELL_PROMPT);

        char* input = shell_read_input();
        
        // User only entered a newline.
        // If we don't do this, we get
        // a segfault.
        if (input[0] == '\0')
        {
            continue;
        }

        // This needs to come before the call to shell_tokenize_input
        // since shell_tokenize_input modifies the input.
        int background_execute = shell_is_background(input);        

        char** execute_args = shell_tokenize_input(input);

        should_exit = shell_execute(execute_args, background_execute);

        // Free allocated memory. 
        free(input);
        free(execute_args);
    }

    puts("");
}

