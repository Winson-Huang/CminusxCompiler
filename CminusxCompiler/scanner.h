#ifndef SCANNER_H_INCLUDED
#define SCANNER_H_INCLUDED
#include <string>
using namespace std;

//变量区
extern ifstream srcf;
extern ofstream of;
extern string infilename;
extern string outfilename;
extern int bufpos;
extern int bufsize;

//自定义类区
enum class TokenType    //记号类型
{
    ENDFILE, ERROR,
    /**保留字**/
    ELSE, IF, INT, RETURN, VOID, WHILE,
    /**专用符号**/
    PLUS, MINUS, MULTI, DIVI, LT, LTE, MT, MTE, EQ, NEQ, ASSIGN, SEMI, COMMA, LPAREN, RPAREN, LSB, RSB, LBR, RBR,
                            /**< <= > >= == != = ; , ( ) [ ] { }**/
    /**其他**/
    ID, NUM
};
enum class State    //状态
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
enum class StateType    //状态类型
{
    ACCPT, UNACCPT, ERROR
};
enum class AlphaBeta    //字母类型
{
    LETTER, DIGIT,
    PLUS, MINUS, MULTI, DIVI, LT, MT, ASSIGN,
    EXCLM, SEMI, COMMA,
    LPAREN, RPAREN, LSB, RSB, LBR, RBR,
    WHITESAPCE, ELSECHR
};
class Token     //记号
{
public:
    TokenType type;
    string value;
    Token()
    {
        type = TokenType::ERROR;
        value = "";
    }
    void Reset()
    {
        type = TokenType::ERROR;
        value = "";
    }
};
//函数区
void Welcome();
void Summary();
void GetFileName();
char GetNextChar();
void GetNextToken();
void GuildTransion();
AlphaBeta ChrToAlpBt(char nxtchr);
TokenType AcptToTknTp(State nowstate, string tknval);   //从一个字符串和当前的状态得到记号的状态
int BinSearResvd(string tknval);
void PrintToken();   //将生成的记号打印出来
//void BackTrace();    //将读取指针移到上一个字符
void PrintElseCharError();   //如果下一个字符是ELSECHAR，直接将未生成的记号输出，并将错误字符输出
void PrintCharPosError();    //下一个字符不是ELSECHAR，有可能是下个记号的首位

#endif // SCANNER_H_INCLUDED
