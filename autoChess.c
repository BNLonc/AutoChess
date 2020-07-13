#include <stdlib.h>
#include <stdio.h> 

//include the header file
#include "autoChess.h"

//create a new baord in memory (constructor)
board_td* newBoard() {
    board_td* newBrd = malloc(sizeof(board_td));

    newBrd->layout = malloc(sizeof(piece_td*) * 32);

    for (int i = 0; i < 32; i++) {
        newBrd->layout[i] = malloc(sizeof(piece_td));

        newBrd->layout[i]->pos = malloc(sizeof(square_td));

        newBrd->layout[i]->moves = malloc(sizeof(square_td*) * 30);

        for (int j = 0; j < 30; j++) {
            newBrd->layout[i]->moves[j] = malloc(sizeof(square_td));
        }
    }

    newBrd->numPieces = 0;

    return newBrd;
}

//destructor for boards
void freeBoard(board_td* brd) {

    for (int i = 0; i < 32; i++) {
        if (brd->layout[i] != NULL) {
            freePiece(brd->layout[i]);
        }
    }

    free(brd->layout);

    free(brd);
}

//destructor for pieces 
void freePiece(piece_td* piece) {
    free(piece->pos);

        for (int j = 0; j < 30; j++) {
            free(piece->moves[j]);
        }

        free(piece->moves);

        free(piece);
}

//copy the square struct orig and return a pointer to a deep copy 
square_td* copySq(square_td* orig) {
    square_td* output = malloc(sizeof(square_td));
    output->row = orig->row;
    output->col = orig->col;
    return output;
}

//check if two square structs are the same 
bool equalSq(square_td* a, square_td* b) {
    if (a->row == b->row && a->col == b->col) {
        return true;
    } else {
        return false;
    }
}

//check if square sq on board brd is occupied, returning a pointer to the occupying piece if so
piece_td* occupant(board_td* brd, square_td* sq) {
    //hold a pointer to the current piece 
    piece_td* currPiece;

    //traverse all pieces
    for (int i = 0; i < 32; i++) {  
        //assign the pointer to the current piece 
        currPiece = brd->layout[i];
        
        //if the current piece exists,
        if (currPiece != NULL) { 
            //if the square the piece is on matches the one we're checking 
            if (equalSq(sq, currPiece->pos)) { 
                //return the pointer to the current piece
                return currPiece; 
            }
        }
    }

    //if we've run through all the pieces and didn't hit a match, return NULL
    return NULL;
}

//mian function 
int main() {
    //create and set up an initial test board
    /*board_td* brd = newBoard();
    setInit(brd);
    //print the test board 
    printBoard(brd);

    printf("\n");

    
    //write out the test board
    writeBoard(brd, "brdOut.txt");*/
    //read in the test file 
    board_td* readIn = readBoard("brdTst.txt");
    
    simulate(readIn, 'w', SIM_DEPTH);
    
    bool cm = inCheckMate(readIn, 'b');

    if (cm) {
        printf("true");
    } else {
        printf("false");
    }

    //board_td* copy = copyBoard(readIn);

    //writeBoard(copy, "copy.txt");

    //write out the test file
    //printBoard(readIn);

    //test inCheck
    //inCheck(readIn, 'b');

    //free both boards 
    freeBoard(readIn);
    //freeBoard(copy);

    return 0;
}

//get the score for board_td struct for a given colour 
int getScore(board_td* brd, char colour) {

    //variable to hold the score to output
    int scoreOut = 0; 

    char opposingColour = 'b';

    if (colour == 'b') {
        opposingColour = 'w';
    }

    if (inCheckMate(brd, colour)) {
        return 6942069;
    } else if (inCheckMate(brd, opposingColour)) {
        return -6942069;
    }
    
    //check about being in check
    if (inCheck(brd, colour)) {
        scoreOut -= QUEE_VAL * 2;
    }
    if (inCheck(brd, opposingColour)) {
        scoreOut += QUEE_VAL * 2;
    }

    piece_td* currPiece = NULL; 
    int enemyOffset = 0;

    for (int i = 0; i < brd->numPieces; i++) {
        currPiece = brd->layout[i];

        enemyOffset = ((currPiece->colour == opposingColour)? -1 : 1);

        switch (currPiece->type) {
            case 'p':
                scoreOut += enemyOffset * PAWN_VAL;
                break;
            case 'r':
                scoreOut += enemyOffset * ROOK_VAL;
                break;
            case 'k':
                scoreOut += enemyOffset * KNIG_VAL;
                break;
            case 'b':
                scoreOut += enemyOffset * BISH_VAL;
                break;
            case 'q':
                scoreOut += enemyOffset * QUEE_VAL;
                break;
            case 'g':
                break;
            default: 
                printf("u dun goofed piece type flags\n");
        }
    }

    return scoreOut;
}

