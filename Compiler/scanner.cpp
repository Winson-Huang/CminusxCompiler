#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

#include "global.h"
#include "tools.h"
#include "scanner.h"

using namespace std;
//变量区
string infilename;
string outfilename;
Token nexttoken;    //下一个记号
//内部
int lineord=0;  //行号
int const MAX = 256;    //缓冲区最大长度
char linebuf[MAX];  //源文件行缓冲
char nxtChr;    //待处理的字符
const int height=28, width=20; //转换表大小
State transion[height][width]; //转换表
StateType sttype[height];   //区分接受状态，非接受状态， 错误状态的三值数组
const int reWordsSize = 6;  const int WordmaxSize = 6;
string reservedWords[reWordsSize] = {"else", "if", "int", "return", "void", "while"};

//函数区
static void BuildTransion()    //从文件构造转换表
{
    //构造转换表
    string transfname = "..\\BuildTransion\\transiondata.txt";
    ifstream transionf;
    transionf.open(transfname, ios::in);
    int temp;
    for(int i = 0; i<height; ++i)
    {
        for(int j = 0;j<width; ++j)
        {
            transionf>>temp;
            transion[i][j] = State(temp);
        }
    }
    transionf.close();
    //区分三类状态
    /**
        ENDFILE, ERROR, COMMENT,
    专用符号
        PLUS, MINUS, MULTI, DIVI, LT, LTE, MT, MTE, EQ, NEQ, ASSIGN, SEMI, COMMA, LPAREN, RPAREN, LSB, RSB, LBR, RBR,
                            < <= > >= == != = ; , ( ) [ ] { }
    其他
    ID, NUM,
    非接受
    INIT, NEQ0, COMMENT0, COMMENT1
    **/
    for(int i= 0;i<height; ++i) //先全部设为接受状态
    {
        sttype[i] = StateType::ACCPT;
    }
    sttype[int(State::ERROR)] = StateType::ERROR;   //错误状态
    for(int i= int(State::INIT);i<= int(State::COMMENT1); i++)  //非接受状态
    {
        sttype[i] = StateType::UNACCPT;
    }
}

static void GetFileName()    //获取文件名
{
    char option;
    cout<<"输入y扫描文件，否则扫描样例：";
    cin>>option;
    if(option == 'y')
    {
        cout<<"请输入带有完整路径和后缀名.cmnsx的源程序文件名:";
        cin>>infilename;
        cout<<"请输入带有完整路径的扫描信息文件名:";
        cin>>outfilename;
    }
    else
    {
        infilename = ".\\testsrc\\sample.cmnsx";
        outfilename = ".\\testsrc\\scanned_sample.txt";
    }
}

void Strtscan()  //欢迎信息与准备工作
{
    string welcomeText = "scanner for cminusx\n version: 0.1 \n扫描程序开始...\n";
    cout<<welcomeText;
    BuildTransion();
    GetFileName();
}

void Endscan()  //结束信息
{
    cout<<"扫描程序结束.\n";
}

static char GetNextChar() //得到下一个字符
{
    if(bufpos>bufsize)//缓冲区已读取完毕,需要更新
    {
        lineord++;  //行号加一
        srcf.getline(linebuf, MAX); //linebuf中没有换行符,末尾应该是\0
        bufpos = 0;
        bufsize = strlen(linebuf);//确定缓冲区实际大小
        cout<<lineord<<": "<<linebuf<<endl;
        of<<lineord<<": "<<linebuf<<endl;
        if(bufsize==0)  //说明是空行，如果直接返回，会返回'\0'
        {
            bufpos++;
            return '\n';
        }
        else return linebuf[bufpos++];
    }
    else if(bufpos==bufsize)    //到达行尾，下一个字符设置为'\n'
    {
        bufpos++;
        return '\n';
    }
    else    //直接读取下一个字符
    {
        return linebuf[bufpos++];
    }
}

