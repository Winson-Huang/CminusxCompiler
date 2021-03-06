2019.11.10 黄文诚 201711140124

# 语法分析器部分(Parser)

## 实验目的

构造语法分析器的目的是从扫描器的输出(记号序列)中构造出语法树. 构造语法树的依据是C-Minus语言的BNF语法. 大体思路是根据该语言的BNF语法编写多个相互调用的识别过程, 并用最顶层的识别过程构造整个语法树. 

## 语法说明

C-minus语言的**BNF**语法如下:

------

1. program → declaration-list
2. declaration-list → declaration-list declaration | declaration
3. declaration → var-declaration | fun-declaration



4. var-declaration → type-specifier **ID** ; | type-specifier **ID** **[NUM]**;

5. type-specifier → **int** | **void**

   

6. fun-declaration → type-specifier **ID** **(** params **)**  compound-stmt

7. params → params-list | **void**

8. param-list → param-list **,** param | param

9. param → type-specifier **ID** | type-specifier **ID** **[ ]**
10. compound-stmt → **{** local-declarations statement-list **}**

11. local-declarations → local-declarations var-declaration | empty

12. statement-list → statement-list statement | empty

13. statement → expression-stmt | compound-stmt | selection-stmt| iteration-stmt | return-stmt

14. expression-stmt → expression **;** | **;**

15. selection-stmt → **if** **(** expression **)** statement| **if** **(** expression **)** statement **else** statement

16. iteration -stmt → **while** **(** expression **)** statement

17. return -stmt → **return** **;**|  **return** expression;

18. expression → var **=** expression | simple-expression

19. var → **ID** | **ID** **[** expression **]**

20. simple-expression → additive-expression relop additive-expression | additive -expression 
21. relop →**<=** | **<** | **>** | **>=** | **==** | **!=**

22. additive-expression → additive-expression addop term | term

23. addop →**+** | **-**

24. term → term mulop factor | factor

