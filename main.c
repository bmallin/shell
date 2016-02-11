/** main.c
 *
 * Author: Benjamin Mallin
 * 
 * This is the shell entry point.
 * the implementation resides in shell.c.
 */

#include "shell.h"

int main(int argc, char** argv)
{
    shell_repl();

    return 0;
}
