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
//��������
static void match(TokenType expected);
//�������Ŀռ����
static TreeNode* Newdclr(Dclrkind dclrkind);
static TreeNode* Newparam();
static TreeNode* Newstmt(Stmtkind stmtkind);
static TreeNode* Newfactor(Factorkind factorkind);
static TreeNode* Newspexp();

//������Ĺ�����鲢
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
static TreeNode* Factor();//����
static TreeNode* VarorCall();
static TreeNode* Arg_seq();//Exp������
static TreeNode* Constnum();

static TreeNode* Spexp(TreeNode* tmpvar);//����ʶ�����
static TreeNode* Calexp(TreeNode* tmpvar);
static TreeNode* Terexp(TreeNode* tmpvar);

//ƥ�����
static void match(TokenType expected)
{
    if (nexttoken.type == expected) GetNextToken(); //ƥ��ɹ�,��һ���Ǻ�
    else {//������
//    syntaxError("unexpected token -> ");
//    printToken(token,tokenString);
//    fprintf(listing,"      ");
    }
    return;
}
//����ռ�
static TreeNode* Newdclr(Dclrkind dclrkind) //Ϊ����������ռ�
{
    TreeNode* reptr = new TreeNode();//����ռ�
    reptr->nodekind = NodeKind::Declaration;
    reptr->kind.dclr = dclrkind;
    reptr->lineno = lineord;//�к�
    reptr->sibling = nullptr;
//�б�ҪΪÿ�����Ӹ���ֵ��, ��!!!!
    for(int i=0;i<MAXCHILDREN;++i){
        reptr->child[i] = nullptr;
    }
    return reptr;
}
static TreeNode* Newparam() //Ϊ�βη���ռ�
{
    TreeNode* reptr = new TreeNode();//����ռ�
    reptr->nodekind = NodeKind::Param;
    reptr->kind.rootkind = true;
    reptr->lineno = lineord;//�к�
    reptr->sibling = nullptr;
    for(int i=0;i<MAXCHILDREN;++i){
        reptr->child[i] = nullptr;
    }
    return reptr;
}
static TreeNode* Newstmt(Stmtkind stmtkind)
{
    TreeNode* reptr = new TreeNode();//����ռ�
    reptr->nodekind = NodeKind::Statement;//�����������
    reptr->kind.stmt = stmtkind; //����������
    reptr->lineno = lineord;//�к�
    reptr->sibling = nullptr;
    for(int i=0;i<MAXCHILDREN;++i){
        reptr->child[i] = nullptr;
    }
    return reptr;
}
static TreeNode* Newfactor(Factorkind factorkind)
{
    TreeNode* reptr = new TreeNode();//����ռ�
    reptr->nodekind = NodeKind::Factor;//����factor����
    reptr->kind.factor = factorkind; //factor��������
    reptr->lineno = lineord;//�к�
    reptr->sibling = nullptr;
    for(int i=0;i<MAXCHILDREN;++i){
        reptr->child[i] = nullptr;
    }
    return reptr;
}
static TreeNode* Newspexp()
{
    TreeNode* reptr = new TreeNode();//����ռ�
    reptr->nodekind = NodeKind::Spexp;
    reptr->lineno = lineord;//�к�
    reptr->sibling = nullptr;
    for(int i=0;i<MAXCHILDREN;++i){
        reptr->child[i] = nullptr;
    }
    return reptr;
}


//�����﷨��
TreeNode* Parse()
{
    TreeNode * t;//�﷨�����ڵ�
    GetNextToken();//��ȡ��һ�����
    t = Dclr_seq();//������һ�����
    if (nexttoken.type!=TokenType::ENDFILE) cout<<"Code ends before file\n";//syntaxError("Code ends before file\n");
    return t;
}