//make a deep copy of a board 
board_td* copyBoard(board_td* orig) {
    
    //allocate a new board
    board_td* newBrd = newBoard();

    //copy the numPieces over 
    newBrd->numPieces = orig->numPieces;

    //copy the pieces in the layout array 
    int i = 0;
    while (i < newBrd->numPieces) {
        piece_td* newPiece = newBrd->layout[i];
        piece_td* origPiece = orig->layout[i];

        newPiece->pos = copySq(origPiece->pos);
        newPiece->type = origPiece->type;
        newPiece->colour = origPiece->colour;
        
        for (int j = 0; j < 30; j++) {
            newPiece->moves[j]->row = origPiece->moves[j]->row;
            newPiece->moves[j]->col = origPiece->moves[j]->col;
        }

        i++;
    }

    while (i < 32) {
        if (newBrd->layout[i] != NULL) {
            freePiece(newBrd->layout[i]);
            newBrd->layout[i] = NULL;
        }
        i++;
    }

    return newBrd; 
}

//soft-copy a board 
void softCopyBoard(board_td* recip, board_td* orig) {
    recip->numPieces = orig->numPieces;

    int i = 0;

    while (i < recip->numPieces) {
        piece_td* recipPiece = recip->layout[i];
        piece_td* origPiece = orig->layout[i];

        recipPiece->pos->row = origPiece->pos->row;
        recipPiece->pos->col = origPiece->pos->col;
        recipPiece->type = origPiece->type;
        recipPiece->colour = origPiece->colour;

        for (int j = 0; j < 30; j++) {
            recipPiece->moves[j]->row = origPiece->moves[j]->row;
            recipPiece->moves[j]->col = origPiece->moves[j]->col;
        }

        i++;
    }

    while (i < 32) {
        recip->layout[i] = NULL;
        i++;
    }
}

//check whether the given colour is in check 
bool inCheck(board_td* brd, char colour) {
    
    piece_td** pieceArr = brd->layout; 

    //find the king of the colour we want 
    int kingInd = 0;
    while (pieceArr[kingInd]->colour != colour || pieceArr[kingInd]->type != 'g') {
        kingInd++;
    }

    //go through every piece on the board 
    for (int i = 0; i < brd->numPieces; i++) {
        
        //ignore pieces of the same colour as the friendly king
        if (pieceArr[i]->colour == colour) {
            continue;
        } else {
            
            //go through this (enemy) piece's every legal move
            int moveInd = 0;
            while (pieceArr[i]->moves[moveInd]->row != 0) {
                
                //if one of its legal moves is the square on which the king sits, return true
                if (pieceArr[i]->moves[moveInd]->row == pieceArr[kingInd]->pos->row && pieceArr[i]->moves[moveInd]->col == pieceArr[kingInd]->pos->col) {
                    return true;
                }

                moveInd++;
            }

        }
    }
    
    return false;
}

//check whether the given colour is in checkmate 
bool inCheckMate(board_td* brd, char colour) {
    /*NOTE: I'm assuming that the legal moves attached to each piece on brd are 
    good and reliable as they come in*/

    //make sure we're actually in check first lmao
    if (inCheck(brd, colour)) {
        
        //create a hypothetical board to work on 
        board_td* hypo = copyBoard(brd);
        
        //for every friendly piece on the board
        for (int i = 0; i < hypo->numPieces; i++) {
            
            //set up a shortcut pointer to the current piece 
            piece_td* currPiece = hypo->layout[i];

            //continue; if the current piece is unfriendly 
            if (currPiece->colour != colour) {
                continue;
            }

            //for every move that this piece can do...
            int moveInd = 0;
            while (currPiece->moves[moveInd]->row != 0) {
                
                //do it and then see if we're no longer in check 
                movePiece(hypo, i, moveInd);

                //if we did a move and are no longer in check, free the hypo board and return false 
                if (!inCheck(hypo, colour)) {
                    freeBoard(hypo);
                    return false;
                }

                //if we get here, we didn't get out of check by making that move, so let's reset the hypo board 
                softCopyBoard(hypo, brd);
                generateMoves(hypo);

                moveInd++;
            }

        }

        freeBoard(hypo);
        return true;

    } else {
        return false;
    }
}

