//Juan Montoya
//cwfunc.h                           

#define _GNU_SOURCE        // enable strfry()              // required for using strfry() extension
#include <stdio.h>      // standard I/O for printf, fgets, fopen   // standard input/output header
#include <string.h>     // strlen, strcpy, strcmp-like operations on C strings   // string utilities
#include <ctype.h>      // isalpha, toupper for character validation/conversion  // character checks
#include <stdlib.h>     // general utilities (e.g., exit codes), fopen uses stdio // general utilities

#define letters 16 //including null character   // store up to 15 letters + '\0' // max chars per word + null
#define amount 20                               // maximum number of words       // up to 20 words total
#define SIZE 15                                 // board is 15x15                // crossword grid size
#define BUFFER_SIZE 256                         // input buffer for fgets lines  // size of input buffer
#define SOLUTION '.'                            // filler for solution board cells // char for empty solution slot
#define PUZZLE '#'                              // filler for puzzle board cells   // char for puzzle blocked cells

// Struct containing mostly unused word info (useful in the future)    // word properties for crossword entries
typedef struct 
{
    char word[letters]; // holds the uppercased word; length <= 15 + '\0'  // actual word in uppercase
    int length;         // cached length so strlen() isn’t repeatedly called // word length for convenience
    int row;              // future: column index for placement (unused yet)  // row index where word starts
    int col;               // future: row index for placement (unused yet)    // column index where word starts
    char orientation; // future: single-char flag 'H' or 'V' (not a C-string) // 'h' for horizontal, 'v' for vertical
    char placement;  // future: single-char status flag (not a C-string)      // 'y' if placed, other if not
} properties;

void initSolution(char board[][SIZE]);              // fills 15x15 board with SOLUTION ('.')  // initialize solution grid
void initPuzzle(char board[][SIZE]);                // fills 15x15 board with PUZZLE ('#')    // initialize puzzle grid
void displayBoard(char board[][SIZE]); // prints board with box frame                       // draw board with borders
void makePuzzle(char solution[][SIZE], char puzzle[][SIZE]);                               // derive puzzle from solution
void bubbleSort(properties wordarray[], int count);      // sorts words longest→shortest    // sort words by length
void uppercase(properties word[]);                  // declared but not implemented/used   // unused prototype
int validInput(properties wordarray[], char buffer[], int count); // reads/validates words from stdin // interactive input
int isValid(int x, int y);                          // checks if coordinates are inside board // bounds checking
int validPlacement(properties wordarray[], int count, char board[][SIZE], int n, int start_row, int start_col, char orientation); // check placement rules
void placeFirstWord(properties wordarray[], char board[][SIZE]); // placeholder for centering first word // put initial word
void makeClues(properties wordarray[], int count);  // generate and print anagram clues     // output clues table
void placeWordsAndReportFailures(properties wordarray[], int count, char solutionboard[][SIZE]); // attempt placement and show failures
char placeNextword(properties wordarray[], int n, int count, char board[][SIZE]); // place one word relative to others
int validInputbatch(properties wordarray[], char buffer[], int count, FILE *fp); // reads/validates words from file // batch input
void upper(char buffer[]);                          // uppercases a char buffer in place    // convert string to uppercase
void cleanup(char *word);                           // strips trailing newline from fgets   // trim newline at end
