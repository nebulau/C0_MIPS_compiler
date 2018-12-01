#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include "compiler.h"
using namespace std;
ofstream MipsOutput;
int strnum = 0;//�����ַ���
int FuncLoc = 0;
int ParaLoc = 0;
int VariLoc = 0;
int VariAddr = 0;
struct VariInfo{
	char VariName[idlen];
	int addr;//����ں�������ַ��ƫ��
	//int type;//int or char
};
struct VariInfo VariTab[info_size];
struct FuncInfo {
	char func_name[idlen];
	struct VariInfo VariTab[info_size];
	//int retvalue;
	//int retaddr;
	int InfoSize;//������������size
	int ParaNum;
	int VariNum;
};
struct FuncInfo FuncTab[info_size];
char* str_name_gen() {
	static char str_name[idlen];
	sprintf_s(str_name, "$str%d", strnum++);
	return str_name;
}
int get_index(char* ident, int funcnum, int* addr) {//�����ǲ����ں�����Ϣ��������±꣬������ȫ�ֱ�������-1������ƫ�ƴ���addr
	int i = 0;
	for (i = 0; i < FuncTab[funcnum].VariNum; i++) {
		if (strcmp(FuncTab[funcnum].VariTab[i].VariName, ident) == 0) {
			*addr = FuncTab[funcnum].VariTab[i].addr;
			return i;
		}
	}
	return -1;
}
int get_funcloc(char* func_name) {
	int i, flag = 0;
	//funcnumΪcompiler.h�����ȫ�ֺ�������
	for (i = 0; i < funcnum; i++) {
		if (strcmp(FuncTab[i].func_name, func_name) == 0) {
			flag = 1; break;
		}
	}
	if (flag == 1) return i;
	return 0;
}
int str_is_num(char* str) {
	if (str[0] == '+' || str[0] == '-' || (str[0] >= '0'&&str[0] <= '9'))
		return 1;
	return 0;
}
void AddHandler(int i) {
	//midcode[i]
	int addr_tp = 0;
	int index_tp;
	string str_tp;
	int ParaNum = FuncTab[FuncLoc].ParaNum;//��ǰ�����Ĳ�������
	//���argu1������
	if (str_is_num(midcode[i].argu1)) {
		str_tp = midcode[i].argu1;
		MipsOutput << "\tli $t0, " << str_tp << '\n';
	}
	else
	{
		index_tp = get_index(midcode[i].argu1, FuncLoc, &addr_tp);
		if (index_tp == -1) {//��ȫ�ֱ���
			str_tp = midcode[i].argu1;
			MipsOutput << "\tlw $t0, " << str_tp << '\n';
		}
		else if (index_tp < ParaNum) {//������ջ���س���
			MipsOutput << "\tlw $t0, " << -addr_tp << "($fp)\n";
		}
		else if (index_tp >= ParaNum && index_tp <= ParaNum + 7) {
			MipsOutput << "\tmove $t0, $s" << index_tp - ParaNum << '\n';
		}
		else {//��δ���䵽sϵ�Ĵ����ı���
			MipsOutput << "\tlw $t0, " << -addr_tp << "($fp)\n";
			//TOTHINK!!!
		}
	}
	if (str_is_num(midcode[i].argu2)) {
		str_tp = midcode[i].argu2;
		MipsOutput << "\tli $t1, " << str_tp << '\n';
	}
	else
	{
		index_tp = get_index(midcode[i].argu2, FuncLoc, &addr_tp);
		if (index_tp == -1) {//��ȫ�ֱ���
			str_tp = midcode[i].argu2;
			MipsOutput << "\tlw $t0, " << str_tp << '\n';
		}
		else if (index_tp < ParaNum) {//������ջ���س���
			MipsOutput << "\tlw $t0, " << -addr_tp << "($fp)\n";
		}
		else if (index_tp >= ParaNum && index_tp <= ParaNum + 7) {
			MipsOutput << "\tmove $t0, $s" << index_tp - ParaNum << '\n';
		}
		else {//��δ���䵽sϵ�Ĵ����ı���
			MipsOutput << "\tlw $t0, " << -addr_tp << "($fp)\n";
			//TOTHINK!!!
		}
	}
	index_tp = get_index(midcode[i].result, FuncLoc, &addr_tp);
	if (index_tp == -1) {
		//ȫ�ֱ���
		MipsOutput << "\tadd $t0, $t0, $t1\n";
		str_tp = midcode[i].result;
		MipsOutput << "\tsw $t0, " << midcode[i].result << '\n';
	}
	else if (index_tp < ParaNum) {
		MipsOutput << "\tadd $t0, $t0, $t1\n";
		MipsOutput << "\tsw $t0, " << -addr_tp << "($fp)\n";
	}
	else if (index_tp >= ParaNum && index_tp <= ParaNum + 7) {
		MipsOutput << "\tadd $s" << index_tp - ParaNum << ", $t0, $t1\n";
	}
	else {//δ���䵽s�Ĵ����ı���
		MipsOutput << "\tadd $t0, $t0, $t1\n";
		MipsOutput << "\tsw $t0, " << -addr_tp << "($fp)\n";
	}
}
void SubHandler(int i) {
	//midcode[i]
	int addr_tp = 0;
	int index_tp;
	string str_tp;
	int ParaNum = FuncTab[FuncLoc].ParaNum;//��ǰ�����Ĳ�������
	//���argu1������
	if (str_is_num(midcode[i].argu1)) {
		str_tp = midcode[i].argu1;
		MipsOutput << "\tli $t0, " << str_tp << '\n';
	}
	else
	{
		index_tp = get_index(midcode[i].argu1, FuncLoc, &addr_tp);
		if (index_tp == -1) {//��ȫ�ֱ���
			str_tp = midcode[i].argu1;
			MipsOutput << "\tlw $t0, " << str_tp << '\n';
		}
		else if (index_tp < ParaNum) {//������ջ���س���
			MipsOutput << "\tlw $t0, " << -addr_tp << "($fp)\n";
		}
		else if (index_tp >= ParaNum && index_tp <= ParaNum + 7) {
			MipsOutput << "\tmove $t0, $s" << index_tp - ParaNum << '\n';
		}
		else {//��δ���䵽sϵ�Ĵ����ı���
			MipsOutput << "\tlw $t0, " << -addr_tp << "($fp)\n";
			//TOTHINK!!!
		}
	}
	if (str_is_num(midcode[i].argu2)) {
		str_tp = midcode[i].argu2;
		MipsOutput << "\tli $t1, " << str_tp << '\n';
	}
	else
	{
		index_tp = get_index(midcode[i].argu2, FuncLoc, &addr_tp);
		if (index_tp == -1) {//��ȫ�ֱ���
			str_tp = midcode[i].argu2;
			MipsOutput << "\tlw $t0, " << str_tp << '\n';
		}
		else if (index_tp < ParaNum) {//������ջ���س���
			MipsOutput << "\tlw $t0, " << -addr_tp << "($fp)\n";
		}
		else if (index_tp >= ParaNum && index_tp <= ParaNum + 7) {
			MipsOutput << "\tmove $t0, $s" << index_tp - ParaNum << '\n';
		}
		else {//��δ���䵽sϵ�Ĵ����ı���
			MipsOutput << "\tlw $t0, " << -addr_tp << "($fp)\n";
			//TOTHINK!!!
		}
	}
	index_tp = get_index(midcode[i].result, FuncLoc, &addr_tp);
	if (index_tp == -1) {
		//ȫ�ֱ���
		MipsOutput << "\tsub $t0, $t0, $t1\n";
		str_tp = midcode[i].result;
		MipsOutput << "\tsw $t0, " << midcode[i].result << '\n';
	}
	else if (index_tp < ParaNum) {
		MipsOutput << "\tsub $t0, $t0, $t1\n";
		MipsOutput << "\tsw $t0, " << -addr_tp << "($fp)\n";
	}
	else if (index_tp >= ParaNum && index_tp <= ParaNum + 7) {
		MipsOutput << "\tsub $s" << index_tp - ParaNum << ", $t0, $t1\n";
	}
	else {//δ���䵽s�Ĵ����ı���
		MipsOutput << "\tsub $t0, $t0, $t1\n";
		MipsOutput << "\tsw $t0, " << -addr_tp << "($fp)\n";
	}
}
void MulHandler(int i) {
	//midcode[i]
	int addr_tp = 0;
	int index_tp;
	string str_tp;
	int ParaNum = FuncTab[FuncLoc].ParaNum;//��ǰ�����Ĳ�������
	//���argu1������
	if (str_is_num(midcode[i].argu1)) {
		str_tp = midcode[i].argu1;
		MipsOutput << "\tli $t0, " << str_tp << '\n';
	}
	else
	{
		index_tp = get_index(midcode[i].argu1, FuncLoc, &addr_tp);
		if (index_tp == -1) {//��ȫ�ֱ���
			str_tp = midcode[i].argu1;
			MipsOutput << "\tlw $t0, " << str_tp << '\n';
		}
		else if (index_tp < ParaNum) {//������ջ���س���
			MipsOutput << "\tlw $t0, " << -addr_tp << "($fp)\n";
		}
		else if (index_tp >= ParaNum && index_tp <= ParaNum + 7) {
			MipsOutput << "\tmove $t0, $s" << index_tp - ParaNum << '\n';
		}
		else {//��δ���䵽sϵ�Ĵ����ı���
			MipsOutput << "\tlw $t0, " << -addr_tp << "($fp)\n";
			//TOTHINK!!!
		}
	}
	if (str_is_num(midcode[i].argu2)) {
		str_tp = midcode[i].argu2;
		MipsOutput << "\tli $t1, " << str_tp << '\n';
	}
	else
	{
		index_tp = get_index(midcode[i].argu2, FuncLoc, &addr_tp);
		if (index_tp == -1) {//��ȫ�ֱ���
			str_tp = midcode[i].argu2;
			MipsOutput << "\tlw $t0, " << str_tp << '\n';
		}
		else if (index_tp < ParaNum) {//������ջ���س���
			MipsOutput << "\tlw $t0, " << -addr_tp << "($fp)\n";
		}
		else if (index_tp >= ParaNum && index_tp <= ParaNum + 7) {
			MipsOutput << "\tmove $t0, $s" << index_tp - ParaNum << '\n';
		}
		else {//��δ���䵽sϵ�Ĵ����ı���
			MipsOutput << "\tlw $t0, " << -addr_tp << "($fp)\n";
			//TOTHINK!!!
		}
	}
	index_tp = get_index(midcode[i].result, FuncLoc, &addr_tp);
	if (index_tp == -1) {
		//ȫ�ֱ���
		MipsOutput << "\tmul $t0, $t0, $t1\n";
		str_tp = midcode[i].result;
		MipsOutput << "\tsw $t0, " << midcode[i].result << '\n';
	}
	else if (index_tp < ParaNum) {
		MipsOutput << "\tmul $t0, $t0, $t1\n";
		MipsOutput << "\tsw $t0, " << -addr_tp << "($fp)\n";
	}
	else if (index_tp >= ParaNum && index_tp <= ParaNum + 7) {
		MipsOutput << "\tmul $s" << index_tp - ParaNum << ", $t0, $t1\n";
	}
	else {//δ���䵽s�Ĵ����ı���
		MipsOutput << "\tmul $t0, $t0, $t1\n";
		MipsOutput << "\tsw $t0, " << -addr_tp << "($fp)\n";
	}
}
void DivHandler(int i) {
	//midcode[i]
	int addr_tp = 0;
	int index_tp;
	string str_tp;
	int ParaNum = FuncTab[FuncLoc].ParaNum;//��ǰ�����Ĳ�������
	//���argu1������
	if (str_is_num(midcode[i].argu1)) {
		str_tp = midcode[i].argu1;
		MipsOutput << "\tli $t0, " << str_tp << '\n';
	}
	else
	{
		index_tp = get_index(midcode[i].argu1, FuncLoc, &addr_tp);
		if (index_tp == -1) {//��ȫ�ֱ���
			str_tp = midcode[i].argu1;
			MipsOutput << "\tlw $t0, " << str_tp << '\n';
		}
		else if (index_tp < ParaNum) {//������ջ���س���
			MipsOutput << "\tlw $t0, " << -addr_tp << "($fp)\n";
		}
		else if (index_tp >= ParaNum && index_tp <= ParaNum + 7) {
			MipsOutput << "\tmove $t0, $s" << index_tp - ParaNum << '\n';
		}
		else {//��δ���䵽sϵ�Ĵ����ı���
			MipsOutput << "\tlw $t0, " << -addr_tp << "($fp)\n";
			//TOTHINK!!!
		}
	}
	if (str_is_num(midcode[i].argu2)) {
		str_tp = midcode[i].argu2;
		MipsOutput << "\tli $t1, " << str_tp << '\n';
	}
	else
	{
		index_tp = get_index(midcode[i].argu2, FuncLoc, &addr_tp);
		if (index_tp == -1) {//��ȫ�ֱ���
			str_tp = midcode[i].argu2;
			MipsOutput << "\tlw $t0, " << str_tp << '\n';
		}
		else if (index_tp < ParaNum) {//������ջ���س���
			MipsOutput << "\tlw $t0, " << -addr_tp << "($fp)\n";
		}
		else if (index_tp >= ParaNum && index_tp <= ParaNum + 7) {
			MipsOutput << "\tmove $t0, $s" << index_tp - ParaNum << '\n';
		}
		else {//��δ���䵽sϵ�Ĵ����ı���
			MipsOutput << "\tlw $t0, " << -addr_tp << "($fp)\n";
			//TOTHINK!!!
		}
	}
	index_tp = get_index(midcode[i].result, FuncLoc, &addr_tp);
	if (index_tp == -1) {
		//ȫ�ֱ���
		MipsOutput << "\tdiv $t0, $t0, $t1\n";
		str_tp = midcode[i].result;
		MipsOutput << "\tsw $t0, " << midcode[i].result << '\n';
	}
	else if (index_tp < ParaNum) {
		MipsOutput << "\tdiv $t0, $t0, $t1\n";
		MipsOutput << "\tsw $t0, " << -addr_tp << "($fp)\n";
	}
	else if (index_tp >= ParaNum && index_tp <= ParaNum + 7) {
		MipsOutput << "\tdiv $s" << index_tp - ParaNum << ", $t0, $t1\n";
	}
	else {//δ���䵽s�Ĵ����ı���
		MipsOutput << "\tdiv $t0, $t0, $t1\n";
		MipsOutput << "\tsw $t0, " << -addr_tp << "($fp)\n";
	}
}
//todo
void FuncHandler(int i) {
	int addr_tp = 0;
	//int index_tp;
	string str_tp;
	int ParaNum = FuncTab[FuncLoc].ParaNum;//��ǰ�����Ĳ�������
	//main�������⴦��
	if (strcmp(midcode[i].argu1, "main") == 0) {
		MipsOutput << "main:\n";
		MipsOutput << "\tmove $fp, $sp\n#����ָ֡��\n";
		MipsOutput << "\tsub $sp, $sp, " << FuncTab[FuncLoc].InfoSize << "\n��ջ�ռ�\n";
	}
	else {
		str_tp = midcode[i].argu1;
		MipsOutput << str_tp << ":\n";
		//MipsOutput << "\tadd $fp, $sp, " << FuncTab[FuncLoc].ParaNum * 4 << "\n#��$fpΪ�������߻���ַ\n";
		MipsOutput << "\tsub $sp, $sp, " << (FuncTab[FuncLoc].VariNum - FuncTab[FuncLoc].ParaNum) * 4;
		MipsOutput << "\n#��ջ�ռ䣬���ű���sϵ�Ĵ�����$ra,$fp\n";
		MipsOutput << "\tsw $s0, 0($sp)\n\tsub $sp, $sp, 4\n\tsw $s1, 0($sp)\n\tsub $sp, $sp, 4\n";
		MipsOutput << "\tsw $s2, 0($sp)\n\tsub $sp, $sp, 4\n\tsw $s3, 0($sp)\n\tsub $sp, $sp, 4\n";
		MipsOutput << "\tsw $s4, 0($sp)\n\tsub $sp, $sp, 4\n\tsw $s5, 0($sp)\n\tsub $sp, $sp, 4\n";
		MipsOutput << "\tsw $s6, 0($sp)\n\tsub $sp, $sp, 4\n\tsw $s7, 0($sp)\n\tsub $sp, $sp, 4\n";
		MipsOutput << "\tsw $ra, 0($sp)\n\tsub $sp, $sp, 4\n\tsw $fp, 0($sp)\n\tsub $sp, $sp, 4\n";
		MipsOutput << "\tadd $fp, $sp, " << FuncTab[FuncLoc].VariNum * 4 + 40<< "\n#��$fpΪ�������߻���ַ\n";
	}

}
//todo
void CallHandler(int i) {
	string str_tp;
	str_tp = midcode[i].argu1;
	MipsOutput << "\tjal " << str_tp << '\n';
}
void PushHandler(int i) {
	int addr_tp = 0;
	int index_tp;
	string str_tp;
	int ParaNum = FuncTab[FuncLoc].ParaNum;//��ǰ�����Ĳ�������
	//���argu1������
	if (str_is_num(midcode[i].argu1)) {
		str_tp = midcode[i].argu1;
		MipsOutput << "\tli $t0, " << str_tp << '\n';
		//MipsOutput << "\tsw $t0, 0($sp)\n\tsub $sp, $sp, 4\n";
	}
	else {
		index_tp = get_index(midcode[i].argu1, FuncLoc, &addr_tp);
		if (index_tp == -1) {
			str_tp = midcode[i].argu1;
			MipsOutput << "\tlw $t0, " << str_tp << '\n';
		}
		else if (index_tp < ParaNum) {//������ջ���س���
			MipsOutput << "\tlw $t0, " << -addr_tp << "($fp)\n";
		}
		else if (index_tp >= ParaNum && index_tp <= ParaNum + 7) {
			MipsOutput << "\tmove $t0, $s" << index_tp - ParaNum << '\n';
		}
		else {//��δ���䵽sϵ�Ĵ����ı���
			MipsOutput << "\tlw $t0, " << -addr_tp << "($fp)\n";
			//TOTHINK!!!
		}
	}
	MipsOutput << "\tsw $t0, 0($sp)\n\tsub $sp, $sp, 4\n";
}
void FuncRetHandler(int i) {
	int addr_tp = 0;
	int index_tp;
	string str_tp;
	int ParaNum = FuncTab[FuncLoc].ParaNum;//��ǰ�����Ĳ�������
	//��result = $v0,��ʱ�����ġ�һ���ں���������
	index_tp = get_index(midcode[i].result, FuncLoc, &addr_tp);
	if (index_tp >= ParaNum + 0 && index_tp <= ParaNum + 7) {
		MipsOutput << "\tmove $s" << index_tp << ", $v0";
	}
	else {
		MipsOutput << "\tsw $v0, " << -addr_tp << "($fp)\n";
	}
}
void VarAssignHandler(int i) {
	int addr_tp = 0;
	int index_tp;
	string str_tp;
	int ParaNum = FuncTab[FuncLoc].ParaNum;//��ǰ�����Ĳ�������
	if (str_is_num(midcode[i].argu1)) {
		str_tp = midcode[i].argu1;
		MipsOutput << "\tli $t0, " << str_tp << '\n';
	}
	else {
		index_tp = get_index(midcode[i].argu1, FuncLoc, &addr_tp);
		if (index_tp == -1) {
			str_tp = midcode[i].argu1;
			MipsOutput << "\tlw $t0, " << str_tp << '\n';
		}
		else if (index_tp < ParaNum) {
			MipsOutput << "\tlw $t0, " << -addr_tp << "($fp)\n";
		}
		else if (index_tp >= ParaNum + 0 && index_tp <= ParaNum + 7) {
			MipsOutput << "\tmove $t0, $s" << index_tp << '\n';
		}
		else {
			MipsOutput << "\tlw $t0, " << -addr_tp << "($fp)\n";
		}
	}
	index_tp = get_index(midcode[i].result, FuncLoc, &addr_tp);
	if (index_tp == -1) {
		str_tp = midcode[i].result;
		MipsOutput << "\tsw $t0, " << str_tp << '\n';
	}
	else if (index_tp < ParaNum) {
		MipsOutput << "\tsw $t0, " << -addr_tp << "($fp)\n";
	}
	else if (index_tp >= ParaNum + 0 && index_tp <= ParaNum + 7) {
		MipsOutput << "\tmove $s" << index_tp << ", $t0";
	}
	else {//��Ҫд����������ջ
		MipsOutput << "\tsw $t0, " << -addr_tp << "($fp)\n";
	}
}
void ArrAssignHandler(int i) {
	int addr_tp = 0;
	int index_tp;
	string str_tp;
	int ParaNum = FuncTab[FuncLoc].ParaNum;//��ǰ�����Ĳ�������
	index_tp = get_index(midcode[i].result, FuncLoc, &addr_tp);
	//$t0�������ַ
	if (index_tp == -1) {
		//ȫ������
		str_tp = midcode[i].result;
		MipsOutput << "\tla $t0, " << str_tp << '\n';
	}
	else {
		MipsOutput << "\tla $t0, " << -addr_tp << "($fp)\n";
	}
	//$t1��ƫ��
	if (str_is_num(midcode[i].argu1)) {
		str_tp = midcode[i].argu1;
		MipsOutput << "\tli $t1, " << str_tp << '\n';
	}
	else {
		index_tp = get_index(midcode[i].argu1, FuncLoc, &addr_tp);
		if (index_tp == -1) {//��ȫ�ֱ���
			str_tp = midcode[i].argu1;
			MipsOutput << "\tlw $t1, " << str_tp << '\n';
		}
		else if (index_tp < ParaNum) {
			MipsOutput << "\tlw $t1, " << -addr_tp << "($fp)\n";
		}
		else if (index_tp >= ParaNum + 0 && index_tp <= ParaNum + 7) {
			MipsOutput << "\tmove $t1, $s" << index_tp << '\n';
		}
		else {
			MipsOutput << "\tlw $t1, " << -addr_tp << "($fp)\n";
		}
	}
	//$t1*4
	MipsOutput << "\tli $t2, 4\n";
	MipsOutput << "mul $t1, $t1, $t2\n";
	//array[$t1] = $t2
	if (str_is_num(midcode[i].argu2)) {
		str_tp = midcode[i].argu2;
		MipsOutput << "\tli $t2, " << str_tp << '\n';
	}
	else {
		index_tp = get_index(midcode[i].argu2, FuncLoc, &addr_tp);
		if (index_tp == -1) {
			str_tp = midcode[i].argu2;
			MipsOutput << "\tlw $t2, " << str_tp << '\n';
		}
		else if (index_tp < ParaNum) {
			MipsOutput << "\tlw $t2, " << -addr_tp << "($fp)\n";
		}
		else if (index_tp >= ParaNum + 0 && index_tp <= ParaNum + 7) {
			MipsOutput << "\tmove $t2, $s" << index_tp << '\n';
		}
		else {
			MipsOutput << "\tlw $t2, " << -addr_tp << "($fp)\n";
		}
	}
	MipsOutput << "\tadd $t0, $t0, $t1\n";
	MipsOutput << "\tsw $t2, 0($t1)\n";
}
void ArrAccessHandler(int i) {
	int addr_tp = 0;
	int index_tp;
	string str_tp;
	int ParaNum = FuncTab[FuncLoc].ParaNum;//��ǰ�����Ĳ�������
	index_tp = get_index(midcode[i].argu1, FuncLoc, &addr_tp);
	//$t0�������� $t1�±꣬ $t2��ʽ
	if(index_tp == -1){//��ȫ������
		str_tp = midcode[i].argu1;
		MipsOutput << "\tla $t0, " << str_tp << '\n';
	}
	else {
		MipsOutput << "\tla $t0, " << -addr_tp << "($fp)\n";
	}
	if (str_is_num(midcode[i].argu2)) {
		str_tp = midcode[i].argu2;
		MipsOutput << "\tli $t1, " << str_tp << '\n';
	}
	else {
		index_tp = get_index(midcode[i].argu2, FuncLoc, &addr_tp);
		if (index_tp == -1) {//��ȫ�ֱ���
			str_tp = midcode[i].argu2;
			MipsOutput << "\tlw $t1, " << str_tp << '\n';
		}
		else if (index_tp < ParaNum) {
			MipsOutput << "\tlw $t1, " << -addr_tp << "($fp)\n";
		}
		else if (index_tp >= ParaNum + 0 && index_tp <= ParaNum + 7) {
			MipsOutput << "\tmove $t1, $s" << index_tp << '\n';
		}
		else {
			MipsOutput << "\tlw $t1, " << -addr_tp << "($fp)\n";
		}
	}
	//$t1*4
	MipsOutput << "\tli $t2, 4\n";
	MipsOutput << "mul $t1, $t1, $t2\n";
	//$t2 = array[$t1]
	MipsOutput << "\tadd $t0, $t0, $t1\n";
	MipsOutput << "\tlw $t2, 0($t1)\n";
	index_tp = get_index(midcode[i].result, FuncLoc, &addr_tp);
	if (index_tp == -1) {
		str_tp = midcode[i].result;
		MipsOutput << "\tsw $t2, " << str_tp << '\n';
	}
	else if (index_tp < ParaNum) {
		MipsOutput << "\tsw $t2, " << -addr_tp << "($fp)\n";
	}
	else if (index_tp >= ParaNum + 0 && index_tp <= ParaNum + 7) {
		MipsOutput << "\tmove $s" << index_tp << ", $t2\n";
	}
	else {
		MipsOutput << "\tsw $t2, " << -addr_tp << "($fp)\n";
	}
}