//move a piece on board brd 
void movePiece(board_td* brd, int pieceInd, int moveInd) {
    
    //point to the piece we're going to land on when we move 
    square_td* landPos = brd->layout[pieceInd]->moves[moveInd];

    //go through the pieces 
    int capInd = 0;
    while (capInd < brd->numPieces) {
        
        //break if we've found a piece we're going to land on 
        square_td* targetPos = brd->layout[capInd]->pos;
        if (equalSq(targetPos, landPos)) {
            break;
        }
        
        capInd++;
    }

    //if we're going to land on a piece, destroy it
    if (capInd < brd->numPieces) {
        freePiece(brd->layout[capInd]);

        brd->layout[capInd] = brd->layout[brd->numPieces - 1];
        brd->layout[brd->numPieces - 1] = NULL;
        brd->numPieces--;
    }

    brd->layout[pieceInd]->pos->row = landPos->row;
    brd->layout[pieceInd]->pos->col = landPos->col;  

    generateMoves(brd);
}

//gernerate all legal moves for all pieces on brd, placing them in the "moves" field of the piece_td structs
void generateMoves(board_td* brd) {
    
    //traverse every piece on the board 
    for (int i = 0; i < 32; i++) { 

        //store a pointer to the current piece to reduce memory accesses
        piece_td* currPiece = brd->layout[i]; 

        //check that the piece whose moves we're about to generate exists
        if (currPiece != NULL) { 
            
            //clear out the list of legals 
            for (int j = 0; j < 30; j++) {
                currPiece->moves[j]->row = 0;
                currPiece->moves[j]->col = 0; 
            } 

            //the number of moves we've added to the list of legal moves on the current piece 
            int movesAdded = 0; 

            //set up a temp variable to hold the square we're thinking of advancing into 
            square_td* advancePos = copySq(currPiece->pos);

            //generate moves by piece type
            switch (currPiece->type) { 
                //PAWNS
                case 'p': ;
                    //hold the row of a given pawn that hasn't moved 
                    int stationaryRow = 0; 
                    
                    //value is 1 or -1, to multiply on vertical movements of black pawns for code reuse
                    int blackOffset = 1; 

                    if (currPiece->colour == 'w') {
                        //white pawns on row 2 haven't moved 
                        stationaryRow = 2; 
                    } else if (currPiece->colour == 'b') {
                        //black pawns on row 7 haven't moved 
                        stationaryRow = 7;
                        //this piece is black, so its movement is in the opposite direction as white ones  
                        blackOffset = -1; 
                    } else {
                        printf("u dun goofed pawn colours");
                    }


                    //HANDLE DOUBLE ADVANCES FOR FIRST MOVES 

                    //add the +/- 2 rows to the move we just added ^
                    advancePos->row += 2 * blackOffset; 

                    //if this pawn hasn't moved and the square two squares up isn't occupied 
                    if (currPiece->pos->row == stationaryRow && occupant(brd, advancePos) == NULL) { 
                        //add the current position advanced by two squares to legals
                        currPiece->moves[movesAdded] = copySq(advancePos);
                        //increment the legals 
                        movesAdded++; 
                    }

                    //HANDLE SINGLE ADVANCES
                    //bring the double advance back a square to handle regular pawn advances
                    advancePos->row -= 1 * blackOffset;

                    //if the square one square up isn't occupied 
                    if (!occupant(brd, advancePos)) { 
                        //add the current position advanced by two squares to legals
                        currPiece->moves[movesAdded] = copySq(advancePos); 
                        //increment the legals 
                        movesAdded++; 
                    }

                    //HANDLE CAPUTRES 
                    //move the temp target square left one 
                    advancePos->col -= 1;

                    //do the capture check twice, reusing the code in a kinda cheeky and ugly way
                    for (int loopCount = 0; loopCount < 2; loopCount++) {
                        //hold a pointer to the target piece for the capture  
                        piece_td* target = occupant(brd, advancePos);
                        
                        //just move on if there's nothing to think about capturing and the square up one and over one is occupied by an opponent piece
                        if (target != NULL && target->colour != currPiece->colour) {
                            currPiece->moves[movesAdded] = copySq(target->pos);
                            movesAdded++;
                        } 

                        //shift the up one, left one to be up one, right one 
                        advancePos->col += 2;
                    }

                    //HANDLE EN PASSANT 
                    //TODO
                    

                    break;

                //handle queens (which really just falls through the cases for rooks and bishops)
                case 'q': ;
                //handle rooks 
                case 'r': ;

                    //do the plus and minus directions
                    for (int i = -1; i <= 1; i+=2) {

                        //increment advancePos to start checks off of the current position
                        advancePos->row += i;
                        //check whether the position is occupied
                        while (occupant(brd, advancePos) == NULL && advancePos->row >= 1 && advancePos->row <= 8) {
                            
                            //add the current position to the list of legal moves
                            currPiece->moves[movesAdded] = copySq(advancePos);
                            movesAdded++;

                            //increment the position we're looking at 
                            advancePos->row += i;
                        }

                        piece_td* finalPos = occupant(brd, advancePos);

                        //if the piece we stopped at is a piece of the opposing colour 
                        if (finalPos != NULL && (finalPos->colour != currPiece->colour)) {
                            //add the current position to the list of legal moves
                            currPiece->moves[movesAdded] = copySq(advancePos);
                            movesAdded++;

                            //increment the position we're looking at 
                            advancePos->row += i;
                        }

                        advancePos->row = currPiece->pos->row;
                        advancePos->col = currPiece->pos->col; 
                    }

                    

                    //do the plus and minus directions
                    for (int i = -1; i <= 1; i+=2) {

                        //increment advancePos to start checks off of the current position
                        advancePos->col += i;
                        //check whether the position is occupied
                        while (occupant(brd, advancePos) == NULL && advancePos->col >= 'a' && advancePos->col <= 'h') {
                            
                            //add the current position to the list of legal moves
                            currPiece->moves[movesAdded] = copySq(advancePos);
                            movesAdded++;

                            //increment the position we're looking at 
                            advancePos->col += i;
                        }
                        
                        piece_td* finalPos = occupant(brd, advancePos);

                        //if the piece we stopped at is a piece of the opposing colour 
                        if (finalPos != NULL && (finalPos->colour != currPiece->colour)) {
                            //add the current position to the list of legal moves
                            currPiece->moves[movesAdded] = copySq(advancePos);
                            movesAdded++;

                            //increment the position we're looking at 
                            advancePos->col += i;
                        }

                        advancePos->row = currPiece->pos->row;
                        advancePos->col = currPiece->pos->col; 
                    }

                    //if we're dealing with a rook, this is the end of the line
                    if (currPiece->type == 'r') {
                        break;
                    }

                    //if we're not dealing with a rook, we're dealing with a queen
                    //reset the position of advancePos to set up for the case 'b' code we're about to fall into 
                    advancePos->row = currPiece->pos->row;
                    advancePos->col = currPiece->pos->col;
                    
                case 'b': ;
                    
                    //run the four diagonal directions
                    for (int i = -1; i <= 1; i += 2) {
                        for (int j = -1; j <= 1; j += 2) {
                            
                            advancePos->col += i;
                            advancePos->row += j;
                            
                            //run one diagonal 
                            while (occupant(brd, advancePos) == NULL && advancePos->col >= 'a' && advancePos->col <= 'h' && advancePos->row >= 1 && advancePos->row <= 8) {
                                
                                //add the current position to the list of legal moves
                                currPiece->moves[movesAdded] = copySq(advancePos);
                                movesAdded++;

                                //increment in the given direction 
                                advancePos->col += i;
                                advancePos->row += j;
                            }

                            piece_td* finalPos = occupant(brd, advancePos);

                            //if the piece we stopped at is a piece of the opposing colour 
                            if (finalPos != NULL && (finalPos->colour != currPiece->colour)) {
                                
                                //add the current position to the list of legal moves
                                currPiece->moves[movesAdded] = copySq(advancePos);
                                movesAdded++;

                                //increment the position we're looking at 
                                advancePos->col += i;
                                advancePos->row += j;
                            }

                            advancePos->row = currPiece->pos->row;
                            advancePos->col = currPiece->pos->col;
                            
                        }
                    }

                    break;
                case 'k': ;

                    //run two directions
                    for (int i = -1; i <= 1; i += 2) {
                        for (int j = -2; j <= 2; j += 4) {
                            
                            //move the piece 
                            advancePos->row = currPiece->pos->row + i;
                            advancePos->col = currPiece->pos->col + j;
                            
                            //if the square is occupied by an enemy or empty, add it to the list of legals 
                            if ((occupant(brd, advancePos) == NULL || occupant(brd, advancePos)->colour != currPiece->colour) && (advancePos->row <= 8 && advancePos->row >= 0 && advancePos->col <= 'h' && advancePos->col >= 'a')) {
                                //add the current position to the list of legal moves
                                currPiece->moves[movesAdded] = copySq(advancePos);
                                movesAdded++;
                            }
                            
                        }
                    }

                    //run two directions
                    for (int i = -2; i <= 2; i += 4) {
                        for (int j = -1; j <= 1; j += 2) {
                            
                            //move the piece 
                            advancePos->row = currPiece->pos->row + i;
                            advancePos->col = currPiece->pos->col + j;
                            
                            //if the square is occupied by an enemy or empty, add it to the list of legals 
                            if ((occupant(brd, advancePos) == NULL || occupant(brd, advancePos)->colour != currPiece->colour)  && (advancePos->row <= 8 && advancePos->row >= 0 && advancePos->col <= 'h' && advancePos->col >= 'a')) {
                                //add the current position to the list of legal moves
                                currPiece->moves[movesAdded] = copySq(advancePos);
                                movesAdded++;
                            }
                            
                        }
                    }
                    break;
                case 'g': ;
                    //go around the piece and examine each position 
                    for (int i = -1; i <= 1; i++) {
                        for (int j = -1; j <= 1; j++) {
                            
                            //skip the current position
                            if (i == 0 && j == 0) {
                                continue;
                            }
                            
                            advancePos->row = currPiece->pos->row + i;
                            advancePos->col = currPiece->pos->col + j;

                            //if advancePos is unoccupied or occupied by an enemy
                            if (occupant(brd, advancePos) == NULL || occupant(brd, advancePos)->colour != currPiece->colour) {
                                //add the current position to the list of legal moves
                                currPiece->moves[movesAdded] = copySq(advancePos);
                                movesAdded++;
                            }
                        }
                    }


                    break;
                default: ;
                    printf("you dun goofed piece selection\n");
                    break;
            }

            //free the temp variable from earlier 
            free(advancePos);
        }
    }
}

