#include <stdlib.h>
#include <stdio.h> 

//include the header file
#include "autoChess.h"

//create a new baord in memory (constructor)
board_td* newBoard() {
    board_td* newBrd = malloc(sizeof(board_td));

    newBrd->layout = malloc(sizeof(piece_td*) * 32);

    for (int i = 0; i < 32; i++) {
        newBrd->layout[i] = newPiece();
    }

    newBrd->numPieces = 0;

    newBrd->turn = 'w';

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

//constructor for pieces
piece_td* newPiece() {
    piece_td* newPc = malloc(sizeof(piece_td));

    newPc->pos = malloc(sizeof(square_td));

    newPc->moves = malloc(sizeof(square_td*) * NUM_MOVES);

    for (int j = 0; j < NUM_MOVES; j++) {
        newPc->moves[j] = malloc(sizeof(square_td));
    }

    return newPc;
}

//destructor for pieces 
void freePiece(piece_td* piece) {
    free(piece->pos);

    for (int j = 0; j < NUM_MOVES; j++) {
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

    //FIRST-ORDER FUNCTIONS 
    /*piece_td* testPiece = newPiece();
    freePiece(testPiece);

    board_td* testBrd = newBoard();
    setInit(testBrd);


    freePiece(testBrd->layout[28]);
    testBrd->layout[28] = testBrd->layout[31];
    testBrd->layout[31] = NULL;
    testBrd->numPieces--;


    printBoard(testBrd);
    writeBoard(testBrd, "boardOut.txt");

    freeBoard(testBrd);

    square_td* sq = malloc(sizeof(square_td));
    sq->row = 5;
    sq->col = 'e';
    square_td* otherSq = malloc(sizeof(square_td));
    otherSq->row = 6;
    otherSq->col = 'b';
    square_td* copyTest = copySq(sq);
    bool equal = equalSq(sq, otherSq);
    bool equal2 = equalSq(sq, copyTest);

    free(sq);
    free(otherSq);
    free(copyTest);*/
    
    //SECOND-ORDER FUNCTIONS
    
    board_td* readIn = readBoard("brdTst.txt");
    //setInit(readIn);

    generateMoves(readIn, true);

    dumpBoard(readIn);

    printBoard(readIn);

    printf("check: %c\n", readIn->check);

    if (inCheckMate(readIn)) {
        printf("checkmate\n");
    } else {
        printf("not checkmate\n");
    }

    printf("Score for %c is: %i\n", readIn->turn, getScore(readIn));

    freeBoard(readIn);

    return 0;
}

//get the score for board_td struct for a given colour 
int getScore(board_td* brd) {

    //variable to hold the score to output
    int scoreOut = 0; 

    //return a crazy low value if we're in checkmate     
    if (inCheckMate(brd)) {
        return -6942069;
    }
    
    //check about being in check
    if (brd->check == brd->turn) {
        scoreOut -= QUEE_VAL * 2;
    }

    piece_td* currPiece = NULL; 

    //traverse all the pieces 
    for (int i = 0; i < brd->numPieces; i++) {
        currPiece = brd->layout[i];

        //add the piece into the score if it's friendly 
        if (currPiece->colour == brd->turn) {
            switch (currPiece->type) {
                case 'p':
                    scoreOut += PAWN_VAL;
                    break;
                case 'r':
                    scoreOut += ROOK_VAL;
                    break;
                case 'n':
                    scoreOut += KNIG_VAL;
                    break;
                case 'b':
                    scoreOut += BISH_VAL;
                    break;
                case 'q':
                    scoreOut += QUEE_VAL;
                    break;
                case 'k':
                    break;
                default: 
                    printf("u dun goofed piece type flags\n");
            }

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

    //copy turn
    newBrd->turn = orig->turn;

    //copy the pieces in the layout array 
    int i = 0;
    while (i < newBrd->numPieces) {
        piece_td* newPiece = newBrd->layout[i];
        piece_td* origPiece = orig->layout[i];

        free(newPiece->pos);

        newPiece->pos = copySq(origPiece->pos);
        newPiece->type = origPiece->type;
        newPiece->colour = origPiece->colour;
        
        for (int j = 0; j < NUM_MOVES; j++) {
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
void softCopyBoard(board_td** recip, board_td* orig) {
    /*recip->numPieces = orig->numPieces;

    int i = 0;
    while (i < recip->numPieces) {
        
        piece_td* recipPiece = recip->layout[i];
        piece_td* origPiece = orig->layout[i];

        //copy into slots that were null in the recipient board 
        if (recipPiece == NULL) {
            //initialise the slot
            recip->layout[i] = newPiece();
            //update the shortcut 
            recipPiece = recip->layout[i];
        }

        recipPiece->pos->row = origPiece->pos->row;
        recipPiece->pos->col = origPiece->pos->col;
        recipPiece->type = origPiece->type;
        recipPiece->colour = origPiece->colour;

        for (int j = 0; j < NUM_MOVES; j++) {
            recipPiece->moves[j]->row = origPiece->moves[j]->row;
            recipPiece->moves[j]->col = origPiece->moves[j]->col;
        }

        i++;
    }

    while (i < 32) {
        recip->layout[i] = NULL;
        i++;
    }*/

    freeBoard(*recip);

    *recip = copyBoard(orig);
}

//check whether the given colour is in check 
void runCheck(board_td* brd) { 

    brd->check = 0;

    char colour = 'b';

    while (brd->check == 0) {
        
        //find the king of the current player  
        int kingInd = 0;
        while (brd->layout[kingInd]->colour != colour || brd->layout[kingInd]->type != 'k') {
            kingInd++;
        }

        if (kingInd == brd->numPieces) {
            printf("no friendly king found -- problem at inCheck()\n");
            brd->check = 'n';
            return;
        }

        //go through every piece on the board 
        for (int i = 0; i < brd->numPieces; i++) {
            
            //ignore pieces of the same colour as the friendly king
            if (brd->layout[i]->colour != colour) {

                //go through this (enemy) piece's every legal move
                int moveInd = 0;
                while (brd->layout[i]->moves[moveInd]->row != 0) {
                    
                    //if one of its legal moves is the square on which the king sits, return true 
                    if (equalSq(brd->layout[i]->moves[moveInd], brd->layout[kingInd]->pos)) {
                        
                        brd->check = colour;
                        return;
                    }

                    moveInd++;
                }

            }
        }

        //set up the colour to cycle through 'b', 'w', then 'n' 
        if (colour == 'b') {
            colour = 'w';
        } else {
            brd->check = 'n';
        }
    }
}

//check whether the given colour is in checkmate 
bool inCheckMate(board_td* brd) {
    /*NOTE: I'm assuming that the legal moves attached to each piece on brd are 
    good and reliable as they come in*/

    /*//get the enemy colour 
    char enemyColour = 'b';
    if (brd->turn == 'b') {
        enemyColour = 'w';
    }*/

    //make sure we're actually in check first lmao
    if (brd->check == brd->turn) {
        
        //create a hypothetical board to work on 
        board_td* hypo = copyBoard(brd);

        //for every friendly piece on the board
        for (int i = 0; i < hypo->numPieces; i++) {
            
            //set up a shortcut pointer to the current piece 
            piece_td* currPiece = hypo->layout[i];

            //continue; if the current piece is unfriendly 
            if (currPiece->colour != brd->turn) {
                continue;
            }

            //for every move that this piece can do...
            int moveInd = 0;
            while (currPiece->moves[moveInd]->row != 0) {

                //do it and then see if we're no longer in check 
                movePiece(hypo, i, moveInd);
                generateMoves(hypo, true);

                //if we did a move and are no longer in check, free the hypo board and return false 
                if (hypo->check != hypo->turn) {
                    freeBoard(hypo);
                    return false;
                }

                //if we get here, we didn't get out of check by making that move, so let's reset the hypo board 
                softCopyBoard(&hypo, brd); 
                //generateMoves(hypo);

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
    
        //don't capture the king 
        if (brd->layout[capInd]->type == 'k') {
            return;
        }
        
        freePiece(brd->layout[capInd]);

        brd->layout[capInd] = brd->layout[brd->numPieces - 1];
        brd->layout[brd->numPieces - 1] = NULL;
        brd->numPieces--;
    }

    piece_td* currPiece = brd->layout[pieceInd]; 

    currPiece->pos->row = landPos->row;
    currPiece->pos->col = landPos->col;  

    if (currPiece->type == 'p' && ((currPiece->pos->row == 8 && currPiece->colour == 'w') || (currPiece->pos->row == 1 && currPiece->colour == 'b'))) {
        currPiece->type = 'q';
    }

    if (brd->turn == 'w') {
        brd->turn = 'b';
    } else {
        brd->turn = 'w';
    }
    
}

//gernerate all legal moves for all pieces on brd, placing them in the "moves" field of the piece_td structs
void generateMoves(board_td* brd, bool doChk) {

    board_td* hypo = copyBoard(brd);

    //traverse every piece on the board 
    for (int i = 0; i < brd->numPieces; i++) { 

        //store a pointer to the current piece to reduce memory accesses 
        piece_td* currPiece = brd->layout[i]; 

        //zero out the list of legals 
        for (int j = 0; j < NUM_MOVES; j++) {
            currPiece->moves[j]->row = 0;
            currPiece->moves[j]->col = 0; 
        } 

        //make sure the piece we're about to generate on gets its turn next 
        if (/*currPiece->colour == brd->turn*/ true) {

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
                        printf("u dun goofed pawn colours\n");
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
                    if (occupant(brd, advancePos) == NULL) { 
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
                    for (int j = -1; j <= 1; j+=2) {

                        //increment advancePos to start checks off of the current position
                        advancePos->row += j;
                        //check whether the position is occupied
                        while (occupant(brd, advancePos) == NULL && advancePos->row >= 1 && advancePos->row <= 8) {
                            
                            //add the current position to the list of legal moves
                            currPiece->moves[movesAdded] = copySq(advancePos);
                            movesAdded++;

                            //increment the position we're looking at 
                            advancePos->row += j;
                        }

                        piece_td* finalPos = occupant(brd, advancePos);

                        //if the piece we stopped at is a piece of the opposing colour 
                        if (finalPos != NULL && (finalPos->colour != currPiece->colour)) {
                            //add the current position to the list of legal moves
                            currPiece->moves[movesAdded] = copySq(advancePos);
                            movesAdded++;

                            //increment the position we're looking at 
                            advancePos->row += j;
                        }

                        advancePos->row = currPiece->pos->row;
                        advancePos->col = currPiece->pos->col; 
                    }

                    

                    //do the plus and minus directions
                    for (int j = -1; j <= 1; j += 2) {

                        //increment advancePos to start checks off of the current position
                        advancePos->col += j;
                        //check whether the position is occupied
                        while (occupant(brd, advancePos) == NULL && advancePos->col >= 'a' && advancePos->col <= 'h') {
                            
                            //add the current position to the list of legal moves
                            currPiece->moves[movesAdded] = copySq(advancePos);
                            movesAdded++;

                            //increment the position we're looking at 
                            advancePos->col += j;
                        }
                        
                        piece_td* finalPos = occupant(brd, advancePos);

                        //if the piece we stopped at is a piece of the opposing colour 
                        if (finalPos != NULL && (finalPos->colour != currPiece->colour)) {
                            //add the current position to the list of legal moves
                            currPiece->moves[movesAdded] = copySq(advancePos);
                            movesAdded++;

                            //increment the position we're looking at 
                            advancePos->col += j;
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
                case 'n': ;

                    //run two directions
                    for (int i = -1; i <= 1; i += 2) {
                        for (int j = -2; j <= 2; j += 4) {
                            
                            //move the piece 
                            advancePos->row = currPiece->pos->row + i;
                            advancePos->col = currPiece->pos->col + j;
                            
                            //if the square is occupied by an enemy or empty, add it to the list of legals 
                            if ((occupant(brd, advancePos) == NULL || occupant(brd, advancePos)->colour != currPiece->colour) && (advancePos->row <= 8 && advancePos->row >= 1 && advancePos->col <= 'h' && advancePos->col >= 'a')) {
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
                            if ((occupant(brd, advancePos) == NULL || occupant(brd, advancePos)->colour != currPiece->colour) && (advancePos->row <= 8 && advancePos->row >= 1 && advancePos->col <= 'h' && advancePos->col >= 'a')) {
                                //add the current position to the list of legal moves
                                currPiece->moves[movesAdded] = copySq(advancePos);
                                movesAdded++;
                            }
                            
                        }
                    }
                    break;
                case 'k': ;
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
                            if ((advancePos->row >= 1 && advancePos->row <= 8 && advancePos->col >= 'a' && advancePos->col <= 'h') && (occupant(brd, advancePos) == NULL || occupant(brd, advancePos)->colour != currPiece->colour)) {
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

    runCheck(brd);

    //if told to do so, go through and remove moves that don't resolve check
    //if we're doing the check on whether we're in check, and someone's in check
    if (doChk && brd->check != 'n') {

        //traverse all pieces
        for (int i = 0; i < brd->numPieces; i++) {

            //shortcut to the current piece
            piece_td* currPiece = brd->layout[i];

            //if we're dealing with a piece that is responding to the check 
            if (currPiece->colour == brd->check) {
                
                //initialise an array to hold indices of good moves 
                int resolvers[NUM_MOVES]; 
                for (int j = 0; j < NUM_MOVES; j++) {
                    resolvers[j] = -1;
                }
                int numResolvers = 0;

                //traverse the current piece's moves 
                for (int j = 0; currPiece->moves[j]->row != 0; j++) {
                    
                    //do move currPiece->moves[j] 
                    movePiece(hypo, i, j);
                    generateMoves(hypo, false);

                    //if check is resolved, remove it from the list 
                    if (hypo->check != brd->check) {
                        
                        //add this index to the list of moves that resolve check
                        resolvers[numResolvers] = j;
                        numResolvers++;
                    }

                    //reset hypo
                    softCopyBoard(&hypo, brd);
                } 

                int clearingIndex = 0;

                //clear out the list of moves but leave those that resolve check 
                for (int j = 0; j < NUM_MOVES; j++) {
                    
                    //if this index is listed as a resolver 
                    if (j == resolvers[clearingIndex]) {
                        clearingIndex++;
                    } else {
                        currPiece->moves[j]->row = 0;
                        currPiece->moves[j]->col = 0;
                    }
                }

                //rearrange the list
                //traverse the list of moves 
                int j = 0;
                while (j < numResolvers) {

                    //if this move is empty
                    if (currPiece->moves[j]->row == 0) {
                        
                        //delete the move
                        free(currPiece->moves[j]);

                        //shuffle each move one slot to the left 
                        for (int k = j; k < NUM_MOVES; k++) {
                            currPiece->moves[k] = currPiece->moves[k+1];
                        }
                        currPiece->moves[NUM_MOVES - 1] = malloc(sizeof(square_td));
                        currPiece->moves[NUM_MOVES - 1]->row = 0;
                        currPiece->moves[NUM_MOVES - 1]->col = 0;

                    } else {
                        j++;
                    }
                }

            }
        }
    }
    
    //check that we're not making any moves that would leave us in check if this isn't a recursive call
    if (doChk) {
        
         //traverse pieces
        for (int i = 0; i < brd->numPieces; i++) {

            //shortcut to the current piece
            piece_td* currPiece = brd->layout[i];

            //traverse all this piece's moves 
            for (int j = 0; currPiece->moves[j]->row != 0; j++) {
                
                //clean out the hypothetical board 
                softCopyBoard(&hypo, brd);

                //do move currPiece->moves[j] 
                movePiece(hypo, i, j);
                generateMoves(hypo, false);

                //if this move leaves that piece's side in check
                if (hypo->check == currPiece->colour) {
                    
                    //delete the move
                    free(currPiece->moves[j]);

                    //shuffle each move one slot to the left 
                    for (int k = j; k < NUM_MOVES; k++) {
                        currPiece->moves[k] = currPiece->moves[k+1];
                    }

                    //insert a blank move in the last slot 
                    currPiece->moves[NUM_MOVES - 1] = malloc(sizeof(square_td));
                    currPiece->moves[NUM_MOVES - 1]->row = 0;
                    currPiece->moves[NUM_MOVES - 1]->col = 0;

                    //decrement j because we just shuffled all the contents of the array to the right  
                    j--;
                }

            }
            
        }

    }

    freeBoard(hypo);
}

//run one layer of the simulation
move_td* simulate(board_td* brd, int depth) {
    //generate the legal moves for all the pieces on the board 
    generateMoves(brd, true);

    if (depth == 1) {
        //base case
        
        //create a hypothetical board to work on 
        board_td* hypo = newBoard();
        
        //create a move to store the best move 
        move_td* best = newMove();

        //for every piece
        for (int i = 0; i < brd->numPieces; i++) {

            //shortcut to the current piece
            piece_td* currPiece = brd->layout[i];

            //if it's this piece's turn 
            if (currPiece->colour == brd->turn) {
                
                //for every move
                for (int j = 0; currPiece->moves[j]->row != 0; j++) {
                    
                    //set up hypo 
                    softCopyBoard(&hypo, brd);

                    //make the move 
                    movePiece(hypo, i, j);
                    generateMoves(hypo, true);

                    //get the score 
                    int score = getScore(hypo);

                    //save the best 
                    if (score >= best->score) {
                        best->score = score;
                        best->pieceInd = i;
                        best->moveInd = j;
                    }
                }

            }

        }

        return best;

    } else {

        //create a hypothetical board to work on 
        board_td* hypo = newBoard();

        //store and set up the best move
        move_td* bestMove = newMove();

        //traverse all pieces 
        for (int i = 0; i < brd->numPieces; i++) {
            
            //shortcut to the current piece
            piece_td* currPiece = brd->layout[i];

            //if we're about to look at a piece whose turn it is
            if (currPiece->colour == hypo->turn) {

                //traverse all moves this piece has 
                for (int j = 0; currPiece->moves[j]->row != 0; j++) {

                    //reset the hypothetical board to the original 
                    softCopyBoard(&hypo, brd);

                    //make the current move in the traversal 
                    movePiece(hypo, i, j);
                    generateMoves(hypo, true);

                    //get the enemy's best response  
                    move_td* enemyBest = simulate(hypo, depth - 1);
                    
                    //make the enemy's response 
                    movePiece(hypo, enemyBest->pieceInd, enemyBest->moveInd);
                    generateMoves(hypo, true);

                    //get the score for me 
                    int finalScore = getScore(hypo);

                    //log it if it's the best
                    if (finalScore >= bestMove->score) {
                        bestMove->score = finalScore;
                        bestMove->pieceInd = i;
                        bestMove->moveInd = j;
                    }
                }
            }
        }

        return bestMove;

        /*
        //recursive case 

        //create a hypothetical board to work on 
        board_td* hypo = copyBoard(brd);

        //get the enemy colour 
        char enemyColour = 'b';
        if (brd->turn == 'b') {
            enemyColour = 'w';
        }

        //recurse out to the next layer of simulation for the enemy 
        move_td* bestEnemy = simulate(hypo, depth - 1);

        //make the best move the enemy can do
        movePiece(hypo, bestEnemy->pieceInd, bestEnemy->moveInd);
        generateMoves(hypo, true);    

        //recurse out to the next layer of simulation for our pieces 
        move_td* bestFriendly = simulate(hypo, depth - 1);
        
        //free temporary moves and boards
        free(bestEnemy);
        freeBoard(hypo);
        
        return bestFriendly;
        */
    }
    
    return NULL;
}

//constructor for moves 
move_td* newMove() {
    move_td* mv = malloc(sizeof(move_td));
    mv->score = INT_MIN;
    mv->pieceInd = -1;
    mv->moveInd = -1;
}

//set up a default board
void setInit(board_td* brd) {
    brd->numPieces = 32;

    for (int i = 0; i < brd->numPieces; i++) {
        if (brd->layout[i] == NULL) {
            brd->layout[i] = newPiece();
        }
    }

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
            currPiece->type = 'n';
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
        currPiece->type = 'k';
    }
}

//print out a board 
void printBoard(board_td* brd) {
    printf("\n");
    for (int r = 8; r >= 1; r--) { //for every row
        for (char c = 'a'; c <= 'h'; c += 1) { //for every column
            char output = '.';

            for (int i = 0; i < brd->numPieces; i++) { //for every piece
                if (brd->layout[i]->pos->row == r && brd->layout[i]->pos->col == c) { //check whether current coords match the current piece
                    output = brd->layout[i]->type - ((brd->layout[i]->colour == 'w')? 32 : 0); 
                    break;
                }
            }

            printf("%c ", output);
        }

        printf("\n\n");
    }
}

//write a board out into a text file 
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

    fprintf(fp, "%c", brd->turn);    

    fclose(fp);
}

//read a board in from a text file 
board_td* readBoard(char* name) {
    board_td* brd = newBoard();

    int pieceIndex = 0;

    FILE* fp = fopen(name, "r");

    char* input = malloc(sizeof(char) * 74);
    char* inputTraverse = input;

    fread(input, sizeof(char), 74, fp);

    //close the file we're reading in 
    fclose(fp);

    for (int r = 8; r >= 1; r--) { //for every row
        for (char c = 'a'; c <= 'h' + 1; c += 1) { //for every column

            //read in the piece 
            char pieceIn = *inputTraverse;

            //skip the empty squares and line breaks
            if (pieceIn != '.' && pieceIn != '\n') {
            
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

    brd->turn = *inputTraverse;

    //free the string we used to read the board in 
    free(input);

    while (pieceIndex < 32) {
        freePiece(brd->layout[pieceIndex]);
        brd->layout[pieceIndex] = NULL;
        pieceIndex++;
    }

    return brd;
}

//print a board as it is in memory 
void dumpBoard(board_td* brd) {
    
    printf("\n\n\n");
    for (int i = 0; i < brd->numPieces; i++) {
        piece_td* currPiece = brd->layout[i];

        printf("Piece %i - %c %c at %c%i \n", i, currPiece->colour, currPiece->type, currPiece->pos->col, currPiece->pos->row);

        for (int j = 0; currPiece->moves[j]->row != 0; j++) {
            printf("    %c%i\n", currPiece->moves[j]->col, currPiece->moves[j]->row);
        }
    }

}