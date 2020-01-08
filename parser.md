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

结构不同的结点实际上都需要一个不同的识别过程, 然而程序中却有可能在同一个上下文中放置不同的结点, 比如声明序列中任意位置都可能是函数声明或变量声明, 这就要求在编程时为它们设计一个统一的过程, 当然这个统一的过程还可能调用子过程. 因此需要为某些结点分类, 以与识别过程相对应. (最后一句是对的吗)

首先程序识别一个声明序列, 所以设一个**类Declaration, 其下又分为var-dclr和fun-dclr两类**.
对于statement, 在重复时也需要识别不同的结构, 因此也归为一类, 设一个**类Statement, 其下分为expression-stmt, compound-stmt, selection-stmt, iteration-stmt, return-stmt**. 
param结点位置比较单一, 单独归为一类**Param**. 
对于简单表达式**Spexp**, 由于关系表达式不能形成递归形式的树, 因此将其分为两种, **relexp结点**最多有两个叶结点子女. **calexp结点**操作表达式归为一类的原因是它是一个可以无限递归的树形结构.

**将乘除和加减分开!!!!**

表达式的各结点之间的关系安排好.



因子结点一方面是操作符表达式的叶结点, 一方面可以有多种不同结构, 也需要统一的识别过程, 因此将其归为一类, **Factor类分为expression, var, call, constnum**. 而识别args的过程其实是将expression连成单链表的过程, 不需要单独设计结点.

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



## 程序设计

4程序的功能和程序说明：模块等

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