#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include "compiler.h"
using namespace std;
ofstream tabmsg, MidOutput;
int id_name_num = 0;//临时变量名计数
int label_name_num = 0;//标签名计数
int midcodec = 0;//中间代码计数
struct mmid midcode[midcode_size];
void entertab(char *ident, int type, int value, int addr, int lev) {
	if (curloc > tab_size) {
		errormsg(20); exit(1);
	}
	//strcpy_s(tab[curloc].ident, ident);
	int i;
	/*
		在全局查找新登录符号表项是否和全局常量，全局变量或者全局函数重名
		函数作用层次为0，参数作用层次为1，参数,局部变量可与函数同名
	*/
	if (lev == 0 || (type >= 6 && type <= 8)) {
		for (i = 0; i < curloc; i++) {
			if (strcmp(tab[i].ident, ident) == 0) {
				errormsg(22);
				return;//出现同名，登录符号表失败
			}
		}
	}
	/*
		函数内的常量变量或者参数，往回查找，知道遇到函数本体名
	*/
	else if (lev == 1) {
		for (i = curloc - 1; tab[i].type != 6 && tab[i].type != 7 && tab[i].type != 8; i--) {
			if (strcmp(tab[i].ident, ident) == 0) {
				errormsg(22);
				return;//出现同名，登录符号表失败
			}
		}
	}
	//没有出现同名，登录符号表
	strcpy_s(tab[curloc].ident, ident);
	tab[curloc].type = type;
	tab[curloc].value = value;
	if ((type >= 0 && type <= 3) || (type >= 9 && type <= 10))	tab[curloc].size = sizeof(int);
	else if (type == 4 || type == 5)	tab[curloc].size = value * sizeof(int);
	tab[curloc].addr = addr;
	tab[curloc].lev = lev;
	curloc++;//当前位置+1
}
/*
	查找符号表,查找到返回在tab的index，未查找到返回-1
*/
int searchtab(char* ident, int funcnum) {
	int i;
	int isfunc = 0;
	for (i = 0; i < curloc; i++) {
		if (tab[i].type >= 6 && tab[i].type <= 8)	isfunc++;
		if (isfunc == funcnum)	break;
	}
	i++;//由于参数和局部变量可以和函数同名所以i++
	int j;
	int is_found = 0;
	//在函数内查找变量、常量,从当前函数下一个index开始查找
	for (j = i; !(tab[j].type >= 6 && tab[j].type <= 8) && j < curloc; j++) {
		if (strcmp(ident, tab[j].ident) == 0) { is_found = 1; break; }               
	}
	if (is_found) return j;
	//回溯查找函数，从当前函数的index往回查找
	for (j = i - 1; j >= 0; j--) {
		if (!(tab[j].type >= 6 && tab[j].type <= 8))	continue;
		if ((tab[j].type >= 6 && tab[j].type <= 8) && (strcmp(ident, tab[j].ident) == 0)) {
			is_found = 1;	
			break;
		}
	}
	if (is_found) return j;
	//在全局量中查变量、常量，遇到第一个函数停止查找
	for (j = 0; !(tab[j].type >= 6 && tab[j].type <= 8); j++) {
		if (strcmp(ident, tab[j].ident) == 0) { is_found = 1; break; }
	}
	if (is_found) return j;
	return -1;
}
void printtab() {
	int i;
	tabmsg.open("tabmsg.txt");
	string type[15];
	type[0] = "const_int";	type[1] = "const_char";	type[2] = "var_int";
	type[3] = "var_char";	type[4] = "int_array";	type[5] = "char_array";
	type[6] = "return_int_func";	type[7] = "return_char_func";
	type[8] = "void_func";	type[9] = "int_para";	type[10] = "char_para";
	for (i = 0; i < curloc; i++) {
		tabmsg << tab[i].ident << ' ' << type[tab[i].type] << ' '
			<< tab[i].value << ' ' << tab[i].size << ' '
			<< tab[i].lev << '\n';
	}
	tabmsg.close();
}
//todo 填中间代码表
void insert_midcode(int type, char* argu1, char* argu2, char* result, int value) { 
	midcode[midcodec].type = type;
	if (argu1 != NULL) {
		strcpy_s(midcode[midcodec].argu1, argu1);
	}
	else strcpy_s(midcode[midcodec].argu1, "");
	if (argu2 != NULL) {
		strcpy_s(midcode[midcodec].argu2, argu2);
	}
	else strcpy_s(midcode[midcodec].argu2, "");
	if (result != NULL) {
		strcpy_s(midcode[midcodec].result, result);
	}
	else strcpy_s(midcode[midcodec].result, "");
	midcode[midcodec].value = value;
	midcodec++;
}
char* id_name_gen() {
	static char id_name[idlen];
	sprintf_s(id_name, "$$%d", id_name_num++);
	return id_name;
}
void print_midcode() {
	MidOutput.open("midcode.txt");
	int i;
	string str;
	char temp[idlen];
	for (i = 0; i < midcodec; i++) {
		switch (midcode[i].type) {
			case CONST_INT: {
				MidOutput << "const int ";
				str = midcode[i].argu1;
				MidOutput << str << " = ";
				MidOutput << midcode[i].value << '\n';
				break;
			}
			case CONST_CHAR: {
				MidOutput << "const char ";
				str = midcode[i].argu1;
				MidOutput << str << " = ";
				sprintf_s(temp, "%c", midcode[i].value);
				str = temp;
				MidOutput << '\'' << str << '\'' << '\n';
				break;
			}
			case VAR_INT: {
				MidOutput << "var int ";
				str = midcode[i].argu1;
				MidOutput << str << '\n';
				break;
			}
			case VAR_CHAR: {
				MidOutput << "var char ";
				str = midcode[i].argu1;
				MidOutput << str << '\n';
				break;
				break;
			}
			case INT_ARR: {
				MidOutput << "int ";
				str = midcode[i].argu1;
				MidOutput << str << '[';
				MidOutput << midcode[i].value << "]\n";
				break;
			}
			case CHAR_ARR: {
				MidOutput << "char ";
				str = midcode[i].argu1;
				MidOutput << str << '[';
				MidOutput << midcode[i].value << "]\n";
				break;
			}
			case ADDOP: {
				str = midcode[i].result;
				MidOutput << str << " = ";
				str = midcode[i].argu1;
				MidOutput << str << " + ";
				str = midcode[i].argu2;
				MidOutput << str << '\n';
				break;
			}
			case SUBOP: {
				str = midcode[i].result;
				MidOutput << str << " = ";
				str = midcode[i].argu1;
				MidOutput << str << " - ";
				str = midcode[i].argu2;
				MidOutput << str << '\n';
				break;
			}
			case MULOP: {
				str = midcode[i].result;
				MidOutput << str << " = ";
				str = midcode[i].argu1;
				MidOutput << str << " * ";
				str = midcode[i].argu2;
				MidOutput << str << '\n';
				break;
			}
			case DIVOP: {
				str = midcode[i].result;
				MidOutput << str << " = ";
				str = midcode[i].argu1;
				MidOutput << str << " / ";
				str = midcode[i].argu2;
				MidOutput << str << '\n';
				break;
			}
			case INT_FUNC: {
				MidOutput << "int ";
				str = midcode[i].argu1;
				MidOutput << str << '(';
				MidOutput << ")\n";
				break;
			}
			case CHAR_FUNC: {
				MidOutput << "char ";
				str = midcode[i].argu1;
				MidOutput << str << '(';
				MidOutput << ")\n";
				break;
			}
			case VOID_FUNC: {
				MidOutput << "void ";
				str = midcode[i].argu1;
				MidOutput << str << '(';
				MidOutput << ")\n";
				break;
			}
			case PARA_INT: {
				MidOutput << "para_int ";
				str = midcode[i].argu1;
				MidOutput << str << '\n';
				break;
			}
			case PARA_CHAR: {
				MidOutput << "para_char ";
				str = midcode[i].argu1;
				MidOutput << str << '\n';
				break;
			}
			case CALL: {
				MidOutput << "call ";
				str = midcode[i].argu1;
				MidOutput << str << '\n';
				break;
			}
			case PUSH: {
				MidOutput << "push ";
				str = midcode[i].argu1;
				MidOutput << str << '\n';
				break;
			}
			case FUNCRET: {
				str = midcode[i].result;
				MidOutput << str << " = RET\n";
				break;
			}
			case VARASSIGN: {
				str = midcode[i].result;
				MidOutput << str << " = ";
				str = midcode[i].argu1;
				MidOutput << str << '\n';				
				break;
			}
			case ARRASSIGN: {
				str = midcode[i].result;
				MidOutput << str << '[';
				str = midcode[i].argu1;
				MidOutput << str << "] = ";
				str = midcode[i].argu2;
				MidOutput << str << '\n';
				break;
			}
			case ARRACCESS: {
				str = midcode[i].result;
				MidOutput << str << " = ";
				str = midcode[i].argu1;
				MidOutput << str << '[';
				str = midcode[i].argu2;
				MidOutput << str << "]\n";
				break;
			}
			case MOREOP: {
				str = midcode[i].argu1;
				MidOutput << str << " > ";
				str = midcode[i].argu2;
				MidOutput << str << "\n";
				break;
			}
			case UNMOREOP: {
				str = midcode[i].argu1;
				MidOutput << str << " <= ";
				str = midcode[i].argu2;
				MidOutput << str << "\n";
				break;
			}
			case LESSOP: {
				str = midcode[i].argu1;
				MidOutput << str << " < ";
				str = midcode[i].argu2;
				MidOutput << str << "\n";
				break;
			}
			case UNLESSOP: {
				str = midcode[i].argu1;
				MidOutput << str << " >= ";
				str = midcode[i].argu2;
				MidOutput << str << "\n";
				break;
			}
			case EQUOP: {
				str = midcode[i].argu1;
				MidOutput << str << " == ";
				str = midcode[i].argu2;
				MidOutput << str << '\n';
				break;
			}
			case UNEQUOP: {
				str = midcode[i].argu1;	
				MidOutput << str << " != ";
				str = midcode[i].argu2;
				MidOutput << str << '\n';
				break;
			}
			case SETLABEL: {
				str = midcode[i].argu1;
				MidOutput << str << ":\n";
				break;
			}
			case GOTO: {
				str = midcode[i].argu1;
				MidOutput << "GOTO " << str << '\n';
				break;
			}
			case BNZ: {
				str = midcode[i].argu1;
				MidOutput << "BNZ " << str << '\n';
				break;
			}
			case BZ: {
				str = midcode[i].argu1;
				MidOutput << "BZ " << str << '\n';
				break;
			}
			case SCANINT: {
				str = midcode[i].argu1;
				MidOutput << "scanf(" << str << ")\n";
				break;
			}
			case SCANCHAR: {
				str = midcode[i].argu1;
				MidOutput << "scanf(" << str << ")\n";
				break;
			}
			case PRINTINT: {
				str = midcode[i].argu1;
				MidOutput << "printf(" << str << ")\n";
				break;
			}
			case PRINTCHAR: {
				str = midcode[i].argu1;
				MidOutput << "printf(" << str << ")\n";
				break;
			}
			case PRINTSTR: {
				str = midcode[i].argu1;
				MidOutput << "printf(\"" << str << "\")\n";
				break;
			}
			case RETEXPR: {
				str = midcode[i].argu1;
				MidOutput << "ret " << str << '\n';
				break;
			}
			case RETNULL: {
				MidOutput << "return null" << '\n';
				break;
			}
			case DEL: {
				//MidOutput << '\n';
				break;
			}
			default: {

			}
		}
	}

	MidOutput.close();
}
char* label_name_gen() {
	static char label_name[idlen];
	sprintf_s(label_name, "Label%d", label_name_num++);
	return label_name;
}
char* numtostr(int num) {
	static char buffer[idlen];
	sprintf_s(buffer, "%d", num);
	return buffer;

}
int strtonum(char* str) {
	unsigned int i = 0;
	int isMinus = 0;
	int value = 0;
	if (str[0] == '+')
		i = 1;
	else if (str[0] == '-') {
		i = 1;
		isMinus = 1;
	}
	for (; i < strlen(str); i++) {
		value = value * 10 + str[i] - '0';
	}
	value = (isMinus) ? -value : value;
	return value;
}
/*
	将所有常量替换
*/
void delconst()                                                                                                                                                                                                                       {
	int i = 0, j = 0, k = 0;
	//char const_tp[idlen];
	//将函数内定义的常量在本函数内替换
	/*for (i = 0; i < funcnum; i++) {
		for (; j < midcodec; j++) {
			if (midcode[j].type >= 6 && midcode[j].type <= 8) {
				for (k = j + 1; k < midcodec; k++) {

				}
			}
		}
	}*/
	//到达第一个函数在midcode的index
	while (i < midcodec) {
		for (; !(midcode[i].type >= 10 && midcode[i].type <= 12) && i < midcodec; i++);
		for (j = i + 1; j < midcodec; j++) {
			if (midcode[j].type == 0 || midcode[j].type == 1) {
				for (k = j + 1; k < midcodec; k++) {
					if (strcmp(midcode[k].argu1, midcode[j].argu1) == 0)
						strcpy_s(midcode[k].argu1, numtostr(midcode[j].value));
					else if (strcmp(midcode[k].argu2, midcode[j].argu1) == 0)
						strcpy_s(midcode[k].argu2, numtostr(midcode[j].value));
					else if (midcode[k].type >= 10 && midcode[k].type <= 12)	break;
				}
			}
			if (midcode[j].type >= 10 && midcode[j].type <= 12)	break;
		}
		i++;
	}
	for (i = 0; i < midcodec; i++) {
		if (midcode[i].type == 0 || midcode[i].type == 1) {
			for (j = i + 1; j < midcodec; j++) {
				if (strcmp(midcode[j].argu1, midcode[i].argu1) == 0)
					strcpy_s(midcode[j].argu1, numtostr(midcode[i].value));
				else if (strcmp(midcode[j].argu2, midcode[i].argu1) == 0)
					strcpy_s(midcode[j].argu2, numtostr(midcode[i].value));
			}
		}
	}
}
/*
	如果出现了一个临时变量$的赋值，且这个临时变量没出现在result位置，则可以全部替换
*/
void generate1() {
	/*
		中间代码不会在result位置有临时变量名的重复
		这里将 $$1 = 1或者$$1 = ident之后的argu1和argu2位置的
		$$1替换成1或者ident
	*/
	int i, j, flag = 0;
	for (i = 0; i < midcodec; i++) {
		if (midcode[i].type == VARASSIGN && strlen(midcode[i].argu1) != 0
			&& strlen(midcode[i].argu2) == 0 && strlen(midcode[i].result) != 0
			&& midcode[i].result[0] == '$') {
			/*j = i + 1;
				if (strcmp(midcode[j].argu1, midcode[i].result) == 0) {
					midcode[i].type = DEL;
						strcpy_s(midcode[j].argu1, midcode[i].argu1);
				}
				else if (strcmp(midcode[j].argu2, midcode[i].result) == 0) {
					midcode[i].type = DEL;
						strcpy_s(midcode[j].argu2, midcode[i].argu1);
				}*/
			for (j = i + 1; j < midcodec; j++) {
				if (strcmp(midcode[j].result, midcode[i].result) == 0) {
					flag = 1; break;
				}
			}
			if (flag == 0) {
				for (j = i + 1; j < midcodec; j++) {
					if (strcmp(midcode[j].argu1, midcode[i].result) == 0) {
						midcode[i].type = DEL;
						strcpy_s(midcode[j].argu1, midcode[i].argu1);
					}
					else if (strcmp(midcode[j].argu2, midcode[i].result) == 0) {
						midcode[i].type = DEL;
						strcpy_s(midcode[j].argu2, midcode[i].argu1);
					}
				}
			}
		}
	}
}
/*
	常量合并
*/
void generate2() {
	//合并简单四则运算,只对临时起名的变量操作
	int i = 0, j;
	char result_tp[idlen];
	for (i = 0; i < midcodec; i++) {
		if (midcode[i].type == ADDOP && str_is_num(midcode[i].argu1) && str_is_num(midcode[i].argu2) && midcode[i].result[0] == '$') {
			midcode[i].type = DEL;
			strcpy_s(result_tp, midcode[i].result);
			strcpy_s(midcode[i].result, numtostr(strtonum(midcode[i].argu1) + strtonum(midcode[i].argu2)));
			for (j = i + 1; j < midcodec; j++) {
				if (strcmp(midcode[j].result, result_tp) == 0)	break;
				if (strcmp(midcode[j].argu1, result_tp) == 0)
					strcpy_s(midcode[j].argu1, midcode[i].result);
				else if (strcmp(midcode[j].argu2, result_tp) == 0)
					strcpy_s(midcode[j].argu2, midcode[i].result);
			}
		}
		else if (midcode[i].type == SUBOP && str_is_num(midcode[i].argu1) && str_is_num(midcode[i].argu2) && midcode[i].result[0] == '$') {
			midcode[i].type = DEL;
			strcpy_s(result_tp, midcode[i].result);
			strcpy_s(midcode[i].result, numtostr(strtonum(midcode[i].argu1) -
				strtonum(midcode[i].argu2)));
			for (j = i + 1; j < midcodec; j++) {
				if (strcmp(midcode[j].result, result_tp) == 0)	break;
				if (strcmp(midcode[j].argu1, result_tp) == 0)
					strcpy_s(midcode[j].argu1, midcode[i].result);
				else if (strcmp(midcode[j].argu2, result_tp) == 0)
					strcpy_s(midcode[j].argu2, midcode[i].result);
			}
		}
		else if (midcode[i].type == MULOP && str_is_num(midcode[i].argu1) && str_is_num(midcode[i].argu2) && midcode[i].result[0] == '$') {
			midcode[i].type = DEL;
			strcpy_s(result_tp, midcode[i].result);
			strcpy_s(midcode[i].result, numtostr(strtonum(midcode[i].argu1) * strtonum(midcode[i].argu2)));
			for (j = i + 1; j < midcodec; j++) {
				if (strcmp(midcode[j].result, result_tp) == 0)	break;
				if (strcmp(midcode[j].argu1, result_tp) == 0)
					strcpy_s(midcode[j].argu1, midcode[i].result);
				else if (strcmp(midcode[j].argu2, result_tp) == 0)
					strcpy_s(midcode[j].argu2, midcode[i].result);
			}
		}
		else if (midcode[i].type == DIVOP && str_is_num(midcode[i].argu1) && str_is_num(midcode[i].argu2) && midcode[i].result[0] == '$') {
			midcode[i].type = DEL;
			strcpy_s(result_tp, midcode[i].result);
			strcpy_s(midcode[i].result, numtostr(int(strtonum(midcode[i].argu1) / strtonum(midcode[i].argu2))));
			for (j = i + 1; j < midcodec; j++) {
				if (strcmp(midcode[j].result, result_tp) == 0)	break;
				if (strcmp(midcode[j].argu1, result_tp) == 0)
					strcpy_s(midcode[j].argu1, midcode[i].result);
				else if (strcmp(midcode[j].argu2, result_tp) == 0)
					strcpy_s(midcode[j].argu2, midcode[i].result);
			}
		}
	}
}