static TreeNode* Dclr_seq()
{ //������������
    TreeNode* reptr = Dclr();
    TreeNode* tmpptr = reptr;//�п���Ϊ��,���Բ���ֱ���ƶ�λ��ָ��
    while( tmpptr != nullptr){//���������ȷ���ɲ�����//ֻҪ�¸��ǺŲ����ļ�����,�ͼ����������nexttoken.type != TokenType::ENDFILE
        tmpptr->sibling = Dclr();
        tmpptr = tmpptr->sibling;//����ָ���ƶ�
    }
    return reptr;
}
//������
static TreeNode* Dclr() //�����������
{
    TreeNode* reptr = nullptr; //��Ҫ���ص�ָ��
    DataType tmpdatatype;
    string tmpname; //ǰ�����Ǻ�
    if(nexttoken.type == TokenType::INT || nexttoken.type == TokenType::VOID){//ʶ����������,��ʱ�����������
        switch(nexttoken.type) //����������������
        {
            case TokenType::INT : tmpdatatype = DataType::INT;break;
            case TokenType::VOID : tmpdatatype = DataType::VOID;break;
            default: break;//cout<<"�������ͳ���\n";//������
        }
        match(nexttoken.type);//��һ���Ǻ�
    }
    else{//���������Ͳ���INTҲ����VOID,�����������Ľṹunexpected token
        if(nexttoken.type == TokenType::ENDFILE) return nullptr;
    }
    if(nexttoken.type == TokenType::ID){//ʶ���ʶ��
        tmpname = nexttoken.value;
        match(nexttoken.type);
    }
    else{
        //�﷨����,���ͺ�߲��Ǳ�ʶ��unexpected token
    }
    //���������token
    if(nexttoken.type == TokenType::SEMI || nexttoken.type == TokenType::LSB){//����ǷֺŻ��������������ͨ��������
        reptr = Var_dclr(tmpdatatype, tmpname);
    }
    else if(nexttoken.type == TokenType::LPAREN){ //�����Ǻ�������
        reptr = Fun_dclr(tmpdatatype, tmpname);
    }
    else{
        //��ʶ�����Ƿֺ�Ҳ����[,�����ϱ��������Ľṹ
    }
    return reptr;
}

static TreeNode* Var_dclr(DataType tmpdatatype, string tmpname) //��������������
{
    TreeNode* reptr = Newdclr(Dclrkind::var_dclr); //Ϊ��������������ռ�
    reptr->attr.vdclrattr.datatype = tmpdatatype; //��������/����ֵ����
    reptr->attr.vdclrattr.name = tmpname; //��ʶ������/������
    //ֻ�������������
    if(nexttoken.type == TokenType::SEMI){//�ֺ�,˵���ǵ�������
        match(nexttoken.type);//nexttokenһ���Ƿֺ�,���ķֺ�
        reptr->attr.vdclrattr.arrlen = "null"; //�������������鳤������ֵ��Ϊnull(�Ƿ�,���ȱ���>0??
    }
    else if(nexttoken.type == TokenType::LSB){//����������
        match(nexttoken.type);//����������
        if(nexttoken.type == TokenType::NUM){
            reptr->attr.vdclrattr.arrlen = nexttoken.value; //�˴�value��string
            match(nexttoken.type);//nexttokenһ������,������
            match(TokenType::RSB);match(TokenType::SEMI);//�����ҷ����źͷֺ�,���match�г���,˵�������Ͻṹ
        }
        else{
            //��Ҫ�������������г��Ȳ������ֵ����
        }
    }
    return reptr;//����һ�������������ĵ�ַ
}

