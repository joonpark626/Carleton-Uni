/*
About this program:
- This program counts words.
- The specific words that will be counted are passed in as command-line
  arguments.
- The program reads words (one word per line) from standard input until EOF or
  an input line starting with a dot '.'
- The program prints out a summary of the number of times each word has
  appeared.
- Various command-line options alter the behavior of the program.

E.g., count the number of times 'cat', 'nap' or 'dog' appears.
> ./main cat nap dog
Given input:
 cat
 .
Expected output:
 Looking for 3 words
 Result:
 cat:1
 nap:0
 dog:0

Note: this code was automatically formatted (styled) using 'indent main.c
-kr'.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "assignment1_test.h"

/* Structures */
typedef struct {
    char *word;
    int counter;
} WordCountEntry;


int process_stream(WordCountEntry entries[], int entry_count)
{
    short line_count = 0;
    char buffer[30];
    char *word;

    /* C4: replace gets with fgets */
    while (fgets(buffer, sizeof(buffer), stdin) != NULL) {
	if (*buffer == '.')
	    break;

	/* C5: strtok() can be used to split a line into individual tokens.
	   For the separator characters we use whitespace (space and
	   tab), as well as the newline character '\n'.  We could also
	   trim the buffer to get rid of the newline, instead. */
	word = strtok(buffer, " \t\n");
	/* C5: strtok returns NULL when no more tokens are available. */
	while (word != NULL) {

	    /* Compare against each entry */
	    int i = 0;
	    while (i < entry_count) {

		if (!strcmp(entries[i].word, word))
		    entries[i].counter++;
		i++;
	    }
	    /* C5: get the next token */
	    word = strtok(NULL, " \t\n");
	}
	line_count++;
    }
    return line_count;
}


void print_result(WordCountEntry entries[], int entry_count, FILE * output)
{
    /* B5: introduce a temporary variable i and use it to count up from 0 */
    unsigned int i;
    /* C2: send output to the right stream */
    fprintf(output, "Result:\n");
    for (i = 0; i < entry_count; i++) {
	/* B5: index the entries array with i */
	fprintf(output, "%s:%d\n", entries[i].word, entries[i].counter);
    }
}


void printHelp(const char *name)
{
    fprintf(stderr, "usage: %s [-h] [-fFILENAME] <word1> ... <wordN>\n",
	    name);
}


int main(int argc, char **argv)
{
    const char *prog_name = *argv;

    /* C3: make entries a pointer instead of an array */
    WordCountEntry *entries;
    int entryCount = 0;

    /* C2: use this variable to store the output stream to use, stdout by
       default */
    FILE *output = stdout;

    /* Entry point for the testrunner program */
    if (argc > 1 && !strcmp(argv[1], "-test")) {
	run_assignment1_test(argc - 1, argv + 1);
	return EXIT_SUCCESS;
    }

    /* C3: we'll allocate (potentially) a little more memory than strictly
       necessary, thus avoiding extensive modifications to the code below. */
    if ((entries = malloc(sizeof(WordCountEntry) * (argc - 1))) == NULL) {
	/* Always a good idea to check the return values of library and system
	   calls! */
	fprintf(stderr, "malloc failed, quitting\n");
	return EXIT_FAILURE;
    }

    /* B4: increment argv once to skip argv[0] */
    argv++;

    while (*argv != NULL) {
	if (**argv == '-') {

	    switch ((*argv)[1]) {
		/* C2: -fFILENAME switch: open FILENAME and set it as the output
		   stream */
	    case 'f':
		if ((output = fopen(*argv + 2, "w")) == NULL) {
		    /* Always a good idea to check the return values of library
		       and system calls! */
		    fprintf(stderr, "Could not open file \"%s\".\n",
			    *argv + 2);
    		    /* C3: don't forget to free up the memory we allocated */
    		    free(entries);
		    return EXIT_FAILURE;
		}
		break;
	    case 'h':
		printHelp(prog_name);

		/* B3: add a break statement to fix the logical flow error */
		break;

	    default:
		fprintf(stderr, "%s: Invalid option %s. Use -h for help.\n",
			prog_name, *argv);
	    }
	} else {
	    /* C3: the LENGTH macro will not work anymore, since entries is now
	       a pointer, not an array */
	    if (entryCount < argc - 1) {
		entries[entryCount].word = *argv;
		entries[entryCount++].counter = 0;
	    }
	}
	argv++;
    }
    if (entryCount == 0) {
	fprintf(stderr,
		"%s: Please supply at least one word. Use -h for help.\n",
		prog_name);
    	/* clean up */
    	free(entries);
	if (output != stdout) {
	    fclose(output);
	}
	return EXIT_FAILURE;
    }
    /* C2: send output to the right stream */
    if (entryCount == 1) {
	fprintf(output, "Looking for a single word\n");
    } else {
	fprintf(output, "Looking for %d words\n", entryCount);
    }

    process_stream(entries, entryCount);
    print_result(entries, entryCount, output);

    /* clean up */
    free(entries);
    if (output != stdout) {
	fclose(output);
    }

    return EXIT_SUCCESS;
}
