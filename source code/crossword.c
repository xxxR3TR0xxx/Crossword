//Juan Montoya
//crossword.c

#include "cwfunc.h"   // include header containing all prototypes, structs, and macros

int main(int argc, char *argv[])   // main function with argc handling 3 program modes
{
    properties wordarray[amount]; // array to store up to 20 words
    char buffer[BUFFER_SIZE];     // buffer used for reading user input or file input
    char solutionboard[SIZE][SIZE]; // 15×15 solution board (stores letters and '.')
    char puzzleboard[SIZE][SIZE];   // 15×15 puzzle board (stores '#' and blanks)

    if(argc == 1) // MODE 1: interactive mode (read words from stdin)
    {
        printf("Anagram Crossword Puzzle Generator\n");  // program header
        printf("----------------------------------\n\n");

        printf("Words?\n"); // ask user for input words
        int count = 0;      // number of successfully validated words so far

        count = validInput(wordarray, buffer, count); // read words until "." or full

        printf("\n"); // spacing before printing boards

        initSolution(solutionboard); // fill solution board with '.'
        initPuzzle(puzzleboard);     // fill puzzle board with '#'

        bubbleSort(wordarray, count); // sort words longest → shortest
        placeFirstWord(wordarray, solutionboard); // place longest word in center

        placeWordsAndReportFailures(wordarray, count, solutionboard); // place remaining words

        // convert solution board into puzzle board with '#' and blanks
        makePuzzle(solutionboard, puzzleboard);

        printf("SOLUTION:\n");      // label for solution printout
        displayBoard(solutionboard); // show final solution board

        printf("\nPUZZLE:\n");       // label for puzzle printout
        displayBoard(puzzleboard);   // show puzzle board

        printf("\n");
        makeClues(wordarray, count); // output list of clues with anagrams
    }
    
    else if (argc == 2)  // MODE 2: input from file, output to terminal
    {
        FILE *fp = fopen(argv[1], "r"); // open input file
        if (fp == NULL)                 // check if file failed to open
        {
            printf("Input file not found: %s\n", argv[1]); // error message
            return 1;                                      // exit with error
        }

        int count = 0;                       // reset counter
        count = validInputbatch(wordarray, buffer, count, fp); // read words from file
        fclose(fp);                          // close file

        printf("\n");

        initSolution(solutionboard); // initialize solution board
        initPuzzle(puzzleboard);     // initialize puzzle board

        bubbleSort(wordarray, count); // sort words by length
        placeFirstWord(wordarray, solutionboard); // place first word

        placeWordsAndReportFailures(wordarray, count, solutionboard); // attempt placement

        makePuzzle(solutionboard, puzzleboard); // build puzzle from solution

        printf("SOLUTION:\n");
        displayBoard(solutionboard); // show solution

        printf("\nPUZZLE:\n");
        displayBoard(puzzleboard);   // show puzzle

        printf("\n");
        makeClues(wordarray, count); // print clues
    }

    else if (argc == 3)   // MODE 3: input file + output file
    {
        FILE *fp = fopen(argv[1], "r");     // open input file
        if (fp == NULL)                     // check if failed
        {
            fprintf(stderr, "Could not open input file: %s\n", argv[1]); // error message
            return 1;                                                    // exit with error
        }

        // Redirect STDOUT to the output file so all printf() writes there
        FILE *out = freopen(argv[2], "w", stdout);
        if (out == NULL)                     // check if failed to open output file
        {
            fprintf(stderr, "Could not open output file: %s\n", argv[2]); // error message
            fclose(fp);                                                    // close input file
            return 1;                                                      // exit
        }

        // now all printed output goes into output file automatically

        int count = validInputbatch(wordarray, buffer, 0, fp); // read input from file
        fclose(fp);                                            // done reading

        initSolution(solutionboard); // initialize solution board
        initPuzzle(puzzleboard);     // initialize puzzle board

        bubbleSort(wordarray, count); // sort words longest → shortest
        placeFirstWord(wordarray, solutionboard); // place longest word

        placeWordsAndReportFailures(wordarray, count, solutionboard); // place rest

        makePuzzle(solutionboard, puzzleboard); // generate puzzle

        printf("SOLUTION:\n");
        displayBoard(solutionboard);            // output solution

        printf("\nPUZZLE:\n");
        displayBoard(puzzleboard);              // output puzzle

        printf("\n");
        makeClues(wordarray, count);            // output clues list

        // program ends here; stdout remains redirected
    }

    else
    {
        // too many arguments → error message
        fprintf(stderr, "More than three arguments are not allowed, please try again.\n");
    }
}