static TreeNode* Fun_dclr(DataType tmpdatatype, string tmpname) //���캯���������
{
    TreeNode* reptr = Newdclr(Dclrkind::fun_dclr); //Ϊ��������������ռ�
    reptr->attr.fdclrattr.datatype = tmpdatatype; //��������/����ֵ����
    reptr->attr.fdclrattr.name = tmpname; //��ʶ������/������
    //�����б�Ĵ���,nexttoken��һ����Բ����
    match(nexttoken.type); //����Բ����
    reptr->child[0] = Param_seq();//�����β��б�
    match(TokenType::RPAREN);//����Բ����)
    //������Ĵ���
    if(nexttoken.type == TokenType::LBR){//�����Ǻ�����
        reptr->child[1] = Compound_stmt();//����һ�����������
    }
    else{
        //������ȱ��������
    }
    return reptr;
}
static TreeNode* Param_seq() //�β��б�ʶ����������
{
    DataType tmpdatatype;//�β�����
    TreeNode* reptr = nullptr;//��ʱ��֪���β��б��Ƿ�Ϊ��
    switch(nexttoken.type) //���������������
    {
        case TokenType::INT : tmpdatatype = DataType::INT;break;
        case TokenType::VOID : tmpdatatype = DataType::VOID;break;
        default: cout<<"�β����ͳ���\n"; return reptr;//������//������
    }
    match(nexttoken.type);//����һ����������
    //�����β��б���VOID�����
    if(nexttoken.type == TokenType::RPAREN){//������,�����б�Ϊ��
        if(tmpdatatype!= DataType::VOID){
            cout<<"��VOID�����β�\n"; //���Ƕ���������Ĵ�����
        }//���β���ֻ��һ��VOID //�����б�Ϊ��,��������Ҫ����ռ�
    }
//��Ϊ�յ��βε�����
    else if(nexttoken.type == TokenType::ID){//�����б�Ϊ��
        //�����һ���β�
        reptr = Newparam(); //Ϊ�βν�����ռ�
        if(tmpdatatype == DataType::VOID){
            cout<<"�β����ͳ���\n";
        }
        reptr->attr.paramattr.datatype = tmpdatatype; //�β�����
        reptr->attr.paramattr.name = nexttoken.value; //��ʶ������/������/�β���
        reptr->attr.paramattr.isary = false;
        match(nexttoken.type);//���ı�ʶ��
        if(nexttoken.type == TokenType::LSB){ //�βο���������
            match(nexttoken.type);
            reptr->attr.paramattr.isary = true;//��һ���β��������β�
            match(TokenType::RSB);//��Ҫ���������
        }
        //���Ӷ���β�
        TreeNode* tmp = reptr;//��ʱtmp����
        while(nexttoken.type == TokenType::COMMA){//�Ƕ���,׼��������һ���β�
            match(nexttoken.type);
            tmp->sibling = Param();
            tmp = tmp->sibling;//�ƶ����е�ָ��
        }
    }
    else{ //�����б���ʽ����,��һ���β����ͺ���������Ҳ���Ǳ�ʶ��
    }
    return reptr;
}
static TreeNode* Param() //�����βν��,��ʱnexttoken��������
{
    TreeNode* reptr = Newparam(); //Ϊ�βν�����ռ�
    if(nexttoken.type == TokenType::INT){
        match(nexttoken.type);//���ͺϷ�,��������
        reptr->attr.paramattr.datatype = DataType::INT;
        if(nexttoken.type == TokenType::ID){
            reptr->attr.paramattr.name = nexttoken.value; //��ʶ������/������/�β���
            reptr->attr.paramattr.isary = false;
            match(nexttoken.type);//���ı�ʶ��
            if(nexttoken.type == TokenType::LSB){ //�βο���������
                match(nexttoken.type);
                match(TokenType::RSB);
                reptr->attr.paramattr.isary = true;//��һ���β��������β�
            }
        }
        else{
            //���ͺ��Ǳ�ʶ��,������
        }
    }
    else{ //�β����Ͳ���INT
        cout<<"�β����ͷǷ�\n";
        delete reptr;//�����ռ�
        reptr = nullptr;
    }
    return reptr;
}

static TreeNode* Compound_stmt() //���츴�������
{
    TreeNode* reptr = Newstmt(Stmtkind::compound_stmt);
    match(TokenType::LBR);//���Ļ�����
    reptr->child[0] = Local_dclr();//ʶ��vardclr����
    reptr->child[1] = Stmt_seq();//ʶ���������
    match(TokenType::RBR);//�����һ�����
    return reptr;
}

static TreeNode* Local_dclr() //���챾�ر�����������!!!!!
{
    TreeNode* reptr = Lvar_dclr();
    TreeNode* tmp = reptr;

    while(tmp != nullptr){//ֻ�е�һ�����ɹ����ɲſ��Լ�������
        tmp->sibling = Lvar_dclr();//���ܷ���nullptr
        tmp = tmp->sibling;//�ƶ�����ָ��
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
        match(nexttoken.type);//��һ���Ǻ�
    }
    else{ //�������������Ͳ���INT�ؼ���
        return reptr;//��Ϊ���ر�����������,����,��ʱ��nexttoken�������һ�����ɽ���ĵ�һ��token
    }
    if(nexttoken.type == TokenType::ID){//ʶ���ʶ��
        tmpname = nexttoken.value;
        match(nexttoken.type);//���ı�ʶ��
    }
    else{
        //�﷨����,���ͺ�߲��Ǳ�ʶ��,unexpected token
    }
    if(nexttoken.type == TokenType::SEMI || nexttoken.type == TokenType::LSB){
        reptr = Var_dclr(tmpdatatype, tmpname);//ʶ��һ�������������
    }
    else{
        //��ʶ����߲��Ƿ�����Ҳ���Ƿֺ�
        //��Ҫ����������Ϣ
    }
    return reptr;
}

