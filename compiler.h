#ifndef COMPILER_H
#define COMPILER_H

#define idlen 128//���ձ�ʶ������Ч����
#define tab_size 1024//���ű���󳤶�
#define midcode_size 4096//�м������󳤶�
#define info_size 512//������Ϣ���ȣ�������������
//��½�ķ��ű���������
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
//�м���������
#define CONST_INT 0		//argu1�����֣�argu2���ޣ�result���ޣ�value��ֵ
#define CONST_CHAR 1	
#define VAR_INT 2		//argu1�����֣�argu2���ޣ�result���ޣ�value����
#define VAR_CHAR 3		
#define INT_ARR 4		//argu1�����֣�argu2���ޣ�result���ޣ�value�泤��
#define CHAR_ARR 5		

#define ADDOP 6			//argu1�����1��argu2����2��result������value����
#define SUBOP 7			
#define MULOP 8			
#define DIVOP 9			

#define INT_FUNC 10
#define CHAR_FUNC 11
#define VOID_FUNC 12
#define PARA_INT 13
#define PARA_CHAR 14
#define CALL 15
#define PUSH 16
#define FUNCRET 17

#define VARASSIGN 18
#define ARRASSIGN 19
#define ARRACCESS 20

#define LESSOP 21		//argu1�����1��argu2����2��result���ޣ�value����
#define UNMOREOP 22
#define MOREOP 23
#define UNLESSOP 24
#define UNEQUOP 25
#define EQUOP 26

#define SETLABEL 27		//argu1��label��argu2���ޣ�result���ޣ�value����
#define GOTO 28
#define BNZ 29
#define BZ 30

#define SCANINT 31
#define SCANCHAR 32
#define PRINTINT 33
#define PRINTCHAR 34
#define PRINTSTR 35

#define RETEXPR 36
#define RETNULL 37

#define DEL 38
extern char chr;
extern char token[idlen];//�洢ÿһ����ȡ��symbol����ÿһ��getsym��ʼ��
extern int num;//the value of int
extern int lc;//��ǰ����
extern int funcnum;//ͳ�ƺ�������
extern int expr_is_char;//�жϱ��ʽ�ǲ��ǵ����ַ�
//ö������
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
//���ű�ṹ�����ر���
struct ttab {
	char ident[idlen];
	int type;//��������const_int:0, const_char:1, var_int:2, var_char:3, int_array:4, char_array:5, return_int_func:6,
			 //return_char_func:7, void_func:8, int_para:9, char_para:10;p.s. 9&10���������ڶ���ı���
	int value;//ֵ���߳���
	int addr;//ident���׵�ַ�������ƫ�Ƶ�ַ
	int size;//ռ�ÿռ��С
	int lev;//������ṹ���
};
extern int type;
extern int value;
extern int addr;
extern int lev;
extern struct ttab tab[tab_size];
extern int curloc;//���ű���
//�м����ṹ�����ر���
struct mmid {
	int type;
	char argu1[idlen];
	char argu2[idlen];
	char result[idlen];
	int value;//���ڴ泣����ֵ��������ĳ��ȣ�����Ĭ��Ϊ0
};
extern mmid midcode[midcode_size];
extern int midcodec;//�м���볤��
extern int id_name_num;//���ڸ�������ʱ����
extern int label_name_num;//���ڱ�ǩ����
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
void assignHandler(char* token);
void overallcase(char* op_tp, char* label_name);
void swicaseHandler();
void scanfHandler();
void printfHandler();
void returnHandler();
void realparaHandler(int j);
void callrefuncHandler(char* token_tp);
void callfuncHandler(char* token_tp);
void refuncHandler();
void unfuncHandler();
void program();

void entertab(char *ident, int type, int value, int addr, int lev);
int searchtab(char* ident, int funcnum);
void printtab();
void insert_midcode(int type, char* argu1, char* argu2, char* result, int value);
char* id_name_gen();
void print_midcode();
char* label_name_gen();
char* numtostr(int num);
void delconst();
void generate1();
char* str_name_gen();
void mips();
#endif // TEMP_H

