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
    boolean newGame(short whiteTime = 0, short blackTime = 0);
    boolean initialize();
    void startGame();
    boolean startClock();
    short loop();
    boolean scanBoard(boolean continuous = true, boolean output = true);
    void setPromotedPiece(short piece);
    void setRed(boolean on);
    void loadGame();
    boolean setupBoard();
    void resetSetupBoard();

    static const short loop_noUpdate = 0;
    static const short loop_endTurn = 1;
    static const short loop_promotion = 2;
    static const short loop_timeout = 3;
    String shortForms[7];
    char fileName[10];
    Leds *leds;
    
  private:
    void scanRow(short row);
    void setDiff();
    void detectMove();
    boolean turnEnd();
    void reduceMoves();
    void debugBoard(short array[8][8]);
    void debugScan(short array[8][8]);
    void outputBoard(boolean debug = true);
    boolean isValidMove(short piece, short movesToCheck[2][2]);
    boolean checkCollisions (short movesToCheck[2][2]);
    boolean inCheck(short kingRow, short kingCol, short kingColour, short checkBoard[8][8]);
    short sign(short val);
    boolean isSlide(short down, short row, short col);
    void resetFixes();
    void fixBoard(String message = "", short lcdRow = 0);
    void setPgnMove();
    void moveAttacker(short attackRow, short attackCol, short piece);
    
    Clock clock;
    
    const short pawn;
    const short knight;
    const short bishop;
    const short rook;
    const short queen;
    const short king;
    
    const short delayRead;
    const short nRows;
    const short endTurn;
    boolean enableEndTurn;
    const short scan;
    boolean enableScan;
    short redLed;
    
    char lastPgnTurn[6];
    
    // if 1, then white's turn, if -1 then black's turn
    short whosTurn;
    short numTurns;
    
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
    short nFixes;
    short fix[3][2];
    String fixMessage;
    short setupPosition[2];
    short setPosition[2];
    boolean forceLoadSelect;
    
    Lcd *lcd;
};

#endif