static TreeNode* Stmt_seq()//�����������
{
    TreeNode* reptr = Stmt();//��ʱ���ܷ���nullptr
    TreeNode* tmp = reptr;//����
    while( (tmp != nullptr) && (nexttoken.type != TokenType::RBR)&&(nexttoken.type != TokenType::ELSE)&&
          (nexttoken.type != TokenType::ENDFILE)&&(nexttoken.type != TokenType::ERROR)){ //ɸѡ�����Ǻ�,�����Ǻ��ڱ𴦴���
        tmp->sibling = Stmt();
        //�����쳣���break;
        tmp = tmp->sibling;//�˺�tmp����Ϊ��
    }
    return reptr;
}

static TreeNode* Stmt() //���������Stmt()��Ҫ��֤ʶ�𲻳����ʱ���ؿ�
{
    TreeNode* reptr = nullptr;
    switch(nexttoken.type)
    {
        case TokenType::LBR : reptr = Compound_stmt();break;
        case TokenType::IF : reptr = Selection_stmt();break;//�������
        case TokenType::WHILE : reptr = Iteration_stmt();break;//ѭ�����
        case TokenType::RETURN : reptr = Return_stmt();break;
        default: reptr = Expression_stmt();break; //���ʽ�˽׶�������,���д��󽻸�������
    }
    return reptr;//������ʱnexttokenӦ���Ƿֺź��һ���Ǻ�
}

static TreeNode* Selection_stmt() //����if���
{
    TreeNode* reptr = Newstmt(Stmtkind::selection_stmt);
    match(nexttoken.type);//����if
    //�����һ�����
    if(nexttoken.type == TokenType::LPAREN){ //������
        match(nexttoken.type);//����
        reptr->child[0] = Expression();
        match(TokenType::RPAREN);//����������
    }
    else{
        //�����Ŵ���
    }
    //����ڶ������
    reptr->child[1] = Stmt();//��Ϊ��䴦��
    //������������
    if(nexttoken.type == TokenType::ELSE){
        match(nexttoken.type);
        reptr->child[2] = Stmt();//��Ϊ��䴦��
    }//�������Ĭ�ϵ�ָ�����nullptr����Ҫ�ı�
    return reptr;
}

static TreeNode* Iteration_stmt() //����while���
{
    TreeNode* reptr = Newstmt(Stmtkind::iteration_stmt);
    match(nexttoken.type);//����while
    //�����һ�����
    if(nexttoken.type == TokenType::LPAREN){ //������
        match(nexttoken.type);//����
        reptr->child[0] = Expression();
        match(TokenType::RPAREN);//����������
    }
    else{
        //�����Ŵ���
    }
    //����ڶ������
    reptr->child[1] = Stmt();//��Ϊ��䴦��
    return reptr;
}

static TreeNode* Return_stmt() //����return���
{
    TreeNode* reptr = Newstmt(Stmtkind::return_stmt);
    match(nexttoken.type);//����return
    if(nexttoken.type != TokenType::SEMI){ //������Ƿֺ�,��һ������
        reptr->child[0] = Expression();//����һ�����ʽ
        //������
    }//��ʱ�����Ƿֺ�
    match(TokenType::SEMI);
    return reptr;
}
//����Expression_stmt���
static TreeNode* Expression_stmt()
{
    TreeNode* reptr = Newstmt(Stmtkind::expression_stmt); //����ռ�
    if(nexttoken.type != TokenType::SEMI){ //���ǷֺŲ���Ҫ��һ������
        reptr ->child[0] = Expression(); //������ʽ���
        //������
    }//�����Ƿֺ�, match
    match(TokenType::SEMI);
    return reptr;
}

