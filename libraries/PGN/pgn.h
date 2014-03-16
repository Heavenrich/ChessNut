#ifndef PGN_H
#define PGN_H

#include <Arduino.h>
#include <SD.h>

#define ASCII_A_UPPER 65
#define ASCII_Z_UPPER 90
#define ASCII_A_LOWER 97
#define ASCII_H_LOWER 104
#define ASCII_1 49
#define ASCII_8 56
#define A_LOWER 97

class PGN {
public:
  PGN();
	//PGN(char file[12]);
  void setFile(char file[12]);
  boolean readFile();
  void closeFile();
	//void stringParser(String a,short b);
  boolean stringParser(String a);
	//void pgnParserPieces(char x, short b);
  short pgnParserPieces(char x);

  File chosenfile;
  short boardList[7];
	short boardMoves;
	short movesCount;
  short separationCount;
	short color;
  boolean castle;
  boolean castleKingSide;
  static const short notChecked = -1000;
  static const short notUsed = -100;
};

#endif