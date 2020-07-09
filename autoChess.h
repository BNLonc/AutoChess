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

//define a null pointer for better readability 
#define NULL 0

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

//main function
int main();

//get the score for board_td struct for a given colour 
int getScore(board_td* brd, char colour);

//check whether the given colour is in check 
bool inCheck(board_td* brd, char colour);

//check whether the given colour is in checkmate
bool inCheckMate(board_td* brd, char colour);

//move a piece from ci, ri to cf, rf on board brd 
board_td* movePiece(board_td* brd, char ci, int ri, char cf, int rf);