25. mulop →***** | **/**

26. factor → **(** expression **)** | var | call | **NUM**

27. call → **ID** **(** args **)**

28. args → arg-list | empty

29. arg-list → arg-list **,** expression | expression

------

其中粗体表示终结符.

通过以上BNF语法可以看出, C-minus程序由声明序列组成, 而声明分为两种, 一种是变量声明, 另一种是函数声明. 声明一个变量需要指出变量的类型和变量名. 声明一个函数则需要指出返回值类型, 函数名以及参数列表, 如果识别出语句序列子结构, 也被归为函数声明. 语句序列子结构中有本地变量声明以及语句序列. 而语句又分为许多种.

语法分析器的输入是扫描器的输出, 即记号序列, 而扫描器的调用方式是获取下一个记号并存储在nexttoken变量中. 分析器可以不断调用这一过程, 并根据不断增长的记号序列来识别各种语法结构. 最终构建整个语法树.



## 实验原理

对于C-minus语言, 语法分析器可以用递归下降分析法来实现, 为了将递归下降算法实现为具体的程序, 需要将**BNF**语法改写为**EBNF**语法. EBNF语法用**重复和可选结构**来改写BNF中的**递归和选择**, 它们的符号分别是`{}`和`[]`, 这两个符号不会以粗体出现, 以与相应的终结符区分. 考虑程序的实现, 我们需要设计一个类来统一表示语法树结点的结构. **为了设计这个类, 首先需要根据语法来分析语法树结点的不同类型与结构.** 此处列出改写后的语法以及对结点类型和结构的分析:

------

1. program → declaration-list

2. declaration-list → declaration { declaration }

   一个程序可以组织为声明类型结点的链表, 即整颗语法树是一个单链表的形式, 因此declaration结点都需要**兄弟指针**.

3. declaration → var-declaration | fun-declaration

4. var-declaration → type-specifier **ID** [ **[NUM]** ] **;**

   注意此处嵌套的方括号, 外层的没有加粗, 表示其内部的结构是可选的.
   结构可以设计为存储**三个属性**, 分别是数据类型 `DataType{INT, VOID}`, 标识符的名称`string name`, 以及数组长度字面值`string arrlen`, 这里可以规定如果arrlen值为负就表示不是数组, 不需要存储子女结点, 即它一定是叶结点. 考虑到第2条规则, 为其添加**一个兄弟指针**.

5. type-specifier → **int** | **void**

   类型标识符不需要组织为语法树结点, 当作结点的**属性**存储即可, 可以单独声明为枚举类型`DataType{INT, VOID}`

6. fun-declaration → type-specifier **ID** **(** params **)**  compound-stmt

   此处中译本错误. 结构可以设计为存储**两个属性**, 与var-declaration的前两个属性完全相同, 它还有**两个子女指针**, 第一个子女是自己的参数列表, 可以利用param类型组织成一个单链表, 另一个是函数体, 详情见后. 还有**兄弟指针**.

7. params → params-list | **void**

8. param-list → param { **,** param }

9. param → type-specifier **ID** [ **[ ]** ]

   param结点结构可以设计为存储**三个属性**, 分别是数据类型 `DataType{INT, VOID}`, 标识符的名称`string name`, 以及标识参数是变量还是数组的布尔变量`bool kind`, 考虑到规则6, param结点需要链接成线性表, 为其添加**一个兄弟指针**, 如果链表为空即表示void.

10. compound-stmt → **{** local-declarations statement-list **}**
    该结构非常固定, 有且仅有两个有先后关系的子女, 分别是local-declarations和statement-list, 所以存储**两个子女指针**即可. 考虑到规则12, 还需要**兄弟指针**. 如果改进属性将其设计为声明与语句可以交替出现, 那么可以将其子女设计为一个链表, 对应的修改就是var-declaration的兄弟指针和statement的兄弟指针可以混用. 

11. local-declarations → { var-declaration }

12. statement-list → { statement }

    由于是重复结构, 在**所有statement类型结点中都需要一个兄弟指针**.

13. statement → expression-stmt | compound-stmt | selection-stmt| iteration-stmt | return-stmt

14. expression-stmt → [ expression ] **;** 

    表达式语句的结构可以设计为**一个子女指针**, 指向表达式结点, 若为空表示语句为空, 另一个是**兄弟指针**.

15. selection-stmt → **if** **(** expression **)** statement [ **else** statement ]

    选择结构可以设计为**三个子女指针**, 一个指针存储表达式结构, 一个指针存储真逻辑的语句, 一个指针存储假逻辑的语句, 还有**兄弟指针**. 

16. iteration -stmt → **while** **(** expression **)** statement

    循环结构有**两个子女指针**, 一个指向表达式结点, 另一个指向循环体语句. 还有**兄弟指针**. 

17. return -stmt → **return** [expression] **;**

    返回语句结点只需要**一个子女指针**, 指向表达式结点. 也需要**兄弟指针**.

18. expression → { var **=** } simple-expression

    表达式结点需要**两个子女指针**, 一个指针用于将左值链接成一个单链表, 另一个存储一个简单表达式结点. 由规则27可知, 还需要**兄弟指针**. 还可以有**一个属性**`int val`, 来存储表达式的值.

19. var → **ID** [ **[** expression **]** ]

    变量结点需要**一个属性**`string name`, 存储标识符字面值. 还有**一个子女指针**, 指向一个表达式或为空. 考虑到规则18, 还需要一个**兄弟指针**. 

20. simple-expression → additive-expression [ relop additive-expression ] 

    简单表达式结点需要**两个子女指针**指向两个加法表达式, 还有**一个属性**`string relop`存储算符.

21. relop →**<=** | **<** | **>** | **>=** | **==** | **!=**

22. additive-expression → term { addop term }

    虽然这里是重复结构, 但不设计为线性表, 而是按照BNF中的结构, 设置**两个子女指针和一个属性**, 分别存储additive-expression结点和乘积结点和算符. 	

23. addop →**+** | **-**

24. term → factor { mulop factor }

    与additive-expression相同, 不设计为线性表, 乘积结点需要**两个子女指针和一个属性**, 分别存储term结点, 因子结点和算符. 

25. mulop →***** | **/**

26. factor → **(** expression **)** | var | call | **NUM**

    这个识别过程说明需要为expression, var, call和常数四种结点设计一个统一的过程, 同时需要为NUM设计一个常数结点. 常数结点仅需要一个属性来存储字面值. 

27. call → **ID** **(** args **)**

    调用结点需要**一个属性**存储函数名, 还有**一个子女指针**将参数链接成单链表. 由于参数列表是由多个expression组成的, 因此需要在expression结点中添加兄弟指针.

28. args → arg-list | empty

29. arg-list → expression { **,** expression } 

------