//����Expression���
static TreeNode* Expression()
{
    TreeNode* reptr = Newfactor(Factorkind::expression);
    TreeNode* tmpvar = nullptr;//����ȷ������ֵ������ֵ��var
    reptr->child[0] = Var_seq(tmpvar); //������Ҫʶ��Var_seq
    reptr->child[1] = Spexp(tmpvar);
    if((reptr->child[0]==nullptr) && (reptr->child[1]==nullptr)){
        delete reptr;reptr = nullptr;
    }
    return reptr;
}
static TreeNode* Var_seq(TreeNode*& tmpvar) //����Var����
{
    tmpvar = nullptr;//Ĭ��״̬���ǿ�ָ��
    TreeNode* reptr = Var();//����һ��var,������ID��ʼ,��᷵��nullptr
    if(reptr == nullptr){
        return nullptr;
    }//ֻ��ʶ���һ��var��  �п��ܼ�������
    if(nexttoken.type == TokenType::ASSIGN){//���var���һ��=,��ôreptrά��ԭ״
        match(nexttoken.type);//���ĵȺ�
    }
    else{//������Ǹ�ֵ��,��Ϊ�򵥱��ʽ�ĵ�һ�����
        tmpvar = reptr;
        reptr = nullptr;//���ռ�ת����tmpvarָ��,�Լ�����Ϊ��
    }
    TreeNode* tmp = reptr;
    TreeNode* ptmp = tmp;//��Ϊtmp��ǰ���ָ��
    while(tmp != nullptr ){//�����һ��var����ɹ�,�����ѭ��,��ʱ����nexttoken��ID
        tmp->sibling = Var();//�ȼٶ����ӵ�β�� //���Varʶ��û�ɹ�,�����nullptr
        ptmp = tmp;//����ǰ���ָ��
        tmp = tmp->sibling;//ָ���ƶ�
        if(tmp == nullptr){//��Ӧ����var�����
            break;
        }
        if(nexttoken.type == TokenType::ASSIGN){ //����Ǹ�ֵ,�����Ϊvar����
            match(nexttoken.type);//���ĵȺ�
        }
        else{ //�����һ��var�����򵥱��ʽ�Ŀ�ʼ
            tmpvar = tmp; //����ַ����tmpvar,�ﵽ����ֵ������
            ptmp->sibling = nullptr;//�ƻ�β������
        }
    } //�¸��ǺŲ���ID,��ô��ֹVar��������
    return reptr;
}
static TreeNode* Var() //����Var���
{
    TreeNode* reptr = nullptr;
    if(nexttoken.type == TokenType::ID){ //��ʱ����ȷ������������ⲿ����Var֮ǰ�����ж�
        reptr = Newfactor(Factorkind::var);//����ռ�,ָ���Ա��Ϊ��
        reptr->attr.name = nexttoken.value;//����ʶ�����ƴ���
        match(nexttoken.type);//���ı�ʶ��
        if(nexttoken.type == TokenType::LSB){ //������� ������,��ô�������������Ա
            match(nexttoken.type);
            reptr->child[0] = Expression();//ʶ��һ�����ʽ
            match(TokenType::RSB);//������ ����
        }//�����������Ҫ����.
    }
    return reptr;
}

static TreeNode* Factor()
{//������factorͳһ����
    TreeNode* reptr = nullptr;//ʶ�����Ű����ı��ʽ
    if(nexttoken.type == TokenType::LPAREN){
        reptr = Expression();//ע��exp�в�������)
        match(TokenType::RPAREN);//���� )
    }
    else if(nexttoken.type == TokenType::NUM){
        reptr = Constnum();
    }
    else if(nexttoken.type == TokenType::ID){
        reptr = VarorCall();
    }
    else{
        //����factor
    }
    return reptr;
}

