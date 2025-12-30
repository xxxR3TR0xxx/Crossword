//Juan Montoya
//cwfunc.c

#include "cwfunc.h"   // include header containing definitions and prototypes

int isValid(int x, int y)   // checks whether coordinates are on the 15x15 board
{
    if (x < 0 || x >= SIZE || y < 0 || y >= SIZE)  // boundary check for row/col limits
    {
        return 0;   // return false when outside bounds
    }
    else
    {
        return 1;   // return true when inside bounds
    }
}

int validPlacement(properties wordarray[], int count, char board[][SIZE], int n, int start_row, int start_col, char orientation)
{
    int len = wordarray[n].length;   // store length of placing word
    int intersections = 0;           // counter for intersection occurrences

    if (orientation == 'v')          // vertical placement case
    {
        for (int t = 0; t < len; t++)        // iterate each character position of the word
        {
            int r = start_row + t;           // compute board row index
            int c = start_col;               // column stays constant for vertical words

            if (!isValid(c, r)) return 0;    // invalid cell → reject placement

            char existing = board[r][c];     // character currently on board cell

            if (existing == SOLUTION)        // SOLUTION '.' means empty
            {
                // For an empty cell, check spacing on left/right
                if (isValid(c - 1, r) && board[r][c - 1] != SOLUTION) return 0;
                if (isValid(c + 1, r) && board[r][c + 1] != SOLUTION) return 0;
            }
            else
            {
                // Cell has a letter → must match placing word
                if (existing != wordarray[n].word[t]) return 0;
                intersections++;              // count intersection
                // Do not check left/right because intersection is legal
            }
        }

        if (isValid(start_col, start_row - 1) && board[start_row - 1][start_col] != SOLUTION)
            return 0;     // check top cap

        if (isValid(start_col, start_row + len) && board[start_row + len][start_col] != SOLUTION)
            return 0;     // check bottom cap
    }
    else    // orientation == 'h'    // horizontal placement rules
    {
        for (int t = 0; t < len; t++)        // iterate each character
        {
            int r = start_row;               // row constant for horizontal
            int c = start_col + t;           // column moves

            if (!isValid(c, r)) return 0;    // invalid spot if off grid

            char existing = board[r][c];     // get board cell content

            if (existing == SOLUTION)        // empty cell
            {
                // check above/below for spacing
                if (isValid(c, r - 1) && board[r - 1][c] != SOLUTION) return 0;
                if (isValid(c, r + 1) && board[r + 1][c] != SOLUTION) return 0;
            }

            else
            {
                // must be a matching intersection letter
                if (existing != wordarray[n].word[t]) return 0;
                intersections++;             // count intersection
                // do not check above/below at an intersection
            }
        }

        if (isValid(start_col - 1, start_row) && board[start_row][start_col - 1] != SOLUTION)
            return 0;      // check left cap

        if (isValid(start_col + len, start_row) && board[start_row][start_col + len] != SOLUTION)
            return 0;      // check right cap
    }

    return intersections == 1;   // must have exactly one valid intersection
}

void makePuzzle(char solution[][SIZE], char puzzle[][SIZE])
{
    for (int r = 0; r < SIZE; r++)        // loop through each row
    {
        for (int c = 0; c < SIZE; c++)    // loop through each column
        {
            if (solution[r][c] == SOLUTION)     // empty cell in solution board
            {
                puzzle[r][c] = PUZZLE;          // becomes '#' in puzzle
            }
            else                                // letter present in solution
            {
                puzzle[r][c] = ' ';             // becomes blank space in puzzle
            }
        }
    }
}

char placeNextword(properties wordarray[], int n, int count, char board[][SIZE])
{
    for (int i = 0; i < count; i++)              // iterate through placed words
    {
        if (wordarray[i].placement != 'y')       // skip non-placed words
            continue;

        // attempt vertical placement if the placed word is horizontal
        if (wordarray[i].orientation == 'h')
        {
            for (int j = 0; j < wordarray[i].length; j++) // scan letters of placed word
            {
                for (int k = 0; k < wordarray[n].length; k++) // scan letters of placing word
                {
                    if (wordarray[i].word[j] == wordarray[n].word[k]) // found matching letter
                    {
                        int start_col = wordarray[i].col + j;     // compute intersection column
                        int start_row = wordarray[i].row - k;     // compute intersection row

                        if (!validPlacement(wordarray, count, board, n, start_row, start_col, 'v'))
                            continue;    // skip if placement invalid

                        wordarray[n].placement   = 'y';     // mark word as placed
                        wordarray[n].orientation = 'v';     // orientation vertical
                        wordarray[n].row = start_row;       // store row
                        wordarray[n].col = start_col;       // store col

                        for (int l = 0; l < wordarray[n].length; l++)
                            board[start_row + l][start_col] = wordarray[n].word[l];   // place letters

                        return 'y';    // successfully placed
                    }
                }
            }
        }

        // attempt horizontal placement if the placed word is vertical
        if (wordarray[i].orientation == 'v')
        {
            for (int j = 0; j < wordarray[i].length; j++)  // scan placed word
            {
                for (int k = 0; k < wordarray[n].length; k++) // scan placing word
                {
                    if (wordarray[i].word[j] == wordarray[n].word[k]) // matching letter found
                    {
                        int start_row = wordarray[i].row + j; // compute row intersection
                        int start_col = wordarray[i].col - k; // compute col intersection

                        if (!validPlacement(wordarray, count, board, n, start_row, start_col, 'h'))
                            continue; // placement invalid

                        wordarray[n].placement   = 'y';   // mark word as placed
                        wordarray[n].orientation = 'h';   // set orientation
                        wordarray[n].row = start_row;     // update row
                        wordarray[n].col = start_col;     // update col

                        for (int l = 0; l < wordarray[n].length; l++)
                            board[start_row][start_col + l] = wordarray[n].word[l];  // place horizontally

                        return 'y';    // success
                    }
                }
            }
        }
    }

    return 'n';  // no placement possible for this word
}

