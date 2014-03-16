#include "pgn.h"

PGN::PGN() {
}

/*PGN::PGN(char file[12]) {
    chosenfile = SD.open(file);
    movesCount = 0;
    boardMoves = 0;
    String move = "";
    while(chosenfile.available()){
        char temp = chosenfile.read();
        Serial.println("read" + String(temp));
        if (temp != ' ') {
            move += String(temp);
            Serial.println("strcat: " + String(move));
        } else {
            Serial.println("listOfMoves " + String(move));
            listOfMoves[movesCount] = move;
            movesCount++;
            move = "";
        }
    }
    Serial.println("movesCount: " + String(movesCount));

    chosenfile.close();
    for (int i = 0; i < movesCount; i++) {
        if (i % 3 != 0) {
            Serial.println("stringParser: " + listOfMoves[i]);
            stringParser(listOfMoves[i],color);
            color = -1;
        }
        else {
            color = 1;
        }
    }
}*/

void PGN::setFile(char file[12]) {
  chosenfile = SD.open(file);
  movesCount = 0;
  boardMoves = 0;
}

boolean PGN::readFile() {
  String move = "";
  while(chosenfile.available()){
    char temp = chosenfile.read();
    Serial.println("read" + String(temp));
    if (temp != ' ') {
      move += String(temp);
      Serial.println("strcat: " + move);
    } else {
      Serial.println("listOfMoves " + move);
      if (movesCount % 3 != 0) {
        Serial.println("stringParser: " + move);
        stringParser(move,color);
        color = -1;
      }
      else {
        color = 1;
      }
      movesCount++;
      return true;
    }
  }

  return false;
}

void PGN::closeFile() {
  Serial.println("movesCount: " + String(movesCount));
  chosenfile.close();
}

