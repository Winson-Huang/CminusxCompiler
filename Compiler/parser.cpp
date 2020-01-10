#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

#include "global.h"
#include "tools.h"
#include "scanner.h"
#include "parser.h"

using namespace std;

TreeNode* syntaxtree = nullptr;
//函数声明
static void match(TokenType expected);
//五大类结点的空间分配
static TreeNode* Newdclr(Dclrkind dclrkind);
static TreeNode* Newparam();
static TreeNode* Newstmt(Stmtkind stmtkind);
static TreeNode* Newfactor(Factorkind factorkind);
static TreeNode* Newspexp();

//各类结点的构造与归并
static TreeNode* Dclr_seq();
static TreeNode* Dclr();
static TreeNode* Var_dclr(DataType tmpdatatype, string tmpname);
static TreeNode* Fun_dclr(DataType tmpdatatype, string tmpname);

static TreeNode* Param_seq();
static TreeNode* Param();

static TreeNode* Compound_stmt();
static TreeNode* Local_dclr();
static TreeNode* Lvar_dclr();
static TreeNode* Stmt_seq();
static TreeNode* Stmt();
static TreeNode* Selection_stmt();
static TreeNode* Iteration_stmt();
static TreeNode* Return_stmt();
static TreeNode* Expression_stmt();

static TreeNode* Expression();
static TreeNode* Var_seq(TreeNode*& tmpvar);
static TreeNode* Var();
static TreeNode* Factor();//核心
static TreeNode* VarorCall();
static TreeNode* Arg_seq();//Exp的序列
static TreeNode* Constnum();

static TreeNode* Spexp(TreeNode* tmpvar);//三层识别过程
static TreeNode* Calexp(TreeNode* tmpvar);
static TreeNode* Terexp(TreeNode* tmpvar);

//匹配过程
static void match(TokenType expected)
{
    if (nexttoken.type == expected) GetNextToken(); //匹配成功,下一个记号
    else {//错误处理
//    syntaxError("unexpected token -> ");
//    printToken(token,tokenString);
//    fprintf(listing,"      ");
    }
    return;
}
//分配空间
static TreeNode* Newdclr(Dclrkind dclrkind) //为声明结点分配空间
{
    TreeNode* reptr = new TreeNode();//分配空间
    reptr->nodekind = NodeKind::Declaration;
    reptr->kind.dclr = dclrkind;
    reptr->lineno = lineord;//行号
    reptr->sibling = nullptr;
//有必要为每个孩子赋空值吗, 有!!!!
    for(int i=0;i<MAXCHILDREN;++i){
        reptr->child[i] = nullptr;
    }
    return reptr;
}
static TreeNode* Newparam() //为形参分配空间
{
    TreeNode* reptr = new TreeNode();//分配空间
    reptr->nodekind = NodeKind::Param;
    reptr->kind.rootkind = true;
    reptr->lineno = lineord;//行号
    reptr->sibling = nullptr;
    for(int i=0;i<MAXCHILDREN;++i){
        reptr->child[i] = nullptr;
    }
    return reptr;
}
static TreeNode* Newstmt(Stmtkind stmtkind)
{
    TreeNode* reptr = new TreeNode();//分配空间
    reptr->nodekind = NodeKind::Statement;//都是语句类型
    reptr->kind.stmt = stmtkind; //语句的子类型
    reptr->lineno = lineord;//行号
    reptr->sibling = nullptr;
    for(int i=0;i<MAXCHILDREN;++i){
        reptr->child[i] = nullptr;
    }
    return reptr;
}
static TreeNode* Newfactor(Factorkind factorkind)
{
    TreeNode* reptr = new TreeNode();//分配空间
    reptr->nodekind = NodeKind::Factor;//都是factor类型
    reptr->kind.factor = factorkind; //factor的子类型
    reptr->lineno = lineord;//行号
    reptr->sibling = nullptr;
    for(int i=0;i<MAXCHILDREN;++i){
        reptr->child[i] = nullptr;
    }
    return reptr;
}
static TreeNode* Newspexp()
{
    TreeNode* reptr = new TreeNode();//分配空间
    reptr->nodekind = NodeKind::Spexp;
    reptr->lineno = lineord;//行号
    reptr->sibling = nullptr;
    for(int i=0;i<MAXCHILDREN;++i){
        reptr->child[i] = nullptr;
    }
    return reptr;
}


