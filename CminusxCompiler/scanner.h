#ifndef SCANNER_H_INCLUDED
#define SCANNER_H_INCLUDED
#include <string>
using namespace std;

//������
extern ifstream srcf;
extern ofstream of;
extern string infilename;
extern string outfilename;
extern int bufpos;
extern int bufsize;

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
//������
void Welcome();
void Summary();
void GetFileName();
char GetNextChar();
void GetNextToken();
void GuildTransion();
AlphaBeta ChrToAlpBt(char nxtchr);
TokenType AcptToTknTp(State nowstate, string tknval);   //��һ���ַ����͵�ǰ��״̬�õ��Ǻŵ�״̬
int BinSearResvd(string tknval);
void PrintToken();   //�����ɵļǺŴ�ӡ����
//void BackTrace();    //����ȡָ���Ƶ���һ���ַ�
void PrintElseCharError();   //�����һ���ַ���ELSECHAR��ֱ�ӽ�δ���ɵļǺ���������������ַ����
void PrintCharPosError();    //��һ���ַ�����ELSECHAR���п������¸��Ǻŵ���λ

#endif // SCANNER_H_INCLUDED
