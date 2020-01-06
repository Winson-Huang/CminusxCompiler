#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

#include "global.h"
#include "tools.h"
#include "scanner.h"

using namespace std;
//������
string infilename;
string outfilename;
Token nexttoken;    //��һ���Ǻ�
//�ڲ�
int lineord=0;  //�к�
int const MAX = 256;    //��������󳤶�
char linebuf[MAX];  //Դ�ļ��л���
char nxtChr;    //��������ַ�
const int height=28, width=20; //ת�����С
State transion[height][width]; //ת����
StateType sttype[height];   //���ֽ���״̬���ǽ���״̬�� ����״̬����ֵ����
const int reWordsSize = 6;  const int WordmaxSize = 6;
string reservedWords[reWordsSize] = {"else", "if", "int", "return", "void", "while"};

//������
static void BuildTransion()    //���ļ�����ת����
{
    //����ת����
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
    //��������״̬
    /**
        ENDFILE, ERROR, COMMENT,
    ר�÷���
        PLUS, MINUS, MULTI, DIVI, LT, LTE, MT, MTE, EQ, NEQ, ASSIGN, SEMI, COMMA, LPAREN, RPAREN, LSB, RSB, LBR, RBR,
                            < <= > >= == != = ; , ( ) [ ] { }
    ����
    ID, NUM,
    �ǽ���
    INIT, NEQ0, COMMENT0, COMMENT1
    **/
    for(int i= 0;i<height; ++i) //��ȫ����Ϊ����״̬
    {
        sttype[i] = StateType::ACCPT;
    }
    sttype[int(State::ERROR)] = StateType::ERROR;   //����״̬
    for(int i= int(State::INIT);i<= int(State::COMMENT1); i++)  //�ǽ���״̬
    {
        sttype[i] = StateType::UNACCPT;
    }
}

static void GetFileName()    //��ȡ�ļ���
{
    char option;
    cout<<"����yɨ���ļ�������ɨ��������";
    cin>>option;
    if(option == 'y')
    {
        cout<<"�������������·���ͺ�׺��.cmnsx��Դ�����ļ���:";
        cin>>infilename;
        cout<<"�������������·����ɨ����Ϣ�ļ���:";
        cin>>outfilename;
    }
    else
    {
        infilename = ".\\testsrc\\sample.cmnsx";
        outfilename = ".\\testsrc\\scanned_sample.txt";
    }
}

void Strtscan()  //��ӭ��Ϣ��׼������
{
    string welcomeText = "scanner for cminusx\n version: 0.1 \nɨ�����ʼ...\n";
    cout<<welcomeText;
    BuildTransion();
    GetFileName();
}

void Endscan()  //������Ϣ
{
    cout<<"ɨ��������.\n";
}

static char GetNextChar() //�õ���һ���ַ�
{
    if(bufpos>bufsize)//�������Ѷ�ȡ���,��Ҫ����
    {
        lineord++;  //�кż�һ
        srcf.getline(linebuf, MAX); //linebuf��û�л��з�,ĩβӦ����\0
        bufpos = 0;
        bufsize = strlen(linebuf);//ȷ��������ʵ�ʴ�С
        cout<<lineord<<": "<<linebuf<<endl;
        of<<lineord<<": "<<linebuf<<endl;
        if(bufsize==0)  //˵���ǿ��У����ֱ�ӷ��أ��᷵��'\0'
        {
            bufpos++;
            return '\n';
        }
        else return linebuf[bufpos++];
    }
    else if(bufpos==bufsize)    //������β����һ���ַ�����Ϊ'\n'
    {
        bufpos++;
        return '\n';
    }
    else    //ֱ�Ӷ�ȡ��һ���ַ�
    {
        return linebuf[bufpos++];
    }
}

static AlphaBeta ChrToAlpBt(char nxtchr)   //����ʼ�ַ�ת��Ϊ��ĸ��ö������
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

    if( (ascofNc >=int('a')&&ascofNc<=int('z')) || (ascofNc >=int('A')&& ascofNc <=int('Z')) )  //��ĸ
    {
        nxtAlphBt = AlphaBeta::LETTER;
    }
    else if( (ascofNc>=int('0')) && (ascofNc<=int('9')) ) //����
    {
        nxtAlphBt = AlphaBeta::DIGIT;
    }
    else if( (ascofNc == int(' ')) ||(ascofNc == int('\n')) ||(ascofNc == int('\t')) )    //�հ׸�
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

static int BinSearResvd(string tknval) //�����ַ������ر������б���±�, ����������оͷ���-1
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

