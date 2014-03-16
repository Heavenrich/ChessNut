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
  separationCount = -1;
}

boolean PGN::readFile() {
  String move = "";
  castle = false;
  while(chosenfile.available()){
    char temp = chosenfile.read();
    Serial.println("read" + String(temp));
    if (temp != ' ') {
      move += String(temp);
      Serial.println("strcat: " + move);
    } else {
      Serial.println("listOfMoves " + move);
      boolean parserOK = true;
      separationCount++;
      if (separationCount % 3 != 0) {
        if (move == "O-O") {
          castle = true;
          castleKingSide = true;
        } else if (move == "O-O-O") {
          castle = true;
          castleKingSide = false;
        } else {
          Serial.println("stringParser: " + move);
          parserOK = stringParser(move);
          color = -1;
        }
        movesCount++;
        return parserOK;
      }
    }
  }

  return false;
}

void PGN::closeFile() {
  Serial.println("movesCount: " + String(movesCount));
  chosenfile.close();
}

boolean PGN::stringParser(String a) {
  for (short i = 0; i < 7; i++) {
    boardList[i] = notChecked;
  }

  for (short i = 0; i < a.length(); i++) {
    if (boardList[0] == notChecked) {
      if (a[i] > ASCII_A_UPPER && a[i] < ASCII_Z_UPPER) {
        boardList[0] = pgnParserPieces(a[0]);
      } else {
        boardList[0] = pgnParserPieces('P');
      }
    } else if (boardList[1] == notChecked || boardList[2] == notChecked) {
      if (boardList[1] == notChecked) {
        if (a[i] >= ASCII_A_LOWER && a[i] <= ASCII_H_LOWER) {
          boardList[1] = a[i] - ASCII_A_LOWER;
        } else {
          boardList[1] = notUsed;
          if (a[i] >= ASCII_1 && a[i] <= ASCII_8) {
            boardList[2] = a[i] - ASCII_1;
          }
        }
      } else if (boardList[2] == notChecked) {
        if (a[i] >= ASCII_1 && a[i] <= ASCII_8) {
          boardList[2] = a[i] - ASCII_1;
        } else {
          boardList[2] = notUsed;
        }
      }
    } else if (boardList[3] == notChecked) {
      if (a[i] == 'x') {
        boardList[3] = 1;
      } else {
        boardList[3] = notUsed;
      }
    } else if (boardList[4] == notChecked) {
      if (a[i] >= ASCII_A_LOWER && a[i] <= ASCII_H_LOWER) {
        boardList[4] = a[i] - ASCII_A_LOWER;
      } else {
        boardList[4] = notUsed;
        Serial.println("String parser failed: didn't get destination letter");
        return false;
      }
    } else if (boardList[5] == notChecked) {
      if (a[i] >= ASCII_1 && a[i] <= ASCII_8) {
        boardList[5] = a[i] - ASCII_1;
      } else {
        boardList[5] = notUsed;
        Serial.println("String parser failed: didn't get destination number");
        return false;
      }
    } else if (boardList[6] == notChecked) {
      if (a[i] = '=') {
        i++;
        if (a[i] >= ASCII_A_UPPER && a[i] <= ASCII_A_LOWER) {
          boardList[6] = pgnParserPieces(a[i]);
        } else {
          boardList[6] = notUsed;
          Serial.println("String parser failed: didn't get piece for promotion");
          return false;
        }
      } else {
        boardList[6] = notUsed;
      }
    } else if (boardList[7] == notChecked) {
      if (a[i] == '+') {
        boardList[7] = 1;
      } else if (a[i] == '#') {
        boardList[7] = 2;
      } else {
        boardList[7] = notUsed;
      }
    } else {
      Serial.println("String parser failed: ran over our array");
      return false;
    }
  }
  for (short i = 0; i < 8; i++) {
    if (boardList[i] == notChecked) {
      return false;
    }
  }
  return true;
}

 short PGN::pgnParserPieces(char x) {
    switch (x) {
        case 'K':
            return 6;
            break;
        case 'Q':
            return 5;
            break;
        case 'R':
            return 4;
            break;
        case 'B':
            return 3;
            break;
        case 'N':
            return 2;
            break;
        default:
            return 1;
            break;
    }
}

/*void PGN::stringParser(String a, short b) {
    if (a.length() == 2) {
        boardList[0]=a[1]-49;
        boardList[1]=a[0]-97;
        boardList[2]=1*b;
    }
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
}*/