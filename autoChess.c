#include "autoChess.h"

//create a new baord in memory (constructor)
board_td* newBoard() {
    board_td* newBrd = malloc(sizeof(board_td));

    for (int i = 0; i < 32; i++) {
        newBrd->layout[i] = malloc(sizeof(piece_td));
    }

    newBrd->numPieces = 32;

    return newBrd;
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
piece_td* isOccupied(board_td* brd, square_td* sq) {
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

//todo
int main() {
    board_td* brd = newBoard();
    
    setInit(brd);

    printBoard(brd);

    return 0;
}

//get the score for board_td struct for a given colour 
int getScore(board_td* brd, char colour) {
    int scoreOut = 0; //variable to hold the score to output  

    if (inCheckMate(brd, colour)) {
        return 6942069;
    }
    if (inCheck(brd, colour)) {
        scoreOut = QUEE_VAL * 2;
    }
    
    for (int i = 0; i < brd->numPieces; i++) {
        switch ((brd->layout[i])->type) {
            case 'p':
                scoreOut += PAWN_VAL;
                break;
            case 'r':
                scoreOut += ROOK_VAL;
                break;
            case 'k':
                scoreOut += KNIG_VAL;
                break;
            case 'b':
                scoreOut += BISH_VAL;
                break;
            case 'q':
                scoreOut += QUEE_VAL;
                break;
            default: 
                printf("u dun goofed piece type flags");
        }
    }

    return scoreOut;
}

//check whether the given colour is in check 
bool inCheck(board_td* brd, char colour) {
    
}

//check whether the given colour is in checkmate
bool inCheckMate(board_td* brd, char colour) {

}

//move a piece from ci, ri to cf, rf on board brd 
board_td* movePiece(board_td* brd, char ci, int ri, char cf, int rf) {

}

void generateMoves(board_td* brd) {
    //traverse every piece on the board 
    for (int i = 0; i < 32; i++) { 
        //store a pointer to the current piece to reduce memory accesses
        piece_td* currPiece = brd->layout[i]; 

        //check that the piece whose moves we're about to generate exists
        if (currPiece != NULL) { //TODO what the hell is the null identifier?
            
            //clear out the list of legals 
            for (int i = 0; i < 30; i++) { 
                free(currPiece->moves[i]);
                currPiece->moves[i] = NULL; 
            } 

            //the number of moves we've added to the list of legal moves on the current piece 
            int movesAdded = 0; 

            //generate moves by piece type
            switch (currPiece->type) { 
                //PAWNS
                case 'p': 
                    //hold the row of a given pawn that hasn't moved 
                    int stationaryRow; 
                    
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

                    //set up a temp variable to hold the square we're thinking of advancing into 
                    square_td* advancePos = copySq(currPiece->pos);
                    //add the +/- 2 rows to the move we just added ^
                    advancePos->row += 2 * blackOffset; 

                    //if this pawn hasn't moved and the square two squares up isn't occupied 
                    if (currPiece->pos->row == stationaryRow && isOccupied(brd, advancePos) == NULL) { 
                        //add the current position advanced by two squares to legals
                        currPiece->moves[movesAdded] = advancePos; 
                        //increment the legals 
                        movesAdded++; 
                    }

                    //HANDLE SINGLE ADVANCES
                    //bring the double advance back a square to handle regular pawn advances
                    advancePos->row -= 1 * blackOffset;

                    //if the square one square up isn't occupied 
                    if (!isOccupied(brd, advancePos)) { 
                        //add the current position advanced by two squares to legals
                        currPiece->moves[movesAdded] = advancePos; 
                        //increment the legals 
                        movesAdded++; 
                    }

                    //HANDLE CAPUTRES 
                    //move the temp target square left one 
                    advancePos->col -= 1;

                    //do the capture check twice, reusing the code in a kinda cheeky and ugly way
                    for (int loopCount = 0; loopCount < 2; loopCount++) {
                        //hold a pointer to the target piece for the capture  
                        piece_td* target = isOccupied(brd, advancePos);
                        
                        //just move on if there's nothing to think about capturing and the square up one and over one is occupied by an opponent piece
                        if (target != NULL && target->colour != currPiece->colour) {
                            currPiece->moves[movesAdded] = target->pos;
                            movesAdded++;
                        } 

                        //shift the up one, left one to be up one, right one 
                        advancePos->col += 2;
                    }
                    

                    //free the temp variable from earlier 
                    free(advancePos);

                    break;
            }
        }
    }
}

//set up a default board
void setInit(board_td* brd) {
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
            currPiece->pos->col = ((i == 8)? 1 : 8) + 'a';
            currPiece->pos->row = (offset == 16)? 8 : 1;
            currPiece->colour = (offset == 16)? 'b' : 'w';
            currPiece->type = 'r';
        }

        //set up knights
        for (int i = 10; i <= 11; i++) {
            currPiece = brd->layout[i + offset];
            currPiece->pos->col = ((i == 10)? 2 : 7) + 'a';
            currPiece->pos->row = (offset == 16)? 8 : 1;
            currPiece->colour = (offset == 16)? 'b' : 'w';
            currPiece->type = 'k';
        }
        
        //set up bishops
        for (int i = 12; i <= 13; i++) {
            currPiece = brd->layout[i + offset];
            currPiece->pos->col = ((i == 12)? 3 : 6) + 'a';
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
    for (int r = 1; r <= 8; r++) { //for every row
        for (char c = 'a'; c <= 'h'; c += 1) { //for every column
            for (int i = 0; i < brd->numPieces; i++) { //for every piece
                if (brd->layout[i]->pos->row == r && brd->layout[i]->pos->col == c) { //check whether current coords match the current piece
                    printf("%s", brd->layout[i]->type + (brd->layout[i]->colour == 'w')? 32 : 0); 
                } else {
                    printf(".");
                }
            }           
        }
    }
}