static TreeNode* VarorCall()
{
    TreeNode* reptr = nullptr;//��ʱ����ȷ������
    string tmpname = nexttoken.value;//һ���Ǹ���ʶ��
    match(nexttoken.type);
    if(nexttoken.type == TokenType::LPAREN){ //Բ����,����call,����Բ���ž�һ����var
        reptr = Newfactor(Factorkind::call);//����ռ�
        reptr->attr.name = tmpname;//������
        match(TokenType::LPAREN);//����������
        reptr->child[0] = Arg_seq();//ʵ���б�
        match(TokenType::RPAREN);//����������
    }//call�������
    else{
        reptr = Newfactor(Factorkind::var); //�϶��Ǹ�������
        reptr->attr.name = tmpname;
        if(nexttoken.type == TokenType::LSB){ //������,�����Ա
            match(nexttoken.type);
            reptr->child[0] = Expression();//ʶ��һ�����ʽ
            match(TokenType::RSB);//������ ����
        }
    }
    return reptr;
}
static TreeNode* Arg_seq()//ʵ���б�
{
    TreeNode* reptr = Expression(); //����ʶ��һ�����ʽ
    TreeNode* tmp = reptr;
    while((tmp!= nullptr) && (nexttoken.type == TokenType::COMMA)){ //����Ƕ���,�ͼ���ʶ��
        match(nexttoken.type);//���Ķ���
        tmp->sibling = Expression();//ʶ����ʽ
        tmp = tmp->sibling;//����ָ��ǰ��
    }
    return reptr;
}

static TreeNode* Constnum()
{
    TreeNode* reptr = Newfactor(Factorkind::constnum);
    reptr->attr.name = nexttoken.value; //�����ֵ�����ֵ��������,����Ҫ��һ������
    match(TokenType::NUM);
    return reptr;
}

static TreeNode* Spexp(TreeNode* tmpvar)
{//���tmpvar���ǿ�,˵���򵥱��ʽ�ĵ�һ��var�Ѿ���ʶ��,���������жϽ�����Ӧ��ʶ����һ��,�������������������
    TreeNode* reptr = Calexp(tmpvar);//��ʶ���������ʽ,���ʶ�����û�й�ϵ���,�����ֱ�ӵ���calexp���
    if(nexttoken.type == TokenType::LTE||nexttoken.type == TokenType::LT||
       nexttoken.type == TokenType::MT||nexttoken.type == TokenType::MTE||
       nexttoken.type == TokenType::EQ||nexttoken.type == TokenType::NEQ){ //ʶ��������ϵ�����
        TreeNode* tmp = reptr; //���Ѵ�����������ʽ���ĵ�ַ����tmp
        reptr = Newspexp();//Ϊ���ص�ָ�뿪��һ���µĿռ�
        reptr->child[0] = tmp;//�����¿ռ����Ůָ��ָ��ԭ���Ŀռ�//��֪�������᲻�����

        reptr->attr.op = nexttoken.type; //��ʵ���˱�ʶ����������type���洢
        reptr->kind.spexp = Spexpkind::relexp;//����й�ϵ����Ϳ϶��ǹ�ϵ���ʽ
        match(nexttoken.type);
        reptr->child[1] = Calexp(nullptr);//�ٴ�ͷʶ��һ���������ʽ
        //������Ҫ�������
    }//������ǹ�ϵ���,����Ҫ�����Ĵ���
    return reptr;
}

static TreeNode* Calexp(TreeNode* tmpvar)
{
    TreeNode* reptr = Terexp(tmpvar);//��ʶ��һ��term
    while( nexttoken.type == TokenType::PLUS||nexttoken.type == TokenType::MINUS ){//�Ӽ�
        TreeNode* tmp = reptr; //���Ѵ�����������ʽ���ĵ�ַ����tmp
        reptr = Newspexp();//Ϊ���ص�ָ�뿪��һ���µĿռ�
        reptr->child[0] = tmp;//�����¿ռ����Ůָ��ָ��ԭ���Ŀռ�//��֪�������᲻�����

        reptr->attr.op = nexttoken.type; //��ʵ���˱�ʶ����������type���洢
        reptr->kind.spexp = Spexpkind::addexp;//����мӼ�����һ����add���
        match(nexttoken.type);
        reptr->child[1] = Terexp(nullptr);//��ʶ��һ��Ter
        //�������
    }
    return reptr;
}
static TreeNode* Terexp(TreeNode* tmpvar)
{
    TreeNode* reptr = nullptr;//��ʱ��ȷ��ʶ��ʲô
    if(tmpvar == nullptr){//�ж�tmpvar�Ƿ�Ϊ��
        reptr = Factor(); //���Ϊ��,��һ��varδʶ�� //ʶ��һ��factor
    }//�����Ϊ��,��һ��var�Ѿ���ʶ��,������֪����var����call
    else if(nexttoken.type == TokenType::LPAREN){
        match(nexttoken.type);
        reptr = Newfactor(Factorkind::call);
        reptr->attr.name = tmpvar->attr.name;
        reptr->child[0] = Arg_seq();//ʶ������б�
        match(TokenType::RPAREN);//������
    }
    else{
        reptr = tmpvar;//�������������,ֱ�����Ӽ���
    }
    while(nexttoken.type == TokenType::MULTI||nexttoken.type == TokenType::DIVI){//�˳�
        TreeNode* tmp = reptr; //���Ѵ�����������ʽ���ĵ�ַ����tmp
        reptr = Newspexp();//Ϊ���ص�ָ�뿪��һ���µĿռ�
        reptr->child[0] = tmp;//�����¿ռ����Ůָ��ָ��ԭ���Ŀռ�//��֪�������᲻�����

        reptr->attr.op = nexttoken.type; //��ʵ���˱�ʶ����������type���洢
        reptr->kind.spexp = Spexpkind::terexp;
        match(nexttoken.type);
        reptr->child[1] = Factor();//��ʶ��һ��factor
        //�������
    }
    return reptr;
}

