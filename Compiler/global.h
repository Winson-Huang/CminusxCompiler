#ifndef GLOBAL_H_INCLUDED
#define GLOBAL_H_INCLUDED
#include <string>
using namespace std;

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

/**************************************************/
/***********   用于语法分析的语法树   ************/
/**************************************************/


//变量区
//main中定义
extern ifstream srcf;
extern ofstream of;
extern int bufpos;
extern int bufsize;
//scanner中定义
extern string infilename;
extern string outfilename;
extern Token nexttoken;    //下一个记号
//parser中定义

#endif // GLOBAL_H_INCLUDED
