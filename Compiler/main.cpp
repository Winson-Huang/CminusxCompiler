/******************************/
/**cminusx                   **/
/**author: Wencheng Huang    **/
/******************************/
#include <iostream>
#include <fstream>
#include <string>
#include "global.h"
#include "tools.h"
#include "scanner.h"
#include "parser.h"
using namespace std;
ifstream srcf;  //�ļ����
ofstream of;
int bufpos = 1; //��ȡָ��
int bufsize = 0;    //������ʵ�ʳ���

int main()
{
    Strtsp(); //ɨ��׶ε�׼������
    srcf.open(&(infilename[0]), ios::in);//���ļ������ļ�
    of.open(&(outfilename[0]), ios::out);
    if(!srcf.is_open()){
        cout<<"Դ�����ļ���ʧ��"<<endl;
        return 0;
    }
    else if(!of.is_open()){
        cout<<"ɨ����Ϣ�ļ���ʧ��"<<endl;
        return 0;
    }
    //ѭ��ȡ�Ǻ�!(srcf.eof()&& bufpos>bufsize)
//    while(nexttoken.type!=TokenType::ENDFILE){
//        GetNextToken();
//    }
    //�ر��ļ�
//    �����﷨����

    TreeNode* syntaxtree = Parse();
    of.close();
    srcf.close();
    of.open(&(syntaxfilename[0]), ios::out);
    cout<<"syntaxtree:\n";
    PrintTree(syntaxtree,0);
    of.close();

    Endsp();

    return 0;
}
