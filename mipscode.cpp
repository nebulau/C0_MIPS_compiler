#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include "compiler.h"
using namespace std;
ofstream MipsOutput;
int strnum = 0;//计数字符串
char* str_name_gen() {
	static char str_name[idlen];
	sprintf_s(str_name, "$str%d", strnum++);
	return str_name;
}



void mips() {
	int i = 0;
	string str;
	char str_name[idlen];
	MipsOutput.open("mipscode.txt");
	MipsOutput << ".data\n";
	/*
		将全局变量写入data段
	*/
	for (i = 0; i < curloc; i++) {
		if (tab[i].type == var_int || tab[i].type == var_char) {
			str = tab[i].ident;
			MipsOutput << '\t';
			MipsOutput << str << ":.word 0\n";
		}
		else if (tab[i].type == int_array || tab[i].type == char_array) {
			str = tab[i].ident;
			MipsOutput << '\t';
			MipsOutput << str << ":.word 0:" << 4 * tab[i].value << '\n';
		}
		if (tab[i].type >= 6 && tab[i].type <= 8)	break;
	}
	/*
		将字符串写进data段
	*/
	for (i = 0; i < midcodec; i++) {
		if (midcode[i].type == PRINTSTR) {
			strcpy_s(str_name, str_name_gen());
			str = str_name;
			MipsOutput << '\t';
			MipsOutput << str << ":.asciiz \"";
			str = midcode[i].argu1;
			MipsOutput << str << "\"\n";
			strcpy_s(midcode[i].argu1, str_name);
		}
	}
	MipsOutput << ".text\n";
	MipsOutput << "j main\n";
	for (i = 0; i < midcodec; i++) {
		switch (midcode[i].type)
		{
		case ADDOP:

			break;
		case SUBOP:

			break;
		case MULOP:

			break;
		case DIVOP:

			break;
		case INT_FUNC:

			break;
		case CHAR_FUNC:

			break;
		case VOID_FUNC:

			break;
		case PARA_INT:

			break;
		case PARA_CHAR:

			break;
		case CALL:

			break;
		case PUSH:

			break;
		case FUNCRET:

			break;
		case VARASSIGN:

			break;
		case ARRASSIGN:

			break;
		case ARRACCESS:

			break;
		case LESSOP:

			break;
		case UNMOREOP:

			break;
		case MOREOP:

			break;
		case UNLESSOP:

			break;
		case UNEQUOP:

			break;
		case EQUOP:

			break;
		case SETLABEL:

			break;
		case GOTO:

			break;
		case BNZ:

			break;
		case BZ:

			break;
		case SCANINT:

			break;
		case SCANCHAR:

			break;
		case PRINTINT:

			break;
		case PRINTCHAR:

			break;
		case PRINTSTR:

			break;
		case RETEXPR:

			break;
		case RETNULL:

			break;
		default:
			break;
		}
	}



	MipsOutput.close();
}