#ifndef GLOBAL_H_INCLUDED
#define GLOBAL_H_INCLUDED
#include <string>
using namespace std;
//自定义类区
/**************************************************/
/***********   用于扫描器的数据结构   ************/
/**************************************************/
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
/***********   用于语法树的数据结构   ************/
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
int const MAXCHILDREN = 3; //最多有三个子女

class vdclrAttr{
public:
    DataType datatype; //变量声明类型
    string name; //变量名字符串
    string arrlen; //数组长度字面值
};
class fdclrAttr{
public:
    DataType datatype; //函数声明类型
    string name; //函数名
};
class paramAttr{
public:
    DataType datatype; //参数类型
    string name; //参数名
    bool isary; //是否是数组
};

class TreeNode{
public:
    TreeNode* child[MAXCHILDREN]; //子女结点
    TreeNode* sibling; //兄弟结点
    int lineno;//行号
    NodeKind nodekind; //结点类型
    union {
        Dclrkind dclr; Stmtkind stmt; Factorkind factor; Spexpkind spexp;
        bool rootkind;//是否根分类,比如Param就没有二级分类
    } kind;//这个语法并没有为联合体类型命名,直接声明了变量
    union Attr{
        vdclrAttr vdclrattr;
        fdclrAttr fdclrattr;
        paramAttr paramattr;
        string name;//var,call,constnum
        int val;//expression, constnum
        TokenType op;//记号类型?除了标识符,都可以用这个存
        Attr(){}
        ~Attr(){}
    } attr; //属性
	//Exptype;
};

//变量区
//main中定义
extern ifstream srcf;
extern ofstream of;
extern int bufpos;
extern int bufsize;
//scanner中定义
extern int lineord;
extern string infilename;
extern string outfilename;
extern string syntaxfilename;
extern Token nexttoken;    //下一个记号
extern int lineord; //当前记号的行号
//parser中定义
extern TreeNode* syntaxtree;//语法树根节点, 由于语法树是一个声明的单链表, 所以实际上是一个头指针.


#endif // GLOBAL_H_INCLUDED
