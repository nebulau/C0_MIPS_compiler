#ifndef COMPILER_H
#define COMPILER_H

#define idlen 128//接收标识符的有效长度
#define tab_size 1024//符号表最大长度
#define midcode_size 4096//中间代码最大长度
//登陆的符号表的项的类型
#define const_int 0
#define const_char 1
#define var_int 2
#define var_char 3
#define int_array 4
#define char_array 5
#define return_int_func 6
#define return_char_func 7
#define void_func 8
#define int_para 9
#define char_para 10
//中间代码的类型
#define CONST_INT 0
#define CONST_CHAR 1
#define VAR_INT 2
#define VAR_CHAR 3
#define ADDOP 4
#define SUBOP 5
#define MULOP 6
#define DIVOP 7
#define INT_FUNC 8
#define CHAR_FUNC 9
#define VOID_FUNC 10
#define CALL 11
#define PUSH 12
#define FUNCRET 13
#define VARASSIGN 14
#define ARRASSIGN 15
#define ARRACCESS 16
#define MOREOP 17
#define UNMOREOP 18
#define LESSOP 19
#define UNLESSOP 20
#define EQUOP 21
#define UNEQUOP 22

enum sym {
	NOTSY,      //0  not symbol      
	CONSTSY,    //1  const     
	INTSY,      //2  int    
	CHARSY,     //3  char 
	VOIDSY,     //4  void 
	MAINSY,     //5  main 
	IFSY,       //6  if 
	WHILESY,    //7  while 
	SWITCHSY,   //8  switch
	CASESY,     //9  case 
	DEFAULTSY,  //10  default 
	SCANFSY,    //11  scanf 
	PRINTFSY,   //12  printf 
	RETURNSY,   //13 return 
	IDSY,       //14  标识符 
	UNDERLINESY,//15  _
	PLUSSY,     //16  + 
	MINUSSY,    //17  - 
	STARSY,     //18  * 
	DIVISY,     //19  /
	LESSSY,     //20  <
	UNMORESY,   //21  <=
	MORESY,     //22  >
	UNLESSSY,   //23 >=
	UNEQUSY,    //24  !=
	EQUSY,      //25  ==
	ASSIGNSY,   //26  =
	LPARSY,     //27  (
	RPARSY,     //28  )
	LBRASY,     //29  [  
	RBRASY,     //30  ]
	COMMASY,    //31  ,
	SEMISY,     //32  分号  
	INTVALUE,   //33  int值
	CHARVALUE,  //34  char值
	STRINGVALUE,//35  string值
	LBRACE,     //36  {  
	RBRACE,     //37  }  
	COLONSY     //38  冒号
};
extern enum sym symbol;
//符号表结构体和相关变量
struct ttab {
	char ident[idlen];
	int type;//数据类型const_int:0, const_char:1, var_int:2, var_char:3, int_array:4, char_array:5, return_int_func:6,
			 //return_char_func:7, void_func:8, int_para:9, char_para:10;p.s. 9&10看作函数内定义的变量
	int value;//值或者长度
	int addr;//ident的首地址或者相对偏移地址
	int size;//占用空间大小
	int lev;//作用域结构层次
};
extern char chr;
extern char token[idlen];//存储每一个获取的symbol，在每一次getsym初始化
extern int num;//the value of int
extern int lc;//当前行数
//临时存储要填符号表的全局变量
extern int type;
extern int value;
extern int addr;
extern int lev;
extern struct ttab tab[tab_size];
extern int curloc;//存储当前在符号表的位置
//中间代码结构体和相关变量
struct mmid {
	int type;
	char argu1[idlen];
	char argu2[idlen];
	char result[idlen];
	int value;//用于存常量的值或者数组的长度，其他默认为0
};
extern mmid midcode[midcode_size];
extern int midcodec;//中间代码计数
extern int id_name_num;//用于给变量临时起名
//声明一些判断空格换行和既定终结符的函数
int isSpace();
int isNewline();
int isTab();
int clearToken();
int isLetter();
int isDigit();
int isDigit19();
int isComma();
int isSemi();
int isPlus();
int isMinus();
int isStar();
int isDivi();
int isLpar();
int isRpar();
int isEqu();
int isLess();
int isMore();
int isRelation(int SYM);
//函数声明
void nextch();
void skip();
void catToken();
enum sym reserver();
int transNum();
void setup();
void errormsg(int n);
int getsym();
void overallconst(int SYM, int fieldflag);
void constHandler(int fieldflag);
void overallvar(int SYM, int fieldflag);
void paramHandler();
void termHandler();
void factorHandler();
void exprHandler();
void statementHandler();
void ifHandler();
void whileHandler();
void assignHandler();
void overallcase();
void swicaseHandler();
void scanfHandler();
void printfHandler();
void returnHandler();
void realparaHandler();
void callfuncHandler();
void refuncHandler();
void unfuncHandler();
void program();
void entertab(char *ident, int type, int value, int addr, int lev);
int searchtab(char* ident, int filed);
void printtab();
char* id_name_gen();
#endif // TEMP_H