static AlphaBeta ChrToAlpBt(char nxtchr)   //将初始字符转化为字母表枚举类型
{
    /**
    LETTER, DIGIT,
    PLUS, MINUS, MULTI, DIVI, LT, MT, ASSIGN,
    EXCLM, SEMI, COMMA,
    LPAREN, RPAREN, LSB, RSB, LBR, RBR,
    WHITESAPCE, ELSECHR
    **/
    int ascofNc = int(nxtchr);
    AlphaBeta nxtAlphBt = AlphaBeta::ELSECHR;

    if( (ascofNc >=int('a')&&ascofNc<=int('z')) || (ascofNc >=int('A')&& ascofNc <=int('Z')) )  //字母
    {
        nxtAlphBt = AlphaBeta::LETTER;
    }
    else if( (ascofNc>=int('0')) && (ascofNc<=int('9')) ) //数字
    {
        nxtAlphBt = AlphaBeta::DIGIT;
    }
    else if( (ascofNc == int(' ')) ||(ascofNc == int('\n')) ||(ascofNc == int('\t')) )    //空白格
    {
        nxtAlphBt = AlphaBeta::WHITESAPCE;
    }
    else
    {
        switch (ascofNc)
            {
                case '+': nxtAlphBt = AlphaBeta::PLUS;break;
                case '-': nxtAlphBt = AlphaBeta::MINUS;break;
                case '*': nxtAlphBt = AlphaBeta::MULTI;break;
                case '/': nxtAlphBt = AlphaBeta::DIVI;break;
                case '<': nxtAlphBt = AlphaBeta::LT;break;
                case '>': nxtAlphBt = AlphaBeta::MT;break;
                case '=': nxtAlphBt = AlphaBeta::ASSIGN;break;
                case '!': nxtAlphBt = AlphaBeta::EXCLM;break;
                case ';': nxtAlphBt = AlphaBeta::SEMI;break;
                case ',': nxtAlphBt = AlphaBeta::COMMA;break;
                case '(': nxtAlphBt = AlphaBeta::LPAREN;break;
                case ')': nxtAlphBt = AlphaBeta::RPAREN;break;
                case '[': nxtAlphBt = AlphaBeta::LSB;break;
                case ']': nxtAlphBt = AlphaBeta::LSB;break;
                case '{': nxtAlphBt = AlphaBeta::LBR;break;
                case '}': nxtAlphBt = AlphaBeta::RBR;break;
                default: break;
            }
    }
    return nxtAlphBt;
}

static int BinSearResvd(string tknval) //根据字符串返回保留字列表的下标, 如果不在其中就返回-1
{
    int start = 0; int terminal = reWordsSize-1; int cen;
    while(start<=terminal)
    {
        cen = (start + terminal)/2;
        if(tknval==reservedWords[cen])
        {
            return cen;
        }
        else if(tknval<reservedWords[cen])
        {
            terminal = cen-1;
        }
        else
        {
            start = cen+1;
        }
    }
    return -1;
}

static TokenType AcptToTknTp(State nowstate, string tknval)   //从一个字符串和一个接受状态得到记号的类型
{
    TokenType re = TokenType::ERROR;
    //只需考虑接受状态,观察State和TokenType
    if(nowstate == State::ID)   //如果接受状态是标识符,需要判定是否为保留字
    {
        if(tknval.size()<= WordmaxSize)
        {
            int index = BinSearResvd(tknval);
            if(index!=-1){
                return TokenType(int(TokenType::ELSE)+index);   //如果是保留字,就返回相应的记号类型
            }
            else return TokenType::ID;
        }
        else return TokenType::ID;
    }
    else if(nowstate == State::NUM) return TokenType::NUM;
    else if( (nowstate>=State::PLUS) && (nowstate<= State::RBR))
    {
        return TokenType(int(nowstate)-int(State::PLUS)+int(TokenType::PLUS));
    }
    else return re;
}

