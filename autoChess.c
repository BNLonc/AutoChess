#include <stdlib.h>
#include <stdio.h> 
#include <time.h>

//include the header file
#include "autoChess.h"

//create a new baord in memory (constructor)
board_td* newBoard() {
    board_td* newBrd = malloc(sizeof(board_td));

    newBrd->numPieces = 0;

    newBrd->turn = 'w';

    return newBrd;
}

//copy the square struct orig and return a pointer to a deep copy 
inline void copySq(square_td* recip, square_td orig) {

    recip->row = orig.row;
    recip->col = orig.col;
}

//check if two square structs are the same 
inline bool equalSq(square_td a, square_td b) {
    
    return (a.col == b.col && a.row == b.row);
}

//check if square sq on board brd is occupied, returning a pointer to the occupying piece if so
int occupant(board_td* brd, square_td sq) {

    piece_td* layout = brd->layout;

    int numPc = brd->numPieces;

    //traverse all pieces
    for (int i = 0; i < numPc; ++i) {  

        //if the square the piece is on matches the one we're checking 
        if (equalSq(sq, layout[i].pos)) { 
            
            //return the pointer to the current piece
            return i; 
        }

    }

    //if we've run through all the pieces and didn't hit a match, return NULL
    return -1;
}

//main function 
int main() {

    /*board_td* readIn = readBoard("brdTst.txt");
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

    move_td* best4 = simulate(readIn, SIM_DEPTH);
    printf("Best move for %c at full depth is: ", readIn->turn);
    printMove(readIn, best4);

    writeBoard(readIn, "brdOut.txt");

    free(best1);
    free(best2);
    free(best3);
    free(best4);

    free(readIn);*/
    
    runPlay();

    return 0;
}

//the frontend loop for playing against the bot 
void runPlay() {

    char input;

    board_td* brd = newBoard();
    setInit(brd);
    generateMoves(brd, true);

    char* result;

    do {
        system("clear");
        printBoard(brd);

        //debug
        //dumpBoard(brd);
        
        printf("Enter command to (q)uit, (r)estart, (s)ave game, (l)oad game, or (m)ake a move: ");
        input = getchar();

        //grab the extra newline character left behind 
        getchar();

        char fileName[STR_LEN];

        switch (input) {
            case 'q':

                system("clear");

                break;

            case 'r':
                setInit(brd);
                generateMoves(brd, true);
                break;

            case 's':
                printf("Enter the name you want to use for your save file: ");
                result = fgets(fileName, STR_LEN, stdin);

                trimNewline(fileName);

                writeBoard(brd, fileName);

                break;

            case 'l':
                
                free(brd);

                do {

                    if (brd == NULL) {
                        printf("Save file not found\n");
                    }
                    
                    printf("Enter the name of your save file: ");
                    result = fgets(fileName, STR_LEN, stdin);
                    trimNewline(fileName);

                    brd = readBoard(fileName);

                } while (brd == NULL);

                generateMoves(brd, true);

                break;
            
            case 'm':

                printf("What is the location of the piece you'd like to move? ");
                char location[STR_LEN];
                result = fgets(location, STR_LEN, stdin);

                square_td locationSq;
                locationSq.col = location[0];
                locationSq.row = ((int)location[1]) - 48;

                int index = occupant(brd, locationSq);

                //if we're trying to move off a bad location
                if (index == -1 || brd->layout[index].colour != 'w') {

                    printf("Invaid piece - press ENTER to continue\n");
                    getchar();

                } else {
                    //the location is good 

                    printf("Where do you want to move this piece? ");
                    char destination[STR_LEN];
                    result = fgets(destination, STR_LEN, stdin);

                    square_td destinationSq;
                    destinationSq.col = destination[0];
                    destinationSq.row = ((int)destination[1]) - 48;

                    piece_td currPiece = brd->layout[index];

                    int moveIndex = 0;
                    while (currPiece.moves[moveIndex].row != 0) {
                        
                        if (equalSq(currPiece.moves[moveIndex], destinationSq)) {
                            break;
                        }
                        
                        ++moveIndex;
                    }

                    if (currPiece.moves[moveIndex].row == 0) {
                        
                        printf("Invalid move - press ENTER to continue\n");
                        getchar();
                        
                        break;

                    } else {
                        //the move is good 

                        movePiece(brd, index, moveIndex);
                        generateMoves(brd, true);

                        if (inCheckMate(brd)) {
                            printf("White wins!");
                            exit(0);
                        }

                        printf("Reggie is moving...\n");

                        move_td* blackMove = simulate(brd, SIM_DEPTH);

                        movePiece(brd, blackMove->pieceInd, blackMove->moveInd);
                        generateMoves(brd, true);

                        if (inCheckMate(brd)) {
                            printf("Black wins!");
                            exit(0);
                        }

                        //printBoard(brd);
                    }

                }

                break;
            
            default:
                printf("Command unrecognised; try again\n");
        }

    } while (input != 'q');

}

