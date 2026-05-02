/* SLIM - This is the Interceptor for slim.
 * 1- Spwans aother program that caputures stdout to stderr
 * 2- Filters the output using regex to keep error lines only
 * 3- Prints the clean results to stdout
 * Author : Marawan Yakout
 * Date Created : 2nd May 2026
 */

#define _POSIX_C_SOURCE 200809L /* popen and pclose */
#include <stdio.h> /* basics - printf , fprintf, fgets, popen(needed) popen, pclose, FILE */
#include <stdlib.h> /* exit , malloc , free */
#include <string.h> /* strlen strnat , strncpy */
#include <regex.h> /* regex compatibility - regcomp, regexec, regfree - POSIX regex*/

#ifdef _WIN32
#error "Slim developers = We dont use windows"
#endif 

//==========DEFINITIONS====================
#define MAX_CMD_LEN 4096  // max CMD line @args
#define MAX_LINE_LEN 1024 // max lines allowed as output
#define MAX_OUTPUT 65536  // max captured output

#define ERROR_PATTERN "(error|ERROR|Error|fatal|FAILED|undefined reference)"
#define WARNING_PATTERN "(warning|WARNING|Warning)"

//==========FUNCTIONS====================
/* 
 * Transforms "gcc main.c" to " gcc main.c 2>&1"
 * @param cmd_out is a buffer for arguments to be stored
 */
static void build_command(int argc, char *argv[], char *cmd_out)
{
	cmd_out[0] = '\0'; //placing an empty string in cmd out
	//takes in args in cmd_out buffer
	for(int i = 1; i < argc; ++i)
	{
		strncat(cmd_out, argv[i], MAX_CMD_LEN - strlen(cmd_out) -1); 	
		if (i < argc -1) // basically not after the last word 
		{
			strncat(cmd_out, " ", MAX_CMD_LEN - strlen(cmd_out) -1);
		}
	}
	// this takes stderr(fd2) into stdout(fd1) so popen can capture errors
	strncat(cmd_out, " 2>&1", MAX_CMD_LEN - strlen(cmd_out) -1); 

	// DEBUGGING ONLY +++++++++++++++++++++++++++++++++++++++++
	// %d prints an integer (argc)
	// %s prints a string (argv[1])
	// %d prints the integer value of the char cmd_out[1] (which is 0 for '\0')
	// printf("testing: argc = %d | argv[0] = %s |\n cmd_out = %s\n", argc, argv[0], cmd_out);
}
/* 
* compares the pattern with the line given by from gcc
*/ 
static int is_match(regex_t *pattern, const char *line)
{
	return regexec(pattern, line, 0, NULL, 0) == 0;
}

//==========MAIN====================
int main(int argc, char *argv[])
{
	if(argc < 2)
	{
		fprintf(stderr, "Usage: Slim <command> [args ...]\n");
		return 1;
	}

	char command[MAX_CMD_LEN]; // an array holding commands
	build_command(argc,argv,command);
	fprintf(stderr, "[slim] Running: %s\n", command); // primative but lets keep it for now

	regex_t error_regex, warning_regex; //regex pattern holders

	//REG_EXTENDED && REG_ICASE flags for finding patterns
	regcomp(&error_regex, ERROR_PATTERN, REG_EXTENDED | REG_ICASE);
	regcomp(&warning_regex, WARNING_PATTERN, REG_EXTENDED | REG_ICASE);

	//pipe to the cmd comand with a file pointer
	FILE *pipe_fp = popen(command, "r"); // reading output of the command
	if (!pipe_fp)
	{
		fprintf(stderr, "[slim] popen() function failed\n");
		return 1;
	}

	// Reading output line by line
	char line[MAX_LINE_LEN];
	char error_block[MAX_OUTPUT];
	error_block[0] = '\0';
	int found = 0;

	while (fgets(line,sizeof(line), pipe_fp) != NULL) 
	{
		if(is_match(&error_regex, line) || is_match(&warning_regex, line))
		{
			strncat(error_block, line, MAX_OUTPUT - strlen(error_block) -1);
			++found;
			// other lines are sucessfully compiled and automatically dopped
		}
	}
	int exit_status = pclose(pipe_fp);
	regfree(&error_regex);
	regfree(&warning_regex);

	if(found == 0)
	{
		printf("[slim] No errors detected. Successful Build\n");
		return 0;
	}

	// if exit_status failed return 1 else return 0;
	printf("%s",error_block); //<-- python takes this via stdin
	return (exit_status != 0) ? 1 : 0;
}
