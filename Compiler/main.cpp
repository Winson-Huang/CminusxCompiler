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
ifstream srcf;  //文件句柄
ofstream of;
int bufpos = 1; //读取指针
int bufsize = 0;    //缓冲区实际长度

int main()
{
    Strtscan();
    srcf.open(&(infilename[0]), ios::in);//以文件名打开文件
    of.open(&(outfilename[0]), ios::out);
    if(!srcf.is_open()){
        cout<<"源代码文件打开失败"<<endl;
        return 0;
    }
    else if(!of.is_open()){
        cout<<"扫描信息文件打开失败"<<endl;
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