//构造语法树
TreeNode* Parse()
{
    TreeNode * t;//语法树根节点
    GetNextToken();//获取第一个结点
    t = Dclr_seq();//构建第一个结点
    if (nexttoken.type!=TokenType::ENDFILE) cout<<"Code ends before file\n";//syntaxError("Code ends before file\n");
    return t;
}

static TreeNode* Dclr_seq()
{ //构造声明序列
    TreeNode* reptr = Dclr();
    TreeNode* tmpptr = reptr;//有可能为空,所以不能直接移动位置指针
    while( tmpptr != nullptr){//这个条件不确定可不可以//只要下个记号不是文件结束,就继续构建结点nexttoken.type != TokenType::ENDFILE
        tmpptr->sibling = Dclr();
        tmpptr = tmpptr->sibling;//序列指针移动
    }
    return reptr;
}
//构造结点
static TreeNode* Dclr() //两种声明结点
{
    TreeNode* reptr = nullptr; //需要返回的指针
    DataType tmpdatatype;
    string tmpname; //前两个记号
    if(nexttoken.type == TokenType::INT || nexttoken.type == TokenType::VOID){//识别数据类型,此时并不检查类型
        switch(nexttoken.type) //计算数据类型属性
        {
            case TokenType::INT : tmpdatatype = DataType::INT;break;
            case TokenType::VOID : tmpdatatype = DataType::VOID;break;
            default: break;//cout<<"数据类型出错\n";//出错处理
        }
        match(nexttoken.type);//下一个记号
    }
    else{//声明的类型不是INT也不是VOID,不符合声明的结构unexpected token
        if(nexttoken.type == TokenType::ENDFILE) return nullptr;
    }
    if(nexttoken.type == TokenType::ID){//识别标识符
        tmpname = nexttoken.value;
        match(nexttoken.type);
    }
    else{
        //语法错误,类型后边不是标识符unexpected token
    }
    //处理第三个token
    if(nexttoken.type == TokenType::SEMI || nexttoken.type == TokenType::LSB){//如果是分号或左方括是数组或普通变量声明
        reptr = Var_dclr(tmpdatatype, tmpname);
    }
    else if(nexttoken.type == TokenType::LPAREN){ //可能是函数声明
        reptr = Fun_dclr(tmpdatatype, tmpname);
    }
    else{
        //标识符后不是分号也不是[,不符合变量声明的结构
    }
    return reptr;
}

static TreeNode* Var_dclr(DataType tmpdatatype, string tmpname) //构造变量声明结点
{
    TreeNode* reptr = Newdclr(Dclrkind::var_dclr); //为变量声明结点分配空间
    reptr->attr.vdclrattr.datatype = tmpdatatype; //数据类型/返回值类型
    reptr->attr.vdclrattr.name = tmpname; //标识符名称/函数名
    //只有以下两种情况
    if(nexttoken.type == TokenType::SEMI){//分号,说明是单个变量
        match(nexttoken.type);//nexttoken一定是分号,消耗分号
        reptr->attr.vdclrattr.arrlen = "null"; //将单变量的数组长度字面值设为null(非法,长度必须>0??
    }
    else if(nexttoken.type == TokenType::LSB){//可能是数组
        match(nexttoken.type);//消耗左括号
        if(nexttoken.type == TokenType::NUM){
            reptr->attr.vdclrattr.arrlen = nexttoken.value; //此处value是string
            match(nexttoken.type);//nexttoken一定是数,消耗数
            match(TokenType::RSB);match(TokenType::SEMI);//消耗右方括号和分号,如果match中出错,说明不符合结构
        }
        else{
            //需要处理数组声明中长度不是数字的情况
        }
    }
    return reptr;//返回一个变量声明结点的地址
}

