//define the smallest int value
#define INT_MIN -2147483648

//define the largest int value
#define INT_MAX 2147483647

//the depth we want to make the simulation of future steps max out to
#define SIM_DEPTH 3

//the number of legal moves a piece can have 
#define NUM_MOVES 30

//the number of pieces boards should hold 
#define NUM_PIECES 32

//a struct to hold a move 
struct move {
    int pieceInd;
    int moveInd;
    int score;
} typedef move_td;

//a struct to represent a square, such as "e5"
struct square {
    int row;
    char col;
} typedef square_td;

//a struct to represent chess pieces 
struct piece {
    char type;
    char colour;
    square_td pos;
    square_td moves[NUM_MOVES];
} typedef piece_td;

//a struct to represent chess boards
struct board {
    piece_td layout[NUM_PIECES];
    int numPieces; //the number of pieces on the board 
    char turn; 
    char check; //code for who is in check: w for white, b for black, n for nobody 
} typedef board_td;

//constants for piece values
#define PAWN_VAL 10
#define ROOK_VAL 50
#define KNIG_VAL 30
#define BISH_VAL 30
#define QUEE_VAL 90

//allow bool (boolean) variables 
typedef enum {false, true} bool; 

//allocate and return a pointer to a new board struct in memory
board_td* newBoard();  

//deallocate a board and its contents completely 
void freeBoard(board_td*);  

//constructor for pieces
piece_td* newPiece();  

//destructor for pieces
void freePiece(piece_td*);  

//copy the square struct orig and return a pointer to a deep copy 
void copySq(square_td*, square_td);  

//check if two square structs are the same 
bool equalSq(square_td, square_td);  

//check if square sq on board brd is occupied, returning a pointer to the occupying piece if so
int occupant(board_td*, square_td);  

//get the score for board_td struct for a given colour 
int getScore(board_td*);  

//make a deep copy of a board 
board_td* copyBoard(board_td*);  

//soft-copy a board
void softCopyBoard(board_td**, board_td*);  

//check whether the current colour is in check 
bool inCheck(board_td*); //-------------

//check whether the current colour is in checkmate
bool inCheckMate(board_td*); //-------------

//move a piece from ci, ri to cf, rf on board brd 
void movePiece(board_td*, int, int); //problem-------------

void generateMoves(board_td*, bool); //problem-------------

move_td* simulate(board_td*, int); //problem-------------

//constructor for moves 
move_td* newMove();

void setInit(board_td*);  

void printBoard(board_td*);  

void writeBoard(board_td*, char*);  

board_td* readBoard(char*);  

//print a board as it is in memory 
void dumpBoard(board_td* brd);  

//print a move 
void printMove(board_td*, move_td*);