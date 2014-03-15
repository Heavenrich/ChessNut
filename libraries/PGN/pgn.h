#ifndef PGN_H
#define PGN_H

#include <Arduino.h>
#include <SD.h>


class PGN {
public:
	PGN(char file[12]);
	void stringParser(String a,short b);
	void pgnParserPieces(char x, short b);

	String listOfMoves[20];
	short boardList[20][5];
	short boardMoves;
	short movesCount;
	short color;
};

#endif