​		结构不同的结点需要不同的识别过程, 然而程序中却有可能在同一个上下文中放置不同的结点, 比如声明序列中任意位置都可能是函数声明或变量声明, 这就要求在编程时为它们设计一个统一的过程, 或者在某些情况下当某个结点的第一个记号已经识别后仍不能确定结点的准确类型, 也需要为几个可选的节点类型设计一个统一的过程, 再用这个统一的过程去构建不同的结点. 当然这个统一的过程还可能调用子过程. 也就是说, 一个识别过程允许构造多种结点. 
​		同时还可能出现这种情况: 同一个结点类型, 在某些情况下可以(必须)从它的第一个记号开始识别, 比如形参列表的第二个形参, 而在另一些情况下却无法识别, 比如形参列表的第一个形参, 必须先识别数据类型后, 判断它是不是VOID, 才能确定是否构造形参结点. 因此, 一个类型的结点也允许多个识别过程构造.
​		而为结点分类的意义是语法树结点类型也是语法树结点的一种信息, 后边的步骤会需要用到.

​		接下来结合已得到的EBNF说明识别过程和结点的构造过程.

## 程序设计

​		这一部分说明各类结点的识别过程, 并转化为代码. 结点是按照它们的结构进行分类的, 但如何将某几类归为一大类则是需要结合识别过程来设计的. 这个归类过程的主要目的也是为了简化语法树的生成过程. 因此**先对语法进行综合分析, 然后给出需要的数据结构**. 在建立语法树的过程中需要不断开辟新的内存空间, 因此需要动态分配内存, 由于语法树需要一直保存到代码生成的步骤, 也就是说程序结束才需要释放内存, 因此可以不手动释放, 操作系统会在程序结束时自动释放内存. 

​		语法分析器识别一个声明序列, 而声明有两种识别过程, 一种是变量声明, 一种是函数声明, 设一个**类Declaration, 其下又分为var-dclr和fun-dclr两类**, 用以区分这两种识别构造出的结点. 考虑到声明序列的识别, 需要为两种声明的识别设计一个统一的过程Dclr(), 识别序列的过程只需要一直调用Dclr()即可. 识别序列的过程命名为Dclr_seq(). 建立语法树的过程, 命名为Parse(), 调用Dclr_seq().
​		对于过程Dclr()的设计, 由于Declaration有两种, 前两个记号完全相同, 如果将构造两种Declaration的过程都写成函数, 那么需要推迟到第三个记号时才可确定调用哪个过程, 即前两个记号的信息需要保存下来, 且两种声明的识别过程都需要参数来传递这两个记号的信息.
​		对于变量声明过程Var_dclr(), 开始处理的第一个记号是";"或"[", 按照规则处理即可. 对于函数声明过Fun_dclr(), 首先消耗了"(", 接着期望识别形参列表, 将其设计为一个函数Param_seq(), 它返回形参列表的第一个元素的指针, 将形参结点单独归为一类**Param**, Param_seq()中需要处理形参列表是VOID的情况, 也需要完成不为空的形参的链接, 在这个链接过程中, 对第一个形参, 因为构建它的时候已经识别了标识符, 所以它的识别过程跟一般的Param()不同. 在此处单独设计即可, 要注意形参可能是数组的情况, 对于多个形参的识别, 可以设计一个Param()过程, 然后用循环来调用, 循环条件与形参的分隔符","有关. 构建好形参列表后, 识别一个")", 接着构建函数体, 在构建之前可以先判断前导记号是否合法.
​		对于compound-stmt的识别, 考虑到之后的statement序列识别也有可能会用到, 将其设计为一个过程Compound_stmt(), 这个过程比较简单, 顺序构建两个子女即可, 分别调用Local_dclr()过程和Stmt_seq()过程.

​		先讨论Local_dclr()过程, 这个过程实际上是识别变量声明序列的, 但是却不能直接调用, 因为变量声明序列需要两个参数, 所以在Local_dclr()过程中应该参照Dclr_seq()和Dclr()过程中的设计, 建立本地变量声明的序列.
​		现在讨论Stmt_seq()过程, 这个过程是识别语句序列, 实际上是程序大多数情况下的主体, 需要不断识别语句, 终止条件除了右花括号以外可以添加几个比较简单的错误情况. 识别语句的过程不仅被Stmt_seq()调用, 因此必须将识别单个语句的过程单独设计一个过程Stmt(). 这个过程将识别不同语句的过程统一起来, 调用识别四种语句的过程Selection_stmt(), Iteration_stmt(), Return_stmt() ,Expression_stmt(). 根据以上分析, 可知需要设一个**类Statement, 其下分为expression-stmt, compound-stmt, selection-stmt, iteration-stmt, return-stmt五种结点**. 而Stmt_seq()的作用, 就是通过调用过程来把不同类型的语句结点链接到同一个语句序列上. 但是表达式语句没有明确的前导记号, 所以只能把不属于其他四种语句的情况全部归入表达式语句的构造过程, 让它去判断到底要不要构造表达式语句结点. 实际上, 复合语句的识别过程已经讨论, 除了复合语句和表达式语句的两外三种语句的结构比较简单, 按照EBNF语法分别设计识别过程即可, 对于表达式语句, 只有以分号开始时可以直接构造为空, 其他情况具体如何处理, 则交给表达式结点的识别过程.

