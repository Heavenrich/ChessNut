#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <vector>

using namespace std;
void stringParser(string a,string b);
void pgnParserPieces(char x);
void pgnParserRow(char y);
void pgnParserColumn(char z);
void pgnParserCheck(char w);
std::string final_move ="";

int main()
{
   string exampleString;
   std::vector<string> testarray;
   string color ="";
  
  ifstream in("input.txt");
  while(in>>exampleString) {
      testarray.push_back(exampleString);
      /*cout<<exampleString<<endl;*/
  }
  
  for (int i = 0; i < testarray.size(); ++i)
  {
    if(i%3 != 0) {
        /* Run through functions*/
        
        stringParser(testarray[i],color);
        // cout << final_move << endl;
        // final_move = "";
        color = "black";
    }
    else {
        color = "white";
    }

  }

   return 0;
}

void stringParser(string a, string b){
    if (a.size() == 2){
        cout << "pawn" <<endl;
        // final_move.append("pawn");
        pgnParserRow(a[0]);
        pgnParserColumn(a[1]);
    }
    // 3 chars
    // pawn eating another piece
    // pawn moving in check
    // big piece moving
    else if (a.size() == 3) {
        if (a[0] == 'x') {
            // final_move.append("pawn");
            pgnParserRow(a[1]);
            pgnParserColumn(a[2]);
        }
        else if (a[2] == '+' || a[2] == '#'){
            // final_move.append("pawn");
            cout << "pawn" << endl;
            pgnParserRow(a[0]);
            pgnParserColumn(a[1]);
            pgnParserCheck(a[2]);
        }
        // else if (a[0] == "O" ) {
            // do castle move
        // }
        else {
            pgnParserPieces(a[0]);
            pgnParserRow(a[1]);
            pgnParserColumn(a[2]);
        }
    }
    // 4 chars either for pawn promoting
    // pawn eating another piece and in check
    // pawn doing a en passon use . equals or big piece eating another or                   pawn putting another in check
    // OR same pieces move to one spot (rooks,QQ, knight)
    else if (a.size() == 4){
        if (a[0] == 'x') {
            cout << "pawn" << endl;
            pgnParserRow(a[1]);
            pgnParserColumn(a[2]);
            pgnParserCheck(a[3]);
        }
        else if (isupper(a[0]) && a[1] == 'x') {
            pgnParserPieces(a[0]);
            pgnParserRow(a[2]);
            pgnParserColumn(a[3]);
        }
        else if (isupper(a[0]) && a[1] != 'x') {
            // final_move.append("pawn");
            pgnParserPieces(a[0]);
            pgnParserRow(a[1]);
            pgnParserColumn(a[2]);
            pgnParserCheck(a[3]);
        }
        else if (a[3] == '+' || a[3] == '#'){
            pgnParserRow(a[0]);
            pgnParserColumn(a[1]);
            pgnParserPieces(a[2]);
            pgnParserCheck(a[3]);
        }
        else if (a[2] == '='){
            pgnParserRow(a[0]);
            pgnParserColumn(a[1]);
            pgnParserPieces(a[3]);
        }
        else {
            if (islower(a[0])) {
                 final_move.append("pawn");
                 cout<<"pawn"<<endl;
                 pgnParserRow(a[2]);
                 pgnParserColumn(a[3]);
            }
            else {
            pgnParserPieces(a[1]);
            pgnParserRow(a[2]);
            pgnParserColumn(a[3]);
            }
        }
    }
    // 5 chars either for pawn promoting and check OR
    // 2 of same pieces going to a place with check in mind(rooks,pawns,QQ) OR
    // big pieces overtaking and doing a check
    else if (a.size() == 5){
        if (a[0] == 'x') {
            // handles both cases
            pgnParserRow(a[1]);
            pgnParserColumn(a[2]);
            pgnParserPieces(a[4]);
        }
        else if (isupper(a[0]) && a[1] == 'x') {
            pgnParserPieces(a[0]);
            pgnParserRow(a[2]);
            pgnParserColumn(a[3]);
            pgnParserCheck(a[4]);
        }
        else {
            if (islower(a[0])) {
                pgnParserPieces(a[3]);
                pgnParserRow(a[0]);
                pgnParserColumn(a[1]);
                pgnParserCheck(a[4]);  
            }
            // else {
            //     pgnParserPieces(a[1]);
            //     pgnParserRow(a[2]);
            //     pgnParserColumn(a[3]);
            // }
        }
    }
    else if (a.size() == 6) {
        pgnParserRow(a[1]);
        pgnParserColumn(a[2]);
        pgnParserPieces(a[4]);
        pgnParserCheck(a[5]);
    }
}

void pgnParserPieces(char x) {
    switch (x) {
        case 'K':
            final_move.append("king");
            cout<<"King"<<endl;
            break;
        case 'Q':
            final_move.append("queen");        
            cout<<"Queen"<<endl;
            break;
        case 'R':
            final_move.append("rook");
            cout<<"Rook"<<endl;
            break;
        case 'B':
            final_move.append("bishop");
            cout<<"Bishop"<<endl;
            break;
        case 'N':
            final_move.append("knight");
            cout<<"Knight"<<endl;
            break;
        default:
            cout<<"She put no choice"<<endl;
            break;
        }
}
void pgnParserRow(char y) {
    switch (y) {
        case 'a':
           final_move.append("1");
            cout<<"1"<<endl;
            break;
        case 'b':
            final_move.append("2");        
            cout<<"2"<<endl;
            break;
        case 'c':
            final_move.append("3");
            cout<<"3"<<endl;
            break;
        case 'd':
            final_move.append("4");
            cout<<"4"<<endl;
            break;
        case 'e':
            final_move.append("5");
            cout<<"5"<<endl;
            break;
        case 'f':
            final_move.append("6");
            cout<<"6"<<endl;
            break;
        case 'g':
            final_move.append("7");
            cout<<"7"<<endl;
            break;
        case 'h':
            final_move.append("8");
            cout<<"8"<<endl;
            break;
        default:
            cout<<"She put no choice"<<endl;
            break;
        }
}
void pgnParserColumn(char z) {
    switch (z) {
        case '1':
            final_move.append("1");
            cout<<"1"<<endl;
            break;
        case '2':
            final_move.append("2");        
            cout<<"2"<<endl;
            break;
        case '3':
            final_move.append("3");
            cout<<"3"<<endl;
            break;
        case '4':
            final_move.append("4");
            cout<<"4"<<endl;
            break;
        case '5':
            final_move.append("5");
            cout<<"5"<<endl;
            break;
        case '6':
            final_move.append("6");
            cout<<"6"<<endl;
            break;
        case '7':
            final_move.append("7");
            cout<<"7"<<endl;
            break;
        case '8':
            final_move.append("8");
            cout<<"8"<<endl;
            break;
        default:
            cout<<"She put no choice"<<endl;
            break;
        }
        /*got to next line and */
}
void pgnParserCheck(char w) {
    switch (w) {
        case '+':
            final_move.append("1");
            cout<<"Check"<<endl;
            break;
        case '#':
            final_move.append("2");        
            cout<<"CheckMATE"<<endl;
            break;
        default:
            cout<<"She put no choice"<<endl;
            break;
        }
        /*got to next line and */
}