void placeWordsAndReportFailures(properties wordarray[], int count, char solutionboard[][SIZE])
{
    int remainingIdx[amount];       // store indices of words still not placed
    int remainingCount = 0;         // number of words still to try

    // Initially, all words except the first (already placed) are "remaining"
    for (int i = 1; i < count; i++)
    {
        remainingIdx[remainingCount] = i;   // store word index
        remainingCount++;
    }

    int placedSomething = 1;  // flag to track if any word was placed in a pass

    // Keep making passes as long as we place at least one word per pass
    while (remainingCount > 0 && placedSomething)
    {
        placedSomething = 0;           // reset flag for this pass

        int newRemainingIdx[amount];   // words still not placed after this pass
        int newRemainingCount = 0;     // count of those words

        // Try to place each remaining word
        for (int r = 0; r < remainingCount; r++)
        {
            int idx = remainingIdx[r];   // index of current word in wordarray
            char placed = placeNextword(wordarray, idx, count, solutionboard);

            if (placed == 'n')
            {
                // Still not placeable → keep it in the remaining list
                newRemainingIdx[newRemainingCount] = idx;
                newRemainingCount++;
            }
            else
            {
                // Successfully placed in this pass
                placedSomething = 1;
            }
        }

        // Update remaining list for the next pass
        for (int i = 0; i < newRemainingCount; i++)
        {
            remainingIdx[i] = newRemainingIdx[i];
        }
        remainingCount = newRemainingCount;
    }

    // After all passes, any indices left in remainingIdx[] truly could not be placed
    if (remainingCount > 0)
    {
        printf("\nWords that could NOT be placed:\n");
        for (int i = 0; i < remainingCount; i++)
        {
            int idx = remainingIdx[i];          // original index in wordarray
            printf(" - %s\n", wordarray[idx].word);
        }
        printf("\n");
    }
    else
    {
        printf("\nAll words placed successfully.\n\n");
    }
}


int validInput(properties wordarray[], char buffer[], int count)
{
    while(1)   // loop to read words from stdin
    {
        fgets(buffer, BUFFER_SIZE, stdin); // read line from input
        if(count >= 19) break;            // stop at max 20 words
        
        if(buffer[0] == '.')              // terminate input when '.' is typed
        {
            break;
        }

        cleanup(buffer);                  // remove newline at end of string

        int b = 0;                        // flag for invalidity

        if(strlen(buffer) < 2 || strlen(buffer) > 15) // enforce 2–15 length limit
        {
            printf("This word is invalid, too long/short %s\n", buffer);
            b += 1;                       // mark invalid
            continue;                     // skip further checks
        }

        for(int i = 0; i < strlen(buffer); i++) // check every character
        {
            int a = 0;                   // limit invalid message to once
            if(isalpha(buffer[i]) == 0)  // reject non-letter characters
            {
                a += 1;
                if(a == 1)
                {
                    printf("This word is invalid, not alpha %s\n", buffer);
                    b += 1;              // mark invalid
                    break;               // stop scanning this word
                }
            }
        }

        if(b > 0)                        // invalid → skip adding
        {
            continue;
        }
        
        if(b == 0)                       // valid → store
        {
            upper(buffer);                           // convert to uppercase
            strcpy(wordarray[count].word, buffer);   // store word
            wordarray[count].length = strlen(buffer);// store length
            count++;                                  // increment count
        }
    }

    return count;   // return number of valid words
}

