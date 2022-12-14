#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"

#include "special.h"
#include "type.h"
#include "expr.h"
#include "stmt.h"
#include "param_list.h"
#include "decl.h"
#include "token.h"

extern int yyparse();
extern FILE *yyin;
extern int yydebug;
extern int yywrap();
extern struct decl *parser_result;

int main(int argc, char *argv[])
{
	yydebug = 0;
	int i = 1;
	char flag[BUFSIZ];
	char t_str[BUFSIZ];
	int print_flag = 0;

	if (i == argc)
	{
		fprintf(stderr, "No specified file or flags \n\n");
		return usage(3);
	}

	while (i < argc)
	{
		if (argv[i][0] == '-')
		{
			strcpy(flag, argv[i]);
			if (strcmp(flag, "-h") == 0)
			{
				return usage(0);
			}
			else if (strcmp(flag, "-scan") == 0)
			{
				yyin = fopen(argv[i + 1], "r");
				if (!yyin)
					return usage(4);
			}
			else if (strcmp(flag, "-parse") == 0)
			{
				yyin = fopen(argv[i + 1], "r");
				if (!yyin)
					return usage(4);
			}
			else if (strcmp(flag, "-i") == 0)
			{
				yyin = stdin;
			}
			else if (strcmp(flag, "-debug") == 0)
			{
				yydebug = 1;
			}
			else if (strcmp(flag, "-print") == 0)
			{
				print_flag = 1;
			}
			else
			{
				return usage(1);
			}
		}
		else
		{
			break;
		}
		i++;
	}
	printf("//	CSE 40243 B-Minor Parser\n");
	printf("//	Enter a valid  B-Minor Code \n\n");

	if (yywrap() == 1)
	{
		printf("//	Scan Successful\n");
	}
	else
	{
		printf("Scan Unsuccessful\n");
		return 1;
	}
	if (yyparse() == 0)
	{
		printf("//	Parse Successful\n");
	}
	else
	{
		printf("Parse Unsuccessful\n");
		return 1;
	}

	if (print_flag == 1)
	{
		// we want to print the output
		decl_print(parser_result, 0);
	}

	return 0;
}

int usage(int exit_code)
{

	fprintf(stderr, "bminor scanner.... sample use is \n\t \
					./bminor -s FILENAME\n\tflags to use:\
					\n\t\t-h help\
					\n\t\t-scan to scan file in the next argument\
					\n\t\t-parse to parse file in the next argument\
					\n\t\t-i for interpreter\
					\n\t\t-print to print reformated code\
					\n\t\t-debug for debugging output\n");

	return exit_code;
}