static TreeNode* Fun_dclr(DataType tmpdatatype, string tmpname) //构造函数声明结点
{
    TreeNode* reptr = Newdclr(Dclrkind::fun_dclr); //为函数声明结点分配空间
    reptr->attr.fdclrattr.datatype = tmpdatatype; //数据类型/返回值类型
    reptr->attr.fdclrattr.name = tmpname; //标识符名称/函数名
    //参数列表的处理,nexttoken中一定是圆括号
    match(nexttoken.type); //消耗圆括号
    reptr->child[0] = Param_seq();//构建形参列表
    match(TokenType::RPAREN);//消耗圆括号)
    //函数体的处理
    if(nexttoken.type == TokenType::LBR){//期望是函数体
        reptr->child[1] = Compound_stmt();//构造一个复合语句结点
    }
    else{
        //函数体缺少左括号
    }
    return reptr;
}
static TreeNode* Param_seq() //形参列表不识别左右括号
{
    DataType tmpdatatype;//形参类型
    TreeNode* reptr = nullptr;//暂时不知道形参列表是否为空
    switch(nexttoken.type) //计算参数类型属性
    {
        case TokenType::INT : tmpdatatype = DataType::INT;break;
        case TokenType::VOID : tmpdatatype = DataType::VOID;break;
        default: cout<<"形参类型出错\n"; return reptr;//出错处理//待补充
    }
    match(nexttoken.type);//消耗一个参数类型
    //处理形参列表是VOID的情况
    if(nexttoken.type == TokenType::RPAREN){//右括号,参数列表为空
        if(tmpdatatype!= DataType::VOID){
            cout<<"非VOID但无形参\n"; //这是对这种情况的错误处理
        }//若形参中只有一个VOID //参数列表为空,根本不需要分配空间
    }
//不为空的形参的链接
    else if(nexttoken.type == TokenType::ID){//参数列表不为空
        //处理第一个形参
        reptr = Newparam(); //为形参结点分配空间
        if(tmpdatatype == DataType::VOID){
            cout<<"形参类型出错\n";
        }
        reptr->attr.paramattr.datatype = tmpdatatype; //形参类型
        reptr->attr.paramattr.name = nexttoken.value; //标识符名称/函数名/形参名
        reptr->attr.paramattr.isary = false;
        match(nexttoken.type);//消耗标识符
        if(nexttoken.type == TokenType::LSB){ //形参可能是数组
            match(nexttoken.type);
            reptr->attr.paramattr.isary = true;//这一个形参是数组形参
            match(TokenType::RSB);//需要补充错误处理
        }
        //链接多个形参
        TreeNode* tmp = reptr;//此时tmp不空
        while(nexttoken.type == TokenType::COMMA){//是逗号,准备构造下一个形参
            match(nexttoken.type);
            tmp->sibling = Param();
            tmp = tmp->sibling;//移动序列的指针
        }
    }
    else{ //参数列表形式错误,第一个形参类型后不是右括号也不是标识符
    }
    return reptr;
}
static TreeNode* Param() //构造形参结点,此时nexttoken中是类型
{
    TreeNode* reptr = Newparam(); //为形参结点分配空间
    if(nexttoken.type == TokenType::INT){
        match(nexttoken.type);//类型合法,消耗类型
        reptr->attr.paramattr.datatype = DataType::INT;
        if(nexttoken.type == TokenType::ID){
            reptr->attr.paramattr.name = nexttoken.value; //标识符名称/函数名/形参名
            reptr->attr.paramattr.isary = false;
            match(nexttoken.type);//消耗标识符
            if(nexttoken.type == TokenType::LSB){ //形参可能是数组
                match(nexttoken.type);
                match(TokenType::RSB);
                reptr->attr.paramattr.isary = true;//这一个形参是数组形参
            }
        }
        else{
            //类型后不是标识符,错误处理
        }
    }
    else{ //形参类型不是INT
        cout<<"形参类型非法\n";
        delete reptr;//撤销空间
        reptr = nullptr;
    }
    return reptr;
}

static TreeNode* Compound_stmt() //构造复合语句结点
{
    TreeNode* reptr = Newstmt(Stmtkind::compound_stmt);
    match(TokenType::LBR);//消耗花括号
    reptr->child[0] = Local_dclr();//识别vardclr序列
    reptr->child[1] = Stmt_seq();//识别语句序列
    match(TokenType::RBR);//消耗右花括号
    return reptr;
}

