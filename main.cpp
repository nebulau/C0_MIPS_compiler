#include <iostream>	
#include <fstream>
#include <string>
#include <stdio.h>
#include <string.h>
#include "compiler.h"
using namespace std;
//C:\workspace\vsapp\Project2\Project2\16061196_test.txt
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
int funcnum = 0;
int expr_is_char = 0;//判断表达式是不是单个字符
int occur_ret = 0;

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
	//while(chr == ' ' || chr == '\t' || chr == '\n')
		chr = file.get();
	if (chr == '\n')    lc++;
}
void skip() {
	while (chr != ';')  nextch();
	nextch();
}
/*
	跳读直到遇到下一个正确语法成分的开头，
	用initializer_list实现可变参数
*/
void skipsym(std::initializer_list<int> const &items) {
	const int* p;
	int flag = 0;
	do {
		getsym();
		for (p = items.begin(); p != items.end(); p++) {
			if (symbol == *p) {
				flag = 1; break;
			}
		}
	} while (flag == 0);	
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
	errmsg[22] = "the same ident registered to ttab.";		  errmsg[23] = "left of assignment not in tab.";
	errmsg[25] = "char assigned to int_array.";				  errmsg[26] = "this array not in tab.";
	errmsg[27] = "char assigned to var_int.";                 errmsg[28] = "int assigned to var_char.";
	errmsg[29] = "this var not in tab.";					  errmsg[30] = "undefined function.";
	errmsg[31] = "function without return value";			  errmsg[32] = "type mismatch.";
	errmsg[33] = "para mismatch.";
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
		if (isLetter() || isDigit() || (chr == '_') || (chr == '+')
			|| (chr == '-') || (chr == '*') || (chr == '/')) {
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
			/*else if (chr == '\\') {
				token[strlen(token)] = '\\';
			}*/
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
					insert_midcode(CONST_INT, token_tp, NULL, NULL, num);
					getsym();
					if (symbol == COMMASY) { getsym(); }
					else if (symbol != SEMISY) { skip(); errormsg(10); getsym(); return; }
				}
				else if (symbol == CHARVALUE) {
					entertab(token_tp, const_char, token[0], addr, fieldflag);//const_char登录符号表
					addr += 4;
					insert_midcode(CONST_CHAR, token_tp, NULL, NULL, token[0]);
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
						insert_midcode(CONST_INT, token_tp, NULL, NULL, num);
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
						insert_midcode((SYM == INTSY) ? INT_ARR : CHAR_ARR,
							token_tp, NULL, NULL, num_tp);//TODO 0.20
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
				insert_midcode((SYM == INTSY) ? VAR_INT : VAR_CHAR,
					token_tp, NULL, NULL, 0);
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
				insert_midcode(PARA_INT, token, NULL, NULL, 0);
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
				insert_midcode(PARA_CHAR, token, NULL, NULL, 0);
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
	expr_is_char = 0;
	char factor_tp[idlen];//把上一个因子结果名字存起来
	int op_tp;//缓存* / 
	factorHandler();
	strcpy_s(factor_tp, midcode[midcodec - 1].result);
	while (symbol == STARSY || symbol == DIVISY) {
		op_tp = (symbol == STARSY) ? MULOP : DIVOP;
		expr_is_char = 0;
		getsym();
		factorHandler();
		insert_midcode(op_tp, factor_tp, midcode[midcodec - 1].result,
			id_name_gen(), 0);
		strcpy_s(factor_tp, midcode[midcodec - 1].result);
	}
}
/*
	处理因子
*/
void factorHandler() {
	expr_is_char = 0;
	char token_tp[idlen];//缓存当前标识符名
	int sym_tp;//缓存当前标识符类别码
	int j;//返回数组或者函数在tab表的index
	if (symbol == INTVALUE) {
		insert_midcode(VARASSIGN, numtostr(num), NULL, id_name_gen(), 0);
		getsym();
	}
	else if (symbol == CHARVALUE ) {
		insert_midcode(VARASSIGN, numtostr(token[0]), NULL, id_name_gen(), 0);//以int值存char，tothink
		getsym();
		expr_is_char = 1;
	}
	else if (symbol == IDSY) {//ident类型可能为const_int, const_char, var_int, var_char,
							  //int_array, char_array, return_int_func, return_char_func, 
							  //int_para, char_para
		strcpy_s(token_tp, token);
		getsym();
		//接收到数组元素
		if (symbol == LBRASY) {
			getsym();
			exprHandler();
			if (symbol == RBRASY) { 
				j = searchtab(token_tp, funcnum);
				if(j == -1) { skip(); errormsg(26); getsym(); return; }
				if (tab[j].type != int_array && tab[j].type != char_array) {
					skip(); errormsg(26); getsym(); return;
				}
				if (tab[j].type == char_array)
					expr_is_char = 1;
				insert_midcode(ARRACCESS, token_tp, midcode[midcodec - 1].result, id_name_gen(), 0);
				getsym(); return; 
			}
			else { skip(); errormsg(14); getsym(); return; }
		}
		//接受到有返回值函数调用语句 TODO函数调用
		else if (symbol == LPARSY) { 
			j = searchtab(token_tp, funcnum);
			if (j == -1) { skip(); errormsg(30); getsym(); return; }
			if (tab[j].type != return_int_func && tab[j].type != return_char_func) {
				skip(); errormsg(30); getsym(); return; 
			}
			callrefuncHandler(token_tp); 
			if (tab[j].type == return_char_func)
				expr_is_char = 1;
			insert_midcode(VARASSIGN, midcode[midcodec - 1].result, NULL, id_name_gen(), 0);
			return; 
		}
		//既不是数组也不是函数，则是普通ident继续处理，也可能是char
		j = searchtab(token_tp, funcnum);
		if (j == -1) { skip(); errormsg(29); getsym(); return; }
		if (!(tab[j].type == const_int || tab[j].type == const_char ||
			  tab[j].type == var_int || tab[j].type == var_char || 
		      tab[j].type == int_para || tab[j].type == char_para)) {
			skip(); errormsg(16); getsym(); return;
		}
		if (tab[j].type == const_char || tab[j].type == var_char || tab[j].type == char_para)
			expr_is_char = 1;
		insert_midcode(VARASSIGN, token_tp, NULL, id_name_gen(), 0);
	}
	else if (symbol == LPARSY){
		getsym();
		exprHandler();
		expr_is_char = 0;
		insert_midcode(VARASSIGN, midcode[midcodec - 1].result, NULL, id_name_gen(), 0);
		if (symbol == RPARSY) { getsym(); }
		else { skip(); errormsg(17); getsym(); return; }
	}
	//接收有符号整数
	else if (symbol == PLUSSY || symbol == MINUSSY) {
		sym_tp = symbol;
		getsym();
		if (symbol == INTVALUE) {
			if (sym_tp == MINUSSY)	num = -num;
			insert_midcode(VARASSIGN, numtostr(num), NULL, id_name_gen(), 0);
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
	expr_is_char = 0;
	char term_tp[idlen];//缓存上一个term的结果名
	int is_minus = 0;
	int sym_tp = 0;
	if (symbol == PLUSSY || symbol == MINUSSY) {
		sym_tp = symbol;
		expr_is_char = 0;
		getsym();
		//如果遇到-，看作0 - 
		if (sym_tp == MINUSSY) {
			strcpy_s(term_tp, id_name_gen());
			is_minus = 1;
			insert_midcode(VARASSIGN, numtostr(0), NULL, term_tp, 0);
		}
	}
	termHandler();
	if (is_minus) {
		insert_midcode(SUBOP, term_tp, midcode[midcodec - 1].result, id_name_gen(), 0);
	}
	strcpy_s(term_tp, midcode[midcodec - 1].result);
	while (symbol == PLUSSY || symbol == MINUSSY) {
		sym_tp = symbol;
		getsym();
		termHandler();
		expr_is_char = 0;
		insert_midcode((sym_tp == MINUSSY) ? SUBOP : ADDOP, term_tp, midcode[midcodec - 1].result, id_name_gen(), 0);
		strcpy_s(term_tp, midcode[midcodec - 1].result);
	}
	//do{
	//	if (symbol == PLUSSY || symbol == MINUSSY)	getsym();
	//	termHandler();
	//	if (symbol == PLUSSY || symbol == MINUSSY)	expr_is_char = 0;
	//} while (symbol == PLUSSY || symbol == MINUSSY);
}
/*
	处理复合语句
*/
void statementHandler() {
	char token_tp[idlen];
	if (symbol == LBRACE) {
		getsym();
		//接收语句{}
		if (symbol == RBRACE) {
			getsym();
			return;
		}
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
		occur_ret = 1;
		returnHandler();//要处理之后的分号
		if (symbol == SEMISY)  getsym(); 
		else { skip(); errormsg(19); getsym(); return; }
	}
	else if (symbol == SEMISY){
		getsym();//接收语句 <空>;
	}
	else if (symbol == IDSY) {
		strcpy_s(token_tp, token);
		getsym();
		if (symbol == LPARSY) {//函数调用语句
			callfuncHandler(token_tp);//要处理之后的分号		
			if (symbol == SEMISY) { output << "Line: " << lc << ", This is a callfunc statement!" << '\n'; getsym(); }
			else { skip(); errormsg(19); getsym(); return; }
		}
		else {
			assignHandler(token_tp);
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
	char largu[idlen];
	char rargu[idlen];
	char label_name[idlen];
	int sym_to_op;
	strcpy_s(label_name, label_name_gen());
	getsym();
	if (symbol == LPARSY) {
		getsym();
		exprHandler();
		strcpy_s(largu, midcode[midcodec - 1].result);
		if (isRelation(symbol)) {
			sym_to_op = symbol;
			getsym();
			exprHandler();			
			strcpy_s(rargu, midcode[midcodec - 1].result);
			insert_midcode(sym_to_op + 1, largu, rargu, NULL, 0);
			insert_midcode(BNZ, label_name, NULL, NULL, 0);
		}
		else {
			insert_midcode(EQUOP, largu, NULL, NULL, 0);
			insert_midcode(BZ, label_name, NULL, NULL, 0);
		}
		if (symbol == RPARSY) { 
			output << "Line: " << lc << ", This is a if statement!" << '\n'; 
			getsym(); 
			statementHandler(); 
			insert_midcode(SETLABEL, label_name, NULL, NULL, 0);
		}
		else { skip(); errormsg(17); getsym(); return; }	
	}
	else  { skip(); errormsg(15); getsym(); return; }
}
/*
	处理循环语句
*/
void whileHandler() {
	char label_name1[idlen];
	char label_name2[idlen];
	char largu[idlen];
	char rargu[idlen];
	int sym_to_op;
	strcpy_s(label_name1, label_name_gen());
	strcpy_s(label_name2, label_name_gen());
	getsym();
	if (symbol == LPARSY) {
		getsym();
		insert_midcode(SETLABEL, label_name1, NULL, NULL, 0);
		exprHandler();
		strcpy_s(largu, midcode[midcodec - 1].result);
		if (isRelation(symbol)) {
			sym_to_op = symbol;
			getsym();
			exprHandler();
			strcpy_s(rargu, midcode[midcodec - 1].result);
			insert_midcode(sym_to_op + 1, largu, rargu, NULL, 0);
			insert_midcode(BNZ, label_name2, NULL, NULL, 0);
		}
		else {
			insert_midcode(EQUOP, largu, NULL, NULL, 0);
			insert_midcode(BZ, label_name2, NULL, NULL, 0);
		}
		if (symbol == RPARSY) { 
			output << "Line: " << lc << ", This is a while statement!" << '\n'; 
			getsym(); 
			statementHandler(); 
			insert_midcode(GOTO, label_name1, NULL, NULL, 0);
			insert_midcode(SETLABEL, label_name2, NULL, NULL, 0);
		}
		else { skip(); errormsg(17); getsym(); return; }
	}
	else {}
}
/*
	处理赋值语句
	此时要判断symbol是=还是[
*/
void assignHandler(char *token) {
	int is_array = 0;
	char index[idlen];//临时存储数组下标
	char result[idlen];//临时存储赋值句的右式结果名
	int j;//存储赋值语句左式在符号表的下标，类型可能为var_int, var_char, int_array, char_array
	if (symbol == LBRASY) {//给数组赋值
		getsym();
		exprHandler();
		strcpy_s(index, midcode[midcodec - 1].result);
		if (symbol == RBRASY) { is_array = 1; getsym(); }
		else { skip(); errormsg(16); getsym(); return; }
	}
	if (symbol == ASSIGNSY) {
		getsym();
		exprHandler();
		strcpy_s(result, midcode[midcodec - 1].result);
		j = searchtab(token, funcnum);
		if(j == -1) { skip(); errormsg(23); return; }//已经读到分号 去除getsym
		if (is_array) {
			if (tab[j].type == int_array) {
				if(expr_is_char) { errormsg(25); }
				else {
					insert_midcode(ARRASSIGN, index, result, token, 0);
				}
			}
			else if (tab[j].type == char_array) {
				if (!expr_is_char) { errormsg(25);  }
				else {
					insert_midcode(ARRASSIGN, index, result, token, 0);
				}
			}
			else{ errormsg(26); }
		}
		else {
			if (tab[j].type == var_int || tab[j].type == int_para) {
				if (expr_is_char) {  errormsg(27);  }
				else {
					insert_midcode(VARASSIGN, result, NULL, token, 0);
					//var_int = !expr_is_char
				}
			}
			else if (tab[j].type == var_char || tab[j].type == char_para) {
				if (!expr_is_char) { errormsg(28); }
				else {
					insert_midcode(VARASSIGN, result, NULL, token, 0);
				}
			}
			else { errormsg(29); }
		}
		output << "Line: " << lc << ", This is a assign statement!" << '\n';
	}
	else { skip(); errormsg(16); getsym(); return; }
}
/*
	处理情况表情况及缺省
	接收到case调用
*/
void overallcase(char* op_tp, char* label_name) {
	char case_label[idlen];//存储每个case最后的label
	int sym_tp;//缓存±号
	//todo至少有一个case
	if (symbol != CASESY) { skip(); errormsg(16); getsym(); return; }
	do{
		strcpy_s(case_label, label_name_gen());
		getsym();
		if (symbol == INTVALUE || symbol == CHARVALUE) {
			if(symbol == INTVALUE)
				insert_midcode(EQUOP, op_tp, numtostr(num), NULL, 0);
			else
				insert_midcode(EQUOP, op_tp, numtostr(token[0]), NULL, 0);
			insert_midcode(BNZ, case_label, NULL, NULL, 0);
			getsym();
			if (symbol == COLONSY) {
				getsym();
				statementHandler();
				insert_midcode(GOTO, label_name, NULL, NULL, 0);
				insert_midcode(SETLABEL, case_label, NULL, NULL, 0);
			}
			else{skip(); errormsg(16); getsym(); return;}
		}
		//接收有符号整数
		else if (symbol == PLUSSY || symbol == MINUSSY) {
			sym_tp = symbol;
			getsym();
			if (symbol == INTVALUE) {
				insert_midcode(EQUOP, op_tp, numtostr((sym_tp == PLUSSY) ? num : -num), NULL, 0);
				insert_midcode(BNZ, case_label, NULL, NULL, 0);
				getsym();
				if (symbol == COLONSY) {
					getsym();
					statementHandler();
					insert_midcode(GOTO, label_name, NULL, NULL, 0);
					insert_midcode(SETLABEL, case_label, NULL, NULL, 0);
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
		insert_midcode(SETLABEL, label_name, NULL, NULL, 0);
		getsym();
	}
	else { skip(); errormsg(16); getsym(); return; }
}
/*
	处理情况语句
*/
void swicaseHandler() {
	char op_tp[idlen];//缓存switch的表达式结果名字
	char label_name[idlen];//缓存switch最后的标签
	strcpy_s(label_name, label_name_gen());
	getsym();
	if (symbol == LPARSY) {
		getsym();
		exprHandler();
		strcpy_s(op_tp, midcode[midcodec - 1].result);
		if (symbol == RPARSY) {
			output << "Line: " << lc << ", This is a switch statement!" << '\n';
			getsym();
			if (symbol == LBRACE) { 
				getsym(); 
				overallcase(op_tp, label_name); 
			}
			else { skip(); errormsg(16); getsym(); return; }
		}
	}
	else {skip(); errormsg(15); getsym(); return; }
		
}
/*
	处理scanf和printf语句
*/
void scanfHandler() {
	char token_tp[idlen];//缓存ident
	int j;//返回查找ident的下标
	getsym();
	if (symbol == LPARSY) {
		getsym();
		while (symbol != RPARSY) {
			if (symbol == IDSY) {
				strcpy_s(token_tp, token);
				j = searchtab(token, funcnum);
				if (j == -1) { errormsg(16); getsym(); getsym();  continue; }
				if (tab[j].type == var_int || tab[j].type == int_para)
					insert_midcode(SCANINT, token_tp, NULL, NULL, 0);
				else if (tab[j].type == var_char || tab[j].type == char_para)
					insert_midcode(SCANCHAR, token_tp, NULL, NULL, 0);
				else {
					errormsg(32);
				}
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
			insert_midcode(PRINTSTR, token, NULL, NULL, 0);
			getsym();
			if (symbol != COMMASY)	insert_midcode(PRINTNEWLINE, NULL, NULL, NULL, 0);
			if (symbol == COMMASY) { 
				getsym(); 
				exprHandler(); 
				if (expr_is_char) {
					insert_midcode(PRINTCHAR, midcode[midcodec - 1].result, NULL, NULL, 0);
					insert_midcode(PRINTNEWLINE, NULL, NULL, NULL, 0);
				}
				else {
					insert_midcode(PRINTINT, midcode[midcodec - 1].result, NULL, NULL, 0);
					insert_midcode(PRINTNEWLINE, NULL, NULL, NULL, 0);
				}
			}
			if (symbol == RPARSY) {
				output << "Line: " << lc << ", This is a printf statement!" << '\n';
				getsym();
			}
			else { skip(); errormsg(6); getsym(); return; }
		}
		else {
			exprHandler();
			if (expr_is_char) {
				insert_midcode(PRINTCHAR, midcode[midcodec - 1].result, NULL, NULL, 0);
				insert_midcode(PRINTNEWLINE, NULL, NULL, NULL, 0);
			}
			else {
				insert_midcode(PRINTINT, midcode[midcodec - 1].result, NULL, NULL, 0);
				insert_midcode(PRINTNEWLINE, NULL, NULL, NULL, 0);
			}
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
			insert_midcode(RETEXPR, midcode[midcodec - 1].result, NULL, NULL, 0);
			output << "Line: " << lc << ", This is a return statement!" << '\n';
			getsym();
			return;
		}
		else { skip(); errormsg(17); getsym(); return; }
	}
	insert_midcode(RETNULL, NULL, NULL, NULL, 0);
	if (symbol == SEMISY)	output << "Line: " << lc << ", This is a return statement!" << '\n';
}
/*
	处理函数调用实参表
	此时symbol = (
*/
void realparaHandler(int j) {
	j = j + 1;
	getsym();
	if (symbol != RPARSY) {
		exprHandler();
		if (expr_is_char) {
			if (tab[j++].type == char_para) {
				insert_midcode(PUSH, midcode[midcodec - 1].result, NULL, NULL, 0);
			}
			else{ errormsg(33); }
		}
		else {
			if (tab[j++].type == int_para) {
				insert_midcode(PUSH, midcode[midcodec - 1].result, NULL, NULL, 0);
			}
			else { errormsg(33); }
		}
		while (symbol == COMMASY) { 
			getsym(); 
			exprHandler(); 
			if (expr_is_char) {
				if (tab[j++].type == char_para) {
					insert_midcode(PUSH, midcode[midcodec - 1].result, NULL, NULL, 0);
				}
				else { errormsg(33); }
			}
			else {
				if (tab[j++].type == int_para) {
					insert_midcode(PUSH, midcode[midcodec - 1].result, NULL, NULL, 0);
				}
				else { errormsg(33); }
			}
		}
		if (symbol != RPARSY) { skip(); errormsg(16); getsym(); return; }
	}
	getsym();
}
/*
	处理函数调用语句
	接收到 标识符( 开始
*/
void callrefuncHandler(char* token_tp) {
	int j;
	j = searchtab(token_tp, funcnum);
	if (j == -1) {  errormsg(30);  }
	if(!(tab[j].type>=6&&tab[j].type<=8)){ skip(); errormsg(30); getsym(); return; }
	realparaHandler(j);
	insert_midcode(CALL, token_tp, NULL, NULL, 0);
	insert_midcode(FUNCRET, NULL, NULL, id_name_gen(), 0);

}
void callfuncHandler(char* token_tp) {
	int j;
	j = searchtab(token_tp, funcnum);
	if (j == -1) { errormsg(30); }
	if (!(tab[j].type >= 6 && tab[j].type <= 8)) { skip(); errormsg(30); getsym(); return; }
	realparaHandler(j);
	insert_midcode(CALL, token_tp, NULL, NULL, 0);
}
/*
	处理有返回值函数定义
	接收到int 标识符(或者 char 标识符(开始调用，此时symbol=LPARSY
*/
void refuncHandler() {
	char token_tp[idlen] = { '\0' };//缓存token
	int sym_tp;//缓存变量是int还是char
	int num_tp;//缓存intvalue
	funcnum++;
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
							insert_midcode((sym_tp == INTSY) ? INT_ARR : CHAR_ARR, 
								token_tp, NULL, NULL, num_tp);
							getsym();
						}
						else { skip(); errormsg(14); getsym(); return; }
					}
					else { skip(); errormsg(13); getsym(); return; }
				}
				else {
					entertab(token_tp, (sym_tp == INTSY) ? var_int : var_char, NULL, addr, 1);
					addr += 4;
					insert_midcode((sym_tp == INTSY) ? VAR_INT : VAR_CHAR, token_tp, NULL, NULL, 0);
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
		//检查有没有return
		occur_ret = 0;
		do {
			statementHandler();
		} while (symbol != RBRACE);
		if(occur_ret == 0)
			insert_midcode(RETNULL, NULL, NULL, NULL, 0);
		if (symbol == RBRACE) 
			getsym();
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
							insert_midcode((sym_tp == INTSY) ? INT_ARR : CHAR_ARR, token_tp, NULL,NULL,num_tp);
							getsym();
						}
						else { skip(); errormsg(14); getsym(); return; }
					}
					else { skip(); errormsg(13); getsym(); continue; }
				}
				else {
					entertab(token_tp, (sym_tp == INTSY) ? var_int : var_char, NULL, addr, 0);
					addr += 4;
					insert_midcode((sym_tp == INTSY) ? VAR_INT : VAR_CHAR, token_tp, NULL, NULL, 0);
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
		if (symbol != INTSY && symbol != CHARSY && symbol != VOIDSY)	exit(3);
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
		insert_midcode((sym_tp == INTSY) ? INT_FUNC : CHAR_FUNC,
			token_tp, NULL, NULL, 0);
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
					insert_midcode((sym_tp == INTSY) ? INT_FUNC : CHAR_FUNC,
						token_tp, NULL, NULL, 0);
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
				insert_midcode(VOID_FUNC, token, NULL, NULL, 0);
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
	funcnum++;
	insert_midcode(VOID_FUNC, token, NULL, NULL, 0);
	
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
							entertab(token_tp, (sym_tp == INTSY) ? int_array : char_array, 
								num_tp, addr, 1);
							addr += tab[curloc - 1].size;
							insert_midcode((sym_tp == INTSY) ? INT_ARR : CHAR_ARR,
								token_tp, NULL, NULL, num_tp);
							getsym();
						}
						else { skip(); errormsg(14); getsym(); continue; }
					}
					else { skip(); errormsg(13); getsym(); continue; }
				}
				else {
					entertab(token_tp, (sym_tp == INTSY) ? var_int : var_char,
						NULL, addr, 1);
					addr += 4;
					insert_midcode((sym_tp == INTSY) ? VAR_INT : VAR_CHAR,
						token_tp, NULL, NULL, 0);
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
	cout << "Input file path within 100 characters.(e.g. C:\\User\\Doc\\test.txt)" << endl;
	cin.getline(path, 100);
	file.open(path);
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
	delconst();//delconst需要在generate之前
	char op;
	cout << "Open generate1?(y/other)\n";
	cin >> op;
	if (op == 'y')
		generate1();
	cout << "Open generate2?(y/other)\n";
	cin >> op;
	if (op == 'y')
		generate2();
	print_midcode();
	mips();
	file.close();
	output.close();
	cout << "编译完成.\n语法信息以及报错信息已存入output.txt.\n中间代码已存入midcode.txt.\nMIPS代码已存入mipscode.txt.\n";
	system("pause");
	return 0;
}
