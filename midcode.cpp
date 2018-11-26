#include <iostream>
#include <fstream>
#include <string.h>
#include "temp.h"

void entertab(char *ident, int type, int value, int addr, int lev) {
	if (curloc > tab_size) {
		errormsg(20); exit(1);
	}
	//strcpy(tab[curloc].ident, ident);
	int i;
	/*
		在全局查找新登录符号表项是否和全局常量，全局变量或者全局函数重名
		函数作用层次为0，参数作用层次为1，参数可与函数同名
	*/
	if (lev == 0 || (type >= 6 && type << 8)) {
		for (i = 0; i < curloc; i++) {
			if (strcmp(tab[i].ident, ident)  == 0)	return;//出现同名，登录符号表失败
		}
	}
	/*
		函数内的常量变量或者参数，往回查找，知道遇到函数本体名
	*/
	else if (lev == 1) {
		for (i = curloc; tab[i].type != 6 && tab[i].type != 7 && tab[i].type != 8; i--) {
			if (strcmp(tab[i].ident, ident) == 0)	return;//出现同名，登录符号表失败
		}
	}
	//没有出现同名，登录符号表
	strcpy(tab[curloc].ident, ident);
	tab[curloc].type = type;
	tab[curloc].value = value;
	if ((type >= 0 && type <= 3) || (type >= 9 && type <= 10))	tab[curloc].size = sizeof(int);
	else if (type == 4 || type == 5)	tab[curloc].size = value * sizeof(int);
	tab[curloc].addr = addr;
	tab[curloc].lev = lev;
}
/*
	查找符号表
*/
int search(char* ident, int filed) {

}