static TreeNode* Local_dclr() //构造本地变量声明序列!!!!!
{
    TreeNode* reptr = Lvar_dclr();
    TreeNode* tmp = reptr;

    while(tmp != nullptr){//只有第一个结点成功生成才可以继续进行
        tmp->sibling = Lvar_dclr();//可能返回nullptr
        tmp = tmp->sibling;//移动链表指针
    }
    return reptr;
}
static TreeNode* Lvar_dclr()
{
    TreeNode* reptr = nullptr;
    DataType tmpdatatype;
    string tmpname;
    if(nexttoken.type == TokenType::INT){
        tmpdatatype = DataType::INT;
        match(nexttoken.type);//下一个记号
    }
    else{ //变量声明的类型不是INT关键字
        return reptr;//视为本地变量声明结束,返回,此时的nexttoken还是最近一次生成结点后的第一个token
    }
    if(nexttoken.type == TokenType::ID){//识别标识符
        tmpname = nexttoken.value;
        match(nexttoken.type);//消耗标识符
    }
    else{
        //语法错误,类型后边不是标识符,unexpected token
    }
    if(nexttoken.type == TokenType::SEMI || nexttoken.type == TokenType::LSB){
        reptr = Var_dclr(tmpdatatype, tmpname);//识别一个变量声明结点
    }
    else{
        //标识符后边不是方括号也不是分号
        //需要产生错误信息
    }
    return reptr;
}

static TreeNode* Stmt_seq()//构造语句序列
{
    TreeNode* reptr = Stmt();//此时可能返回nullptr
    TreeNode* tmp = reptr;//副本
    while( (tmp != nullptr) && (nexttoken.type != TokenType::RBR)&&(nexttoken.type != TokenType::ELSE)&&
          (nexttoken.type != TokenType::ENDFILE)&&(nexttoken.type != TokenType::ERROR)){ //筛选几个记号,其他记号在别处处理
        tmp->sibling = Stmt();
        //处理异常情况break;
        tmp = tmp->sibling;//此后tmp可能为空
    }
    return reptr;
}

static TreeNode* Stmt() //构造语句结点Stmt()需要保证识别不出语句时返回空
{
    TreeNode* reptr = nullptr;
    switch(nexttoken.type)
    {
        case TokenType::LBR : reptr = Compound_stmt();break;
        case TokenType::IF : reptr = Selection_stmt();break;//条件语句
        case TokenType::WHILE : reptr = Iteration_stmt();break;//循环语句
        case TokenType::RETURN : reptr = Return_stmt();break;
        default: reptr = Expression_stmt();break; //表达式此阶段无特征,如有错误交给它处理
    }
    return reptr;//正常此时nexttoken应该是分号后第一个记号
}

static TreeNode* Selection_stmt() //构造if结点
{
    TreeNode* reptr = Newstmt(Stmtkind::selection_stmt);
    match(nexttoken.type);//消耗if
    //处理第一个结点
    if(nexttoken.type == TokenType::LPAREN){ //左括号
        match(nexttoken.type);//消耗
        reptr->child[0] = Expression();
        match(TokenType::RPAREN);//消耗右括号
    }
    else{
        //左括号错误
    }
    //处理第二个结点
    reptr->child[1] = Stmt();//作为语句处理
    //处理第三个结点
    if(nexttoken.type == TokenType::ELSE){
        match(nexttoken.type);
        reptr->child[2] = Stmt();//作为语句处理
    }//其他情况默认的指针就是nullptr不需要改变
    return reptr;
}

static TreeNode* Iteration_stmt() //构造while结点
{
    TreeNode* reptr = Newstmt(Stmtkind::iteration_stmt);
    match(nexttoken.type);//消耗while
    //处理第一个结点
    if(nexttoken.type == TokenType::LPAREN){ //左括号
        match(nexttoken.type);//消耗
        reptr->child[0] = Expression();
        match(TokenType::RPAREN);//消耗右括号
    }
    else{
        //左括号错误
    }
    //处理第二个结点
    reptr->child[1] = Stmt();//作为语句处理
    return reptr;
}