static void PrintToken()   //将生成的记号打印出来
{
    of<<"\t"<<lineord<<": ";
    cout<<"\t"<<lineord<<": ";
    /**保留字**/
    if((nexttoken.type >= TokenType::ELSE) && (nexttoken.type<= TokenType::WHILE) )
    {
        of<<"reserved word--"<<nexttoken.value<<endl;
        cout<<"reserved word--"<<nexttoken.value<<endl;
    }
    /**专用符号**/
    else if((nexttoken.type >= TokenType::PLUS) && (nexttoken.type<= TokenType::RBR))
    {
        of<<nexttoken.value<<endl;
        cout<<nexttoken.value<<endl;
    }
    /**其他**/
    else if(nexttoken.type == TokenType::ID)
    {
        of<<"identifier, name="<<nexttoken.value<<endl;
        cout<<"identifier, name="<<nexttoken.value<<endl;
    }
    else if(nexttoken.type == TokenType::NUM)
    {
        of<<"number, value="<<nexttoken.value<<endl;
        cout<<"number, value="<<nexttoken.value<<endl;
    }
    else{
        of<<"else"<<endl;
        cout<<"else"<<endl;
    }
}

static void PrintElseCharError()   //如果下一个字符是ELSECHAR，直接将未生成的记号输出，并将错误字符输出
{
    string halftoken="";
    if(nexttoken.value!="")
    {
        halftoken = "unfinished token--"+nexttoken.value+" ";
    }
    of<<"\t"<<lineord<<": "<<halftoken<<"errorchar: "<<nxtChr<<endl;
    cout<<"\t"<<lineord<<": "<<halftoken<<"errorchar: "<<nxtChr<<endl;
}

static void PrintCharPosError()    //下一个字符不是ELSECHAR，有可能是下个记号的首位
{
    string halftoken="";
    if(nexttoken.value!="") //该分支应该必定会进入
    {
        halftoken = "unfinished token--"+nexttoken.value+" ";
    }
    of<<"\t"<<lineord<<": "<<halftoken<<endl;
    cout<<"\t"<<lineord<<": "<<halftoken<<endl;
}

void GetNextToken() //得到下一个记号
{
    //初始化
    AlphaBeta nxtABt = AlphaBeta::ELSECHR;
    State now = State::INIT;
    State next = now;
    while( !(srcf.eof()&&(bufpos>bufsize)) )    //只有读到eof并且缓冲区读完才会结束循环
    {
        nxtChr = GetNextChar(); //获取下一个字符
        nxtABt = ChrToAlpBt(nxtChr);    //转换为字母表中的字母
        next = transion[int(now)][int(nxtABt)];//生成下一个状态
        if( (next!=State::ENDFILE)&&(next!=State::ERROR) )    //不是错误状态， 继续转换
        {
            if(nxtABt!=AlphaBeta::WHITESAPCE) nexttoken.value += nxtChr;//保存字符
            now = next; //状态转换
        }
        else                                                  //如果是错误状态,需要判定上一个状态
        {
            if(sttype[int(now)]==StateType::ACCPT)  //上一个状态是接受状态， 可以生成记号,需要回溯
            {
                //回溯
                bufpos--;
                //如果是注释，则忽略，重新开始
                if(now == State::COMMENT)
                {
                    now = State::INIT;  //重置状态
                    nexttoken.Reset();  //重置记号
                    continue;   //重新进行迭代
                }//如果不是注释， 可以生成记号
                nexttoken.type = AcptToTknTp(now, nexttoken.value);               //通过接受状态和记号串值确定记号类型
                PrintToken();//打印记号
                nexttoken.Reset();  //重置记号
                return ;
            }
            else    //上一个状态不是接受状态, 重置状态, 重新开始
            {
                if(nxtABt == AlphaBeta::ELSECHR)    //是其他字符,不需要回溯,取下一个字符
                {
                    PrintElseCharError();   //生成错误信息
                }
                else    //可能是下个记号首位,不能删除,需要回溯
                {
                    bufpos--;   //回溯
                    PrintCharPosError();    //生成错误信息(输出未识别完成的记号)
                }
                //重头开始
                nexttoken.Reset();  //重置记号
                now = State::INIT;  //重置状态
                continue;
            }
        }
    }
    //如果到达文件结尾
    now = State::ENDFILE;
    nexttoken.type = TokenType::ENDFILE;
    return;
}
