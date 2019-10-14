/******************************/
/**scanner for cminusx       **/
/**author: Wencheng Huang    **/
/******************************/
#include <iostream>
#include <fstream>
#include <string>
#include "scanner.h"
using namespace std;


int main()
{
    Welcome();
    GetFileName();
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
    Summary();

    return 0;
}