void PGN::stringParser(String a, short b) {
    if (a.length() == 2) {
        boardList[0]=a[1]-49;
        boardList[1]=a[0]-97;
        boardList[2]=1*b;
    }
    
    // 3 chars
    // pawn eating another piece
    // pawn moving in check
    // big piece moving
    else if (a.length() == 3) {
        if (a[0] == 'x') {
            boardList[0]=a[2]-49;;
            boardList[1]=a[1]-97;
            boardList[2]=1*b;
            boardList[3]=-1;
            boardList[4]=-1;
        }
        else if (a[2] == '+' || a[2] == '#'){
            boardList[0]=a[1]-49;
            boardList[1]=a[0]-97;
            boardList[2]=1*b;
            boardList[3]=-1;
            boardList[4]=-1;
        }
        else if (a[0] == 'O' ) {
            boardList[0]=-1;
            boardList[1]=-1;
            boardList[2]=6*b;
            boardList[3]=-1;
            boardList[4]=-1;
        }
        else {
            boardList[0]=a[2]-49;
            boardList[1]=a[1]-97;
            pgnParserPieces(a[0],b);
            boardList[3]=-1;
            boardList[4]=-1;
        }
    }

    // 4 chars either for pawn promoting
    // pawn eating another piece and in check
    // pawn doing a en passon use . equals or big piece eating another or                   pawn putting another in check
    // OR same pieces move to one spot (rooks,QQ, knight)
    else if (a.length() == 4){
        if (a[3] == '#') {
            boardList[0]=a[2]-49;
            boardList[1]=a[1]-97;
            pgnParserPieces(a[0],b);
            boardList[3]=-1;
            boardList[4]=-1;
        }
        else if (a[0] == 'O' ) {
            boardList[0]=0;
            boardList[1]=0;
            boardList[2]=1*b;
            boardList[3]=-1;
            boardList[4]=-1;
        }
        else if (a[0] == 'x') {
            boardList[0]=a[2]-49;
            boardList[1]=a[1]-97;
            boardList[2]=1*b;
            boardList[3]=-1;
            boardList[4]=-1;
        }
        else if (isupper(a[0]) && a[1] == 'x') {
            boardList[0]=a[3]-49;
            boardList[1]=a[2]-97;
            boardList[3]=-1;
            boardList[4]=-1;
            pgnParserPieces(a[0],b);
        }
        else if (isupper(a[0]) && a[1] != 'x' && a[3] == '+') {
            boardList[0]=a[2]-49;
            boardList[1]=a[1]-97;
            pgnParserPieces(a[0],b);
            boardList[3]=-1;
            boardList[4]=-1;
        }
        else if (isupper(a[0]) && a[1] != 'x' && isdigit(a[1])) {
            boardList[0]=a[3]-49;
            boardList[1]=a[2]-97;
            pgnParserPieces(a[0],b);
            boardList[3]=-1;
            boardList[4]=-1;
        }
         else if (a[3] == '+' || a[3] == '#'){
            boardList[0]=a[1]-49;
            boardList[1]=a[0]-97;
            pgnParserPieces(a[2],b);
            boardList[3]=-1;
            boardList[4]=-1;
        }
        else if (a[2] == '='){
            boardList[0]=a[1]-49;
            boardList[1]=a[0]-97;
            pgnParserPieces(a[3],b);
            boardList[3]=-2;
            boardList[4]=-2;
        }
        else if (isupper(a[0]) && a[1] != 'x' && isalpha(a[1])) {
            boardList[0]=a[3]-49;
            boardList[1]=a[2]-97;
            pgnParserPieces(a[0],b);
            boardList[3]=-1;
            boardList[4]=-1;
        }
        else if (islower(a[0])) {
            boardList[0]=a[3]-49;
            boardList[1]=a[2]-97;
            boardList[2]=1*b;
            boardList[3]=-1;
            boardList[4]=-1;
        }
            // else {
            // pgnParserPieces(a[1],b);
            // pgnParserRow(a[2]);
            // pgnParserColumn(a[3]);
            // }
    }
    // 5 chars either for pawn promoting and check OR
    // 2 of same pieces going to a place with check in mind(rooks,pawns,QQ) OR
    // big pieces overtaking and doing a check
    else if (a.length() == 5){
        if (a[0] == 'x') {
            // handles both cases
            boardList[0]=a[2]-49;
            boardList[1]=a[1]-97;
            pgnParserPieces(a[4],b);
            boardList[3]=-1;
            boardList[4]=-1;
        }
        else if (isupper(a[0]) && a[1] == 'x') {
            boardList[0]=a[3]-49;
            boardList[1]=a[2]-97;
            pgnParserPieces(a[0],b);
            boardList[3]=-1;
            boardList[4]=-1;
        }
        else if (a[0] == 'O' ) {
            boardList[0]=-2;
            boardList[1]=-2;
            boardList[2]=6*b;
            boardList[3]=-1;
            boardList[4]=-1;
        }
        else {
            if (islower(a[0])) {
                boardList[0]=a[1]-49;
                boardList[1]=a[0]-97;
                pgnParserPieces(a[3],b);
                boardList[3]=-2;
                boardList[4]=-2;
            }
            // else {
            //     pgnParserPieces(a[1],b);
            //     pgnParserRow(a[2]);
            //     pgnParserColumn(a[3]);
            // }
        }
    }
    else if (a.length() == 6) {
        if (a[0] == 'O' ) {
            boardList[0]=-2;
            boardList[1]=-2;
            boardList[2]=6*b;
            boardList[3]=-1;
            boardList[4]=-1;
        }
        else {
            boardList[0]=a[2]-49;
            boardList[1]=a[1]-97;
            pgnParserPieces(a[4],b);
            boardList[3]=-2;
            boardList[4]=-2;
        }
    }
    boardMoves++;
}

void PGN::pgnParserPieces(char x, short b) {
    switch (x) {
        case 'K':
            boardList[2]=6*b;
            break;
        case 'Q':
            boardList[2]=5*b;
            break;
        case 'R':
            boardList[2]=4*b;
            break;
        case 'B':
            boardList[2]=3*b;
            break;
        case 'N':
            boardList[2]=2*b;
            break;
        default:
            boardList[2]=1*b;
            break;
    }
}