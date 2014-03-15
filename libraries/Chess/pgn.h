#ifndef PGN_H
#define PGN_H

#include <Arduino.h>
#include <SD.h>
#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <vector>


class PGN {
public:
	PGN(string file);
	void stringParser(string a,string b);
	void pgnParserPieces(char x);
	void pgnParserRow(char y);
	void pgnParserColumn(char z);
	void pgnParserCheck(char w);
	std::string final_move = "";


	string exampleString;
	std::vector<string> listOfMoves;
	string color = "";
};

#endif