static void PrintIndent(int layer)
{//��ӡ����
    for(int i= 0;i<layer;++i){
        cout<<"  ";of<<"  ";
    }
}
string opchar[] = {"+", "-", "*", "/", "<", "<=", ">", ">=", "==", "!="};
void PrintTree(TreeNode* tree, int layer)
{ //��ӡ�﷨�� //������
    if(tree == nullptr) return;
    PrintIndent(layer);
    if(tree->nodekind == NodeKind::Declaration){//���������
        if(tree->kind.dclr == Dclrkind::var_dclr){ //����Ǳ�������������������,û����Ů,���ֵ�
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
        else if(tree->kind.dclr == Dclrkind::fun_dclr){ //����
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
        PrintTree(tree->sibling, layer);//�ֵ�ָ��
    }
    else if(tree->nodekind == NodeKind::Param){//����Ƿǿ��β�
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
    else if(tree->nodekind == NodeKind::Factor){//��������ӽ��
        if(tree->kind.factor == Factorkind::call){//����Ǻ�������
            cout<<"CALL: "<<tree->attr.name<<"\n";of<<"CALL: "<<tree->attr.name<<"\n";
            PrintIndent(layer+1);cout<<"Args:\n";of<<"Args:\n";
            PrintTree(tree->child[0],layer+2);
        }
        else if(tree->kind.factor == Factorkind::constnum){//�ǳ���
            cout<<"Const: "<<tree->attr.name<<"\n";of<<"Const: "<<tree->attr.name<<"\n";
        }
        else if(tree->kind.factor == Factorkind::expression){//���ʽ
            cout<<"Exp: \n";of<<"Exp: \n";
            PrintIndent(layer+1);cout<<"left: ";of<<"left: ";
            if(tree->child[0] == nullptr){cout<<"null";of<<"null";} cout<<"\n";of<<"\n";
            PrintTree(tree->child[0], layer+2);
            PrintIndent(layer+1);cout<<"right: ";of<<"right: ";
            if(tree->child[1] == nullptr){cout<<"null";of<<"null";} cout<<"\n";of<<"\n";
            PrintTree(tree->child[1], layer+2);
            PrintTree(tree->sibling,layer);
        }
        else if(tree->kind.factor == Factorkind::var){//����
            cout<<"Var: "<<tree->attr.name<<"\n";of<<"Var: "<<tree->attr.name<<"\n";
            if(tree->child[0]!=nullptr){
                PrintIndent(layer+1);cout<<"index: \n";of<<"index: \n";
                PrintTree(tree->child[0], layer+2);
            }
            PrintTree(tree->sibling,layer);
        }
    }
    else if(tree->nodekind == NodeKind::Spexp){//�򵥱��ʽ���
        cout<<"Spexp: "<<opchar[ int(tree->attr.op) - int(TokenType::PLUS)]<<"\n";of<<"Spexp: "<<opchar[ int(tree->attr.op) - int(TokenType::PLUS)]<<"\n";
        PrintTree(tree->child[0],layer+1);
        PrintTree(tree->child[1],layer+1);
    }
    else if(tree->nodekind == NodeKind::Statement){
        //���
        if(tree->kind.stmt == Stmtkind::compound_stmt){ //�������
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