//trim the '\n' off the end of the given string
void trimNewline(char* string) {
    int i = 0;
    while (string[i] != '\n') {
        i++;
    }
    string[i] = 0;
}

//get the score for board_td struct for a given colour 
int getScore(board_td* brd) {

    //return a crazy low value if we're in checkmate     
    if (inCheckMate(brd)) {
        return INT_MIN;
    }

    //shortcut to turn 
    char turn = brd->turn;

    //save the enemy colour 
    char enemyColour = 'b';
    if (turn == 'b') {
        enemyColour = 'w';
    }

    //check whether the enemy is in checkmate 
    char savedTurn = turn;
    brd->turn = enemyColour;
    if (inCheckMate(brd)) {
        return INT_MAX;
    }
    brd->turn = savedTurn;
    
    //shortcut to check
    char check = brd->check;

    //variable to hold the score to output
    int scoreOut = 0; 

    //check about being in check
    if (check == turn) {
        scoreOut -= QUEE_VAL * 2;
    } else if (check == enemyColour) {
        scoreOut += QUEE_VAL * 2;
    } 

    //set up shortcuts
    piece_td* layout = brd->layout;
    int numPieces = brd->numPieces;

    //traverse all the pieces 
    for (int i = 0; i < numPieces; ++i) {
        
        //temporary variable to hold the score we're going to add for this piece
        int scoreAdd = 0;

        switch (layout[i].type) {
            case 'p':
                scoreAdd += PAWN_VAL;
                break;
            case 'r':
                scoreAdd += ROOK_VAL;
                break;
            case 'n':
                scoreAdd += KNIG_VAL;
                break;
            case 'b':
                scoreAdd += BISH_VAL;
                break;
            case 'q':
                scoreAdd += QUEE_VAL;
                break;
            case 'k':
                break;
            default: 
                printf("u dun goofed piece type flags\n");
        }

        //store the bonus we're giving this piece for being in or covering the centre
        int centreBonus = 0;
        int coverageBonus = 0;

        //check if the piece occupies the centre 
        if (/*layout[i].pos.row <= 6 && layout[i].pos.row >= 3 && layout[i].pos.col <= 'f' && layout[i].pos.col >= 'c'*/(layout[i].pos.row == 4 || layout[i].pos.row == 5) && (layout[i].pos.col == 'd' || layout[i].pos.col == 'e')) {
            
            //use bit-shifts to add scoreAdd * 1/(2^3) or scoreAdd * 1/16 to centreBonus
            //centreBonus += scoreAdd >> 4;
            
            //give greater scores to pieces in the very centre four squares on the board
            //if ((layout[i].pos.row == 4 || layout[i].pos.row == 5) && (layout[i].pos.col == 'd' || layout[i].pos.col == 'e')) {
                
                //use bit-shifts to add scoreAdd * 1/(2^4) or scoreAdd * 1/8 to centreBonus
                centreBonus += scoreAdd >> 3;
            //}

        } else {
            //do a bonus for pieces that cover the centre 

            //traverse the piece's moves 
            for (int j = 0; layout[i].moves[j].row != 0; j++) {
                
                //give greater scores to pieces covering the very centre four squares on the board
                if ((layout[i].moves[j].row == 4 || layout[i].moves[j].row == 5) && (layout[i].moves[j].col == 'd' || layout[i].moves[j].col == 'e')) {
                    
                    //use bit-shifts to add scoreAdd * 1/(2^5) or scoreAdd * 1/32 to centreBonus
                    coverageBonus += scoreAdd >> 5;
                }    
            }

            
        }

        //add in the larger  bonus
        if (centreBonus > coverageBonus) {
            scoreAdd += centreBonus;
        } else {
            scoreAdd += coverageBonus;
        }  
        
        if (layout[i].hasMoved == false) {
            scoreAdd = 3 * (scoreAdd >> 2); 
        }

        //set the score to add as positive or negative depending on the colour 
        if (layout[i].colour == enemyColour) {
            scoreAdd *= -1;
        }

        scoreOut += scoreAdd;

    }

    return scoreOut;
}

//make a deep copy of a board 
board_td* copyBoard(board_td* orig) {
    
    //allocate a new board
    board_td* newBrd = newBoard();

    softCopyBoard(&newBrd, orig);

    return newBrd; 
}

