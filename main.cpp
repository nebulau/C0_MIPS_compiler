#include <iostream>	
#include <fstream>
#include <string>
#include <stdio.h>
#include <string.h>
#include "compiler.h"
using namespace std;
//C:\workspace\vsapp\Project2\Project2\test.txt
ifstream file;
ofstream output;
string errmsg[50];
//定义全局变量
enum sym symbol;//每一次调用getsym初始化为NOTSY
char chr;
char token[idlen];//存储每一个获取的symbol，在每一次getsym初始化
int num = 0;//the value of int
int lc = 0;//当前行数
//临时存储要填符号表的全局变量
int type = 0;
int value = 0;
int addr = 0;
int lev = 0;
struct ttab tab[tab_size];
int curloc = 0;//存储当前在符号表的位置

//定义一些判断空格换行和既定终结符的函数
int isSpace() {
	return (chr == ' ') ? 1 : 0;
}
int isNewline() {
	return (chr == '\n') ? 1 : 0;
}
int isTab() {
	return (chr == '\t') ? 1 : 0;
}
int clearToken() {
	int i;
	for (i = 0; i < idlen; i++)   token[i] = '\0';
	return 0;
}
int isLetter() {
	return ((chr >= 'a'&&chr <= 'z') ||
		(chr >= 'A'&&chr <= 'Z') ||
		(chr == '_')) ? 1 : 0;
}
int isDigit() {
	return (chr >= '0'&&chr <= '9') ? 1 : 0;
}
int isDigit19() {
	return (chr > '0'&&chr <= '9') ? 1 : 0;
}
int isComma() {
	return (chr == ',') ? 1 : 0;
}
int isSemi() {
	return (chr == ';') ? 1 : 0;
}
int isPlus() {
	return (chr == '+') ? 1 : 0;
}
int isMinus() {
	return (chr == '-') ? 1 : 0;
}
int isStar() {
	return (chr == '*') ? 1 : 0;
}
int isDivi() {
	return (chr == '/') ? 1 : 0;
}
int isLpar() {
	return (chr == '(') ? 1 : 0;
}
int isRpar() {
	return (chr == ')') ? 1 : 0;
}
int isEqu() {
	return (chr == '=') ? 1 : 0;
}
int isLess() {
	return (chr == '<') ? 1 : 0;
}
int isMore() {
	return (chr == '>') ? 1 : 0;
}
int isRelation(int SYM) {
	return (SYM == LESSSY ||
		SYM == MORESY ||
		SYM == UNLESSSY ||
		SYM == UNMORESY ||
		SYM == UNEQUSY ||
		SYM == EQUSY) ? 1 : 0;
}

