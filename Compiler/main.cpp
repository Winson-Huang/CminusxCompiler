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
ifstream srcf;  //文件句柄
ofstream of;
int bufpos = 1; //读取指针
int bufsize = 0;    //缓冲区实际长度

int main()
{
    Strtsp(); //扫描阶段的准备工作
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
    //循环取记号!(srcf.eof()&& bufpos>bufsize)
//    while(nexttoken.type!=TokenType::ENDFILE){
//        GetNextToken();
//    }
    //关闭文件
//    测试语法分析

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
