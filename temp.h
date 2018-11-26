#ifndef TEMP_H
#define TEMP_H
#define idlen 64//���ձ�ʶ������Ч����
#define tab_size 512//���ű���󳤶�

extern enum sym symbol;
//���ű�ṹ��
struct ttab {
	char ident[idlen];
	int type;//��������const int:0, const char:1, var int:2, var char:3, int_array:4, char_array:5, return_int_func:6,
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
//��ʱ�洢Ҫ����ű��ȫ�ֱ���
extern int type;
extern int value;
extern int addr;
extern int lev;
extern struct ttab tab[tab_size];
extern int curloc;//�洢��ǰ�ڷ��ű��λ��
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

