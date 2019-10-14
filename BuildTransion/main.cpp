/**生成转换表**/
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

enum class State:int
{
    ENDFILE, ERROR, COMMENT,
    /**专用符号**/
    PLUS, MINUS, MULTI, DIVI, LT, LTE, MT, MTE, EQ, NEQ, ASSIGN, SEMI, COMMA, LPAREN, RPAREN, LSB, RSB, LBR, RBR,
                            /**< <= > >= == != = ; , ( ) [ ] { }**/
    /**其他**/
    ID, NUM,
    /**非接受**/
    INIT, NEQ0, COMMENT0, COMMENT1
};
const int height = 28, width = 20;
string Statestring[height] =
{
    "ENDFILE", "ERROR", "COMMENT",
    /**专用符号**/
    "PLUS", "MINUS", "MULTI", "DIVI", "LT", "LTE", "MT", "MTE", "EQ", "NEQ", "ASSIGN", "SEMI", "COMMA", "LPAREN", "RPAREN", "LSB", "RSB", "LBR", "RBR",
                            /**< <= > >= == != = ; ", "( ) [ ] { }**/
    /**其他**/
    "ID", "NUM",
    /**非接受**/
    "INIT", "NEQ0", "COMMENT0", "COMMENT1"
};
enum class AlphaBeta:int
{
    LETTER, DIGIT,
    PLUS, MINUS, MULTI, DIVI, LT, MT, ASSIGN,
    EXCLM, SEMI, COMMA,
    LPAREN, RPAREN, LSB, RSB, LBR, RBR,
    WHITESAPCE, ELSECHR
};
string Alphastring[width]=
{
    "LETTER", "DIGIT",
    "PLUS", "MINUS", "MULTI", "DIVI", "LT", "MT", "ASSIGN",
    "EXCLM", "SEMI", "COMMA",
    "LPAREN", "RPAREN", "LSB", "RSB", "LBR", "RBR",
    "WHITESAPCE", "ELSECHR"
};
int main()
{
    State transion[28][20];
    for(int i = 0; i< 28; ++i)
    {
        for(int j = 0;j<20;++j)
        {
            transion[i][j]= State::ERROR;
        }
    }
    for(int i=0; i<width;++i)
    {
        transion[int(State::ENDFILE)][i] = State::ENDFILE;
    }
//    for(int i=AlphaBeta::LETTER; i<AlphaBeta::ELSECHR;++i)
//    {
//        transion[State::ERROR][i] = State::ERROR;
//    }
    //注释不用处理
    //PLUS, MINUS, MULTI不用处理
    transion[int(State::DIVI)][int(AlphaBeta::MULTI)] = State::COMMENT0;
    transion[int(State::LT)][int(AlphaBeta::ASSIGN)] = State::LTE;//LTE不用处理
    transion[int(State::MT)][int(AlphaBeta::ASSIGN)] = State::MTE;//MTE不用处理
    //EQ, NEQ不用处理
    transion[int(State::ASSIGN)][int(AlphaBeta::ASSIGN)] = State::EQ;
    //其他专用符号不用处理
    transion[int(State::ID)][int(AlphaBeta::LETTER)] = State::ID;
    transion[int(State::NUM)][int(AlphaBeta::DIGIT)] = State::NUM;
    //INIT是最特殊的
    transion[int(State::INIT)][int(AlphaBeta::LETTER)] = State::ID;
    transion[int(State::INIT)][int(AlphaBeta::DIGIT)] = State::NUM;
    transion[int(State::INIT)][int(AlphaBeta::PLUS)] = State::PLUS;
    transion[int(State::INIT)][int(AlphaBeta::MINUS)] = State::MINUS;
    transion[int(State::INIT)][int(AlphaBeta::MULTI)] = State::MULTI;
    transion[int(State::INIT)][int(AlphaBeta::DIVI)] = State::DIVI;
    transion[int(State::INIT)][int(AlphaBeta::LT)] = State::LT;
    transion[int(State::INIT)][int(AlphaBeta::MT)] = State::MT;
    transion[int(State::INIT)][int(AlphaBeta::ASSIGN)] = State::ASSIGN;
    transion[int(State::INIT)][int(AlphaBeta::EXCLM)] = State::NEQ0;
    transion[int(State::INIT)][int(AlphaBeta::SEMI)] = State::SEMI;
    transion[int(State::INIT)][int(AlphaBeta::COMMA)] = State::COMMA;
    transion[int(State::INIT)][int(AlphaBeta::LPAREN)] = State::LPAREN;
    transion[int(State::INIT)][int(AlphaBeta::RPAREN)] = State::RPAREN;
    transion[int(State::INIT)][int(AlphaBeta::LSB)] = State::LSB;
    transion[int(State::INIT)][int(AlphaBeta::RSB)] = State::RSB;
    transion[int(State::INIT)][int(AlphaBeta::LBR)] = State::LBR;
    transion[int(State::INIT)][int(AlphaBeta::RBR)] = State::RBR;
    transion[int(State::INIT)][int(AlphaBeta::WHITESAPCE)] = State::INIT;

    transion[int(State::NEQ0)][int(AlphaBeta::ASSIGN)] = State::NEQ;
    //处理注释
    for(int i=0; i<width;++i)
    {
        transion[int(State::COMMENT0)][i] = State::COMMENT0;
    }
    transion[int(State::COMMENT0)][int(AlphaBeta::MULTI)] = State::COMMENT1;
    for(int i=0; i<width;++i)
    {
        transion[int(State::COMMENT1)][i] = State::COMMENT0;
    }
    transion[int(State::COMMENT1)][int(AlphaBeta::MULTI)] = State::COMMENT1;
    transion[int(State::COMMENT1)][int(AlphaBeta::DIVI)] = State::COMMENT;
    //处理完毕

    ofstream outcsv,outdata;
    outcsv.open("transion.csv", ios::out);
    outdata.open("transiondata.txt", ios::out);
    for(int i = int(State::ENDFILE); i<height; ++i)
    {
        for(int j = int(AlphaBeta::LETTER); j<width; ++j)
        {
            outdata<<int(transion[i][j])<<" ";
            outcsv<<Statestring[int(transion[i][j])];
            if(j!=width-1) outcsv<<",";
            else outcsv<<"\n";
        }
    }

    outcsv.close();


    return 0;
}
