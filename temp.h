#ifndef TEMP_H
#define TEMP_H
#define idlen 64//接收标识符的有效长度
#define tab_size 512//符号表最大长度

extern enum sym symbol;
//符号表结构体
struct ttab {
	char ident[idlen];
	int type;//数据类型const int:0, const char:1, var int:2, var char:3, int_array:4, char_array:5, return_int_func:6,
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
void overallvar(int fieldflag);
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
int search(char* ident, int filed);
#endif // TEMP_H