​		对于expression的识别过程, 在很多地方都需要用到, 且expression的识别过程比较统一, 所以将expression设为结点**Expression**, 并将expression的识别过程设计为构造**Expression**结点的过程. 这个结点以一个可能为空的**Var结点**序列构成, 这种序列只有在此处会遇到, 为了方便设计一个独立的识别过程Var_seq().
​		Var_seq()很复杂, 因为Var序列是一个var结点和一个"="记号的重复序列, 但是后边紧跟简单表达式**Spexp**, 而简单表达式也有可能是Var, 这为Var序列识别的终止条件带来了很大的问题. Var_seq()中, 识别出一个Var结点后, 再识别一个"="记号, 如果此时下一个记号是var的前导记号, 那么并无法判断**是应该直接调用Var的识别过程并且把var结点加到序列末尾, 还是应该调用简单表达式的识别过程并连接到另一个子女指针**. 但是如果下一个记号不是Var的前导记号, 则一定需要终止var序列的识别过程, 开始进行下一步的操作. 那么到底何时应该继续var序列的识别过程? 实际上, 可以先不进行判断, 继续把它当作Var序列中的结点进行构造, 如果在过程中识别"="记号时发现实际记号不是"=", 那么就把已构造好但未链接到var序列末尾的这个结点当作简单表达式的第一个结点, 转而处理简单表达式, 也就是构造**Expression**的第二个子女, 而处理简单表达式的过程仅在此处出现, 它的设计需要针对此处的具体环境进行.
​		对于简单表达式**Spexp**的识别, 暂时不考虑上文提到的复杂环境, 先讨论它的一般结构, 考虑到结合性和优先级, 将其分为三种结点, 并对应三个层级的识别过程, **relexp结点**有两个叶结点子女**addexp结点指针**, **addexp结点**是一个递归的结构, 其叶结点是**terexp结点指针**, 也是递归结构的树, 其叶结点是factor结点. 顶层的识别过程为Spexp(), 返回一个简单表达式树, 这个简单表达式树可以是一个relexp树, 也可以是一个addexp树或terexp树. 第二层识别过程为Calexp(), 返回一个简单表达式树, 可以是addexp树或terexp树. 第三层识别过程为Terexp(), 识别的只可以是terexp结点, 这三层识别过程一起构造出完整的简单表达式树. 将这三种结点归为一大类**Spexp**. 
​		接下来需要处理上文提到的复杂环境, 即, 开始识别简单表达式时, nexttoken中存储的有可能已经不是结构的第一个记号. 第一种情况是在识别var序列的过程中"="后边根本就不是Var(的前导token), 此时可将nexttoken中的记号视为简单表达式的第一个记号进行识别. 第一种情况是识别var之后发现后边不是"=", 这个var结点就应期望是一个简单表达式的结点, 但此时它已经被识别, 不能用普通的识别过程识别简单表达式, 这个信息需要通过参数传入简单表达式识别过程中进行处理, 而因为Var是最底层的**terexp结点**识别过程中的情况, 因此这一参数需要**层层递进传入terexp()过程中**, 在这个过程中进行进一步处理. 除此之外, 还要考虑这一"异常信息"的来源是 Var_seq()函数, 而这个函数的返回值是语法树结点指针, 因此 Var_seq()过程也需要一个引用型的形参来将这一信息传给外界.