//soft-copy a board 
void softCopyBoard(board_td** recip, board_td* orig) {
    
    board_td* newBrd = *recip;

    //copy the numPieces over 
    newBrd->numPieces = orig->numPieces;

    //copy turn
    newBrd->turn = orig->turn;

    //set up a shortcut 
    int numPieces = newBrd->numPieces;

    //set up shortcuts 
    piece_td* newLayout = newBrd->layout;
    piece_td* origLayout = orig->layout;

    //copy the pieces in the layout array 
    register int i = 0;
    while (i < numPieces) {

        //copy position
        newLayout[i].pos.row = origLayout[i].pos.row;
        newLayout[i].pos.col = origLayout[i].pos.col;

        //copy type and colour 
        newLayout[i].type = origLayout[i].type;
        newLayout[i].colour = origLayout[i].colour;

        //copy moves
        for (register int j = 0; j < NUM_MOVES; ++j) {
            newLayout[i].moves[j].row = origLayout[i].moves[j].row;
            newLayout[i].moves[j].col = origLayout[i].moves[j].col;
        }

        newLayout[i].hasMoved = origLayout[i].hasMoved;

        ++i;
    }

    while (i < NUM_PIECES) {
        
        //zero out pieces that shouldn't exist in the copy 
        newLayout[i].pos.row = 0;

        ++i;
    }

    //free(*recip);

    //*recip = copyBoard(orig);
}