static TokenType AcptToTknTp(State nowstate, string tknval)   //��һ���ַ�����һ������״̬�õ��Ǻŵ�����
{
    TokenType re = TokenType::ERROR;
    //ֻ�迼�ǽ���״̬,�۲�State��TokenType
    if(nowstate == State::ID)   //�������״̬�Ǳ�ʶ��,��Ҫ�ж��Ƿ�Ϊ������
    {
        if(tknval.size()<= WordmaxSize)
        {
            int index = BinSearResvd(tknval);
            if(index!=-1){
                return TokenType(int(TokenType::ELSE)+index);   //����Ǳ�����,�ͷ�����Ӧ�ļǺ�����
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

static void PrintToken()   //�����ɵļǺŴ�ӡ����
{
    of<<"\t"<<lineord<<": ";
    cout<<"\t"<<lineord<<": ";
    /**������**/
    if((nexttoken.type >= TokenType::ELSE) && (nexttoken.type<= TokenType::WHILE) )
    {
        of<<"reserved word--"<<nexttoken.value<<endl;
        cout<<"reserved word--"<<nexttoken.value<<endl;
    }
    /**ר�÷���**/
    else if((nexttoken.type >= TokenType::PLUS) && (nexttoken.type<= TokenType::RBR))
    {
        of<<nexttoken.value<<endl;
        cout<<nexttoken.value<<endl;
    }
    /**����**/
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

static void PrintElseCharError()   //�����һ���ַ���ELSECHAR��ֱ�ӽ�δ���ɵļǺ���������������ַ����
{
    string halftoken="";
    if(nexttoken.value!="")
    {
        halftoken = "unfinished token--"+nexttoken.value+" ";
    }
    of<<"\t"<<lineord<<": "<<halftoken<<"errorchar: "<<nxtChr<<endl;
    cout<<"\t"<<lineord<<": "<<halftoken<<"errorchar: "<<nxtChr<<endl;
}

static void PrintCharPosError()    //��һ���ַ�����ELSECHAR���п������¸��Ǻŵ���λ
{
    string halftoken="";
    if(nexttoken.value!="") //�÷�֧Ӧ�ñض������
    {
        halftoken = "unfinished token--"+nexttoken.value+" ";
    }
    of<<"\t"<<lineord<<": "<<halftoken<<endl;
    cout<<"\t"<<lineord<<": "<<halftoken<<endl;
}

void GetNextToken() //�õ���һ���Ǻ�
{
    //��ʼ��
    AlphaBeta nxtABt = AlphaBeta::ELSECHR;
    State now = State::INIT;
    State next = now;
    while( !(srcf.eof()&&(bufpos>bufsize)) )    //ֻ�ж���eof���һ���������Ż����ѭ��
    {
        nxtChr = GetNextChar(); //��ȡ��һ���ַ�
        nxtABt = ChrToAlpBt(nxtChr);    //ת��Ϊ��ĸ���е���ĸ
        next = transion[int(now)][int(nxtABt)];//������һ��״̬
        if( (next!=State::ENDFILE)&&(next!=State::ERROR) )    //���Ǵ���״̬�� ����ת��
        {
            if(nxtABt!=AlphaBeta::WHITESAPCE) nexttoken.value += nxtChr;//�����ַ�
            now = next; //״̬ת��
        }
        else                                                  //����Ǵ���״̬,��Ҫ�ж���һ��״̬
        {
            if(sttype[int(now)]==StateType::ACCPT)  //��һ��״̬�ǽ���״̬�� �������ɼǺ�,��Ҫ����
            {
                //����
                bufpos--;
                //�����ע�ͣ�����ԣ����¿�ʼ
                if(now == State::COMMENT)
                {
                    now = State::INIT;  //����״̬
                    nexttoken.Reset();  //���üǺ�
                    continue;   //���½��е���
                }//�������ע�ͣ� �������ɼǺ�
                nexttoken.type = AcptToTknTp(now, nexttoken.value);               //ͨ������״̬�ͼǺŴ�ֵȷ���Ǻ�����
                PrintToken();//��ӡ�Ǻ�
                nexttoken.Reset();  //���üǺ�
                return ;
            }
            else    //��һ��״̬���ǽ���״̬, ����״̬, ���¿�ʼ
            {
                if(nxtABt == AlphaBeta::ELSECHR)    //�������ַ�,����Ҫ����,ȡ��һ���ַ�
                {
                    PrintElseCharError();   //���ɴ�����Ϣ
                }
                else    //�������¸��Ǻ���λ,����ɾ��,��Ҫ����
                {
                    bufpos--;   //����
                    PrintCharPosError();    //���ɴ�����Ϣ(���δʶ����ɵļǺ�)
                }
                //��ͷ��ʼ
                nexttoken.Reset();  //���üǺ�
                now = State::INIT;  //����״̬
                continue;
            }
        }
    }
    //��������ļ���β
    now = State::ENDFILE;
    nexttoken.type = TokenType::ENDFILE;
    return;
}
