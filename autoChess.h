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
    square_td* pos;
    square_td** moves;
} typedef piece_td;

//a struct to represent chess boards
struct board {
    piece_td** layout;
    int numPieces; //the number of pieces on the board 
    char turn; 
    //int score;
} typedef board_td;

//define the smallest int value
#define INT_MIN -2147483648

//the depth we want to make the simulation of future steps max out to
#define SIM_DEPTH 4

//the number of legal moves a piece can have 
#define NUM_MOVES 30

//constants for piece values
#define PAWN_VAL 1
#define ROOK_VAL 5 
#define KNIG_VAL 3 
#define BISH_VAL 3
#define QUEE_VAL 9

//allow bool (boolean) variables 
typedef enum {false, true} bool; 

//KTBW = KNOWN TO BE WORKING 

//allocate and return a pointer to a new board struct in memory
board_td* newBoard(); //KTBW

//deallocate a board and its contents completely 
void freeBoard(board_td*); //KTBW

//constructor for pieces
piece_td* newPiece(); //KTBW

//destructor for pieces
void freePiece(piece_td*); //KTBW

//copy the square struct orig and return a pointer to a deep copy 
square_td* copySq(square_td*); //KTBW

//check if two square structs are the same 
bool equalSq(square_td*, square_td*); //KTBW

//check if square sq on board brd is occupied, returning a pointer to the occupying piece if so
piece_td* occupant(board_td*, square_td*); //KTBW

//get the score for board_td struct for a given colour 
int getScore(board_td*); //KTBW

//make a deep copy of a board 
board_td* copyBoard(board_td*); 

//soft-copy a board
void softCopyBoard(board_td*, board_td*);

//check whether the current colour is in check 
bool inCheck(board_td*); 

//check whether the current colour is in checkmate
bool inCheckMate(board_td*); 

//move a piece from ci, ri to cf, rf on board brd 
void movePiece(board_td*, int, int); //problem

void generateMoves(board_td*, bool); //problem

move_td* simulate(board_td*, int); //problem

void setInit(board_td*); //KTBW

void printBoard(board_td*); //KTBW

void writeBoard(board_td*, char*); //KTBW

board_td* readBoard(char*); //KTBW