void ConditionHandler(int i) {
	/*
		��Ҫ��i+1���Դ���ָ��
		û��UNEQUOP��!=
		EQUOP֮�����BZ ����0��ת 
		EQUOP֮�����BNZ ������ת
		�����֮��ֻ��BNZ
	*/
	int addr_tp = 0;
	int index_tp;
	string str_tp;
	int ParaNum = FuncTab[FuncLoc].ParaNum;//��ǰ�����Ĳ�������
	if (midcode[i].type == LESSOP) {
		//blt $t0, $t1, label
		if (str_is_num(midcode[i].argu1)) {
			str_tp = midcode[i].argu1;
			MipsOutput << "\tli $t0, " << str_tp << '\n';
		}
		else {
			index_tp = get_index(midcode[i].argu1, FuncLoc, &addr_tp);
			if (index_tp == -1) {
				str_tp = midcode[i].argu1;
				MipsOutput << "\tlw $t0, " << str_tp << '\n';
			}
			else if (index_tp < ParaNum) {
				MipsOutput << "\tlw $t0, " << -addr_tp << "($fp)\n";
			}
			else if (index_tp >= ParaNum + 0 && index_tp <= ParaNum + 7) {
				MipsOutput << "\tmove $t0, $s" << index_tp << '\n';
			}
			else {
				MipsOutput << "\tlw $t0, " << -addr_tp << "($fp)\n";
			}
		}

		if (str_is_num(midcode[i].argu2)) {
			str_tp = midcode[i].argu2;
			MipsOutput << "\tli $t1, " << str_tp << '\n';
		}
		else {
			index_tp = get_index(midcode[i].argu2, FuncLoc, &addr_tp);
			if (index_tp == -1) {
				str_tp = midcode[i].argu2;
				MipsOutput << "\tlw $t1, " << str_tp << '\n';
			}
			else if (index_tp < ParaNum) {
				MipsOutput << "\tlw $t1, " << -addr_tp << "($fp)\n";
			}
			else if (index_tp >= ParaNum + 0 && index_tp <= ParaNum + 7) {
				MipsOutput << "\tmove $t1, $s" << index_tp << '\n';
			}
			else {
				MipsOutput << "\tlw $t1, " << -addr_tp << "($fp)\n";
			}
		}
		str_tp = midcode[i + 1].argu1;
		MipsOutput << "\tblt $t0, $t1, " << str_tp << '\n';
	}
	else if (midcode[i].type == UNMOREOP) {
		//ble
		if (str_is_num(midcode[i].argu1)) {
			str_tp = midcode[i].argu1;
			MipsOutput << "\tli $t0, " << str_tp << '\n';
		}
		else {
			index_tp = get_index(midcode[i].argu1, FuncLoc, &addr_tp);
			if (index_tp == -1) {
				str_tp = midcode[i].argu1;
				MipsOutput << "\tlw $t0, " << str_tp << '\n';
			}
			else if (index_tp < ParaNum) {
				MipsOutput << "\tlw $t0, " << -addr_tp << "($fp)\n";
			}
			else if (index_tp >= ParaNum + 0 && index_tp <= ParaNum + 7) {
				MipsOutput << "\tmove $t0, $s" << index_tp << '\n';
			}
			else {
				MipsOutput << "\tlw $t0, " << -addr_tp << "($fp)\n";
			}
		}

		if (str_is_num(midcode[i].argu2)) {
			str_tp = midcode[i].argu2;
			MipsOutput << "\tli $t1, " << str_tp << '\n';
		}
		else {
			index_tp = get_index(midcode[i].argu2, FuncLoc, &addr_tp);
			if (index_tp == -1) {
				str_tp = midcode[i].argu2;
				MipsOutput << "\tlw $t1, " << str_tp << '\n';
			}
			else if (index_tp < ParaNum) {
				MipsOutput << "\tlw $t1, " << -addr_tp << "($fp)\n";
			}
			else if (index_tp >= ParaNum + 0 && index_tp <= ParaNum + 7) {
				MipsOutput << "\tmove $t1, $s" << index_tp << '\n';
			}
			else {
				MipsOutput << "\tlw $t1, " << -addr_tp << "($fp)\n";
			}
		}
		str_tp = midcode[i + 1].argu1;
		MipsOutput << "\tble $t0, $t1, " << str_tp << '\n';
	}
	else if (midcode[i].type == MOREOP) {
		//bgt
		if (str_is_num(midcode[i].argu1)) {
			str_tp = midcode[i].argu1;
			MipsOutput << "\tli $t0, " << str_tp << '\n';
		}
		else {
			index_tp = get_index(midcode[i].argu1, FuncLoc, &addr_tp);
			if (index_tp == -1) {
				str_tp = midcode[i].argu1;
				MipsOutput << "\tlw $t0, " << str_tp << '\n';
			}
			else if (index_tp < ParaNum) {
				MipsOutput << "\tlw $t0, " << -addr_tp << "($fp)\n";
			}
			else if (index_tp >= ParaNum + 0 && index_tp <= ParaNum + 7) {
				MipsOutput << "\tmove $t0, $s" << index_tp << '\n';
			}
			else {
				MipsOutput << "\tlw $t0, " << -addr_tp << "($fp)\n";
			}
		}

		if (str_is_num(midcode[i].argu2)) {
			str_tp = midcode[i].argu2;
			MipsOutput << "\tli $t1, " << str_tp << '\n';
		}
		else {
			index_tp = get_index(midcode[i].argu2, FuncLoc, &addr_tp);
			if (index_tp == -1) {
				str_tp = midcode[i].argu2;
				MipsOutput << "\tlw $t1, " << str_tp << '\n';
			}
			else if (index_tp < ParaNum) {
				MipsOutput << "\tlw $t1, " << -addr_tp << "($fp)\n";
			}
			else if (index_tp >= ParaNum + 0 && index_tp <= ParaNum + 7) {
				MipsOutput << "\tmove $t1, $s" << index_tp << '\n';
			}
			else {
				MipsOutput << "\tlw $t1, " << -addr_tp << "($fp)\n";
			}
		}
		str_tp = midcode[i + 1].argu1;
		MipsOutput << "\tbgt $t0, $t1, " << str_tp << '\n';
	}
	else if (midcode[i].type == UNLESSOP) {
		//bge
		if (str_is_num(midcode[i].argu1)) {
			str_tp = midcode[i].argu1;
			MipsOutput << "\tli $t0, " << str_tp << '\n';
		}
		else {
			index_tp = get_index(midcode[i].argu1, FuncLoc, &addr_tp);
			if (index_tp == -1) {
				str_tp = midcode[i].argu1;
				MipsOutput << "\tlw $t0, " << str_tp << '\n';
			}
			else if (index_tp < ParaNum) {
				MipsOutput << "\tlw $t0, " << -addr_tp << "($fp)\n";
			}
			else if (index_tp >= ParaNum + 0 && index_tp <= ParaNum + 7) {
				MipsOutput << "\tmove $t0, $s" << index_tp << '\n';
			}
			else {
				MipsOutput << "\tlw $t0, " << -addr_tp << "($fp)\n";
			}
		}

		if (str_is_num(midcode[i].argu2)) {
			str_tp = midcode[i].argu2;
			MipsOutput << "\tli $t1, " << str_tp << '\n';
		}
		else {
			index_tp = get_index(midcode[i].argu2, FuncLoc, &addr_tp);
			if (index_tp == -1) {
				str_tp = midcode[i].argu2;
				MipsOutput << "\tlw $t1, " << str_tp << '\n';
			}
			else if (index_tp < ParaNum) {
				MipsOutput << "\tlw $t1, " << -addr_tp << "($fp)\n";
			}
			else if (index_tp >= ParaNum + 0 && index_tp <= ParaNum + 7) {
				MipsOutput << "\tmove $t1, $s" << index_tp << '\n';
			}
			else {
				MipsOutput << "\tlw $t1, " << -addr_tp << "($fp)\n";
			}
		}
		str_tp = midcode[i + 1].argu1;
		MipsOutput << "\tbge $t0, $t1, " << str_tp << '\n';
	}
	else if (midcode[i].type == EQUOP) {
		if (midcode[i + 1].type == BZ) {
			if (str_is_num(midcode[i].argu1)) {
				str_tp = midcode[i].argu1;
				MipsOutput << "\tli $t0, " << str_tp << '\n';
				MipsOutput << "\tbeq $t0, $0, ";
				str_tp = midcode[i + 1].argu1;
				MipsOutput << str_tp << '\n';
			}
			else {
				index_tp = get_index(midcode[i].argu1, FuncLoc, &addr_tp);
				if (index_tp == -1) {//��ȫ�ֱ���
					str_tp = midcode[i].argu1;
					MipsOutput << "\tlw $t0, " << str_tp << '\n';
					MipsOutput << "\tbeq $t0, $0, ";
					str_tp = midcode[i + 1].argu1;
					MipsOutput << str_tp << '\n';
				}
				else if (index_tp >= 0 && index_tp <= 7) {
					MipsOutput << "\tmove $t0, $s" << index_tp << '\n';
					MipsOutput << "\tbeq $t0, $0, ";
					str_tp = midcode[i + 1].argu1;
					MipsOutput << str_tp << '\n';
				}
				else {//��ջ��
					MipsOutput << "\tlw $t0, " << -addr_tp << "($fp)\n";
					MipsOutput << "\tbeq $t0, $0, ";
					str_tp = midcode[i + 1].argu1;
					MipsOutput << str_tp << '\n';
				}
			}
		}
		else if (midcode[i + 1].type == BNZ) {
			//argu1��2������ת��argu2������
			if (str_is_num(midcode[i].argu1)) {
				str_tp = midcode[i].argu1;
				MipsOutput << "\tli $t0, " << str_tp << '\n';
			}
			else {
				index_tp = get_index(midcode[i].argu1, FuncLoc, &addr_tp);
				if (index_tp == -1) {
					str_tp = midcode[i].argu1;
					MipsOutput << "\tlw $t0, " << str_tp << '\n';
				}
				else if (index_tp < ParaNum) {
					MipsOutput << "\tlw $t0, " << -addr_tp << "($fp)\n";
				}
				else if (index_tp >= ParaNum + 0 && index_tp <= ParaNum + 7) {
					MipsOutput << "\tmove $t0, $s" << index_tp << '\n';
				}
				else {
					MipsOutput << "\tlw $t0, " << -addr_tp << "($fp)\n";
				}
			}
			str_tp = midcode[i].argu2;
			MipsOutput << "\tli $t1, " << str_tp;
			MipsOutput << "\tbne $t0, $t1, ";
			str_tp = midcode[i + 1].argu1;
			MipsOutput << str_tp << '\n';
		}
	}
}