static TreeNode* Return_stmt() //构造return结点
{
    TreeNode* reptr = Newstmt(Stmtkind::return_stmt);
    match(nexttoken.type);//消耗return
    if(nexttoken.type != TokenType::SEMI){ //如果不是分号,进一步处理
        reptr->child[0] = Expression();//返回一个表达式
        //错误处理
    }//此时期望是分号
    match(TokenType::SEMI);
    return reptr;
}
//构造Expression_stmt结点
static TreeNode* Expression_stmt()
{
    TreeNode* reptr = Newstmt(Stmtkind::expression_stmt); //分配空间
    if(nexttoken.type != TokenType::SEMI){ //不是分号才需要进一步处理
        reptr ->child[0] = Expression(); //构造表达式结点
        //错误处理
    }//期望是分号, match
    match(TokenType::SEMI);
    return reptr;
}

//构造Expression结点
static TreeNode* Expression()
{
    TreeNode* reptr = Newfactor(Factorkind::expression);
    TreeNode* tmpvar = nullptr;//不能确定是左值还是右值的var
    reptr->child[0] = Var_seq(tmpvar); //首先需要识别Var_seq
    reptr->child[1] = Spexp(tmpvar);
    if((reptr->child[0]==nullptr) && (reptr->child[1]==nullptr)){
        delete reptr;reptr = nullptr;
    }
    return reptr;
}
static TreeNode* Var_seq(TreeNode*& tmpvar) //构造Var序列
{
    tmpvar = nullptr;//默认状态下是空指针
    TreeNode* reptr = Var();//构造一个var,若不是ID开始,则会返回nullptr
    if(reptr == nullptr){
        return nullptr;
    }//只有识别出一个var才  有可能继续进行
    if(nexttoken.type == TokenType::ASSIGN){//如果var后跟一个=,那么reptr维持原状
        match(nexttoken.type);//消耗等号
    }
    else{//如果不是赋值号,视为简单表达式的第一个结点
        tmpvar = reptr;
        reptr = nullptr;//将空间转交给tmpvar指针,自己设置为空
    }
    TreeNode* tmp = reptr;
    TreeNode* ptmp = tmp;//作为tmp的前结点指针
    while(tmp != nullptr ){//如果第一个var构造成功,则进行循环,此时期望nexttoken是ID
        tmp->sibling = Var();//先假定链接到尾部 //如果Var识别没成功,则会是nullptr
        ptmp = tmp;//保存前结点指针
        tmp = tmp->sibling;//指针移动
        if(tmp == nullptr){//对应不是var的情况
            break;
        }
        if(nexttoken.type == TokenType::ASSIGN){ //如果是赋值,将其归为var序列
            match(nexttoken.type);//消耗等号
        }
        else{ //把最后一个var当作简单表达式的开始
            tmpvar = tmp; //将地址传给tmpvar,达到返回值的作用
            ptmp->sibling = nullptr;//破坏尾部的链
        }
    } //下个记号不是ID,那么终止Var序列生成
    return reptr;
}
static TreeNode* Var() //构造Var结点
{
    TreeNode* reptr = nullptr;
    if(nexttoken.type == TokenType::ID){ //暂时不能确定所有情况下外部进入Var之前都会判断
        reptr = Newfactor(Factorkind::var);//分配空间,指针成员都为空
        reptr->attr.name = nexttoken.value;//将标识符名称传递
        match(nexttoken.type);//消耗标识符
        if(nexttoken.type == TokenType::LSB){ //如果是左 方括号,那么就期望是数组成员
            match(nexttoken.type);
            reptr->child[0] = Expression();//识别一个表达式
            match(TokenType::RSB);//消耗右 方括
        }//其他情况不需要处理.
    }
    return reptr;
}

static TreeNode* Factor()
{//将四种factor统一起来
    TreeNode* reptr = nullptr;//识别括号包裹的表达式
    if(nexttoken.type == TokenType::LPAREN){
        reptr = Expression();//注意exp中不会消耗)
        match(TokenType::RPAREN);//消耗 )
    }
    else if(nexttoken.type == TokenType::NUM){
        reptr = Constnum();
    }
    else if(nexttoken.type == TokenType::ID){
        reptr = VarorCall();
    }
    else{
        //不是factor
    }
    return reptr;
}

