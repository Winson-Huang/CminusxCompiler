//该文件是parser对外的接口
#ifndef PARSER_H_INCLUDED
#define PARSER_H_INCLUDED
TreeNode* Parse();
//打印语法树
void PrintTree(TreeNode* tree, int layer);

#endif // PARSER_H_INCLUDED
