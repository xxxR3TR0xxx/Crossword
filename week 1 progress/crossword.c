//Juan Montoya
//crossword.c

#include <stdio.h>      // standard I/O for printf, fgets, fopen
#include <string.h>     // strlen, strcpy, strcmp-like operations on C strings
#include <unistd.h>     // (currently unused) POSIX utilities; safe to keep as include
#include <ctype.h>      // isalpha, toupper for character validation/conversion
#include <stdlib.h>     // general utilities (e.g., exit codes), fopen uses stdio

#define letters 16 //including null character   // store up to 15 letters + '\0'
#define amount 20                               // maximum number of words 
#define SIZE 15                                 // board is 15x15
#define BUFFER_SIZE 256                         // input buffer for fgets lines
#define SOLUTION '.'                            // filler for solution board cells
#define PUZZLE '#'                              // filler for puzzle board cells

// Struct containing mostly unused word info (useful in the future)
typedef struct 
{
    char word[letters]; // holds the uppercased word; length <= 15 + '\0'
    int length;         // cached length so strlen() isn’t repeatedly called
    int row;              // future: column index for placement (unused yet)
    int col;               // future: row index for placement (unused yet)
    char orientation; // future: single-char flag 'H' or 'V' (not a C-string)
    char placement;  // future: single-char status flag (not a C-string)
} properties;

void initSolution(char board[][SIZE]);              // fills 15x15 board with SOLUTION ('.')
void initPuzzle(char board[][SIZE]);                // fills 15x15 board with PUZZLE ('#')
void displayBoard(char board[][SIZE]); // prints board with box frame
void bubbleSort(properties wordarray[], int count);      // sorts words longest→shortest
void uppercase(properties word[]);                  // declared but not implemented/used
int validInput(properties wordarray[], char buffer[], int count); // reads/validates words from stdin
int isValid(int x, int y);
int validPlacementHorizontal(properties wordarray[], int count, char board[][SIZE], int n, int k, int start_row, int start_col);
int validPlacementVertical(properties wordarray[], int count, char board[][SIZE], int n, int k, int start_row, int start_col);

int validPlacement(properties wordarray[], int count, char board[][SIZE], int n, int k, int start_row, int start_col, char orientation);

void placeFirstWord(properties wordarray[], char board[][SIZE]); // placeholder for centering first word

char placeNextword(properties wordarray[], int n, int count, char board[][SIZE]);

int validInputbatch(properties wordarray[], char buffer[], int count, FILE *fp); // reads/validates words from file



void upper(char buffer[]);                          // uppercases a char buffer in place
void cleanup(char *word);                           // strips trailing newline from fgets


int main(int argc, char *argv[])
{
    properties wordarray[amount]; // storage for up to 20 valid words
    char buffer[BUFFER_SIZE];     // reusable line buffer for input
    char solutionboard[SIZE][SIZE]; // 15x15 solution board ('.')
    char puzzleboard[SIZE][SIZE];   // 15x15 puzzle board ('#')

    if(argc == 1) // interactive mode (spec usage #1): read words from stdin
    {
        printf("Anagram Crossword Puzzle Generator\n");
        printf("----------------------------------\n\n");

        printf("Words?\n"); // prompt before collecting input
        int count = 0;      // number of valid words stored so far

        count = validInput(wordarray, buffer, count); // loop until '.' or capacity

        printf("\n"); // visual spacing before boards

        initSolution(solutionboard); // fill solutionboard with '.'
        initPuzzle(puzzleboard);     // fill puzzleboard with '#'

        bubbleSort(wordarray, count); // longest -> shortest; prints words
        placeFirstWord(wordarray, solutionboard);

        char placed = 'n';
        for(int i = 1; i < count; i++)
        {
            placed = placeNextword(wordarray, i, count, solutionboard);
            if(placed == 'n')
            {
                printf("This word, %s, can't be placed\n", wordarray[i].word);
                break;

            }
        }
        
        displayBoard(solutionboard); // currently shows only fillers (no placement yet)

        printf("\n\n");                         // spacing between the two boards
        displayBoard(puzzleboard);   // puzzle view; currently all '#'

        //bubbleSort(wordarray, count); // original commented line retained
    }

    else if(argc == 2) // batch mode (spec usage #2): read words from file path
    {
        FILE *fp = NULL;
        char infile[64]; // local filename buffer (not strictly needed here)

        fp = fopen(argv[1], "r"); // open the provided input file in read mode

        if (fp == NULL)  // If file can't be opened, print error and exit
        {
            printf("File not found, due to incorrect reference\n");
            return 1;    // nonzero return code for error
        }

        int count = 0;                                 // word counter for batch
        count = validInputbatch(wordarray, buffer, count, fp); // read from file

        initSolution(solutionboard); // prepare solution board
        initPuzzle(puzzleboard);     // prepare puzzle board

        bubbleSort(wordarray, count); // sort and print words

        placeFirstWord(wordarray, solutionboard);


        //place the other words AND THEN execute the functions below....

        displayBoard(solutionboard); // show solution board (all '.')
        printf("\n\n");                         // blank lines between boards
        displayBoard(puzzleboard);   // show puzzle board (all '#')

    }
    // else: argc > 2 is not explicitly handled; program ends quietly per current design
}

