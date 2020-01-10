#ifndef GLOBAL_H_INCLUDED
#define GLOBAL_H_INCLUDED
#include <string>
using namespace std;
//�Զ�������
/**************************************************/
/***********   ����ɨ���������ݽṹ   ************/
/**************************************************/
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
/***********   �����﷨�������ݽṹ   ************/
/**************************************************/
enum class NodeKind{
    Declaration, Statement, Param, Spexp, Factor
};
enum class Dclrkind{
    var_dclr, fun_dclr
};
enum class Stmtkind{
    expression_stmt, compound_stmt, selection_stmt, iteration_stmt, return_stmt
};
enum class Factorkind{
    var, expression, call, constnum
};
enum class Spexpkind{
    relexp, addexp, terexp
};
enum class DataType{INT, VOID};
int const MAXCHILDREN = 3; //�����������Ů

class vdclrAttr{
public:
    DataType datatype; //������������
    string name; //�������ַ���
    string arrlen; //���鳤������ֵ
};
class fdclrAttr{
public:
    DataType datatype; //������������
    string name; //������
};
class paramAttr{
public:
    DataType datatype; //��������
    string name; //������
    bool isary; //�Ƿ�������
};

class TreeNode{
public:
    TreeNode* child[MAXCHILDREN]; //��Ů���
    TreeNode* sibling; //�ֵܽ��
    int lineno;//�к�
    NodeKind nodekind; //�������
    union {
        Dclrkind dclr; Stmtkind stmt; Factorkind factor; Spexpkind spexp;
        bool rootkind;//�Ƿ������,����Param��û�ж�������
    } kind;//����﷨��û��Ϊ��������������,ֱ�������˱���
    union Attr{
        vdclrAttr vdclrattr;
        fdclrAttr fdclrattr;
        paramAttr paramattr;
        string name;//var,call,constnum
        int val;//expression, constnum
        TokenType op;//�Ǻ�����?���˱�ʶ��,�������������
        Attr(){}
        ~Attr(){}
    } attr; //����
	//Exptype;
};

//������
//main�ж���
extern ifstream srcf;
extern ofstream of;
extern int bufpos;
extern int bufsize;
//scanner�ж���
extern int lineord;
extern string infilename;
extern string outfilename;
extern string syntaxfilename;
extern Token nexttoken;    //��һ���Ǻ�
extern int lineord; //��ǰ�Ǻŵ��к�
//parser�ж���
extern TreeNode* syntaxtree;//�﷨�����ڵ�, �����﷨����һ�������ĵ�����, ����ʵ������һ��ͷָ��.


#endif // GLOBAL_H_INCLUDED