//run one layer of the simulation
move_td* simulate(board_td* brd, char colour, int depth) {
    //generate the legal moves for all the pieces on the board 
    generateMoves(brd);

    if (depth == 1) {
        //base case 

    } else {
        //recursive case 

    }

    
    return NULL;
}

//set up a default board
void setInit(board_td* brd) {
    brd->numPieces = 32;

    for (int offset = 0; offset <= 16; offset += 16) {
        piece_td* currPiece;
        
        //set up pawns 
        for (int i = 0; i < 8; i++) {
            currPiece = brd->layout[i + offset];
            currPiece->pos->col = i + 'a';
            currPiece->pos->row = (offset == 16)? 7 : 2;
            currPiece->colour = (offset == 16)? 'b' : 'w';
            currPiece->type = 'p';
        }

        //set up rooks 
        for (int i = 8; i <= 9; i++) {
            currPiece = brd->layout[i + offset];
            currPiece->pos->col = ((i == 8)? 1 : 8) + 'a' - 1;
            currPiece->pos->row = (offset == 16)? 8 : 1;
            currPiece->colour = (offset == 16)? 'b' : 'w';
            currPiece->type = 'r';
        }

        //set up knights
        for (int i = 10; i <= 11; i++) {
            currPiece = brd->layout[i + offset];
            currPiece->pos->col = ((i == 10)? 2 : 7) + 'a' - 1;
            currPiece->pos->row = (offset == 16)? 8 : 1;
            currPiece->colour = (offset == 16)? 'b' : 'w';
            currPiece->type = 'k';
        }
        
        //set up bishops
        for (int i = 12; i <= 13; i++) {
            currPiece = brd->layout[i + offset];
            currPiece->pos->col = ((i == 12)? 3 : 6) + 'a' - 1;
            currPiece->pos->row = (offset == 16)? 8 : 1;
            currPiece->colour = (offset == 16)? 'b' : 'w';
            currPiece->type = 'b';
        }

        //set up queens
        currPiece = brd->layout[14 + offset];
        currPiece->pos->col = 'd';
        currPiece->pos->row = currPiece->pos->row = (offset == 16)? 8 : 1;
        currPiece->colour = (offset == 16)? 'b' : 'w';
        currPiece->type = 'q';

        //set up kings
        currPiece = brd->layout[15 + offset];
        currPiece->pos->col = 'e';
        currPiece->pos->row = currPiece->pos->row = (offset == 16)? 8 : 1;
        currPiece->colour = (offset == 16)? 'b' : 'w';
        currPiece->type = 'g';
    }
}

