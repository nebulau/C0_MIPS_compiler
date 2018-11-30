#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include "compiler.h"
using namespace std;
ofstream tabmsg, MidOutput;
int id_name_num = 0;//��ʱ����������
int label_name_num = 0;//��ǩ������
int midcodec = 0;//�м�������
struct mmid midcode[midcode_size];
void entertab(char *ident, int type, int value, int addr, int lev) {
	if (curloc > tab_size) {
		errormsg(20); exit(1);
	}
	//strcpy_s(tab[curloc].ident, ident);
	int i;
	/*
		��ȫ�ֲ����µ�¼���ű����Ƿ��ȫ�ֳ�����ȫ�ֱ�������ȫ�ֺ�������
		�������ò��Ϊ0���������ò��Ϊ1������,�ֲ��������뺯��ͬ��
	*/
	if (lev == 0 || (type >= 6 && type <= 8)) {
		for (i = 0; i < curloc; i++) {
			if (strcmp(tab[i].ident, ident) == 0) {
				errormsg(22);
				return;//����ͬ������¼���ű�ʧ��
			}
		}
	}
	/*
		�����ڵĳ����������߲��������ز��ң�֪����������������
	*/
	else if (lev == 1) {
		for (i = curloc - 1; tab[i].type != 6 && tab[i].type != 7 && tab[i].type != 8; i--) {
			if (strcmp(tab[i].ident, ident) == 0) {
				errormsg(22);
				return;//����ͬ������¼���ű�ʧ��
			}
		}
	}
	//û�г���ͬ������¼���ű�
	strcpy_s(tab[curloc].ident, ident);
	tab[curloc].type = type;
	tab[curloc].value = value;
	if ((type >= 0 && type <= 3) || (type >= 9 && type <= 10))	tab[curloc].size = sizeof(int);
	else if (type == 4 || type == 5)	tab[curloc].size = value * sizeof(int);
	tab[curloc].addr = addr;
	tab[curloc].lev = lev;
	curloc++;//��ǰλ��+1
}
/*
	���ҷ��ű�,���ҵ�������tab��index��δ���ҵ�����-1
*/
int searchtab(char* ident, int funcnum) {
	int i;
	int isfunc = 0;
	for (i = 0; i < curloc; i++) {
		if (tab[i].type >= 6 && tab[i].type <= 8)	isfunc++;
		if (isfunc == funcnum)	break;
	}
	i++;//���ڲ����;ֲ��������Ժͺ���ͬ������i++
	int j;
	int is_found = 0;
	//�ں����ڲ��ұ���������,�ӵ�ǰ������һ��index��ʼ����
	for (j = i; !(tab[j].type >= 6 && tab[j].type <= 8) && j < curloc; j++) {
		if (strcmp(ident, tab[j].ident) == 0) { is_found = 1; break; }
	}
	if (is_found) return j;
	//���ݲ��Һ������ӵ�ǰ������index���ز���
	for (j = i - 1; j >= 0; j--) {
		if (!(tab[j].type >= 6 && tab[j].type <= 8))	continue;
		if ((tab[j].type >= 6 && tab[j].type <= 8) && (strcmp(ident, tab[j].ident) == 0)) {
			is_found = 1;	
			break;
		}
	}
	if (is_found) return j;
	//��ȫ�����в������������������һ������ֹͣ����
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
//todo ���м�����
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
	sprintf_s(id_name, "$%d", id_name_num++);
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
				MidOutput << '\n';
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
//���������ظ���
void generate1() {
	int i, j;
	for (i = 0; i < midcodec; i++) {
		if (midcode[i].type == VARASSIGN && strlen(midcode[i].argu1) != 0
			&& strlen(midcode[i].argu2) == 0 && strlen(midcode[i].result) != 0) {
			
			j = i + 1;
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