static TreeNode* VarorCall()
{
    TreeNode* reptr = nullptr;//暂时不能确定类型
    string tmpname = nexttoken.value;//一定是个标识符
    match(nexttoken.type);
    if(nexttoken.type == TokenType::LPAREN){ //圆括号,期望call,不是圆括号就一定是var
        reptr = Newfactor(Factorkind::call);//分配空间
        reptr->attr.name = tmpname;//函数名
        match(TokenType::LPAREN);//消耗左括号
        reptr->child[0] = Arg_seq();//实参列表
        match(TokenType::RPAREN);//消耗右括号
    }//call处理完毕
    else{
        reptr = Newfactor(Factorkind::var); //肯定是个变量了
        reptr->attr.name = tmpname;
        if(nexttoken.type == TokenType::LSB){ //方括号,数组成员
            match(nexttoken.type);
            reptr->child[0] = Expression();//识别一个表达式
            match(TokenType::RSB);//消耗右 方括
        }
    }
    return reptr;
}
static TreeNode* Arg_seq()//实参列表
{
    TreeNode* reptr = Expression(); //首先识别一个表达式
    TreeNode* tmp = reptr;
    while((tmp!= nullptr) && (nexttoken.type == TokenType::COMMA)){ //如果是逗号,就继续识别
        match(nexttoken.type);//消耗逗号
        tmp->sibling = Expression();//识别表达式
        tmp = tmp->sibling;//序列指针前进
    }
    return reptr;
}

static TreeNode* Constnum()
{
    TreeNode* reptr = Newfactor(Factorkind::constnum);
    reptr->attr.name = nexttoken.value; //将数字的字面值赋给属性,不需要进一步处理
    match(TokenType::NUM);
    return reptr;
}

static TreeNode* Spexp(TreeNode* tmpvar)
{//如果tmpvar不是空,说明简单表达式的第一个var已经被识别,但并不能判断接下来应该识别哪一层,所以这个参数继续传递
    TreeNode* reptr = Calexp(tmpvar);//先识别算数表达式,如果识别结束没有关系算符,本结点直接当作calexp结点
    if(nexttoken.type == TokenType::LTE||nexttoken.type == TokenType::LT||
       nexttoken.type == TokenType::MT||nexttoken.type == TokenType::MTE||
       nexttoken.type == TokenType::EQ||nexttoken.type == TokenType::NEQ){ //识别六个关系运算符
        TreeNode* tmp = reptr; //将已处理的算数表达式结点的地址赋给tmp
        reptr = Newspexp();//为返回的指针开辟一块新的空间
        reptr->child[0] = tmp;//并让新空间的子女指针指向原来的空间//不知道这样会不会出错

        reptr->attr.op = nexttoken.type; //其实除了标识符都可以用type来存储
        reptr->kind.spexp = Spexpkind::relexp;//如果有关系算符就肯定是关系表达式
        match(nexttoken.type);
        reptr->child[1] = Calexp(nullptr);//再从头识别一个算数表达式
        //可能需要处理错误
    }//如果不是关系算符,则不需要其他的处理
    return reptr;
}

static TreeNode* Calexp(TreeNode* tmpvar)
{
    TreeNode* reptr = Terexp(tmpvar);//先识别一个term
    while( nexttoken.type == TokenType::PLUS||nexttoken.type == TokenType::MINUS ){//加减
        TreeNode* tmp = reptr; //将已处理的算数表达式结点的地址赋给tmp
        reptr = Newspexp();//为返回的指针开辟一块新的空间
        reptr->child[0] = tmp;//并让新空间的子女指针指向原来的空间//不知道这样会不会出错

        reptr->attr.op = nexttoken.type; //其实除了标识符都可以用type来存储
        reptr->kind.spexp = Spexpkind::addexp;//如果有加减法就一定是add结点
        match(nexttoken.type);
        reptr->child[1] = Terexp(nullptr);//再识别一个Ter
        //处理错误
    }
    return reptr;
}
static TreeNode* Terexp(TreeNode* tmpvar)
{
    TreeNode* reptr = nullptr;//暂时不确定识别什么
    if(tmpvar == nullptr){//判断tmpvar是否为空
        reptr = Factor(); //如果为空,第一个var未识别 //识别一个factor
    }//如果不为空,第一个var已经被识别,但并不知道是var还是call
    else if(nexttoken.type == TokenType::LPAREN){
        match(nexttoken.type);
        reptr = Newfactor(Factorkind::call);
        reptr->attr.name = tmpvar->attr.name;
        reptr->child[0] = Arg_seq();//识别参数列表
        match(TokenType::RPAREN);//右括号
    }
    else{
        reptr = tmpvar;//如果不是左括号,直接链接即可
    }
    while(nexttoken.type == TokenType::MULTI||nexttoken.type == TokenType::DIVI){//乘除
        TreeNode* tmp = reptr; //将已处理的算数表达式结点的地址赋给tmp
        reptr = Newspexp();//为返回的指针开辟一块新的空间
        reptr->child[0] = tmp;//并让新空间的子女指针指向原来的空间//不知道这样会不会出错

        reptr->attr.op = nexttoken.type; //其实除了标识符都可以用type来存储
        reptr->kind.spexp = Spexpkind::terexp;
        match(nexttoken.type);
        reptr->child[1] = Factor();//再识别一个factor
        //处理错误
    }
    return reptr;
}

