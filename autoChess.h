//a struct to hold a move 
struct move {
    int pieceInd;
    int moveInd;
    int score;
} typedef move_td;

struct square {
    int row;
    char col;
} typedef square_td;

//a struct to represent chess pieces 
struct piece {
    char type;
    char colour;
    square_td* pos;
    square_td** moves;
} typedef piece_td;

//a struct to represent chess boards
struct board {
    piece_td** layout;
    int numPieces; //the number of pieces on the board 
    //char turn; 
    //int score;
} typedef board_td;

//the depth we want to make the simulation of future steps max out to
#define SIM_DEPTH 4

//constants for piece values
#define PAWN_VAL 10
#define ROOK_VAL 40 
#define KNIG_VAL 30 
#define BISH_VAL 50
#define QUEE_VAL 100

//allow bool (boolean) variables 
typedef enum {false, true} bool; 

//allocate and return a pointer to a new board struct in memory
board_td* newBoard();

//copy the square struct orig and return a pointer to a deep copy 
square_td* copySq(square_td*);

//check if two square structs are the same 
bool equalSq(square_td*, square_td*);

//check if square sq on board brd is occupied, returning a pointer to the occupying piece if so
piece_td* occupant(board_td*, square_td*);

//get the score for board_td struct for a given colour 
int getScore(board_td*, char);

//check whether the given colour is in check 
bool inCheck(board_td*, char);

//check whether the given colour is in checkmate
bool inCheckMate(board_td*, char);

//move a piece from ci, ri to cf, rf on board brd 
board_td* movePiece(board_td*, int, int);

void generateMoves(board_td*);

move_td* simulate(board_td*, char, int);

void setInit(board_td*);

void printBoard(board_td*);

void writeBoard(board_td*, char*);

board_td* readBoard(char*);