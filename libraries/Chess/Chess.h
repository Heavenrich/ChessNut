#ifndef CHESS_H
#define CHESS_H

#include <Arduino.h>
#include <SD.h>
#include "Lcd.h"
#include "Leds.h"
#include "Clock.h"

#define MAX_MOVES 50

class Chess {
  public:
    Chess(short delayRead, short endTurn, short redLed, Lcd *lcd, Leds *leds, char cols[8], short gridInput[8], short gridOutput[8]);
    boolean newGame(short whiteTime = 0, short blackTime = 0); // set board, is initialized? -> startGame()
    boolean initialize();
    void startGame(); // let game begin, or if clock let them press select to start the game
    boolean startClock(); // if board is still initialized start the game
    short loop(); // main loop, return a loop_ variable if endTurn, promotion, or clock timeout
    boolean scanBoard(boolean continuous = true, boolean output = true);
    void setPromotedPiece(short piece); // user has chosen "piece" for pawn promotion
    void setRed(boolean on); // set the red led
    void loadGame();
    boolean setupBoard(); // loop for physically setting up board, return true if done
    void resetSetupBoard(); // should be called once before physically setting the board
    boolean moveAttacker(short attackRow, short attackCol, short piece, short fromRow = -1, short fromCol = -1);
    void debugCurrentBoard();
    boolean moveCastle(boolean kingSide);
    void initializeBoard();
    boolean movePromotion(short attackCol, short piece);
    boolean moveEnpassant(short attackRow, short attackCol);


    static const short pawn = 1;
    static const short knight = 2;
    static const short bishop = 3;
    static const short rook = 4;
    static const short queen = 5;
    static const short king = 6;
    
    static const short loop_noUpdate = 0;
    static const short loop_endTurn = 1;
    static const short loop_promotion = 2;
    static const short loop_timeout = 3; // clock ran out
    String shortForms[7];
    char fileName[10];
    Leds *leds;
    short numTurns;
    short whosTurn;
    
  private:
    void scanRow(short row);
    void setDiff();
    void detectMove();
    short turnEnd(); // return edge case for setPgn, -1 for false
    void reduceMoves();
    void debugBoard(short array[8][8]); // Serial output
    void debugScan(short array[8][8]); // Serial output
    void outputBoard(boolean debug = true); // print up to 10 squares that are activated to lcd
    boolean isValidMove(short piece, short movesToCheck[2][2]);
    boolean checkCollisions (short movesToCheck[2][2]);
    boolean inCheck(short kingRow, short kingCol, short kingColour, short checkBoard[8][8], boolean hypothetical=false, boolean noKing=false);
    short sign(short val);
    boolean isSlide(short down, short row, short col);
    void resetFixes(); // called once before fixBoard
    void fixBoard(String message = "", short lcdRow = 0); // output squares different than board to lcd
    void setPgnMove();
    void setWhosTurn(short whosTurn);
    boolean inCheckmate(short kingRow, short kingCol, short kingColour, short checkBoard[8][8]);
    
    Clock clock;
    
    boolean checkmate;
    
    short pgn_state;
    static const short pgn_normal = 0;
    static const short pgn_promotion = 1;
    static const short pgn_castleKing = 2;
    static const short pgn_castleQueen = 3;
    static const short pgn_take = 10;
    
    const short delayRead;
    const short nRows;
    const short endTurn;
    boolean enableEndTurn;
    const short scan;
    boolean enableScan;
    short redLed;
    
    char lastPgnTurn[6];
    
    // if 1, then white's turn, if -1 then black's turn
    
    // list of moves made this turn 
    // moves[0] = {-1, 2, 4} means the first move of the turn was a piece being removed from D2
    // moves[1] = {1, 4, 4} means the second move of the turn was a piece being placed at D4
    short moves[MAX_MOVES][3];
    short numMoves;
    
    short reducedMoves[MAX_MOVES][3];
    short numReducedMoves;
    
    // Black is 0, White is 1; records number of pieces attacking king
    short kingAttackers[2];
    short kingPositions[2][2];
    short kingAttackerPosition[2];
	
    short castlingArrivals[2][2];
    short castlingDepartures[2][2];
    const short castlingKing;
    const short castlingQueen;
    const char *cols;
    const short *gridInput;
    const short *gridOutput;
    const char* file;
    
    short prevScan[8][8];
    short currScan[8][8];
    short diff[8][8];
    short board[8][8];
    short nFixes; // # of current squares to fix, from fixBoard
    short fix[5][2]; // array of col and rows of squares different than board, set in fixBoard
    short setupPosition[2]; // current col row being set
    short setPosition[2]; // last col row of piece set up
    boolean forceLoadSelect; // if true make player press endTurn to confirm it contains right piece
    
    Lcd *lcd;
};

#endif