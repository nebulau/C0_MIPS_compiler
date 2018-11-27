#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include "compiler.h"
using namespace std;
ofstream tabmsg;
int id_name_num = 0;//临时变量名计数
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
		函数作用层次为0，参数作用层次为1，参数可与函数同名
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
	查找符号表
*/
int searchtab(char* ident) {
	return 0;
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
	if (argu2 != NULL) {
		strcpy_s(midcode[midcodec].argu2, argu2);
	}
	if (result != NULL) {
		strcpy_s(midcode[midcodec].result, result);
	}
	midcode[midcodec].value = value;
	midcodec++;
}
char* id_name_gen() {
	static char id_name[idlen];
	sprintf_s(id_name, "@_@%d", id_name_num++);
	return id_name;
}