//print out a board 
void printBoard(board_td* brd) {
    for (int r = 8; r >= 1; r--) { //for every row
        for (char c = 'a'; c <= 'h'; c += 1) { //for every column
            char output = '.';

            for (int i = 0; i < brd->numPieces; i++) { //for every piece
                if (brd->layout[i]->pos->row == r && brd->layout[i]->pos->col == c) { //check whether current coords match the current piece
                    output = brd->layout[i]->type - ((brd->layout[i]->colour == 'w')? 32 : 0); 
                    break;
                }
            }

            printf("%c", output);
        }

        printf("%c", '\n');
    }
}

void writeBoard(board_td* brd, char* name) {
    FILE* fp = fopen(name, "w");

    for (int r = 8; r >= 1; r--) { //for every row
        for (char c = 'a'; c <= 'h'; c += 1) { //for every column
            char output = '.';

            for (int i = 0; i < brd->numPieces; i++) { //for every piece
                if (brd->layout[i]->pos->row == r && brd->layout[i]->pos->col == c) { //check whether current coords match the current piece
                    output = brd->layout[i]->type - ((brd->layout[i]->colour == 'w')? 32 : 0); 
                    break;
                }
            }

            fprintf(fp, "%c", output);
        }

        fprintf(fp, "%c", '\n');
    }

    fclose(fp);
}

