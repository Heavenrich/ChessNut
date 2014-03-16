#ifndef PGN_H
#define PGN_H

#include <Arduino.h>
#include <SD.h>


class PGN {
public:
  PGN();
	//PGN(char file[12]);
  void setFile(char file[12]);
  boolean readFile();
  void closeFile();
	void stringParser(String a,short b);
	void pgnParserPieces(char x, short b);

  File chosenfile;
	short boardList[5];
	short boardMoves;
	short movesCount;
	short color;
};

#endif