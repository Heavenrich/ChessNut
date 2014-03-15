#include "pgn.h"

PGN::PGN(char file[12]){
    File chosenfile = SD.open(file);
    movesCount = 0;
    boardMoves = 0;
    String move = "";
    while(chosenfile.available()){
        char temp = chosenfile.read();
        // Serial.println("read" + String(temp));
        if (temp != ' ') {
            move += String(temp);
            // Serial.println("strcat: " + String(move));
        } else {
            // Serial.println("listOfMoves " + String(move));
            listOfMoves[movesCount] = move;
            movesCount++;
            move = "";
        }
    }
    // Serial.println("movesCount: " + String(movesCount));

    chosenfile.close();
    for (int i = 0; i < movesCount; i++) {
        if (i % 3 != 0) {
            // Serial.println("stringParser: " + listOfMoves[i]);
            stringParser(listOfMoves[i],color);
            color = -1;
        }
        else {
            color = 1;
        }
    }
}

void PGN::stringParser(String a, short b) {
    if (a.length() == 2) {
        boardList[boardMoves][0]=a[1]-49;
        boardList[boardMoves][1]=a[0]-97;
        boardList[boardMoves][2]=1*b;
    }
    
    // 3 chars
    // pawn eating another piece
    // pawn moving in check
    // big piece moving
    else if (a.length() == 3) {
        if (a[0] == 'x') {
            boardList[boardMoves][0]=a[2]-49;;
            boardList[boardMoves][1]=a[1]-97;
            boardList[boardMoves][2]=1*b;
        }
        else if (a[2] == '+' || a[2] == '#'){
            boardList[boardMoves][0]=a[1]-49;
            boardList[boardMoves][1]=a[0]-97;
            boardList[boardMoves][2]=1*b;
        }
        // else if (a[0] == "O" ) {
            // do castle move
        // }
        else {
            boardList[boardMoves][0]=a[2]-49;
            boardList[boardMoves][1]=a[1]-97;
            pgnParserPieces(a[0],b);
        }
    }

    // 4 chars either for pawn promoting
    // pawn eating another piece and in check
    // pawn doing a en passon use . equals or big piece eating another or                   pawn putting another in check
    // OR same pieces move to one spot (rooks,QQ, knight)
    else if (a.length() == 4){
        if (a[3] == '#') {
            boardList[boardMoves][0]=a[2]-49;
            boardList[boardMoves][1]=a[1]-97;
            pgnParserPieces(a[0],b);
        }
        else if (a[0] == 'x') {
            boardList[boardMoves][0]=a[2]-49;
            boardList[boardMoves][1]=a[1]-97;
            boardList[boardMoves][2]=1*b;
        }
        else if (isupper(a[0]) && a[1] == 'x') {
            boardList[boardMoves][0]=a[3]-49;
            boardList[boardMoves][1]=a[2]-97;
            pgnParserPieces(a[0],b);
        }
        else if (isupper(a[0]) && a[1] != 'x' && a[3] == '+') {
            // final_move.append("pawn");
            boardList[boardMoves][0]=a[2]-49;
            boardList[boardMoves][1]=a[1]-97;
            pgnParserPieces(a[0],b);
        }
        else if (isupper(a[0]) && a[1] != 'x' && isdigit(a[1])) {
            // final_move.append("pawn");
            boardList[boardMoves][0]=a[3]-49;
            boardList[boardMoves][1]=a[2]-97;
            pgnParserPieces(a[0],b);
        }
         else if (a[3] == '+' || a[3] == '#'){
            boardList[boardMoves][0]=a[1]-49;
            boardList[boardMoves][1]=a[0]-97;
            pgnParserPieces(a[2],b);
        }
        else if (a[2] == '='){
            boardList[boardMoves][0]=a[1]-49;
            boardList[boardMoves][1]=a[0]-97;
            pgnParserPieces(a[3],b);
        }
        else if (isupper(a[0]) && a[1] != 'x' && isalpha(a[1])) {
            // final_move.append("pawn");
            boardList[boardMoves][0]=a[3]-49;
            boardList[boardMoves][1]=a[2]-97;
            pgnParserPieces(a[0],b);
        }
        else if (islower(a[0])) {
            boardList[boardMoves][0]=a[3]-49;
            boardList[boardMoves][1]=a[2]-97;
            boardList[boardMoves][2]=1*b;
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
            boardList[boardMoves][0]=a[2]-49;
            boardList[boardMoves][1]=a[1]-97;
            pgnParserPieces(a[4],b);
        }
        else if (isupper(a[0]) && a[1] == 'x') {
            boardList[boardMoves][0]=a[3]-49;
            boardList[boardMoves][1]=a[2]-97;
            pgnParserPieces(a[0],b);
        }
        else {
            if (islower(a[0])) {
                boardList[boardMoves][0]=a[1]-49;
                boardList[boardMoves][1]=a[0]-97;
                pgnParserPieces(a[3],b);
            }
            // else {
            //     pgnParserPieces(a[1],b);
            //     pgnParserRow(a[2]);
            //     pgnParserColumn(a[3]);
            // }
        }
    }
    else if (a.length() == 6) {
        boardList[boardMoves][0]=a[2]-49;
        boardList[boardMoves][1]=a[1]-97;
        pgnParserPieces(a[4],b);
    }
    boardMoves++;
}

void PGN::pgnParserPieces(char x, short b) {
    switch (x) {
        case 'K':
            boardList[boardMoves][2]=6*b;
            break;
        case 'Q':
            boardList[boardMoves][2]=5*b;        
            break;
        case 'R':
            boardList[boardMoves][2]=4*b;
            break;
        case 'B':
            boardList[boardMoves][2]=3*b;
            break;
        case 'N':
            boardList[boardMoves][2]=2*b;
            break;
        default:
            boardList[boardMoves][2]=1*b;
            break;
    }
}