board_td* readBoard(char* name) {
    board_td* brd = newBoard();

    int pieceIndex = 0;

    FILE* fp = fopen(name, "r");

    char* input = malloc(sizeof(char) * 64);
    char* inputTraverse = input;

    fread(input, sizeof(char), 71, fp);

    for (int r = 8; r >= 1; r--) { //for every row
        for (char c = 'a'; c <= 'h' + 1; c += 1) { //for every column
            
            //if we're about to read too many pieces, just stop
            if (r == 1 && c == 'h' + 1) {
                continue;
            }

            //read in the piece 
            char pieceIn = *inputTraverse;
            if (pieceIn != '.' && pieceIn != '\n') {
                //skip the empty squares and line breaks
            
                //we've hit a piece 
                brd->numPieces++;

                //set a pointer to the spot for the current piece we're reading in 
                piece_td* currPiece = brd->layout[pieceIndex];

                //check for colour and standardise case 
                if (pieceIn >= 'a') {
                    currPiece->colour = 'b';
                } else {
                    currPiece->colour = 'w';

                    pieceIn += 32;
                }

                //set the type 
                currPiece->type = pieceIn;

                //set the position 
                currPiece->pos->row = r;
                currPiece->pos->col = c;

                //move the index to the next space in the board object  
                pieceIndex++;
            }

            //move the index to the next char in the string 
            inputTraverse++;
        }
    }

    //free the string we used to read the board in 
    free(input);
    //close the file we're reading in 
    fclose(fp);

    while (pieceIndex <= 31) {
        freePiece(brd->layout[pieceIndex]);
        brd->layout[pieceIndex] = NULL;
        pieceIndex++;
    }

    return brd;
}