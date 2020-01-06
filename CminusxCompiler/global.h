#ifndef GLOBAL_H_INCLUDED
#define GLOBAL_H_INCLUDED
#include <string>
using namespace std;

//�Զ�������
enum class TokenType    //�Ǻ�����
{
    ENDFILE, ERROR,
    /**������**/
    ELSE, IF, INT, RETURN, VOID, WHILE,
    /**ר�÷���**/
    PLUS, MINUS, MULTI, DIVI, LT, LTE, MT, MTE, EQ, NEQ, ASSIGN, SEMI, COMMA, LPAREN, RPAREN, LSB, RSB, LBR, RBR,
                            /**< <= > >= == != = ; , ( ) [ ] { }**/
    /**����**/
    ID, NUM
};
enum class State    //״̬
{
    ENDFILE, ERROR, COMMENT,
    /**ר�÷���**/
    PLUS, MINUS, MULTI, DIVI, LT, LTE, MT, MTE, EQ, NEQ, ASSIGN, SEMI, COMMA, LPAREN, RPAREN, LSB, RSB, LBR, RBR,
                            /**< <= > >= == != = ; , ( ) [ ] { }**/
    /**����**/
    ID, NUM,
    /**�ǽ���**/
    INIT, NEQ0, COMMENT0, COMMENT1
};
enum class StateType    //״̬����
{
    ACCPT, UNACCPT, ERROR
};
enum class AlphaBeta    //��ĸ����
{
    LETTER, DIGIT,
    PLUS, MINUS, MULTI, DIVI, LT, MT, ASSIGN,
    EXCLM, SEMI, COMMA,
    LPAREN, RPAREN, LSB, RSB, LBR, RBR,
    WHITESAPCE, ELSECHR
};
class Token     //�Ǻ�
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
/***********   �����﷨�������﷨��   ************/
/**************************************************/


//������
//main�ж���
extern ifstream srcf;
extern ofstream of;
extern int bufpos;
extern int bufsize;
//scanner�ж���
extern string infilename;
extern string outfilename;
extern Token nexttoken;    //��һ���Ǻ�
//parser�ж���

#endif // GLOBAL_H_INCLUDED
