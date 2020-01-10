//该文件是scanner对外的接口，主要给主函数以及语法分析程序使用,scanner.cpp内部的函数不在此声明
#ifndef SCANNER_H_INCLUDED
#define SCANNER_H_INCLUDED
//Strtscan函数必须被调用因为在该函数中构建转换表
void Strtsp();
//Endscan函数仅在展示扫描程序时被调用，由于整个程序的扫描伴随着语法树的构建，因此语法分析与扫描基本同时结束
void Endsp();

void GetNextToken();


#endif // SCANNER_H_INCLUDED