​		对于Factor识别过程, 一方面在前边的说明中是操作符表达式的叶结点, 一方面可以有多种不同结构, 也需要统一的识别过程, 因此将其归为一类, **Factor类分为expression, var, call, constnum**. 第一个token如果是"(", 则调用expression识别过程, 如果是NUM, 调用constnum识别过程, 但若是ID, 无法判断是Var结点还是call结点. 考虑到var结点的识别过程仅在识别表达式和此处需要调用, 而前者在该处不适用(原因是在意识到结点是个Var而不是call时,nexttoken已经不是结构中第一个记号了), 因此为此处的识别过程单独设计一个函数VarorCall(), 函数中处理这种情况下如何生成Var, 而由于Call结点的识别过程仅出现在此处, 因此可以将其识别过程集成在此函数中. 识别args的过程其实是将expression连成单链表的过程, 不需要单独设计结点. 需要注意的是这个列表可能是空的. args仅仅在此处识别, 根据具体环境进行设计即可.

​		各种结点所需的属性是不同的, 可以将需要多个属性的结点分别设计为不同的类, 然后不同的属性种类合并为一个联合体作为语法树结点的属性. 结点的类结构设计如下: (需要改动)

```c++
enum class NodeKind{
    Declaration, Statement, Param, Spexp, Factor
};
enum class Dclrkind{
    var_dclr, fun_dclr
};
enum class Stmtkind{
    expression_stmt, compound_stmt, selection_stmt, iteration_stmt, return_stmt
};
enum class Factorkind{
    var, expression, call, constnum
};
enum class Spexpkind{
    relexp, addexp, terexp
};
enum class DataType{INT, VOID};
int const MAXCHILDREN = 3; //最多有三个子女

class vdclrAttr{
public:
    DataType datatype; //变量声明类型
    string name; //变量名字符串
    string arrlen; //数组长度字面值
};
class fdclrAttr{
public:
    DataType datatype; //函数声明类型
    string name; //函数名
};
class paramAttr{
public:
    DataType datatype; //参数类型
    string name; //参数名
    bool isary; //是否是数组
};

class TreeNode{
public:
    TreeNode* child[MAXCHILDREN]; //子女结点
    TreeNode* sibling; //兄弟结点
    int lineno;//行号
    NodeKind nodekind; //结点类型
    union {
        Dclrkind dclr; Stmtkind stmt; Factorkind factor; Spexpkind spexp;
        bool rootkind;//是否根分类,比如Param就没有二级分类
    } kind;//这个语法并没有为联合体类型命名,直接声明了变量
    union Attr{
        vdclrAttr vdclrattr;
        fdclrAttr fdclrattr;
        paramAttr paramattr;
        string name;//var,call,constnum
        int val;//expression, constnum
        TokenType op;//记号类型?除了标识符,都可以用这个存
        Attr(){}
        ~Attr(){}
    } attr; //属性
	//Exptype;
};
```

函数的声明如下:

```c++
//parse.cpp中
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
//parse.h中
TreeNode* Parse();
//打印语法树
void PrintTree(TreeNode* tree, int layer);
```

​		

​		对于语法树的展示, 在建好语法树后, 结点的类型已经存入结点的数据结构, 可以用于判断并选择打印结点的方式, 所以可以将所有类型结点对打印的处理过程写在一个函数中, 然后分情况对自己进行递归调用.



==设计一个语法完全正确的且包括各种情况的程序作为成功的例子, 设计错误处理,给出处理错误的例子.==

​		错误处理: 如果有些终结符是用于判断结点类型,进而选择生成结点的函数的, 那么其错误处理需要一个单独的else语句来说明, 而如果是其他终结符, 比如为了补全格式的右方括号, 则直接调用match函数, 利用match函数自身进行处理.







## 程序测试

对于下面的例子进行测试, 这个例子基本包括了所有的语法, 且符合语法规则(不一定符合语义规则): 

```c
int gcd (int u, int v,int a[])
{ if (v == 
0) return
 u 
;
else return gcd(v,u-u/v*v);
/* u-u/v*v == 
u mod v */
}

int x; 
int y[3];

void main(void)
{ int x; int y;
x = input(); y = input();
output(gcd( x , y ));

while(x>y){
    int x;
    x=y[1];
    x=y[1]=x=y[1]+y[1+1];
    x=y[0]=1+9*3;
}

}

```



测试发现可以生成正确的语法树:(在文件testsrc\\syntax_sample.txt)