int validInputbatch(properties wordarray[], char buffer[], int count, FILE *fp)
{
    while(1)   // loop to read words from file
    {
        fgets(buffer, BUFFER_SIZE, fp); // read line
        if(count >= 19) break;         // enforce word limit
        
        if(buffer[0] == '.')           // "." ends input
        {
            break;
        }

        cleanup(buffer);               // remove newline

        int b = 0;                     // invalid flag

        if(strlen(buffer) < 2 || strlen(buffer) > 15) // length constraints
        {
            b += 1;
            continue;
        }

        for(int i = 0; i < strlen(buffer); i++) // validate letters only
        {
            int a = 0;
            if(isalpha(buffer[i]) == 0) // reject non-letters
            {
                a += 1;
                if(a == 1)
                {
                    b += 1;           // mark invalid
                    break;            // stop validation
                }
            }
        }

        if(b > 0)
        {
            continue;                 // skip invalid
        }
        
        if(b == 0)
        {
            upper(buffer);                            // convert to uppercase
            strcpy(wordarray[count].word, buffer);    // store word
            wordarray[count].length = strlen(buffer); // store length
            count++;                                  // increment
        }
    }

    return count;   // number of words read
}

void upper(char buffer[])
{
    for(int i = 0; i < strlen(buffer); i++) // iterate all chars
    {
        buffer[i] = toupper(buffer[i]);     // convert to uppercase
    }
}

void cleanup(char *word)
{
    if(word[strlen(word) - 1] == '\n') // if final char is newline
    {
        word[strlen(word) - 1] = '\0'; // replace newline with null terminator
    }
}

void initSolution(char board[][SIZE])
{
    for (int i = 0; i < SIZE; i++)     // rows 0–14
    {
        for (int j = 0; j < SIZE; j++) // cols 0–14
        {
            board[i][j] = SOLUTION;    // fill with '.'
        }
    }
}

void placeFirstWord(properties wordarray[], char board[][SIZE])
{
    int starting_index =  7 - (wordarray[0].length/2); // center word horizontally

    for(int i = 0; i < wordarray[0].length; i++) // write characters onto board
    {
        board[7][starting_index + i] = wordarray[0].word[i]; // place letter
    }

    wordarray[0].row = 7;               // store row
    wordarray[0].col = starting_index;  // store column
    
    wordarray[0].orientation = 'h';     // first word always horizontal
    wordarray[0].placement = 'y';       // mark as placed
}

void initPuzzle(char board[][SIZE])
{
    for (int i = 0; i < SIZE; i++)       // loop through rows
    {
        for (int j = 0; j < SIZE; j++)   // loop through cols
        {
            board[i][j] = PUZZLE;        // fill puzzle with '#'
        }
    }
}

void displayBoard(char board[][SIZE])
{
    printf("+");
    for (int j = 0; j < SIZE; j++) // print top border
    {
        printf("-");
    }
    printf("+\n"); // newline after border

    for (int i = 0; i < SIZE; i++)   // each row of the board
    {
        printf("|");                 // left wall
        for (int j = 0; j < SIZE; j++)
        {
            printf("%c", board[i][j]); // print each character cell
        }
        printf("|\n");               // right wall
    }

    printf("+");
    for (int j = 0; j < SIZE; j++)   // bottom border
    {
        printf("-");
    }
    printf("+\n");
}

void bubbleSort(properties wordarray[], int count)
{
    char tempval[letters];     // temp buffer for swapping words
    int templength;            // temp variable for swapping lengths

    for(int i = 0; i < count - 1; i++)     // outer bubble loop
    {
        for(int j = 0; j < count - 1; j++) // inner bubble loop
        {
            if(strlen(wordarray[j].word) < strlen(wordarray[j + 1].word)) // longer first
            {
                strcpy(tempval, wordarray[j].word);        // swap strings
                templength = wordarray[j].length;          // store length temporarily
                strcpy(wordarray[j].word, wordarray[j + 1].word);
                wordarray[j].length = wordarray[j + 1].length;
                strcpy(wordarray[j + 1].word, tempval);    // complete swap
                wordarray[j + 1].length = templength;
            }
            else
            {
                continue;   // skip when order is correct
            }
        }
    }
}

void makeClues(properties wordarray[], int count)
{
    printf("CLUES:\n");                // heading
    printf("Location | Direction | Anagram\n"); // clue format header

    for (int i = 0; i < count; i++)    // loop through words
    {
        if (wordarray[i].placement != 'y')  // skip unplaced words
            continue;

        char anagram[letters];             // buffer for scrambled word
        strcpy(anagram, wordarray[i].word); // copy word into anagram buffer
        strfry(anagram);                    // shuffle letters randomly

        char direction[10];                // string to hold direction label
        if (wordarray[i].orientation == 'h')   // check orientation
        {
            strcpy(direction, "Across");       // horizontal = across
        }
        else
        {
            strcpy(direction, "Down");         // vertical = down
        }

        printf("%2d, %-4d | %-9s | %s\n",   // print clue line (position, direction, anagram)
            wordarray[i].col,
            wordarray[i].row,
            direction,
            anagram);
    }
}