int isValid(int x, int y)
{
    if (x < 0 || x >= SIZE || y < 0 || y >= SIZE)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

int validPlacementHorizontal(properties wordarray[], int count, char board[][SIZE], int n, int k, int start_row, int start_col)
{
    if(isValid(start_col, start_row - 1) == 0)
    {
        if(board[start_row - 1][start_col] != '.')
            return 0;
    }
    
        for(int r = start_row; r < start_row + wordarray[n].length; r++)
        {
            for(int c = start_col - 1; c  <= start_col + 1; c++)
            {
                if(!isValid(c, r)) continue;

                if(r == start_row + k && c == start_col) continue; 
                
                if(board[r][c] != '.') return 0;
            }
        }
        
    if(isValid(start_col, wordarray[n].length + start_row) == 0)
    {
        if(board[wordarray[n].length + start_row][start_col] != '.')
            return 0;
    }

}

int validPlacementVertical(properties wordarray[], int count, char board[][SIZE], int n, int k, int start_row, int start_col)
{
    int length     = wordarray[n].length;
    int left_col   = start_col - 1;
    int right_col  = start_col + length;

    // LEFT END CAP (col just before first letter)
    if (isValid(left_col, start_row))
    {
        if (board[start_row][left_col] != '.')
            return 0;
    }

    // BODY OF HORIZONTAL WORD
    for (int c = start_col; c < start_col + length; c++)
    {
        for (int r = start_row - 1; r <= start_row + 1; r++)
        {
            if (!isValid(c, r))
                continue;

            // skip the ONE intersection cell (k-th letter)
            if (r == start_row && c == start_col + k)
                continue;

            if (board[r][c] != '.')
                return 0;
        }
    }

    // RIGHT END CAP (col just after last letter)
    if (isValid(right_col, start_row))
    {
        if (board[start_row][right_col] != '.')
            return 0;
    }

    return 1;
}

int validPlacement(properties wordarray[], int count, char board[][SIZE], int n, int k, int start_row, int start_col, char orientation)
{
    if (orientation == 'v')
        return validPlacementVertical(wordarray, count, board, n, k, start_row, start_col);
    else
        return validPlacementHorizontal(wordarray, count, board, n, k, start_row, start_col);
}

char placeNextword(properties wordarray[], int n, int count, char board[][SIZE])
//The idea of nextWord[] 
{
//If placed valid horizontal, is placed valid vertical
//is placevalid vertical, if it returns true then you place it with another function

    for(int i = 0; i < count; i++)
    {
        if(wordarray[i].placement == 'y')
        {
            if(wordarray[i].orientation == 'h') //This function checks if its horizontal
            {
                for(int j = 0; j < wordarray[i].length; j++) //Iterates through the length of PLACED word
                {
                    for(int k = 0; k < wordarray[n].length; k++) //Iterates through the length of PLACING word
                    {
                        
                        if(wordarray[i].word[j] == wordarray[n].word[k]) 
                        {

                            int start_col = wordarray[i].col + j;
                            int start_row = wordarray[i].row - k;
                            
                            if(!validPlacement(wordarray, count, board, n, k, start_row, start_col, 'v'))
                                continue;

                        
                            wordarray[n].placement = 'y';
                            wordarray[n].orientation = 'v';
                            wordarray[n].row = start_row;
                            wordarray[n].col = start_col;
                            for(int l = 0; l < wordarray[n].length; l++)
                            {
                                board[start_row + l][start_col] = wordarray[n].word[l];
                            }
                            return 'y';
                        }
                    }
                }
            }

            if(wordarray[i].orientation == 'v') //This function checks if its horizontal
            {
                for(int j = 0; j < wordarray[i].length; j++) //Iterates through the length of PLACED word
                {
                    for(int k = 0; k < wordarray[n].length; k++) //Iterates through the length of PLACING word
                    {
                        if(wordarray[i].word[j] == wordarray[n].word[k]) 
                        {
                            int start_col = wordarray[i].col + k; 
                            int start_row = wordarray[i].row + j;
                            
                            if(!validPlacement(wordarray, count, board, n, k, start_row, start_col, 'h'))
                                continue;
                     

                            wordarray[n].placement = 'y';
                            wordarray[n].orientation = 'h';
                            wordarray[n].row = start_row;
                            wordarray[n].col = start_col;
                            for(int l = 0; l < wordarray[n].length; l++)
                            {
                                board[start_row][start_col + l] = wordarray[n].word[l];
                            }
                            return 'y';
                        }
                    }

                }

            }
        }
    }

    return 'n';
}

int validInput(properties wordarray[], char buffer[], int count)
{
    // Input loop: read one line at a time from stdin until '.' or capacity
    while(1)
    {
        fgets(buffer, BUFFER_SIZE, stdin); // safe read into buffer (keeps '\n' if room)
        if(count >= 19) break;             // ensure at most 20 words (0..19 valid indices)
        
        if(buffer[0] == '.')               // a line starting with '.' ends the input phase
        {
            break;
        }

        cleanup(buffer);                   // strip trailing newline so strlen is exact

        int b = 0;                         // validation flag (0 = ok; >0 means reject)
        if(strlen(buffer) < 2 || strlen(buffer) > 15) // enforces proper word length 
        {
            printf("This word is invalid, too long/short %s\n", buffer);
            b += 1;                        // mark invalid
            continue;                      // move to next input line
        }

        for(int i = 0; i < strlen(buffer); i++) // check every character of candidate
        {
            int a = 0;                     // local flag to print invalid message once
            if(isalpha(buffer[i]) == 0)    // non-letter detected
            {
                a += 1;
                if(a == 1)                 // only print once per word
                {
                    printf("This word is invalid, not alpha %s\n", buffer);
                    b += 1;                // mark whole word invalid
                    break;                 // stop scanning this word
                }
            }
        }

        if(b > 0)                          // if invalid, skip storing
        {
            continue;
        }
        
        if(b == 0)                         // valid path: normalize case and store
        {
            upper(buffer);                                 // in-place uppercase
            strcpy(wordarray[count].word, buffer);         // copy into struct slot
            wordarray[count].length = strlen(buffer);      // cache the length
            count++;                                       // increment stored count
        }
    }

    return count; // total number of valid words collected
}

int validInputbatch(properties wordarray[], char buffer[], int count, FILE *fp)
{
    // Same as validInput(), but reads from 'fp' and suppresses printed errors.
    while(1)
    {
        fgets(buffer, BUFFER_SIZE, fp); // read a line from the file into buffer
        if(count >= 19) break;          // respect capacity (max 20 words total)
        
        if(buffer[0] == '.')            // period line terminates reading
        {
            break;
        }

        cleanup(buffer);                // remove trailing newline to sanitize input

        int b = 0;                      // validation flag
        if(strlen(buffer) < 2 || strlen(buffer) > 15) // enforce 2..15 letters
        {
            //printf("This word is invalid, too long/short %s\n", buffer);
            b += 1;                     // mark invalid (message suppressed here)
            continue;                   // go read next line
        }

        for(int i = 0; i < strlen(buffer); i++) // scan characters for non-letters
        {
            int a = 0;                  // local one-time message flag (suppressed)
            if(isalpha(buffer[i]) == 0) // if any non-letter, this word is invalid
            {
                a += 1;
                if(a == 1) 
                {
                    //printf("This word is invalid, not alpha %s\n", buffer);
                    b += 1;             // mark invalid
                    break;              // stop checking this word
                }
            }
        }

        if(b > 0)                       // skip storing invalid word
        {
            continue;
        }
        
        if(b == 0)                      // valid: uppercase and store
        {
            upper(buffer);                                 // convert to uppercase
            strcpy(wordarray[count].word, buffer);         // copy into struct
            wordarray[count].length = strlen(buffer);      // cache length
            count++;                                       // bump counter
        }
    }

    return count; // number of valid words read from file
}

void upper(char buffer[])
{
    // Uppercase every character in the buffer (ASCII letters only affected)
    for(int i = 0; i < strlen(buffer); i++) // strlen each loop is fine at this scale
    {
        buffer[i] = toupper(buffer[i]);     // convert in-place
    }
}

void cleanup(char *word)
{
    // If the last character is '\n', delete it by overwriting with '\0'
    if(word[strlen(word) - 1] == '\n') 
    {
        word[strlen(word) - 1] = '\0'; // standard newline-strip after fgets
    }
}

void initSolution(char board[][SIZE])
{
    // Double loop over rows (i) and columns (j) to fill the grid
    for (int i = 0; i < SIZE; i++)     // iterate 1..15 rows
    {
        for (int j = 0; j < SIZE; j++) // iterate 1..15 cols
        {
            board[i][j] = SOLUTION;  // Initialize every cell as '.'

        }
    }
}


void placeFirstWord(properties wordarray[], char board[][SIZE])
{
    int starting_index =  7 - (wordarray[0].length/2);

    //printf("\n%d\n", wordarray[0].length);

    for(int i = 0; i < wordarray[0].length; i++)
    {
        board[7][starting_index + i] = wordarray[0].word[i];
    }
    //I am placing at the ith index where the chars are the same

    wordarray[0].row = 7;
    wordarray[0].col = starting_index; 
    
    wordarray[0].orientation = 'h';
    wordarray[0].placement = 'y';
}



void initPuzzle(char board[][SIZE])
{
    // Same pattern as initSolution but with PUZZLE filler
    for (int i = 0; i < SIZE; i++)     // rows
    {
        for (int j = 0; j < SIZE; j++) // cols
        {
            board[i][j] = PUZZLE;  // Initialize every cell as '#'
        }
    }
}

void displayBoard(char board[][SIZE])
{
    // Top border: '+' + SIZE hyphens + '+'
    printf("+");
    for (int j = 0; j < SIZE; j++) // print '-' SIZE times
    {
        printf("-");
    }
    printf("+\n"); // end the top frame line

    // Each row of the board between vertical bars
    for (int i = 0; i < SIZE; i++) 
    {
        printf("|"); // Left wall for this row
        for (int j = 0; j < SIZE; j++)
        {
            printf("%c", board[i][j]); // Print current cell character
        }
        printf("|\n"); // Right wall and newline to end the row
    }

    // Bottom border mirrors the top
    printf("+");
    for (int j = 0; j < SIZE; j++)
    {
        printf("-");
    }
    printf("+\n"); // end the bottom frame line
}

void bubbleSort(properties wordarray[], int count)
{
    char tempval[letters]; // temporary buffer used to swap string contents
    int templength;

    
    for(int i = 0; i < count - 1; i++)          // outer passes (n-1 passes)
    {
        for(int j = 0; j < count - 1; j++)      // compare adjacent pairs [j] and [j+1]
        {
            if(strlen(wordarray[j].word) < strlen(wordarray[j + 1].word)) // want longer first
            {
                // Swap strings to move longer words toward the front.
                strcpy(tempval, wordarray[j].word);           // save left into temp
                templength = wordarray[j].length;
                strcpy(wordarray[j].word, wordarray[j + 1].word);  // move right into left
                wordarray[j].length = wordarray[j + 1].length;
                strcpy(wordarray[j + 1].word, tempval);       // move temp into right
                wordarray[j + 1].length = templength;

            }
            
            else
            {

                continue; // if everything is in order, do nothing 
            }
        }
        //printf("Pass %d\n", i); 
    }

        /* After sorting, print words in order
        for(int i = 0; i < count; i++)
        {
            printf("%s\n", wordarray[i].word); // prints longest -> shortest by length
        }
        */

}