static void PrintIndent(int layer)
{//打印缩进
    for(int i= 0;i<layer;++i){
        cout<<"  ";of<<"  ";
    }
}
string opchar[] = {"+", "-", "*", "/", "<", "<=", ">", ">=", "==", "!="};
void PrintTree(TreeNode* tree, int layer)
{ //打印语法树 //结点分类
    if(tree == nullptr) return;
    PrintIndent(layer);
    if(tree->nodekind == NodeKind::Declaration){//如果是声明
        if(tree->kind.dclr == Dclrkind::var_dclr){ //如果是变量声明仅有三个属性,没有子女,有兄弟
            cout<<"VAR_dclr:\n";of<<"VAR_dclr:\n";
            PrintIndent(layer+1);cout<<"type: ";of<<"type: ";
            switch(tree->attr.vdclrattr.datatype)
            {
                case DataType::INT : cout<<"INT\n";of<<"INT\n";break;
                case DataType::VOID : cout<<"VOID\n";of<<"VOID\n";break;
            }
            PrintIndent(layer+1);cout<<"name: "<<tree->attr.vdclrattr.name<<"\n";of<<"name: "<<tree->attr.vdclrattr.name<<"\n";
            PrintIndent(layer+1);cout<<"length: "<<tree->attr.vdclrattr.arrlen<<"\n";of<<"length: "<<tree->attr.vdclrattr.arrlen<<"\n";
        }
        else if(tree->kind.dclr == Dclrkind::fun_dclr){ //函数
            cout<<"FUN_dclr:\n";of<<"FUN_dclr:\n";
            PrintIndent(layer+1);cout<<"type: ";of<<"type: ";
            switch(tree->attr.fdclrattr.datatype)
            {
                case DataType::INT : cout<<"INT\n";of<<"INT\n";break;
                case DataType::VOID : cout<<"VOID\n";of<<"VOID\n";break;
            }
            PrintIndent(layer+1);cout<<"name: "<<tree->attr.fdclrattr.name<<"\n";of<<"name: "<<tree->attr.fdclrattr.name<<"\n";

            PrintIndent(layer+1);cout<<"Param_seq: ";of<<"Param_seq: ";
            if(tree->child[0] == nullptr){cout<<"void";of<<"void";} cout<<"\n";of<<"\n";
            PrintTree(tree->child[0],layer+2);

            PrintIndent(layer+1);cout<<"Fun_body:\n";of<<"Fun_body:\n";
            PrintTree(tree->child[1],layer+2);
        }
        PrintTree(tree->sibling, layer);//兄弟指针
    }
    else if(tree->nodekind == NodeKind::Param){//如果是非空形参
        cout<<"Param: ";of<<"Param: ";
        switch(tree->attr.paramattr.datatype)
        {
            case DataType::INT : cout<<"INT ";of<<"INT ";break;
            case DataType::VOID : cout<<"VOID ";of<<"VOID ";break;
        }
        if(tree->attr.paramattr.isary) {cout<<"[] ";of<<"[] ";}
        cout<<tree->attr.paramattr.name<<"\n";of<<tree->attr.paramattr.name<<"\n";
        PrintTree(tree->sibling,layer);
    }
    else if(tree->nodekind == NodeKind::Factor){//如果是因子结点
        if(tree->kind.factor == Factorkind::call){//如果是函数调用
            cout<<"CALL: "<<tree->attr.name<<"\n";of<<"CALL: "<<tree->attr.name<<"\n";
            PrintIndent(layer+1);cout<<"Args:\n";of<<"Args:\n";
            PrintTree(tree->child[0],layer+2);
        }
        else if(tree->kind.factor == Factorkind::constnum){//是常数
            cout<<"Const: "<<tree->attr.name<<"\n";of<<"Const: "<<tree->attr.name<<"\n";
        }
        else if(tree->kind.factor == Factorkind::expression){//表达式
            cout<<"Exp: \n";of<<"Exp: \n";
            PrintIndent(layer+1);cout<<"left: ";of<<"left: ";
            if(tree->child[0] == nullptr){cout<<"null";of<<"null";} cout<<"\n";of<<"\n";
            PrintTree(tree->child[0], layer+2);
            PrintIndent(layer+1);cout<<"right: ";of<<"right: ";
            if(tree->child[1] == nullptr){cout<<"null";of<<"null";} cout<<"\n";of<<"\n";
            PrintTree(tree->child[1], layer+2);
            PrintTree(tree->sibling,layer);
        }
        else if(tree->kind.factor == Factorkind::var){//变量
            cout<<"Var: "<<tree->attr.name<<"\n";of<<"Var: "<<tree->attr.name<<"\n";
            if(tree->child[0]!=nullptr){
                PrintIndent(layer+1);cout<<"index: \n";of<<"index: \n";
                PrintTree(tree->child[0], layer+2);
            }
            PrintTree(tree->sibling,layer);
        }
    }
    else if(tree->nodekind == NodeKind::Spexp){//简单表达式结点
        cout<<"Spexp: "<<opchar[ int(tree->attr.op) - int(TokenType::PLUS)]<<"\n";of<<"Spexp: "<<opchar[ int(tree->attr.op) - int(TokenType::PLUS)]<<"\n";
        PrintTree(tree->child[0],layer+1);
        PrintTree(tree->child[1],layer+1);
    }
    else if(tree->nodekind == NodeKind::Statement){
        //语句
        if(tree->kind.stmt == Stmtkind::compound_stmt){ //复合语句
            cout<<"ComPD: \n";
of<<"ComPD: \n";
            PrintIndent(layer+1); cout<<"Locdclr: \n";
of<<"Locdclr: \n";
            PrintTree(tree->child[0],layer+2);
            PrintIndent(layer+1); cout<<"Stmtlst: \n";
of<<"Stmtlst: \n";
            PrintTree(tree->child[1],layer+2);
            PrintTree(tree->sibling, layer);
        }
        else if(tree->kind.stmt == Stmtkind::expression_stmt){
            cout<<"Expstmt:\n";
of<<"Expstmt:\n";
            PrintTree(tree->child[0],layer+1);
            PrintTree(tree->sibling,layer);
        }
        else if(tree->kind.stmt == Stmtkind::iteration_stmt){
            cout<<"WHILE:\n";
of<<"WHILE:\n";
            PrintIndent(layer+1);cout<<"Condition:\n";
of<<"Condition:\n";
            PrintTree(tree->child[0],layer+2);
            PrintIndent(layer+1);cout<<"Body:\n";
of<<"Body:\n";
            PrintTree(tree->child[1],layer+2);
            PrintTree(tree->sibling,layer);
        }
        else if(tree->kind.stmt == Stmtkind::return_stmt){
            cout<<"RETURN:\n";
of<<"RETURN:\n";
            PrintTree(tree->child[0],layer+1);
            PrintTree(tree->sibling,layer);
        }
        else if(tree->kind.stmt == Stmtkind::selection_stmt){
            cout<<"IF:\n";
of<<"IF:\n";
            PrintIndent(layer+1);cout<<"Condition:\n";
of<<"Condition:\n";
            PrintTree(tree->child[0],layer+2);
            PrintIndent(layer+1);cout<<"Body:\n";
of<<"Body:\n";
            PrintTree(tree->child[1],layer+2);
            if(tree->child[2]!=nullptr){
                PrintIndent(layer+1);cout<<"ELSE:\n";
of<<"ELSE:\n";
                PrintTree(tree->child[2],layer+2);
            }
            PrintTree(tree->sibling, layer);
        }
    }
    return;
}

