#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include "compiler.h"
using namespace std;
ofstream tabmsg;
int id_name_num = 0;//��ʱ����������
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
		�������ò��Ϊ0���������ò��Ϊ1���������뺯��ͬ��
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
	���ҷ��ű�
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
//todo ���м�����
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