void SetLabelHandler(int i) {
	string str_tp;
	str_tp = midcode[i].argu1;
	MipsOutput << str_tp << ":\n";
}

void GotoHandler(int i) {
	string str_tp;
	str_tp = midcode[i].argu1;
	MipsOutput << "\tj " << str_tp << '\n';
}

void ScanIntHandler(int i) {
	int addr_tp = 0;
	int index_tp;
	string str_tp;
	int ParaNum = FuncTab[FuncLoc].ParaNum;//��ǰ�����Ĳ�������
	MipsOutput << "\tli $v0, 5\n";
	MipsOutput << "\tsyscall \n";
	index_tp = get_index(midcode[i].argu1, FuncLoc, &addr_tp);
	if (index_tp == -1) {//��ȫ�ֱ���
		str_tp = midcode[i].argu1;
		MipsOutput << "\tsw $v0, " << str_tp << '\n';
	}
	else if (index_tp < ParaNum) {
		MipsOutput << "\tsw $v0, " << -addr_tp << "($fp)\n";
	}
	else if (index_tp >= ParaNum + 0 && index_tp <= ParaNum + 7) {
		MipsOutput << "\tmove $s" << index_tp << ", $v0\n";
	}
	else {
		MipsOutput << "\tsw $v0, " << -addr_tp << "($fp)\n";
	}
}
void ScanCharHandler(int i) {
	int addr_tp = 0;
	int index_tp;
	string str_tp;
	int ParaNum = FuncTab[FuncLoc].ParaNum;//��ǰ�����Ĳ�������
	MipsOutput << "\tli $v0, 12\n";
	MipsOutput << "\tsyscall \n";
	index_tp = get_index(midcode[i].argu1, FuncLoc, &addr_tp);
	if (index_tp == -1) {//��ȫ�ֱ���
		str_tp = midcode[i].argu1;
		MipsOutput << "\tsw $v0, " << str_tp << '\n';
	}
	else if (index_tp < ParaNum) {
		MipsOutput << "\tsw $v0, " << -addr_tp << "($fp)\n";
	}
	else if (index_tp >= ParaNum + 0 && index_tp <= ParaNum + 7) {
		MipsOutput << "\tmove $s" << index_tp << ", $v0\n";
	}
	else {
		MipsOutput << "\tsw $v0, " << -addr_tp << "($fp)\n";
	}
}
void PrintIntHandler(int i) {
	int addr_tp = 0;
	int index_tp;
	string str_tp;
	int ParaNum = FuncTab[FuncLoc].ParaNum;//��ǰ�����Ĳ�������
	if (str_is_num(midcode[i].argu1)) {
		str_tp = midcode[i].argu1;
		MipsOutput << "\tli $a0, " << str_tp << '\n';
		MipsOutput << "\tli $v0, 1\nsyscall\n";
	}
	else {
		index_tp = get_index(midcode[i].argu1, FuncLoc, &addr_tp);
		if (index_tp == -1) {
			str_tp = midcode[i].argu1;
			MipsOutput << "\tlw $a0, " << str_tp << '\n';
			MipsOutput << "\tli $v0, 1\nsyscall\n";
		}
		else if (index_tp < ParaNum) {
			MipsOutput << "\tlw $a0, " << -addr_tp << "($fp)\n";
			MipsOutput << "\tli $v0, 1\nsyscall\n";
		}
		else if (index_tp >= ParaNum + 0 && index_tp <= ParaNum + 7) {
			MipsOutput << "\tmove $a0, $s" << index_tp << '\n';
			MipsOutput << "\tli $v0, 1\nsyscall\n";
		}
		else {
			MipsOutput << "\tlw $a0, " << -addr_tp << "($fp)\n";
			MipsOutput << "\tli $v0, 1\nsyscall\n";
		}
	}
}
void PrintCharHandler(int i) {
	int addr_tp = 0;
	int index_tp;
	string str_tp;
	int ParaNum = FuncTab[FuncLoc].ParaNum;//��ǰ�����Ĳ�������
	if (str_is_num(midcode[i].argu1)) {
		str_tp = midcode[i].argu1;
		MipsOutput << "\tli $a0, " << str_tp << '\n';
		MipsOutput << "\tli $v0, 11\nsyscall\n";
	}
	else {
		index_tp = get_index(midcode[i].argu1, FuncLoc, &addr_tp);
		if (index_tp == -1) {
			str_tp = midcode[i].argu1;
			MipsOutput << "\tlw $a0, " << str_tp << '\n';
			MipsOutput << "\tli $v0, 11\nsyscall\n";
		}
		else if (index_tp < ParaNum) {
			MipsOutput << "\tlw $a0, " << -addr_tp << "($fp)\n";
			MipsOutput << "\tli $v0, 11\nsyscall\n";
		}
		else if (index_tp >= ParaNum + 0 && index_tp <= ParaNum + 7) {
			MipsOutput << "\tmove $a0, $s" << index_tp << '\n';
			MipsOutput << "\tli $v0, 11\nsyscall\n";
		}
		else {
			MipsOutput << "\tlw $a0, " << -addr_tp << "($fp)\n";
			MipsOutput << "\tli $v0, 11\nsyscall\n";
		}
	}
}
void PrintStrHandler(int i) {
	string str_tp;
	str_tp = midcode[i].argu1;
	MipsOutput << "\tla $a0, " << str_tp << '\n';
	MipsOutput << "\tli $v0, 4\nsyscall\n";
}
void RetExprHandler(int i) {
	int addr_tp = 0;
	int index_tp;
	string str_tp;
	int ParaNum = FuncTab[FuncLoc].ParaNum;//��ǰ�����Ĳ�������
	//��$v0���淵��ֵ
	if (str_is_num(midcode[i].argu1)) {
		str_tp = midcode[i].argu1;
		MipsOutput << "\tli $v0, " << str_tp << '\n';
	}
	else {
		index_tp = get_index(midcode[i].argu1, FuncLoc, &addr_tp);
		if (index_tp == -1) {
			str_tp = midcode[i].argu1;
			MipsOutput << "\tlw $$v0, " << str_tp << '\n';
		}
		else if (index_tp < ParaNum) {
			MipsOutput << "\tlw $v0, " << -addr_tp << "($fp)\n";
		}
		else if (index_tp >= ParaNum + 0 && index_tp <= ParaNum + 7) {
			MipsOutput << "\tmove $v0, $s" << index_tp << '\n';
		}
		else {
			MipsOutput << "\tlw $v0, " << -addr_tp << "($fp)\n";
		}
	}
	//return֮ǰ�ָ�ջָ�뵽�����ߵ�λ��
	MipsOutput << "\tadd $sp, $sp, 4\n\tlw $fp, 0($sp)\n\tadd $sp, $sp, 4\n\tlw $ra, 0($sp)\n";
	MipsOutput << "\tadd $sp, $sp, 4\n\tlw $s7, 0($sp)\n\tadd $sp, $sp, 4\n\tlw $s6, 0($sp)\n";
	MipsOutput << "\tadd $sp, $sp, 4\n\tlw $s5, 0($sp)\n\tadd $sp, $sp, 4\n\tlw $s4, 0($sp)\n";
	MipsOutput << "\tadd $sp, $sp, 4\n\tlw $s3, 0($sp)\n\tadd $sp, $sp, 4\n\tlw $s2, 0($sp)\n";
	MipsOutput << "\tadd $sp, $sp, 4\n\tlw $s1, 0($sp)\n\tadd $sp, $sp, 4\n\tlw $s0, 0($sp)\n";
	MipsOutput << "\tadd $sp, $sp, " << FuncTab[FuncLoc].VariNum * 4;
	MipsOutput << "\tjr $ra\n";
}
void RetNullHandler(int i) {
	if (FuncLoc == funcnum - 1)
		//main��������
		MipsOutput << "\tli $v0, 10\nsyscall\n";
	else {
		MipsOutput << "\tadd $sp, $sp, 4\n\tlw $fp, 0($sp)\n\tadd $sp, $sp, 4\n\tlw $ra, 0($sp)\n";
		MipsOutput << "\tadd $sp, $sp, 4\n\tlw $s7, 0($sp)\n\tadd $sp, $sp, 4\n\tlw $s6, 0($sp)\n";
		MipsOutput << "\tadd $sp, $sp, 4\n\tlw $s5, 0($sp)\n\tadd $sp, $sp, 4\n\tlw $s4, 0($sp)\n";
		MipsOutput << "\tadd $sp, $sp, 4\n\tlw $s3, 0($sp)\n\tadd $sp, $sp, 4\n\tlw $s2, 0($sp)\n";
		MipsOutput << "\tadd $sp, $sp, 4\n\tlw $s1, 0($sp)\n\tadd $sp, $sp, 4\n\tlw $s0, 0($sp)\n";
		MipsOutput << "\tadd $sp, $sp, " << FuncTab[FuncLoc].VariNum * 4;
		MipsOutput << "\tjr $ra\n";
	}
}
void mips() {
	int i = 0;
	int j = 0;
	string str;
	char str_name[idlen];
	MipsOutput.open("mipscode.txt");
	MipsOutput << ".data\n";
	/*
		��ȫ�ֱ���д��data��
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
		���ַ���д��data�Σ�����PRINTSTR��argu1����
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
	/*
		��������Ϣ���������ʼ��
		����ֻ�к����ڶ���ı�����$��ͷ����ʱ����
	*/
	for (i = 0; i < midcodec; i++) {
		//����������ʼͳ��
		if (midcode[i].type >= 10 && midcode[i].type <= 12) {
			ParaLoc = 0;
			VariLoc = 0;
			VariAddr = 0;//40����ΪҪԤ���ռ��$ra��$fp��sϵ�Ĵ���
			strcpy_s(FuncTab[FuncLoc].func_name, midcode[i].argu1);
			for (j = i + 1; j < midcodec; j++) {
				//������һ����������
				if (midcode[j].type >= 10 && midcode[j].type <= 12 || j + 1 == midcodec) {
					FuncTab[FuncLoc].InfoSize = VariAddr;
					FuncTab[FuncLoc].ParaNum = ParaLoc;
					FuncTab[FuncLoc].VariNum = VariLoc;
					FuncLoc++;
					break;
				}
				if (midcode[j].type == VAR_INT || midcode[j].type == VAR_CHAR 
					|| midcode[j].type == PARA_INT || midcode[j].type == PARA_CHAR) 
				{
					if (midcode[j].type == PARA_INT || midcode[j].type == PARA_CHAR)
						ParaLoc++;
					strcpy_s(FuncTab[FuncLoc].VariTab[VariLoc].VariName, midcode[j].argu1);
					FuncTab[FuncLoc].VariTab[VariLoc].addr = VariAddr;
					VariLoc++;
					VariAddr += 4;
				}
				else if (midcode[j].type == INT_ARR || midcode[j].type == CHAR_ARR) {
					strcpy_s(FuncTab[FuncLoc].VariTab[VariLoc].VariName, midcode[j].argu1);
					FuncTab[FuncLoc].VariTab[VariLoc].addr = VariAddr;
					VariLoc++;
					VariAddr += 4 * midcode[j].value;
				}
				else if (midcode[j].type != DEL && midcode[j].result[0] == '$') {
					strcpy_s(FuncTab[FuncLoc].VariTab[VariLoc].VariName, midcode[j].result);
					FuncTab[FuncLoc].VariTab[VariLoc].addr = VariAddr;
					VariLoc++;
					VariAddr += 4;
				}
			}
		}
	}
	FuncLoc = -1;//��ʼ˳�����м����FuncLoc��Ϊ-1
	MipsOutput << ".text\n";
	MipsOutput << "\tj main\n";
	for (i = 0; i < midcodec; i++) {
		switch (midcode[i].type)
		{
		case ADDOP:
			AddHandler(i);
			break;
		case SUBOP:
			SubHandler(i);
			break;
		case MULOP:
			MulHandler(i);
			break;
		case DIVOP:
			DivHandler(i);
			break;
		case INT_FUNC:
			FuncLoc++;
			FuncHandler(i);
			break;
		case CHAR_FUNC:
			FuncLoc++;
			FuncHandler(i);
			break;
		case VOID_FUNC:
			FuncLoc++;
			FuncHandler(i);
			break;
		/*case PARA_INT:

			break;
		case PARA_CHAR:

			break;*/
		case CALL:
			CallHandler(i);
			break;
		case PUSH:
			PushHandler(i);
			break;
		case FUNCRET:
			FuncRetHandler(i);
			break;
		case VARASSIGN:
			VarAssignHandler(i);
			break;
		case ARRASSIGN:
			ArrAssignHandler(i);
			break;
		case ARRACCESS:
			ArrAccessHandler(i);
			break;
		case LESSOP:
			ConditionHandler(i);
			break;
		case UNMOREOP:
			ConditionHandler(i);
			break;
		case MOREOP:
			ConditionHandler(i);
			break;
		case UNLESSOP:
			ConditionHandler(i);
			break;
		case UNEQUOP:
			ConditionHandler(i);
			break;
		case EQUOP:
			ConditionHandler(i);
			break;
		case SETLABEL:
			SetLabelHandler(i);
			break;
		case GOTO:
			GotoHandler(i);
			break;
		/*case BNZ:

			break;
		case BZ:

			break;*/
		case SCANINT:
			ScanIntHandler(i);
			break;
		case SCANCHAR:
			ScanCharHandler(i);
			break;
		case PRINTINT:
			PrintIntHandler(i);
			break;
		case PRINTCHAR:
			PrintCharHandler(i);
			break;
		case PRINTSTR:
			PrintIntHandler(i);
			break;
		case RETEXPR:
			RetExprHandler(i);
			break;
		case RETNULL:
			RetNullHandler(i);
			break;
		default:
			break;
		}
	}



	MipsOutput.close();
}