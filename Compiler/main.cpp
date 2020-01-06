/******************************/
/**scanner for cminusx       **/
/**author: Wencheng Huang    **/
/******************************/
#include <iostream>
#include <fstream>
#include <string>
#include "global.h"
#include "tools.h"
#include "scanner.h"
using namespace std;
ifstream srcf;  //�ļ����
ofstream of;
int bufpos = 1; //��ȡָ��
int bufsize = 0;    //������ʵ�ʳ���

int main()
{
    Strtscan();
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
    while(!(srcf.eof()&& bufpos>bufsize) ){
        GetNextToken();
    }

    of.close();
    srcf.close();
    Endscan();

    return 0;
}