```
FUN_dclr:
  type: INT
  name: gcd
  Param_seq: 
    Param: INT u
    Param: INT v
    Param: INT [] a
  Fun_body:
    ComPD: 
      Locdclr: 
      Stmtlst: 
        IF:
          Condition:
            Exp: 
              left: null
              right: 
                Spexp: ==
                  Var: v
                  Const: 0
          Body:
            RETURN:
              Exp: 
                left: null
                right: 
                  Var: u
          ELSE:
            RETURN:
              Exp: 
                left: null
                right: 
                  CALL: gcd
                    Args:
                      Exp: 
                        left: null
                        right: 
                          Var: v
                      Exp: 
                        left: null
                        right: 
                          Spexp: -
                            Var: u
                            Spexp: *
                              Spexp: /
                                Var: u
                                Var: v
                              Var: v
VAR_dclr:
  type: INT
  name: x
  length: null
VAR_dclr:
  type: INT
  name: y
  length: 3
FUN_dclr:
  type: VOID
  name: main
  Param_seq: void
  Fun_body:
    ComPD: 
      Locdclr: 
        VAR_dclr:
          type: INT
          name: x
          length: null
        VAR_dclr:
          type: INT
          name: y
          length: null
      Stmtlst: 
        Expstmt:
          Exp: 
            left: 
              Var: x
            right: 
              CALL: input
                Args:
        Expstmt:
          Exp: 
            left: 
              Var: y
            right: 
              CALL: input
                Args:
        Expstmt:
          Exp: 
            left: null
            right: 
              CALL: output
                Args:
                  Exp: 
                    left: null
                    right: 
                      CALL: gcd
                        Args:
                          Exp: 
                            left: null
                            right: 
                              Var: x
                          Exp: 
                            left: null
                            right: 
                              Var: y
        WHILE:
          Condition:
            Exp: 
              left: null
              right: 
                Spexp: >
                  Var: x
                  Var: y
          Body:
            ComPD: 
              Locdclr: 
                VAR_dclr:
                  type: INT
                  name: x
                  length: null
              Stmtlst: 
                Expstmt:
                  Exp: 
                    left: 
                      Var: x
                    right: 
                      Var: y
                        index: 
                          Exp: 
                            left: null
                            right: 
                              Const: 1
                Expstmt:
                  Exp: 
                    left: 
                      Var: x
                      Var: y
                        index: 
                          Exp: 
                            left: null
                            right: 
                              Const: 1
                      Var: x
                    right: 
                      Spexp: +
                        Var: y
                          index: 
                            Exp: 
                              left: null
                              right: 
                                Const: 1
                        Var: y
                          index: 
                            Exp: 
                              left: null
                              right: 
                                Spexp: +
                                  Const: 1
                                  Const: 1
                Expstmt:
                  Exp: 
                    left: 
                      Var: x
                      Var: y
                        index: 
                          Exp: 
                            left: null
                            right: 
                              Const: 0
                    right: 
                      Spexp: +
                        Const: 1
                        Spexp: *
                          Const: 9
                          Const: 3

```



## 实验总结

​		怎样构造节点类结构是问题的关键, 观察教材的做法可以看出, 首先节点结构中一定有子树的指针或者后续节点的指针, 其次需要多级类型指明节点的具体类型, 最后需要有一个结构存储节点的附加属性, 还需要一个表达式类型作为后续的准备. 开始尝试时无从下手, 曾试图通过分层结构分析, 发现行不通, 后来认识到构造语法树结点的**目的**是通过一个统一的类来记录不同类型结点所需要存储的信息. 因此应该从逐条分析语法规则开始, 最后对分析进行总结. 

​		在本模块即将完成时我认识到, 该模块的设计最合理的步骤应该是: 
​		首先根据结点的结构对结点进行分类, 确定哪些结构是一个单独的结点, 哪些结构不需要单独设计结点. 最典型的情况就是那些顺序结构只需要组织成链表即可. 这个步骤中确定的结点分类是最低层级的结点分类, 也就是说还没有将小类归为大类, 比如, 没有将六种不同的语句归为一个语句类.
​		然后在设计识别过程时, 需要特别注意的就是同样的结点, 在不同的位置识别时有可能需要设计不同的函数, 比如在识别形参列表时, 若第一个形参是存在的, 那么识别它的过程与识别第二个形参以及以后的形参的过程应该设计为不同函数, 因为在识别第一个形参时, 其类型标识符已经被消耗, 而之后的形参则没有被消耗, 且所有形参不论位置在哪里, 识别完成时不应该消耗掉不属于它的记号.
​		除此之外, 在某些情况下还需要将不同结点的识别过程综合到一个识别过程中, 以便于重复调用, 构造节点序列. 这些在上文中也说明过. 而在编写程序的时候发现将某些类似的结点归为一个大类, 可以为程序设计带来方便, 并且增加可读性, 但至少在本阶段, 归为几个大类这一动作对程序实现来说不是必须的.