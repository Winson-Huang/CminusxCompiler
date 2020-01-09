2019.11.10 黄文诚

# 语法分析器部分(Parser)



## 实验目的

构造语法分析器的目的是从扫描器的输出(记号序列)中构造出语法树. 构造语法树的依据是C-Minus语言的BNF语法. 大体思路是根据该语言的BNF语法编写多个相互调用的识别过程, 并用最顶层的识别过程构造整个语法树. (待续)



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

结构不同的结点实际上都需要一个不同的识别过程, 然而程序中却有可能在同一个上下文中放置不同的结点, 比如声明序列中任意位置都可能是函数声明或变量声明, 这就要求在编程时为它们设计一个统一的过程, 当然这个统一的过程还可能调用子过程. 为结点分类的意义是语法树结点类型也是语法树结点的一种信息, 后边的步骤会需要用到.

(过渡一下



## 程序设计

4程序的功能和程序说明：模块等

结合识别过程讲

首先程序识别一个声明序列, 所以设一个**类Declaration, 其下又分为var-dclr和fun-dclr两类**.
对于statement, 在重复时也需要识别不同的结构, 因此也归为一类, 设一个**类Statement, 其下分为expression-stmt, compound-stmt, selection-stmt, iteration-stmt, return-stmt**. 
param结点位置比较单一, 单独归为一类**Param**. 



​		对于expression的识别过程, 在很多地方都需要用到, 且expression的识别过程比较统一, 所以将expression设为结点**Expression**, 并将expression的识别过程设计为构造**Expression**结点的过程. 这个结点以一个可能为空的**Var结点**序列构成, 这种序列只有在此处会遇到, 为了方便设计一个独立的识别过程Var_seq().
​		Var_seq()很复杂, 因为Var序列是一个var结点和一个"="记号的重复序列, 但是后边紧跟简单表达式**Spexp**, 而简单表达式也有可能是Var, 这为Var序列识别的终止条件带来了很大的问题. Var_seq()中, 识别出一个Var结点后, 再识别一个"="记号, 如果此时下一个记号是var的前导记号, 那么并无法判断**是应该直接调用Var的识别过程并且把var结点加到序列末尾, 还是应该调用简单表达式的识别过程并连接到另一个子女指针**. 但是如果下一个记号不是Var的前导记号, 则一定需要终止var序列的识别过程, 开始进行下一步的操作. 那么到底何时应该继续var序列的识别过程? 实际上, 可以先不进行判断, 继续把它当作Var序列中的结点进行构造, 如果在过程中识别"="记号时发现实际记号不是"=", 那么就把已构造好但未链接到var序列末尾的这个结点当作简单表达式的第一个结点, 转而处理简单表达式, 也就是构造**Expression**的第二个子女, 而处理简单表达式的过程仅在此处出现, 它的设计需要针对此处的具体环境进行.
​		对于简单表达式**Spexp**的识别, 暂时不考虑上文提到的复杂环境, 先讨论它的一般结构, 考虑到结合性和优先级, 将其分为三种结点, 并对应三个层级的识别过程, **relexp结点**有两个叶结点子女**addexp结点指针**, **addexp结点**是一个递归的结构, 其叶结点是**terexp结点指针**, 也是递归结构的树, 其叶结点是factor结点. 顶层的识别过程为Spexp(), 返回一个简单表达式树, 这个简单表达式树可以是一个relexp树, 也可以是一个addexp树或terexp树. 第二层识别过程为Calexp(), 返回一个简单表达式树, 可以是addexp树或terexp树. 第三层识别过程为Terexp(), 识别的只可以是terexp结点, 这三层识别过程一起构造出完整的简单表达式树. 将这三种结点归为一大类**Spexp**. 
​		接下来需要处理上文提到的复杂环境, 即, 开始识别简单表达式时, nexttoken中存储的有可能已经不是结构的第一个记号. 第一种情况是在识别var序列的过程中"="后边根本就不是Var(的前导token), 此时可将nexttoken中的记号视为简单表达式的第一个记号进行识别. 第一种情况是识别var之后发现后边不是"=", 这个var结点就应期望是一个简单表达式的结点, 但此时它已经被识别, 不能用普通的识别过程识别简单表达式, 这个信息需要通过参数传入简单表达式识别过程中进行处理, 而因为Var是最底层的**terexp结点**识别过程中的情况, 因此这一参数需要层层递进传入terexp()过程中, 在这个过程中进行进一步处理. 除此之外, 还要考虑这一"异常信息"的来源是 Var_seq()函数, 而这个函数的返回值是语法树结点指针, 因此 Var_seq()过程也需要一个引用型的形参来将这一信息传给外界.

​		对于Factor识别过程, 一方面在前边的说明中是操作符表达式的叶结点, 一方面可以有多种不同结构, 也需要统一的识别过程, 因此将其归为一类, **Factor类分为expression, var, call, constnum**. 第一个token如果是"(", 则调用expression识别过程, 如果是NUM, 调用constnum识别过程, 但若是ID, 无法判断是Var结点还是call结点. 考虑到var结点的识别过程仅在识别表达式和此处需要调用, 而前者在该处不适用(原因是在意识到结点是个Var而不是call时,nexttoken已经不是结构中第一个记号了), 因此为此处的识别过程单独设计一个函数VarorCall(), 函数中主要处理这种情况下如何生成Var, 而由于Call结点的识别过程仅出现在此处, 因此可以将其识别过程集成在此函数中.

==所以接下来就是处理实参列表和表达式, 在完成后检查所有过程==

hwangwc@qq.com

47921347+AXILUOWEI@users.noreply.github.com

​		而识别args的过程其实是将expression连成单链表的过程, 不需要单独设计结点.

各种结点所需的属性是不同的, 可以将需要多个属性的结点分别设计为不同的类, 然后不同的属性种类合并为一个联合体作为语法树结点的属性. 

则结点的类结构设计如下: (需要改动)

```c++
enum class NodeKind{
    Declaration, Statement, Param, Opexp, Factor
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
        Dclrkind dclr; Stmtkind stmt; Factorkind factor;
        bool mulayer;//是否多层
    } kind;//这个语法并没有为联合体类型命名,直接声明了变量
    union Attr{
        vdclrAttr vdclrattr;
        fdclrAttr fdclrattr;
        paramAttr paramattr;
        string name;//var,call,constnum
        int val;//expression, constnum
        TokenType op;//记号类型?
        Attr(){}
        ~Attr(){}
    } attr; //属性
	//Exptype;
};
```



​		这一部分说明各类结点的识别过程, 并转化为代码. 
在建立语法树的过程中需要不断开辟新的内存空间, 因此需要动态分配内存, 由于语法树需要一直保存到代码生成的步骤, 也就是说知道程序结束才需要释放内存, 因此可以不手动释放, 操作系统会在程序结束时自动释放内存. 
​		整个程序其实是一个由声明结点链接成的单链表, 因此构造语法树的最上层函数是识别声明的函数, 并且是一个尾递归函数(可以改写成循环). 
​		Declaration有两种, 但直到识别到第三个终结符时才能识别类别, 如果将构造两种Declaration的过程都写成函数, 那么该函数需要三个参数. 
​		可以在识别Param过程中检测形参类型.

​		在表达式的产生式expression → { var **=** } simple-expression中, 由于simple-expression也可能终结于var, 因此var序列的产生函数需要有参数.
​		simple-expression即程序中的Opexp结点, 实际上是由factor和算符组成的树状结构, 采用EBNF提供的重复结构, 可以在程序中利用循环很容易地完成优先度和结合性.







​		错误处理: 如果有些终结符是用于判断结点类型,进而选择生成结点的函数的, 那么其错误处理需要一个单独的else语句来说明, 而如果是其他终结符, 比如为了补全格式的右方括号, 则直接调用match函数, 利用match函数自身进行处理.







## 程序测试

5输入实例（至少一个成功和一个失败的例子）和运行结果





## 实验总结

6总结：得到的经验、遇到的问题、改进方案等

怎样构造节点类结构是问题的关键, 观察教材的做法可以看出, 首先节点结构中一定有子树的指针或者后续节点的指针, 其次需要多级类型指明节点的具体类型, 最后需要有一个结构存储节点的附加属性, 还需要一个表达式类型作为后续的准备

到底怎么从零构造语法树, 首先并不是每一条产生式都对应一种结点, 比如重复结构一般不需要单独设计结点, 选择结构也不需要, 但是没有固定的规则, 只能在设计时根据是否方便为原则灵活处理. 其次, 确定好到底哪些产生式需要设计结点之后, 为这些结点设计一个统一的结构, 声明为结点类, 然后对于其他未设计结点的产生式, 设计一些统一生成不同节点的函数, 当然这个函数中肯定需要一些子函数来分别生成不同种类的结点. 

个人认为构造语法树的关键在于构造语法树结点的结构, 最开始尝试时无从下手, 曾试图通过分层结构分析, 发现行不通, 后来认识到构造语法树结点的目的是通过一个统一的类来记录不同类型结点所需要存储的信息. 因此应该从逐条分析语法规则开始, 最后对分析进行总结. 

感觉语法分析和语义分析的关系比这一部分与扫描器的关系更密切. 