//check whether the given colour is in check 
void runCheck(board_td* brd) { 

    brd->check = 0;

    char colour = 'b';

    //set up shortcuts
    piece_td* layout = brd->layout;
    int numPieces = brd->numPieces;

    while (brd->check == 0) {
        
        //find the index of the king of the current player  
        int kingInd = 0;
        while (layout[kingInd].type != 'k' || layout[kingInd].colour != colour) {
            ++kingInd;
        }

        if (kingInd == numPieces) {
            printf("no friendly king found -- problem at inCheck()\n");
            brd->check = 'n';
            return;
        }

        //go through every piece on the board 
        for (int i = 0; i < numPieces; ++i) {
            
            //ignore pieces of the same colour as the friendly king
            if (layout[i].colour != colour) {

                //go through this (enemy) piece's every legal move
                int moveInd = 0;
                while (layout[i].moves[moveInd].row != 0) {
                    
                    //if one of its legal moves is the square on which the king sits, return true 
                    if (equalSq(layout[i].moves[moveInd], layout[kingInd].pos)) {
                        
                        //assign check and jump out
                        brd->check = colour;
                        return;
                    }

                    ++moveInd;
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

    //make sure we're actually in check first lmao
    if (brd->check == brd->turn) {
        
        //create a hypothetical board to work on 
        board_td* hypo = copyBoard(brd);

        //shortcut to hypo num pieces 
        int numPieces = brd->numPieces;

        //shortcut to hypo layout
        piece_td* hypoLayout = hypo->layout;

        //shortcut to turn
        char turn = brd->turn;

        //for every friendly piece on the board
        for (int i = 0; i < numPieces; ++i) {
            
            //continue; if the current piece is unfriendly 
            if (hypoLayout[i].colour != turn) {
                continue;
            }

            //for every move that this piece can do...
            int moveInd = 0;
            while (hypoLayout[i].moves[moveInd].row != 0) {

                //do it and then see if we're no longer in check 
                movePiece(hypo, i, moveInd);
                generateMoves(hypo, true);

                //if we did a move and are no longer in check, free the hypo board and return false 
                if (hypo->check != hypo->turn) {
                    free(hypo);
                    return false;
                }

                //if we get here, we didn't get out of check by making that move, so let's reset the hypo board 
                softCopyBoard(&hypo, brd); 
                //generateMoves(hypo);

                ++moveInd;
            }

        }

        free(hypo);
        return true;

    } else {
        return false;
    }
}

//move a piece on board brd 
void movePiece(board_td* brd, int pieceInd, int moveInd) {
    
    //set the hasMoved  field
    brd->layout[pieceInd].hasMoved = true;

    //point to the piece we're going to land on when we move 
    square_td landPos = brd->layout[pieceInd].moves[moveInd];

    //create shortcuts
    int numPieces = brd->numPieces;
    piece_td* layout = brd->layout;

    //go through the pieces to find the index of the piece we're going to capture  
    int capInd = 0;
    while (capInd < numPieces) {
        
        //break if we've found a piece we're going to land on 
        square_td targetPos = layout[capInd].pos;
        if (equalSq(targetPos, landPos)) {
            break;
        }
        
        ++capInd;
    }

    //if we're going to land on a piece, destroy it
    if (capInd < numPieces) {
    
        //don't capture the king 
        if (layout[capInd].type == 'k') {
            return;
        }
        
        layout[capInd].pos.row = 0;

        layout[capInd] = layout[numPieces - 1];
        layout[numPieces - 1].pos.row = 0;
        --(brd->numPieces);
        --numPieces;

        //check whether we were working on the last piece in the array (that just got moved elsewhere) 
        if (pieceInd == numPieces) {
            pieceInd = capInd;
        }
    }

    layout[pieceInd].pos.row = landPos.row;
    layout[pieceInd].pos.col = landPos.col;  

    if (layout[pieceInd].type == 'p' && ((layout[pieceInd].pos.row == 8 && layout[pieceInd].colour == 'w') || (layout[pieceInd].pos.row == 1 && layout[pieceInd].colour == 'b'))) {
        layout[pieceInd].type = 'q';
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

    //set up shortcuts 
    int numPieces = brd->numPieces;
    piece_td* layout = brd->layout;

    //traverse every piece on the board 
    for (int i = 0; i < numPieces; ++i) { 

        //zero out the list of legals 
        for (int j = 0; j < NUM_MOVES; ++j) {
            layout[i].moves[j].row = 0;
            layout[i].moves[j].col = 0; 
        } 

        //the number of moves we've added to the list of legal moves on the current piece 
        int movesAdded = 0; 

        //set up a temp variable to hold the square we're thinking of advancing into 
        square_td advancePos; 
        copySq(&advancePos, layout[i].pos);

        //generate moves by piece type
        switch (layout[i].type) { 
            //PAWNS
            case 'p': ;
                //hold the row of a given pawn that hasn't moved 
                int stationaryRow = 0; 
                
                //value is 1 or -1, to multiply on vertical movements of black pawns for code reuse
                int blackOffset = 1; 

                if (layout[i].colour == 'w') {
                    //white pawns on row 2 haven't moved 
                    stationaryRow = 2; 
                } else if (layout[i].colour == 'b') {
                    //black pawns on row 7 haven't moved 
                    stationaryRow = 7;
                    //this piece is black, so its movement is in the opposite direction as white ones  
                    blackOffset = -1; 
                } else {
                    printf("u dun goofed pawn colours\n");
                }


                //HANDLE DOUBLE ADVANCES FOR FIRST MOVES 

                //add the +/- 2 rows to the move we just added ^
                advancePos.row += 2 * blackOffset; 

                //if this pawn hasn't moved and the square two squares up isn't occupied 
                if (layout[i].pos.row == stationaryRow && occupant(brd, advancePos) == -1) { 
                    //add the current position advanced by two squares to legals
                    copySq(&(layout[i].moves[movesAdded]), advancePos);
                    //increment the legals 
                    ++movesAdded; 
                }

                //HANDLE SINGLE ADVANCES
                //bring the double advance back a square to handle regular pawn advances
                advancePos.row -= 1 * blackOffset;

                //if the square one square up isn't occupied 
                if (occupant(brd, advancePos) == -1) { 
                    //add the current position advanced by two squares to legals
                    copySq(&(layout[i].moves[movesAdded]), advancePos);
                    //increment the legals 
                    ++movesAdded; 
                }

                //HANDLE CAPUTRES 
                //move the temp target square left one 
                advancePos.col -= 1;

                //do the capture check twice, reusing the code in a kinda cheeky and ugly way
                for (int loopCount = 0; loopCount < 2; ++loopCount) {
                    
                    //hold a pointer to the target piece for the capture  
                    int target = occupant(brd, advancePos);
                    
                    //just move on if there's nothing to think about capturing and the square up one and over one is occupied by an opponent piece
                    if (target != -1 && layout[target].colour != layout[i].colour) {
                        
                        copySq(&(layout[i].moves[movesAdded]), brd->layout[target].pos);

                        ++movesAdded;
                    } 

                    //shift the up one, left one to be up one, right one 
                    advancePos.col += 2;
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
                    advancePos.row += j;
                    //check whether the position is occupied
                    while (occupant(brd, advancePos) == -1 && advancePos.row >= 1 && advancePos.row <= 8) {
                        
                        //add the current position to the list of legal moves
                        copySq(&(layout[i].moves[movesAdded]), advancePos);
                        ++movesAdded;

                        //increment the position we're looking at 
                        advancePos.row += j;
                    }

                    int finalPos = occupant(brd, advancePos);

                    //if the piece we stopped at is a piece of the opposing colour 
                    if (finalPos != -1 && (brd->layout[finalPos].colour != layout[i].colour)) {

                        //add the current position to the list of legal moves
                        copySq(&(layout[i].moves[movesAdded]), advancePos);
                        ++movesAdded;

                        //increment the position we're looking at 
                        advancePos.row += j;
                    }

                    advancePos.row = layout[i].pos.row;
                    advancePos.col = layout[i].pos.col; 
                }

                

                //do the plus and minus directions
                for (int j = -1; j <= 1; j += 2) {

                    //increment advancePos to start checks off of the current position
                    advancePos.col += j;
                    //check whether the position is occupied
                    while (occupant(brd, advancePos) == -1 && advancePos.col >= 'a' && advancePos.col <= 'h') {
                        
                        //add the current position to the list of legal moves
                        copySq(&(layout[i].moves[movesAdded]), advancePos);
                        ++movesAdded;

                        //increment the position we're looking at 
                        advancePos.col += j;
                    }
                    
                    int finalPos = occupant(brd, advancePos);

                    //if the piece we stopped at is a piece of the opposing colour 
                    if (finalPos != -1 && (brd->layout[finalPos].colour != layout[i].colour)) {
                        //add the current position to the list of legal moves
                        copySq(&(layout[i].moves[movesAdded]), advancePos);
                        ++movesAdded;

                        //increment the position we're looking at 
                        advancePos.col += j;
                    }

                    advancePos.row = layout[i].pos.row;
                    advancePos.col = layout[i].pos.col; 
                }

                //if we're dealing with a rook, this is the end of the line
                if (layout[i].type == 'r') {
                    break;
                }

                //if we're not dealing with a rook, we're dealing with a queen
                //reset the position of advancePos to set up for the case 'b' code we're about to fall into 
                advancePos.row = layout[i].pos.row;
                advancePos.col = layout[i].pos.col;
                
            case 'b': ;
                
                //run the four diagonal directions
                for (int j = -1; j <= 1; j += 2) {
                    for (int k = -1; k <= 1; k += 2) {
                        
                        advancePos.col += j;
                        advancePos.row += k;
                        
                        //run one diagonal 
                        while (occupant(brd, advancePos) == -1 && advancePos.col >= 'a' && advancePos.col <= 'h' && advancePos.row >= 1 && advancePos.row <= 8) {
                            
                            //add the current position to the list of legal moves
                            copySq(&(layout[i].moves[movesAdded]), advancePos);
                            ++movesAdded;

                            //increment in the given direction 
                            advancePos.col += j;
                            advancePos.row += k;
                        }

                        int finalPos = occupant(brd, advancePos);

                        //if the piece we stopped at is a piece of the opposing colour 
                        if (finalPos != -1 && (brd->layout[finalPos].colour != layout[i].colour)) {
                            
                            //add the current position to the list of legal moves
                            copySq(&(layout[i].moves[movesAdded]), advancePos);
                            ++movesAdded;

                            //increment the position we're looking at 
                            advancePos.col += j;
                            advancePos.row += k;
                        }

                        advancePos.row = layout[i].pos.row;
                        advancePos.col = layout[i].pos.col;
                        
                    }
                }

                break;
            case 'n': ;

                //run two directions
                for (int j = -1; j <= 1; j += 2) {
                    for (int k = -2; k <= 2; k += 4) {
                        
                        //move the piece 
                        advancePos.row = layout[i].pos.row + j;
                        advancePos.col = layout[i].pos.col + k;
                        
                        int advIndex = occupant(brd, advancePos);

                        //if the square is occupied by an enemy or empty, add it to the list of legals 
                        if ((advIndex == -1 || brd->layout[advIndex].colour != layout[i].colour) && (advancePos.row <= 8 && advancePos.row >= 1 && advancePos.col <= 'h' && advancePos.col >= 'a')) {
                            //add the current position to the list of legal moves
                            copySq(&(layout[i].moves[movesAdded]), advancePos);
                            ++movesAdded;
                        }
                        
                    }
                }

                //run two directions
                for (int j = -2; j <= 2; j += 4) {
                    for (int k = -1; k <= 1; k += 2) {
                        
                        //move the piece 
                        advancePos.row = layout[i].pos.row + j;
                        advancePos.col = layout[i].pos.col + k;
                        
                        int advIndex = occupant(brd, advancePos);

                        //if the square is occupied by an enemy or empty, add it to the list of legals 
                        if ((advIndex == -1 || brd->layout[advIndex].colour != layout[i].colour) && (advancePos.row <= 8 && advancePos.row >= 1 && advancePos.col <= 'h' && advancePos.col >= 'a')) {
                            //add the current position to the list of legal moves
                            copySq(&(layout[i].moves[movesAdded]), advancePos);
                            ++movesAdded;
                        }
                        
                    }
                }
                break;
            case 'k': ;
                //go around the piece and examine each position 
                for (int j = -1; j <= 1; ++j) {
                    for (int k = -1; k <= 1; ++k) {
                        
                        //skip the current position
                        if (j == 0 && k == 0) {
                            continue;
                        }
                        
                        advancePos.row = layout[i].pos.row + j;
                        advancePos.col = layout[i].pos.col + k;

                        //if advancePos is unoccupied or occupied by an enemy
                        if ((advancePos.row >= 1 && advancePos.row <= 8 && advancePos.col >= 'a' && advancePos.col <= 'h') && (occupant(brd, advancePos) == -1 || layout[occupant(brd, advancePos)].colour != layout[i].colour)) {
                            //add the current position to the list of legal moves
                            copySq(&(layout[i].moves[movesAdded]), advancePos);
                            ++movesAdded;
                        }
                    }
                }


                break;
            default: ;
                printf("you dun goofed piece selection\n");
                break;
        }
    
    }

    runCheck(brd);

    //if told to do so, go through and remove moves that don't resolve check
    //if we're doing the check on whether we're in check, and someone's in check
    if (doChk && brd->check != 'n') {

        //traverse all pieces
        for (int i = 0; i < brd->numPieces; ++i) {

            //if we're dealing with a piece that is responding to the check 
            if (layout[i].colour == brd->check) {
                
                //initialise an array to hold indices of good moves 
                int resolvers[NUM_MOVES]; 
                for (int j = 0; j < NUM_MOVES; ++j) {
                    resolvers[j] = -1;
                }
                int numResolvers = 0;

                //traverse the current piece's moves 
                for (int j = 0; layout[i].moves[j].row != 0; ++j) {
                    
                    //do move layout[i].moves[j] 
                    movePiece(hypo, i, j);
                    generateMoves(hypo, false);

                    //if check is resolved, remove it from the list 
                    if (hypo->check != brd->check) {
                        
                        //add this index to the list of moves that resolve check
                        resolvers[numResolvers] = j;
                        ++numResolvers;
                    }

                    //reset hypo
                    softCopyBoard(&hypo, brd);
                } 

                int clearingIndex = 0;

                //clear out the list of moves but leave those that resolve check 
                for (int j = 0; j < NUM_MOVES; ++j) {
                    
                    //if this index is listed as a resolver 
                    if (j == resolvers[clearingIndex]) {
                        ++clearingIndex;
                    } else {
                        layout[i].moves[j].row = 0;
                        layout[i].moves[j].col = 0;
                    }
                }

                //rearrange the list
                //traverse the list of moves 
                int j = 0;
                while (j < numResolvers) {

                    //if this move is empty
                    if (layout[i].moves[j].row == 0) {
                        
                        //delete the move TODO is this necessary?
                        layout[i].moves[j].row = 0;
                        layout[i].moves[j].col = 0;

                        //shuffle each move one slot to the left 
                        for (int k = j; k < NUM_MOVES; ++k) {
                            layout[i].moves[k] = layout[i].moves[k+1];
                        }
                        layout[i].moves[NUM_MOVES - 1].row = 0;
                        layout[i].moves[NUM_MOVES - 1].col = 0;

                    } else {
                        ++j;
                    }
                }

            }
        }
    }
    
    //check that we're not making any moves that would leave us in check if this isn't a recursive call
    if (doChk) {
        
         //traverse pieces
        for (int i = 0; i < brd->numPieces; ++i) {

            //traverse all this piece's moves 
            for (int j = 0; layout[i].moves[j].row != 0; ++j) {
                
                //clean out the hypothetical board 
                softCopyBoard(&hypo, brd);

                //do move layout[i].moves[j] 
                movePiece(hypo, i, j);
                generateMoves(hypo, false);

                //if this move leaves that piece's side in check
                if (hypo->check == layout[i].colour) {
                    
                    //delete the move TODO is this necessary?
                    layout[i].moves[j].row = 0;
                    layout[i].moves[j].col = 0;

                    //shuffle each move one slot to the left 
                    for (int k = j; k < NUM_MOVES; ++k) {
                        layout[i].moves[k] = layout[i].moves[k+1];
                    }

                    //insert a blank move in the last slot 
                    layout[i].moves[NUM_MOVES - 1].row = 0;
                    layout[i].moves[NUM_MOVES - 1].col = 0;

                    //decrement j because we just shuffled all the contents of the array to the right  
                    j--;
                }

            }
            
        }

    }

    free(hypo);
}   

//run one layer of the simulation
move_td* simulate(board_td* brd, int depth) {
    
    /*printf("Running depth");
    for (int i = 0; i < depth; ++i) {
        printf(" ");
    }
    printf("%i\n", depth);*/

    //generate the legal moves for all the pieces on the board 
    generateMoves(brd, true);

    //set up shortcuts 
    int numPieces = brd->numPieces;
    piece_td* layout = brd->layout;
    char turn = brd->turn;

    if (depth == 1) {
        //base case

        //create a hypothetical board to work on 
        board_td* hypo = newBoard();
        
        //create a move to store the best move 
        move_td* best = newMove();
        best->score = INT_MAX;

        //for every piece
        for (int i = 0; i < numPieces; ++i) {

            //if it's this piece's turn 
            if (layout[i].colour == turn) {

                //for every move
                for (int j = 0; layout[i].moves[j].row != 0; ++j) {
                    
                    //set up hypo 
                    softCopyBoard(&hypo, brd);

                    //make the move 
                    movePiece(hypo, i, j);
                    generateMoves(hypo, true);

                    //get the enemy's resulting score 
                    int score = getScore(hypo);

                    //save the best for us (worst for them)
                    if (score <= best->score) {
                        best->score = score;
                        best->pieceInd = i;
                        best->moveInd = j;
                    }
                }

            }

        }

        free(hypo);

        return best;

    } else {
        //recursive case 

        //create a hypothetical board to work on 
        board_td* hypo = newBoard();

        hypo->turn = turn;

        //store and set up the best move
        move_td* bestMove = newMove();

        //traverse all pieces 
        for (int i = 0; i < numPieces; ++i) {

            //if we're about to look at a piece whose turn it is
            if (layout[i].colour == turn) {

                //traverse all moves this piece has 
                for (int j = 0; layout[i].moves[j].row != 0; ++j) {

                    //reset the hypothetical board to the original 
                    softCopyBoard(&hypo, brd);

                    /*if (depth == 3 && i == 0 && j == 0) {
                        //debug 
                        printBoard(hypo);
                        dumpBoard(hypo);
                    }*/
                    
                    //make the current move in the traversal 
                    movePiece(hypo, i, j);
                    generateMoves(hypo, true);

                    //if we didn't put the enemy in checkmate
                    if (!inCheckMate(brd)) {

                        //get the enemy's best response  
                        move_td* enemyBest = simulate(hypo, depth - 1);
                        
                        //make the enemy's response 
                        movePiece(hypo, enemyBest->pieceInd, enemyBest->moveInd);
                        generateMoves(hypo, true);

                        free(enemyBest);

                    } else {
                        
                        //just cycle the turns if we put the enemy in checkmate 
                        if (hypo->turn == 'w') {
                            hypo->turn = 'b';
                        } else {
                            hypo->turn = 'w';
                        }

                    }

                    //get the score for me 
                    int finalScore = getScore(hypo);
                    
                    /*if (depth == 3) {
                        //debug
                        printf("-+=+- Running simulation depth 3: piece %i, move %i, got score %i\n", i, j, finalScore);
                    }*/
                    
                    //log it if it's the best
                    if (finalScore >= bestMove->score) {
                        bestMove->score = finalScore;
                        bestMove->pieceInd = i;
                        bestMove->moveInd = j;
                    }

                }
            }
        }

        free(hypo);

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
        free(hypo);
        
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

    return mv;
}

//set up a default board
void setInit(board_td* brd) {

    brd->numPieces = NUM_PIECES;
    piece_td* layout = brd->layout;

    for (int offset = 0; offset <= 16; offset += 16) {
        
        //set up pawns 
        for (int i = 0; i < 8; ++i) {
            layout[i + offset].pos.col = i + 'a';
            layout[i + offset].pos.row = (offset == 16)? 7 : 2;
            layout[i + offset].colour = (offset == 16)? 'b' : 'w';
            layout[i + offset].type = 'p';
        }

        //set up rooks 
        for (int i = 8; i <= 9; ++i) {
            layout[i + offset].pos.col = ((i == 8)? 1 : 8) + 'a' - 1;
            layout[i + offset].pos.row = (offset == 16)? 8 : 1;
            layout[i + offset].colour = (offset == 16)? 'b' : 'w';
            layout[i + offset].type = 'r';
        }

        //set up knights
        for (int i = 10; i <= 11; ++i) {
            layout[i + offset].pos.col = ((i == 10)? 2 : 7) + 'a' - 1;
            layout[i + offset].pos.row = (offset == 16)? 8 : 1;
            layout[i + offset].colour = (offset == 16)? 'b' : 'w';
            layout[i + offset].type = 'n';
        }
        
        //set up bishops
        for (int i = 12; i <= 13; ++i) {
            layout[i + offset].pos.col = ((i == 12)? 3 : 6) + 'a' - 1;
            layout[i + offset].pos.row = (offset == 16)? 8 : 1;
            layout[i + offset].colour = (offset == 16)? 'b' : 'w';
            layout[i + offset].type = 'b';
        }

        int i = 14;

        //set up queens
        layout[i + offset].pos.col = 'd';
        layout[i + offset].pos.row = (offset == 16)? 8 : 1;
        layout[i + offset].colour = (offset == 16)? 'b' : 'w';
        layout[i + offset].type = 'q';

        i = 15;

        //set up kings
        layout[i + offset].pos.col = 'e';
        layout[i + offset].pos.row = (offset == 16)? 8 : 1;
        layout[i + offset].colour = (offset == 16)? 'b' : 'w';
        layout[i + offset].type = 'k';
    }

    for (int i = 0; i < NUM_PIECES; i++) {
        layout[i].hasMoved = false;
    }
}

//print out a board 
void printBoard(board_td* brd) {
    
    printf("\n");

    //set up shortcuts 
    piece_td* layout = brd->layout;
    int numPieces = brd->numPieces;

    printf("    ");

    for (char i = 'a'; i <= 'h'; i++) {
        printf("%c ", i);
    }

    printf("\n   ----------------\n");

    //for every row
    for (int r = 8; r >= 1; r--) { 
        
        printf("%i | ", r);

        //for every column
        for (char c = 'a'; c <= 'h'; c = c + 1) { 
            char output = '.';

            //for every piece
            for (int i = 0; i < numPieces; ++i) { 
                
                //check whether current coords match the current piece
                if (layout[i].pos.row == r && layout[i].pos.col == c) { 
                    output = layout[i].type - ((layout[i].colour == 'w')? 32 : 0); 
                    break;
                }
            }

            printf("%c ", output);
        }

        printf("\n\n");
    }

    /*if (brd->turn == 'w') {
        printf("White");
    } else if (brd->turn == 'b') {
        printf("Black");
    } else {
        printf("ERROR");
    }
    printf("'s turn\n\n");*/
    
}

//write a board out into a text file 
void writeBoard(board_td* brd, char* name) {
    
    //open the file 
    FILE* fp = fopen(name, "w");

    //set up shortcuts 
    piece_td* layout = brd->layout;
    int numPieces = brd->numPieces;

    char moveStr[numPieces];

    for (int i = 0; i < numPieces; i++) {
        moveStr[i] = '0';
    }

    int stringIndex = 0;

    //for every row
    for (int r = 8; r >= 1; --r) { 

        //for every column
        for (char c = 'a'; c <= 'h'; ++c) { 
            
            //set a default output 
            char output = '.';

            //for every piece
            for (int i = 0; i < numPieces; ++i) { 
                
                //check whether current coords match the current piece
                if (layout[i].pos.row == r && layout[i].pos.col == c) { 
                    
                    //save the piece 
                    output = layout[i].type - ((layout[i].colour == 'w')? 32 : 0); 
                    
                    //insert into the hasMoved string 
                    if (layout[i].hasMoved) {
                        moveStr[stringIndex] = '1';
                    } 

                    ++stringIndex;

                    break;
                }
            }

            fprintf(fp, "%c", output);
        }

        fprintf(fp, "%c", '\n');
    }

    fprintf(fp, "%c\n", brd->turn);

    for (int i = 0; i < numPieces; i++) {
        fprintf(fp, "%c", moveStr[i]);
    }

    fclose(fp);
}

//read a board in from a text file 
board_td* readBoard(char* name) {

    //initialise a new board
    board_td* brd = newBoard();

    //set up the index of the piece we're reading in
    int pieceIndex = 0;

    //open the file 
    FILE* fp = fopen(name, "r");

    if (fp == NULL) {
        return NULL;
    }

    //set up pointers to read the input file 
    char* input = malloc(sizeof(char) * 106);
    char* inputTraverse = input;

    //load in the file 
    int read = fread(input, sizeof(char), 106, fp);

    //check the proper read-in 
    if (read != 106) {
        printf("you goofed reading, you read %i", read);
        return NULL;
    }

    //close the file we're reading in 
    fclose(fp);

    //set up shortcuts 
    piece_td* layout = brd->layout;

    //for every row
    for (int r = 8; r >= 1; r--) { 

        //for every column
        for (char c = 'a'; c <= 'h' + 1; c = c + 1) { 

            //read in the piece 
            char pieceIn = *inputTraverse;

            //skip the empty squares and line breaks
            if (pieceIn != '.' && pieceIn != '\n') {
            
                //we've hit a piece 
                ++(brd->numPieces);

                //check for colour and standardise case 
                if (pieceIn >= 'a') {
                    layout[pieceIndex].colour = 'b';
                } else {
                    layout[pieceIndex].colour = 'w';

                    pieceIn = pieceIn + 32;
                }

                //set the type 
                layout[pieceIndex].type = pieceIn;

                //set the position 
                layout[pieceIndex].pos.row = r;
                layout[pieceIndex].pos.col = c;

                //move the index to the next space in the board object  
                ++pieceIndex;
            }

            //move the index to the next char in the string 
            ++inputTraverse;
        }
    }

    brd->turn = *inputTraverse;
    ++inputTraverse;

    for (int i = 0; i < brd->numPieces; i++) {
        ++inputTraverse;
        char readIn = *inputTraverse;
        if (readIn == '1') {
            brd->layout[i].hasMoved = true;
        } else if (readIn == '0') {
            brd->layout[i].hasMoved = false;
        } else {
            printf("problem reading in hasMoved");
        }
    }

    //free the string we used to read the board in 
    free(input);

    while (pieceIndex < NUM_PIECES) {
        layout[pieceIndex].pos.row = 0;
        ++pieceIndex;
    }

    return brd;
}

//print a board as it is in memory 
void dumpBoard(board_td* brd) {
    
    printf("\n\n");
    
    //set up shortcuts 
    piece_td* layout = brd->layout;
    //int numPieces = brd->numPieces;
    
    //traverse the pieces 
    for (int i = 0; i < NUM_PIECES; ++i) {
        
        //if the piece doesn't exist 
        if (layout[i].pos.row == 0) {
            printf("NULL\n");
        } else {
        
            printf("Piece %i - %c %c at %c%i - ", i, layout[i].colour, layout[i].type, layout[i].pos.col, layout[i].pos.row);

            if (layout[i].hasMoved) {
                printf("Has Moved");
            } else {
                printf("Hasn't Moved");
            }

            printf("\n");

            for (int j = 0; layout[i].moves[j].row != 0; ++j) {
                printf("    %c%i\n", layout[i].moves[j].col, layout[i].moves[j].row);
            }
        }
    }

}

//print a move 
void printMove(board_td* brd, move_td* mv) {
    piece_td thisPiece = brd->layout[mv->pieceInd];
    square_td thisMove = thisPiece.moves[mv->moveInd];
    
    printf("%c %c at %c%i to %c%i\n", thisPiece.colour, thisPiece.type, thisPiece.pos.col, thisPiece.pos.row, thisMove.col, thisMove.row);
}