void nextch() {
	chr = file.get();
	if (chr == '\n')    lc++;
}
void skip() {
	while (chr != ';')  nextch();
	nextch();
}
void catToken() {	
	if(strlen(token)<idlen)	token[strlen(token)] = chr;	
}
enum sym reserver() {//查找保留字，返回保留字类别码，未查找到返回0
	if (strcmp(token, "const") == 0)   return CONSTSY;
	if (strcmp(token, "int") == 0)   return INTSY;
	if (strcmp(token, "char") == 0)   return CHARSY;
	if (strcmp(token, "void") == 0)   return VOIDSY;
	if (strcmp(token, "main") == 0)   return MAINSY;
	if (strcmp(token, "if") == 0)   return IFSY;
	if (strcmp(token, "while") == 0)   return WHILESY;
	if (strcmp(token, "switch") == 0)   return SWITCHSY;
	if (strcmp(token, "case") == 0)   return CASESY;
	if (strcmp(token, "default") == 0)   return DEFAULTSY;
	if (strcmp(token, "scanf") == 0)   return SCANFSY;
	if (strcmp(token, "printf") == 0)   return PRINTFSY;
	if (strcmp(token, "return") == 0)   return RETURNSY;
	return IDSY;
}
int transNum() {
	int i, value = 0;
	for (i = 0; token[i] != '\0'; i++) {
		value = value * 10 + token[i] - '0';
	}
	return value;
}
void setup() {
	errmsg[0] = "number starts with 0.";                      errmsg[1] = "unknown character.";
	errmsg[2] = "character doesn`t end with single quotark."; errmsg[3] = "undefied character in string.";
	errmsg[4] = "string doesn`t end with double quotark.";    errmsg[5] = "unkown nextch.";
	errmsg[6] = "missing int and char.";                      errmsg[7] = "illegal ident.";
	errmsg[8] = "missing =.";                                 errmsg[9] = "missing intvalue or charvalue.";
	errmsg[10] = "missing comma or semicolon.";               errmsg[11] = "missing charvalue.";
	errmsg[12] = "sth wrong follows const definition";        errmsg[13] = "illegal array length.";
	errmsg[14] = "missing ].";                                errmsg[15] = "missing (.";
	errmsg[16] = "unknown mistake.";						  errmsg[17] = "missing ).";
	errmsg[18] = "illegal expressions.";					  errmsg[19] = "missing ;.";
	errmsg[20] = "ttab is full, exit(1).";					  errmsg[21] = "len of defined array is 0.";
}
void errormsg(int n) {
	output << "error line:" << lc << ", " << errmsg[n] << '\n';
}
int getsym() {
	clearToken();
	symbol = NOTSY;
	while (isSpace() || isNewline() || isTab())    nextch();
	if (file.eof()) {
		output << "end of file, incomplete C0_program\n"; exit(0);
	}
	if (isLetter()) {
		while (isLetter() || isDigit() || (chr == '_')) { catToken(); nextch(); }
		symbol = reserver();
	}
	else if (isDigit()) {//不识别前导零整数，这里识别不带±的整数
		if (chr == '0') {
			catToken(); nextch();
			if (isDigit()) { skip(); symbol = NOTSY; errormsg(0); return 5; }
		}
		while (isDigit()) { catToken(); nextch(); }
		num = transNum();
		symbol = INTVALUE;
	}
	else if (isPlus())
		{symbol = PLUSSY; catToken(); nextch(); }
	
	else if (isMinus())
	{ symbol = MINUSSY; catToken(); nextch(); }
	else if (isStar()) { symbol = STARSY; catToken(); nextch(); }
	else if (isDivi()) { symbol = DIVISY; catToken(); nextch(); }
	else if (isLpar()) { symbol = LPARSY; catToken(); nextch(); }
	else if (isRpar()) { symbol = RPARSY; catToken(); nextch(); }
	else if (chr == '[') { symbol = LBRASY; catToken(); nextch(); }
	else if (chr == ']') { symbol = RBRASY; catToken(); nextch(); }
	else if (chr == '{') { symbol = LBRACE; catToken(); nextch(); }
	else if (chr == '}') { symbol = RBRACE; catToken(); nextch(); }
	else if (isComma()) { symbol = COMMASY; catToken(); nextch(); }
	else if (isSemi()) { symbol = SEMISY; catToken(); nextch(); }
	else if (isEqu()) {
		catToken();
		nextch();
		if (isEqu()) { symbol = EQUSY; catToken(); nextch(); }
		else { symbol = ASSIGNSY; }
	}
	else if (isLess()) {
		catToken();
		nextch();
		if (isEqu()) { symbol = UNMORESY; catToken(); nextch(); }
		else { symbol = LESSSY; }
	}
	else if (isMore()) {
		catToken();
		nextch();
		if (isEqu()) { symbol = UNLESSSY; catToken(); nextch(); }
		else { symbol = MORESY; }
	}
	else if (chr == '\'') {
		nextch();
		if (isLetter() | isDigit() | (chr == '_') | (chr == '+')
			| (chr == '-') | (chr == '*') | (chr == '/')) {
			catToken(); nextch();
		}
		else { skip(); symbol = NOTSY; errormsg(1); return 1; }
		if (chr != '\'') { skip(); symbol = NOTSY; errormsg(2); return 1; }
		else { symbol = CHARVALUE; nextch(); }
	}
	else if (chr == '\"') {
		nextch();
		while (chr != '\"') {
			if ((chr == 32) | (chr == 33) |
				(chr > 34 && chr < 127)) {
				catToken(); nextch();
			}//检查字符串是否合法
			else { skip(); symbol = NOTSY; errormsg(3); return 2; }
		}
		if (chr != '\"') { skip(); symbol = NOTSY; errormsg(4); return 2; }
		else { symbol = STRINGVALUE; nextch(); }
	}
	else if (chr == ':') { symbol = COLONSY; catToken(); nextch(); }
	else if (chr == '!') {
		nextch();
		if (chr == '=') { symbol = UNEQUSY; nextch(); }
		else { skip(); symbol = NOTSY; errormsg(16); return 2; }
	}
	else { skip(); symbol = NOTSY; errormsg(5); return 3; }//不符合文法的词开头
	return 0;
}
/*
	处理常量说明，filedflag为作用域
*/
void overallconst(int SYM, int fieldflag) {
	char token_tp[idlen] = { '\0' };//缓存ident
	int sym_tp;//缓存±号
	getsym();
	while (symbol != SEMISY) {
		if (symbol == IDSY) {
			strcpy_s(token_tp, token);
			getsym();
			if (symbol == ASSIGNSY) {
				getsym();
				if (symbol == INTVALUE) {
					entertab(token_tp, const_int, num, addr, fieldflag);//const_int登录符号表
					addr += 4;
					getsym();
					if (symbol == COMMASY) { getsym(); }
					else if (symbol != SEMISY) { skip(); errormsg(10); getsym(); return; }
				}
				else if (symbol == CHARVALUE) {
					entertab(token_tp, const_char, token[0], addr, fieldflag);//const_char登录符号表
					addr += 4;
					getsym();
					if (symbol == COMMASY) { getsym(); }
					else if (symbol != SEMISY) { skip(); errormsg(10); getsym(); return; }
				}
				//处理有符号整数
				else if (symbol == PLUSSY || symbol == MINUSSY) {
					sym_tp = symbol;//缓存±号
					getsym();
					if (symbol == INTVALUE) {
						if (sym_tp == MINUSSY) num = -num;
						entertab(token_tp, const_int, num, addr, fieldflag);//有±号整数登录符号表
						addr += 4;
						getsym();
						if (symbol == COMMASY) { getsym(); }
						else if (symbol != SEMISY) { skip(); errormsg(10); getsym(); return; }
					}
					else { skip(); errormsg(16); getsym(); return; }
				}
				else { skip(); errormsg(9); getsym(); return; }
			}
			else { skip(); errormsg(8); getsym(); return; }
		}
		else { skip(); errormsg(7); getsym(); return; }
	}
	output << "Line: " << lc << ", This is a const definition statement!" << '\n';
	getsym();
}
void constHandler(int fieldflag) {
	while (symbol == CONSTSY) {
		getsym();
		if (symbol == INTSY || symbol == CHARSY)
			overallconst(symbol, fieldflag);
		else { skip(); errormsg(6); getsym(); }
	}
}
/*
	处理变量说明
	接收;或者a, b, c ;
*/
void overallvar(int SYM, int fieldflag) {
	char token_tp[idlen] = {'\0'};//缓存ident
	int num_tp;//缓存数组长度
	while (symbol != SEMISY) {
		if (symbol == IDSY) {
			strcpy_s(token_tp, token);
			getsym();
			if (symbol == LBRASY) {//接收数组
				getsym();
				if (symbol == INTVALUE) {
					if(num == 0) { skip(); errormsg(21); getsym(); return; }
					num_tp = num;
					getsym();
					if (symbol == RBRASY) {
						entertab(token_tp, (SYM == INTSY) ? int_array : char_array, num, addr, fieldflag);
						addr += tab[curloc - 1].size;
						getsym(); 
					}
					else { skip(); errormsg(14); getsym(); return; }
				}
				else { skip(); errormsg(13); getsym(); return; }
			}
			//不是数组，登录ident到符号表
			else {
				entertab(token_tp, (SYM == INTSY) ? var_int : var_char, NULL, addr, fieldflag);
				addr += sizeof(int);
			}
			if (symbol == COMMASY) { getsym(); }
			else if (symbol != SEMISY) { skip(); errormsg(10); getsym(); return; }
		}
		else { skip(); errormsg(7); getsym(); return; }
	}
	output << "Line: " << lc << ", This is a var definition statement!" << '\n';
	getsym();
}
/*
	处理形参表，此时symbol = LPARSY
*/
void paramHandler() {
	getsym();
	while (symbol != RPARSY) {
		if (symbol == INTSY) {
			getsym();
			if (symbol == IDSY) {
				entertab(token, int_para, NULL, addr, 1);
				getsym();
				if (symbol == COMMASY) { getsym(); }
				else if (symbol != RPARSY) { skip(); errormsg(6); getsym(); return; }
			}
			else { skip(); errormsg(7); getsym(); return; }
		}
		else if (symbol == CHARSY) {
			getsym();
			if (symbol == IDSY) {
				entertab(token, char_para, NULL, addr, 1);
				getsym();
				if (symbol == COMMASY) { getsym(); }
				else if (symbol != RPARSY) { skip(); errormsg(6); getsym(); return; }
			}
			else { skip(); errormsg(7); getsym(); return; }
		}
		else { skip(); errormsg(6); getsym(); return; }
	}
	output << "Line: " << lc << ", This is a function definition statement!" << '\n';
	getsym();
}
void termHandler() {
	factorHandler();
	while (symbol == STARSY || symbol == DIVISY) {
		getsym();
		factorHandler();
	}
}
/*
	处理因子
*/
void factorHandler() {
	if (symbol == INTVALUE)	getsym();
	else if (symbol == CHARVALUE)	getsym();
	else if (symbol == IDSY) {
		getsym();
		//接收到数组元素
		if (symbol == LBRASY) {
			getsym();
			exprHandler();
			if (symbol == RBRASY) { getsym(); return; }
			else { skip(); errormsg(14); getsym(); return; }
		}
		//接受到有返回值函数调用语句
		else if (symbol == LPARSY) { callfuncHandler(); return; }
	}
	else if (symbol == LPARSY){
		getsym();
		exprHandler();
		if (symbol == RPARSY) { getsym(); }
		else { skip(); errormsg(17); getsym(); return; }
	}
	//接收有符号整数
	else if (symbol == PLUSSY || symbol == MINUSSY) {
		getsym();
		if (symbol == INTVALUE) {
			getsym();
		}
		else { skip(); errormsg(16); getsym(); return; }
	}
	else { skip(); errormsg(18); getsym(); return; }
}
/*
	处理表达式
	表达式处理只在该出现表达式的地方调用
*/
void exprHandler() {
	do{
		if (symbol == PLUSSY || symbol == MINUSSY)	getsym();
		termHandler();
	} while (symbol == PLUSSY || symbol == MINUSSY);
}
/*
	处理复合语句
*/
void statementHandler() {
	if (symbol == LBRACE) {
		getsym();
		do {
			statementHandler();
		} while (symbol != RBRACE);
		getsym();
	}
	else if (symbol == IFSY) {
		ifHandler();
	}
	else if (symbol == WHILESY) {
		whileHandler();
	}
	else if (symbol == SCANFSY) {
		scanfHandler();//要处理之后的分号
		if (symbol == SEMISY)	getsym();
		else { skip(); errormsg(19); getsym(); return; }
	}
	else if (symbol == PRINTFSY) {
		printfHandler();//要处理之后的分号
		if (symbol == SEMISY)	getsym();
		else { skip(); errormsg(19); getsym(); return; }
	}
	else if (symbol == SWITCHSY) {
		swicaseHandler();
	}
	else if (symbol == RETURNSY) {
		returnHandler();//要处理之后的分号
		if (symbol == SEMISY)  getsym(); 
		else { skip(); errormsg(19); getsym(); return; }
	}
	else if (symbol == SEMISY){
		getsym();//接收<空>;
	}
	else if (symbol == IDSY) {
		getsym();
		if (symbol == LPARSY) {
			realparaHandler();//要处理之后的分号
			if (symbol == SEMISY) { output << "Line: " << lc << ", This is a callfunc statement!" << '\n'; getsym(); }
			else { skip(); errormsg(19); getsym(); return; }
		}
		else {
			assignHandler();
			//要处理之后的分号
			if (symbol == SEMISY)	getsym();
			else { skip(); errormsg(16); getsym(); return; }
		}
	}
	else { skip(); errormsg(19); getsym(); return; }
}
/*
	处理if语句
*/
void ifHandler() {
	getsym();
	if (symbol == LPARSY) {
		getsym();
		exprHandler();
		if (isRelation(symbol)) {
			getsym();
			exprHandler();			
		}
		if (symbol == RPARSY) { output << "Line: " << lc << ", This is a if statement!" << '\n'; 
			getsym(); statementHandler(); }
		else { skip(); errormsg(17); getsym(); return; }	
	}
	else  { skip(); errormsg(15); getsym(); return; }
}
/*
	处理循环语句
*/
void whileHandler() {
	getsym();
	if (symbol == LPARSY) {
		getsym();
		exprHandler();
		if (isRelation(symbol)) {
			getsym();
			exprHandler();
		}
		if (symbol == RPARSY) { output << "Line: " << lc << ", This is a while statement!" << '\n'; getsym(); statementHandler(); }
		else { skip(); errormsg(17); getsym(); return; }
	}
	else {}
}
/*
	处理赋值语句
	此时要判断symbol是=还是[
*/
void assignHandler() {
	if (symbol == LBRASY) {
		getsym();
		exprHandler();
		if (symbol == RBRASY) { getsym(); }
		else { skip(); errormsg(16); getsym(); return; }
	}
	if (symbol == ASSIGNSY) {
		getsym();
		exprHandler();
		output << "Line: " << lc << ", This is a assign statement!" << '\n';
	}
	else { skip(); errormsg(16); getsym(); return; }
}
/*
	处理情况表情况及缺省
	接收到case调用
*/
void overallcase() {
	//todo至少有一个case
	if (symbol != CASESY) { skip(); errormsg(16); getsym(); return; }
	do{
		getsym();
		if (symbol == INTVALUE || symbol == CHARVALUE) {
			getsym();
			if (symbol == COLONSY) {
				getsym();
				statementHandler();
			}
			else{skip(); errormsg(16); getsym(); return;}
		}
		//接收有符号整数
		else if (symbol == PLUSSY || symbol == MINUSSY) {
			getsym();
			if (symbol == INTVALUE) {
				getsym();
				if (symbol == COLONSY) {
					getsym();
					statementHandler();
				}
				else { skip(); errormsg(16); getsym(); return; }
			}
			else { skip(); errormsg(16); getsym(); return; }
		}
		else{skip(); errormsg(16); getsym(); return;}	
	} while (symbol == CASESY);
	if (symbol == DEFAULTSY) {
		getsym();
		if (symbol == COLONSY) { getsym(); statementHandler(); }
		else { skip(); errormsg(16); getsym(); return; }
	}
	if (symbol == RBRACE) {
		output << "Line: " << lc << ", This is a switch_case statement!" << '\n';
		getsym();
	}
	else { skip(); errormsg(16); getsym(); return; }
}
/*
	处理情况语句
*/
void swicaseHandler() {
	getsym();
	if (symbol == LPARSY) {
		getsym();
		exprHandler();
		if (symbol == RPARSY) {
			output << "Line: " << lc << ", This is a switch statement!" << '\n';
			getsym();
			if (symbol == LBRACE) { getsym(); overallcase(); }
			else { skip(); errormsg(16); getsym(); return; }
		}
	}
	else {skip(); errormsg(15); getsym(); return; }
		
}
/*
	处理scanf和printf语句
*/
void scanfHandler() {
	getsym();
	if (symbol == LPARSY) {
		getsym();
		while (symbol != RPARSY) {
			if (symbol == IDSY) {
				getsym();
				if (symbol == COMMASY) { getsym(); }
				else if (symbol != RPARSY) { skip(); errormsg(6); getsym(); return; }
			}
			else { skip(); errormsg(6); getsym(); return; }
		}
		output << "Line: " << lc << ", This is a scanf statement!" << '\n';
		getsym();
	}
	else { skip(); errormsg(15); getsym(); return; }
}
void printfHandler() {
	getsym();
	if (symbol == LPARSY) {
		getsym();
		if (symbol == STRINGVALUE) {
			getsym();
			if (symbol == COMMASY) { getsym(); exprHandler(); }
			if (symbol == RPARSY) {
				output << "Line: " << lc << ", This is a printf statement!" << '\n';
				getsym();
			}
			else { skip(); errormsg(6); getsym(); return; }
		}
		else {
			exprHandler();
			if (symbol == RPARSY) {
				output << "Line: " << lc << ", This is a printf statement!" << '\n';
				getsym();
			}
			else { skip(); errormsg(6); getsym(); return; }
		}
	}
	else { skip(); errormsg(6); getsym(); return; }
}
/*
	处理返回语句
*/
void returnHandler() {
	getsym();
	if (symbol == LPARSY) {
		getsym();
		exprHandler();
		if (symbol == RPARSY) {
			output << "Line: " << lc << ", This is a return statement!" << '\n';
			getsym();
			return;
		}
		else { skip(); errormsg(17); getsym(); return; }
	}
	if (symbol == SEMISY)	output << "Line: " << lc << ", This is a return statement!" << '\n';
}
/*
	处理函数调用实参表
*/
void realparaHandler() {
	getsym();
	while (symbol != RPARSY) {
		exprHandler();
		if (symbol == COMMASY) { getsym(); exprHandler(); }
		else if (symbol != RPARSY) { skip(); errormsg(16); getsym(); return; }
	}
	getsym();
}
/*
	处理函数调用语句
	接收到 标识符( 开始
*/
void callfuncHandler() {
	realparaHandler();
}
/*
	处理有返回值函数定义
	接收到int 标识符(或者 char 标识符(开始调用，此时symbol=LPARSY
*/
void refuncHandler() {
	char token_tp[idlen] = { '\0' };//缓存token
	int sym_tp;//缓存变量是int还是char
	int num_tp;//缓存intvalue
	paramHandler();
	if (symbol == LBRACE) {
		getsym();
		constHandler(1);
		/*
			处理函数内变量声明
		*/
		while (symbol == INTSY || symbol == CHARSY) {
			sym_tp = symbol;
			getsym();
			if (symbol == IDSY) {//TODO 如果第一个var是数组怎么办，要处理所有进入overallvar的地方
				strcpy_s(token_tp, token);
				getsym();
				//判断是不是数组
				if (symbol == LBRASY) {
					getsym();
					if (symbol == INTVALUE) {
						if (num == 0) { skip(); errormsg(21); getsym(); return; }
						num_tp = num;
						getsym();
						if (symbol == RBRASY) {
							entertab(token_tp, (sym_tp == INTSY) ? int_array : char_array, num_tp, addr, 1);
							addr += tab[curloc - 1].size;
							getsym();
						}
						else { skip(); errormsg(14); getsym(); return; }
					}
					else { skip(); errormsg(13); getsym(); return; }
				}
				else {
					entertab(token_tp, (sym_tp == INTSY) ? var_int : var_char, NULL, addr, 1);
					addr += 4;
				}
				//
				if (symbol == COMMASY) { getsym(); overallvar(sym_tp, 1); }
				else if (symbol == SEMISY) {
					overallvar(sym_tp, 1);
				}
				else { skip(); errormsg(16); getsym(); continue; }
			}
			else { skip(); errormsg(16); getsym(); continue; }
		}
		do {
			statementHandler();
		} while (symbol != RBRACE);
		if (symbol == RBRACE) {
			getsym();
		}
		else { skip(); errormsg(16); getsym(); return; }
	}
	else { skip(); errormsg(16); getsym(); return; }
}
/*
	处理无返回值函数定义
*/
void unfuncHandler() {}
void program() {
	char token_tp[idlen] = { '\0' };//缓存token
	int sym_tp;//缓存变量是int还是char
	int num_tp;//缓存intvalue
	/*
		处理全局常量说明部分
	*/
	constHandler(0);
	/*
		处理全局变量说明部分，
		遇到标识符(退出循环进入有返回值函数处理部分
		遇到void进入无返回值函数处理部分或者是主函数部分
	*/
	while (true)
	{
		if (symbol == INTSY || symbol == CHARSY) {
			sym_tp = symbol;
			getsym();
			if (symbol == IDSY) {
				strcpy_s(token_tp, token);
				getsym();
				if (symbol == LPARSY)	break;
				//判断数组
				if (symbol == LBRASY) {
					getsym();
					if (symbol == INTVALUE) {
						if (num == 0) { skip(); errormsg(21); getsym(); continue; }
						num_tp = num;
						getsym();
						if (symbol == RBRASY) {
							entertab(token_tp, (sym_tp == INTSY) ? int_array : char_array, num_tp, addr, 0);
							addr += tab[curloc - 1].size;
							getsym();
						}
						else { skip(); errormsg(14); getsym(); return; }
					}
					else { skip(); errormsg(13); getsym(); continue; }
				}
				else {
					entertab(token_tp, (sym_tp == INTSY) ? var_int : var_char, NULL, addr, 0);
					addr += 4;
				}
				if (symbol == COMMASY) { getsym(); overallvar(sym_tp, 0); }
				else if (symbol == SEMISY) {
					overallvar(sym_tp, 0);
				}
				else { skip(); errormsg(16); getsym(); continue; }
			}
			else { skip(); errormsg(16); getsym(); continue; }
		}
		if (symbol == VOIDSY)	break;
	}
	/*
		遇到有返回值函数定义
		接收到标识符(调用refuncHandler
		如果接收到void进入无返回值函数处理部分或者是主函数部分
	*/
	if (symbol == LPARSY) {
		entertab(token_tp,
			(sym_tp == INTSY) ? return_int_func : return_char_func,
			NULL, addr, 0);
		refuncHandler();
	}
	else if (symbol != VOIDSY) { skip(); errormsg(16); getsym(); }

	while (true)
	{
		if (symbol == INTSY || symbol == CHARSY) {
			sym_tp = symbol;
			getsym();
			if (symbol == IDSY) {
				strcpy_s(token_tp, token);
				getsym();
				if (symbol == LPARSY) {
					entertab(token_tp,
						(sym_tp == INTSY) ? return_int_func : return_char_func,
						NULL, addr, 0);
					refuncHandler();
				}
				else { skip(); errormsg(16); getsym(); continue; }
			}
			else { skip(); errormsg(16); getsym(); continue; }
		}
		else if (symbol == VOIDSY) {
			getsym();
			if (symbol == MAINSY)	break;//处理到了主函数部分跳出循环
			if (symbol == IDSY) {
				entertab(token, void_func, NULL, addr, 0);
				getsym();
				if (symbol == LPARSY) {
					refuncHandler();
				}
				else { skip(); errormsg(16); getsym(); continue; }
			}
			else { skip(); errormsg(16); getsym(); continue; }
		}
		else { skip(); errormsg(16); getsym(); continue; }
	}
	output << "Line: " << lc << ", This is a mainfunc statement!" << '\n';
	entertab(token, void_func, NULL, addr, 0);
	getsym();
	if (symbol == LPARSY) { getsym(); }
	else { skip(); errormsg(15); getsym(); }

	if (symbol == RPARSY) { getsym(); }
	else { skip(); errormsg(17); getsym(); }

	if (symbol == LBRACE) {
		getsym();
		constHandler(1);
		/*
			处理主函数内变量声明
		*/
		while (symbol == INTSY || symbol == CHARSY) {
			sym_tp = symbol;
			getsym();
			if (symbol == IDSY) {
				strcpy_s(token_tp, token);
				getsym();
				//判断是不是数组
				if (symbol == LBRASY) {
					getsym();
					if (symbol == INTVALUE) {
						if (num == 0) { skip(); errormsg(21); getsym(); continue; }
						num_tp = num;
						getsym();
						if (symbol == RBRASY) {
							entertab(token_tp, (sym_tp == INTSY) ? int_array : char_array, num_tp, addr, 1);
							addr += tab[curloc - 1].size;
							getsym();
						}
						else { skip(); errormsg(14); getsym(); continue; }
					}
					else { skip(); errormsg(13); getsym(); continue; }
				}
				else {
					entertab(token_tp, (sym_tp == INTSY) ? var_int : var_char, NULL, addr, 1);
					addr += 4;
				}
				//
				if (symbol == COMMASY) { getsym(); overallvar(sym_tp, 1); }
				else if (symbol == SEMISY) {
					overallvar(sym_tp, 1);
				}
				else { skip(); errormsg(16); getsym(); continue; }
			}
			else { skip(); errormsg(16); getsym(); continue; }
		}
		do {
			statementHandler();
		} while (symbol != RBRACE);
		if (symbol == RBRACE) {
			output << "\n***************Finished***************\n" << '\n';
			while (isSpace() || isNewline() || isTab())	nextch();
			if(!file.eof())	output << "Sth wrong in the end of file.";		
		}
		else { skip(); errormsg(16); getsym(); }
	}
	else { skip(); errormsg(16); getsym(); }
}
int main()
{
	char path[100];
	cout << "Input file path within 100 characters.(e.g. C:\\User\\Doc\\test.txt)\nOutput file in output.txt" << endl;
	cin.getline(path, 100);
	file.open("test1.txt");
	output.open("output.txt");
	if (!file) { cout << "No such file!"; return -1; }//file does not exist
	setup();
	/*
		预读一个字符
	*/
	nextch();
	/*
		预读一个符号symbol
	*/
	getsym();
	program();
	printtab();
	file.close();
	output.close();
	//fclose(file);
	//system("pause");
	return 0;
}
