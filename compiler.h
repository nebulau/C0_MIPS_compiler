#ifndef COMPILER_H
#define COMPILER_H
#define idlen 64//���ձ�ʶ������Ч����
#define tab_size 512//���ű���󳤶�
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
	IDSY,       //14  ��ʶ�� 
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
	SEMISY,     //32  �ֺ�  
	INTVALUE,   //33  intֵ
	CHARVALUE,  //34  charֵ
	STRINGVALUE,//35  stringֵ
	LBRACE,     //36  {  
	RBRACE,     //37  }  
	COLONSY     //38  ð��
};
extern enum sym symbol;
//���ű��ṹ��
struct ttab {
	char ident[idlen];
	int type;//��������const_int:0, const_char:1, var_int:2, var_char:3, int_array:4, char_array:5, return_int_func:6,
			 //return_char_func:7, void_func:8, int_para:9, char_para:10;p.s. 9&10���������ڶ���ı���
	int value;//ֵ���߳���
	int addr;//ident���׵�ַ�������ƫ�Ƶ�ַ
	int size;//ռ�ÿռ��С
	int lev;//������ṹ���
};
extern char chr;
extern char token[idlen];//�洢ÿһ����ȡ��symbol����ÿһ��getsym��ʼ��
extern int num;//the value of int
extern int lc;//��ǰ����
//��ʱ�洢Ҫ����ű���ȫ�ֱ���
extern int type;
extern int value;
extern int addr;
extern int lev;
extern struct ttab tab[tab_size];
extern int curloc;//�洢��ǰ�ڷ��ű���λ��
//����һЩ�жϿո��кͼȶ��ս���ĺ���
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
//��������
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
#endif // TEMP_H
