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
		��ȫ�ֲ����µ�¼���ű����Ƿ��ȫ�ֳ�����ȫ�ֱ�������ȫ�ֺ�������
		�������ò��Ϊ0���������ò��Ϊ1���������뺯��ͬ��
	*/
	if (lev == 0 || (type >= 6 && type << 8)) {
		for (i = 0; i < curloc; i++) {
			if (strcmp(tab[i].ident, ident)  == 0)	return;//����ͬ������¼���ű�ʧ��
		}
	}
	/*
		�����ڵĳ����������߲��������ز��ң�֪����������������
	*/
	else if (lev == 1) {
		for (i = curloc; tab[i].type != 6 && tab[i].type != 7 && tab[i].type != 8; i--) {
			if (strcmp(tab[i].ident, ident) == 0)	return;//����ͬ������¼���ű�ʧ��
		}
	}
	//û�г���ͬ������¼���ű�
	strcpy(tab[curloc].ident, ident);
	tab[curloc].type = type;
	tab[curloc].value = value;
	if ((type >= 0 && type <= 3) || (type >= 9 && type <= 10))	tab[curloc].size = sizeof(int);
	else if (type == 4 || type == 5)	tab[curloc].size = value * sizeof(int);
	tab[curloc].addr = addr;
	tab[curloc].lev = lev;
}
/*
	���ҷ��ű�
*/
int search(char* ident, int filed) {

}