#ifndef CHESS_H
#define CHESS_H

#include <Arduino.h>
#include <LiquidCrystal.h>

#define MAX_MOVES 20

class Chess {
  public:
    Chess();
    Chess(short period, short endTurn, short scan, LiquidCrystal *lcd, char cols[8], short gridInput[8], short gridOutput[8]);
    boolean initialize();
    void newGame();
    boolean loop();
    
  private:
    boolean scanBoard(boolean continuous = true, boolean output = true);
    void scanRow(short row);
    void setDiff();
    void detectMove();
    boolean turnEnd();
    void reduceMoves();
    void debugBoard(short array[8][8]);
    void debugScan(short array[8][8]);
    void outputBoard();
    
    const short pawn;
    const short knight;
    const short bishop;
    const short rook;
    const short queen;
    const short king;
    
    const short period;
    const short nRows;
    const short endTurn;
    boolean enableEndTurn;
    const short scan;
    boolean enableScan;
    
    // true if a game is underway
    boolean gameOn;
    // if 1, then white's turn, if -1 then black's turn
    short whosTurn;
    
    // list of moves made this turn 
    // moves[0] = {-1, 2, 4} means the first move of the turn was a piece being removed from D2
    // moves[1] = {1, 4, 4} means the second move of the turn was a piece being placed at D4
    short moves[MAX_MOVES][3];
    short numMoves;
    
    char cols[8];
    short gridInput[8];
    short gridOutput[8];
    
    short prevScan[8][8];
    short currScan[8][8];
    short diff[8][8];
    short board[8][8];
    
    LiquidCrystal *lcd;
};

#endif