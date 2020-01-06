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

2. declaration-list → { declaration }

   一个程序可以组织为声明类型结点的链表, 即整颗语法树是一个单链表的形式. 

3. declaration → var-declaration | fun-declaration

4. var-declaration → type-specifier **ID** [ **[NUM]** ] **;**

   注意此处嵌套的方括号, 外层的没有加粗, 表示其内部的结构是可选的.
   结构可以设计为存储**三个属性**, 分别是数据类型 `DataType{INT, VOID}`, 标识符的名称`string name`, 以及数组长度`int arrlen`, 这里可以规定如果arrlen为负就表示不是数组, 不需要存储子女结点, 即它一定是叶结点. 考虑到第11条规则, 为其添加**一个子女指针**.

5. type-specifier → **int** | **void**

   类型标识符不需要组织为语法树结点, 当作结点的**属性**存储即可, 可以单独声明为枚举类型`DataType{INT, VOID}`

6. fun-declaration → type-specifier **ID** **(** params **)**  compound-stmt

   此处中译本错误. 结构可以设计为存储**两个属性**, 与var-declaration的前两个属性完全相同, 它还有**两个子女指针**, 第一个子女是自己的参数列表, 可以利用param类型组织成一个单链表, 另一个是函数体, 详情见后.

7. params → params-list | **void**

8. param-list → param { **,** param }

9. param → type-specifier **ID** [ **[ ]** ]

   结构可以设计为存储**三个属性**, 分别是数据类型 `DataType{INT, VOID}`, 标识符的名称`string name`, 以及标识参数是变量还是数组的布尔变量`bool kind`, 考虑到param结点需要链接成线性表, 为其添加**一个兄弟指针**`treeNode* sibling`, 如果链表为空即表示void.

10. compound-stmt → **{** local-declarations statement-list **}**
    该结构非常固定, 有且仅有两个有先后关系的子女, 分别是local-declarations和statement-list, 所以存储**两个子女指针**即可. 考虑到规则12, 还需要**兄弟指针**. 如果改进属性将其设计为声明与语句可以交替出现, 那么可以将其子女设计为一个链表, 对应的修改就是var-declaration的兄弟指针和statement的兄弟指针可以混用. 

11. local-declarations → { var-declaration }

    该结构也是单链表结构, 不需要单独设计结点, 直接在var-declaration中添加一个兄弟指针即可. 

12. statement-list → { statement }

    同上, 在**所有statement类型结点中都需要一个兄弟指针**.

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

    表达式结点需要**两个子女指针**, 一个指针用于将左值链接成一个单链表, 另一个存储一个简单表达式结点. 由规则27可知, 还需要**兄弟指针**. 

19. var → **ID** [ **[** expression **]** ]

    变量结点需要**一个属性**`string name`, 存储标识符字面值. 还有**一个子女指针**, 指向一个表达式或为空. 考虑到规则18, 还需要一个**兄弟指针**. 

20. simple-expression → additive-expression [ relop additive-expression ] 

    简单表达式结点需要**两个子女指针**指向两个加法表达式, 还有**一个属性**`string relop`存储算符.

21. relop →**<=** | **<** | **>** | **>=** | **==** | **!=**

22. additive-expression → term { addop term }

    加法表达式需要**两个子女指针和一个属性**, 分别存储两个乘积结点和算符. 

23. addop →**+** | **-**

24. term → factor mulop [ factor ]

    乘积结点需要**两个子女指针和一个属性**, 分别存储两个因子结点和算符. 

25. mulop →***** | **/**

26. factor → **(** expression **)** | var | call | **NUM**

    因子结点需要**一个子女指针**或**一个属性**, 指针存储expression或var或call, 属性存储NUM的字面值或者数值. 

27. call → **ID** **(** args **)**

    调用结点需要**一个属性**存储函数名, 还有**一个子女指针**将参数链接成单链表. 由于参数列表是由多个expression组成的, 因此需要在expression结点中添加兄弟指针.

28. args → arg-list | empty

29. arg-list → expression { **,** expression } 

------

考虑到程序的实现, 我们需要声明一个类统一表示节点的结构, 而从它的语法可以看出, 语法树的节点类型非常多, 在这里我们采用教材中的办法, 即: 将类型分级表示.



大错特错!!!!并不是分级表示, 而是分类表示, C- 语言的语法树节点只可能是declaration, exp, stmt, declaration有两种, exp有四种 assign const op id, stmt有五种  注意赋值语句与赋值表达式是不同的.

如果采用教材中的办法, 就需要为语法树的结点进行分类, 分类的依据主要是结点的结构. 分析如下: 语法树的结点首先可以分为三个大类: `NodeKind{declaration, exp, stmt}`, 其中declaration分为`declaration{vardclr, fundclr, param}`, exp分为`exp{assign, const, op, id}`, stmt有分为`stmt{expression, compound, selection, iteration, return-stmt}`. (怎么得到的?归纳不同类型的结构)

var类型结点. 

declaration::vardclr的结构是存储三个属性, 分别是数据类型 `DataType{INT, VOID}`, 标识符的名称`string name`, 以及数组长度`int arrlen`, 这里可以规定如果arrlen为负就表示不是数组, 不需要存储子女结点, 即它一定是叶结点. 
declaration::param的结构是存储三个属性, 分别是数据类型 `DataType{INT, VOID}`, 标识符的名称`string name`, 以及标识参数是变量还是数组的布尔变量`bool kind`, 考虑到param结点需要链接成线性表, 为其添加一个兄弟指针`treeNode* sibling`. 
declaration::fundclr的结构是存储两个属性, 与declaration::vardclr的前两个属性完全相同, 它还有两个子女指针, 第一个子女是自己的参数列表, 可以利用declaration::param类型组织成一个单链表, 另一个是函数体, 详情见后.
exp::assign的结构比较简单, 可以添加一个属性`int val`, 来存储表达式的值, 然后两个子女指针分别为变量链表的头指针和表达式结点指针. 





compound-stmt该结构非常固定, 有且仅有两个有先后关系的子女, 分别是local-declarations和statement-list. 









## 程序设计

4程序的功能和程序说明：模块等



## 程序测试

5输入实例（至少一个成功和一个失败的例子）和运行结果





## 实验总结

6总结：得到的经验、遇到的问题、改进方案等

怎样构造节点类结构是问题的关键, 观察教材的做法可以看出, 首先节点结构中一定有子树的指针或者后续节点的指针, 其次需要多级类型指明节点的具体类型, 最后需要有一个结构存储节点的附加属性, 还需要一个表达式类型作为后续的准备

个人认为构造语法树的关键在于构造语法树结点的结构, 最开始尝试时无从下手, 曾试图通过分层结构分析, 发现行不通, 后来认识到构造语法树结点的目的是通过一个统一的类来记录不同类型结点所需要存储的信息. 因此应该从逐条分析语法规则开始, 最后对分析进行总结. 

