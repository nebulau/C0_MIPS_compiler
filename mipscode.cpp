#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include "compiler.h"
using namespace std;
ofstream MipsOutput;
int strnum = 0;//计数字符串
int FuncLoc = 0;
int ParaLoc = 0;
int VariLoc = 0;
int VariAddr = 0;
struct VariInfo{
	char VariName[idlen];
	int addr;//相对于函数基地址的偏移
	//int type;//int or char
};
struct VariInfo VariTab[info_size];
struct FuncInfo {
	char func_name[idlen];
	struct VariInfo VariTab[info_size];
	//int retvalue;
	//int retaddr;
	int InfoSize;//包含参数数据size
	int ParaNum;
	int VariNum;//包含参数、局部变量、临时变量
};
struct FuncInfo FuncTab[info_size];
char* str_name_gen() {
	static char str_name[idlen];
	sprintf_s(str_name, "$str%d", strnum++);
	return str_name;
}
int get_index(char* ident, int funcnum, int* addr) {//查找是不是在函数信息表里，返回下标，否则是全局变量返回-1，并将偏移存在addr
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
	//funcnum为compiler.h定义的全局函数个数
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
	int op1 = -1;
	int op2 = -1;
	string str_tp;
	int ParaNum = FuncTab[FuncLoc].ParaNum;//当前函数的参数个数
	//如果argu1是数字
	if (str_is_num(midcode[i].argu1)) {
		str_tp = midcode[i].argu1;
		MipsOutput << "\tli $t0, " << str_tp << '\n';
	}
	else
	{
		index_tp = get_index(midcode[i].argu1, FuncLoc, &addr_tp);
		if (index_tp == -1) {//是全局变量
			str_tp = midcode[i].argu1;
			MipsOutput << "\tlw $t0, " << str_tp << '\n';
		}
		else if (index_tp < ParaNum) {//参数从栈加载出来
			MipsOutput << "\tlw $t0, " << -addr_tp << "($fp)\n";
		}
		else if (index_tp >= ParaNum && index_tp <= ParaNum + 7) {
			//MipsOutput << "\tmove $t0, $s" << index_tp - ParaNum << '\n';
			op1 = index_tp - ParaNum;
		}
		else {//是未分配到s系寄存器的变量
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
		if (index_tp == -1) {//是全局变量
			str_tp = midcode[i].argu2;
			MipsOutput << "\tlw $t1, " << str_tp << '\n';
		}
		else if (index_tp < ParaNum) {//参数从栈加载出来
			MipsOutput << "\tlw $t1, " << -addr_tp << "($fp)\n";
		}
		else if (index_tp >= ParaNum && index_tp <= ParaNum + 7) {
			//MipsOutput << "\tmove $t1, $s" << index_tp - ParaNum << '\n';
			op2 = index_tp - ParaNum;
		}
		else {//是未分配到s系寄存器的变量
			MipsOutput << "\tlw $t1, " << -addr_tp << "($fp)\n";
			//TOTHINK!!!
		}
	}
	//addu
	if (op1 != -1) {
		if (op2 != -1) 
			MipsOutput << "\taddu $t0, $s" << op1 << ", $s" << op2 << "\n";
		else
			MipsOutput << "\taddu $t0, $s" << op1 << ", $t1\n";
	}
	else {
		if (op2 != -1) 
			MipsOutput << "\taddu $t0, $t0, $s" << op2 << "\n";
		else
			MipsOutput << "\taddu $t0, $t0, $t1\n";
	}

	index_tp = get_index(midcode[i].result, FuncLoc, &addr_tp);
	if (index_tp == -1) {
		//全局变量
		//MipsOutput << "\taddu $t0, $t0, $t1\n";
		str_tp = midcode[i].result;
		MipsOutput << "\tsw $t0, " << midcode[i].result << '\n';
	}
	else if (index_tp < ParaNum) {
		//MipsOutput << "\taddu $t0, $t0, $t1\n";
		MipsOutput << "\tsw $t0, " << -addr_tp << "($fp)\n";
	}
	else if (index_tp >= ParaNum && index_tp <= ParaNum + 7) {
		//MipsOutput << "\taddu $s" << index_tp - ParaNum << ", $t0, $t1\n";
		MipsOutput << "\tmove $s" << index_tp - ParaNum << ", $t0\n";
	}
	else {//未分配到s寄存器的变量
		//MipsOutput << "\taddu $t0, $t0, $t1\n";
		MipsOutput << "\tsw $t0, " << -addr_tp << "($fp)\n";
	}
}
void SubHandler(int i) {
	//midcode[i]
	int addr_tp = 0;
	int index_tp;
	int op1 = -1;
	int op2 = -1;
	string str_tp;
	int ParaNum = FuncTab[FuncLoc].ParaNum;//当前函数的参数个数
	//如果argu1是数字
	if (str_is_num(midcode[i].argu1)) {
		str_tp = midcode[i].argu1;
		MipsOutput << "\tli $t0, " << str_tp << '\n';
	}
	else
	{
		index_tp = get_index(midcode[i].argu1, FuncLoc, &addr_tp);
		if (index_tp == -1) {//是全局变量
			str_tp = midcode[i].argu1;
			MipsOutput << "\tlw $t0, " << str_tp << '\n';
		}
		else if (index_tp < ParaNum) {//参数从栈加载出来
			MipsOutput << "\tlw $t0, " << -addr_tp << "($fp)\n";
		}
		else if (index_tp >= ParaNum && index_tp <= ParaNum + 7) {
			//MipsOutput << "\tmove $t0, $s" << index_tp - ParaNum << '\n';
			op1 = index_tp - ParaNum;
		}
		else {//是未分配到s系寄存器的变量
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
		if (index_tp == -1) {//是全局变量
			str_tp = midcode[i].argu2;
			MipsOutput << "\tlw $t1, " << str_tp << '\n';
		}
		else if (index_tp < ParaNum) {//参数从栈加载出来
			MipsOutput << "\tlw $t1, " << -addr_tp << "($fp)\n";
		}
		else if (index_tp >= ParaNum && index_tp <= ParaNum + 7) {
			//MipsOutput << "\tmove $t1, $s" << index_tp - ParaNum << '\n';
			op2 = index_tp - ParaNum;
		}
		else {//是未分配到s系寄存器的变量
			MipsOutput << "\tlw $t1, " << -addr_tp << "($fp)\n";
			//TOTHINK!!!
		}
	}
	//addu
	if (op1 != -1) {
		if (op2 != -1)
			MipsOutput << "\tsub $t0, $s" << op1 << ", $s" << op2 << "\n";
		else
			MipsOutput << "\tsub $t0, $s" << op1 << ", $t1\n";
	}
	else {
		if (op2 != -1)
			MipsOutput << "\tsub $t0, $t0, $s" << op2 << "\n";
		else
			MipsOutput << "\tsub $t0, $t0, $t1\n";
	}

	index_tp = get_index(midcode[i].result, FuncLoc, &addr_tp);
	if (index_tp == -1) {
		//全局变量
		//MipsOutput << "\taddu $t0, $t0, $t1\n";
		str_tp = midcode[i].result;
		MipsOutput << "\tsw $t0, " << midcode[i].result << '\n';
	}
	else if (index_tp < ParaNum) {
		//MipsOutput << "\taddu $t0, $t0, $t1\n";
		MipsOutput << "\tsw $t0, " << -addr_tp << "($fp)\n";
	}
	else if (index_tp >= ParaNum && index_tp <= ParaNum + 7) {
		//MipsOutput << "\taddu $s" << index_tp - ParaNum << ", $t0, $t1\n";
		MipsOutput << "\tmove $s" << index_tp - ParaNum << ", $t0\n";
	}
	else {//未分配到s寄存器的变量
		//MipsOutput << "\taddu $t0, $t0, $t1\n";
		MipsOutput << "\tsw $t0, " << -addr_tp << "($fp)\n";
	}
}
void MulHandler(int i) {
	//midcode[i]
	int addr_tp = 0;
	int index_tp;
	int op1 = -1;
	int op2 = -1;
	string str_tp;
	int ParaNum = FuncTab[FuncLoc].ParaNum;//当前函数的参数个数
	//如果argu1是数字
	if (str_is_num(midcode[i].argu1)) {
		str_tp = midcode[i].argu1;
		MipsOutput << "\tli $t0, " << str_tp << '\n';
	}
	else
	{
		index_tp = get_index(midcode[i].argu1, FuncLoc, &addr_tp);
		if (index_tp == -1) {//是全局变量
			str_tp = midcode[i].argu1;
			MipsOutput << "\tlw $t0, " << str_tp << '\n';
		}
		else if (index_tp < ParaNum) {//参数从栈加载出来
			MipsOutput << "\tlw $t0, " << -addr_tp << "($fp)\n";
		}
		else if (index_tp >= ParaNum && index_tp <= ParaNum + 7) {
			//MipsOutput << "\tmove $t0, $s" << index_tp - ParaNum << '\n';
			op1 = index_tp - ParaNum;
		}
		else {//是未分配到s系寄存器的变量
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
		if (index_tp == -1) {//是全局变量
			str_tp = midcode[i].argu2;
			MipsOutput << "\tlw $t1, " << str_tp << '\n';
		}
		else if (index_tp < ParaNum) {//参数从栈加载出来
			MipsOutput << "\tlw $t1, " << -addr_tp << "($fp)\n";
		}
		else if (index_tp >= ParaNum && index_tp <= ParaNum + 7) {
			//MipsOutput << "\tmove $t1, $s" << index_tp - ParaNum << '\n';
			op2 = index_tp - ParaNum;
		}
		else {//是未分配到s系寄存器的变量
			MipsOutput << "\tlw $t1, " << -addr_tp << "($fp)\n";
			//TOTHINK!!!
		}
	}
	//addu
	if (op1 != -1) {
		if (op2 != -1)
			MipsOutput << "\tmul $t0, $s" << op1 << ", $s" << op2 << "\n";
		else
			MipsOutput << "\tmul $t0, $s" << op1 << ", $t1\n";
	}
	else {
		if (op2 != -1)
			MipsOutput << "\tmul $t0, $t0, $s" << op2 << "\n";
		else
			MipsOutput << "\tmul $t0, $t0, $t1\n";
	}

	index_tp = get_index(midcode[i].result, FuncLoc, &addr_tp);
	if (index_tp == -1) {
		//全局变量
		//MipsOutput << "\taddu $t0, $t0, $t1\n";
		str_tp = midcode[i].result;
		MipsOutput << "\tsw $t0, " << midcode[i].result << '\n';
	}
	else if (index_tp < ParaNum) {
		//MipsOutput << "\taddu $t0, $t0, $t1\n";
		MipsOutput << "\tsw $t0, " << -addr_tp << "($fp)\n";
	}
	else if (index_tp >= ParaNum && index_tp <= ParaNum + 7) {
		//MipsOutput << "\taddu $s" << index_tp - ParaNum << ", $t0, $t1\n";
		MipsOutput << "\tmove $s" << index_tp - ParaNum << ", $t0\n";
	}
	else {//未分配到s寄存器的变量
		//MipsOutput << "\taddu $t0, $t0, $t1\n";
		MipsOutput << "\tsw $t0, " << -addr_tp << "($fp)\n";
	}
}
void DivHandler(int i) {
	//midcode[i]
	int addr_tp = 0;
	int index_tp;
	int op1 = -1;
	int op2 = -1;
	string str_tp;
	int ParaNum = FuncTab[FuncLoc].ParaNum;//当前函数的参数个数
	//如果argu1是数字
	if (str_is_num(midcode[i].argu1)) {
		str_tp = midcode[i].argu1;
		MipsOutput << "\tli $t0, " << str_tp << '\n';
	}
	else
	{
		index_tp = get_index(midcode[i].argu1, FuncLoc, &addr_tp);
		if (index_tp == -1) {//是全局变量
			str_tp = midcode[i].argu1;
			MipsOutput << "\tlw $t0, " << str_tp << '\n';
		}
		else if (index_tp < ParaNum) {//参数从栈加载出来
			MipsOutput << "\tlw $t0, " << -addr_tp << "($fp)\n";
		}
		else if (index_tp >= ParaNum && index_tp <= ParaNum + 7) {
			//MipsOutput << "\tmove $t0, $s" << index_tp - ParaNum << '\n';
			op1 = index_tp - ParaNum;
		}
		else {//是未分配到s系寄存器的变量
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
		if (index_tp == -1) {//是全局变量
			str_tp = midcode[i].argu2;
			MipsOutput << "\tlw $t1, " << str_tp << '\n';
		}
		else if (index_tp < ParaNum) {//参数从栈加载出来
			MipsOutput << "\tlw $t1, " << -addr_tp << "($fp)\n";
		}
		else if (index_tp >= ParaNum && index_tp <= ParaNum + 7) {
			//MipsOutput << "\tmove $t1, $s" << index_tp - ParaNum << '\n';
			op2 = index_tp - ParaNum;
		}
		else {//是未分配到s系寄存器的变量
			MipsOutput << "\tlw $t1, " << -addr_tp << "($fp)\n";
			//TOTHINK!!!
		}
	}
	//addu
	if (op1 != -1) {
		if (op2 != -1)
			MipsOutput << "\tdiv $t0, $s" << op1 << ", $s" << op2 << "\n";
		else
			MipsOutput << "\tdiv $t0, $s" << op1 << ", $t1\n";
	}
	else {
		if (op2 != -1)
			MipsOutput << "\tdiv $t0, $t0, $s" << op2 << "\n";
		else
			MipsOutput << "\tdiv $t0, $t0, $t1\n";
	}

	index_tp = get_index(midcode[i].result, FuncLoc, &addr_tp);
	if (index_tp == -1) {
		//全局变量
		//MipsOutput << "\taddu $t0, $t0, $t1\n";
		str_tp = midcode[i].result;
		MipsOutput << "\tsw $t0, " << midcode[i].result << '\n';
	}
	else if (index_tp < ParaNum) {
		//MipsOutput << "\taddu $t0, $t0, $t1\n";
		MipsOutput << "\tsw $t0, " << -addr_tp << "($fp)\n";
	}
	else if (index_tp >= ParaNum && index_tp <= ParaNum + 7) {
		//MipsOutput << "\taddu $s" << index_tp - ParaNum << ", $t0, $t1\n";
		MipsOutput << "\tmove $s" << index_tp - ParaNum << ", $t0\n";
	}
	else {//未分配到s寄存器的变量
		//MipsOutput << "\taddu $t0, $t0, $t1\n";
		MipsOutput << "\tsw $t0, " << -addr_tp << "($fp)\n";
	}
}
void FuncHandler(int i) {
	int addr_tp = 0;
	//int index_tp;
	string str_tp;
	int ParaNum = FuncTab[FuncLoc].ParaNum;//当前函数的参数个数
	//main函数特殊处理
	if (strcmp(midcode[i].argu1, "main") == 0) {
		MipsOutput << "main:\n";
		MipsOutput << "#保存帧指针\n\tmove $fp, $sp\n";
		MipsOutput << "#退栈空间\n\tsub $sp, $sp, " << FuncTab[FuncLoc].InfoSize << '\n';
	}
	else {
		str_tp = midcode[i].argu1;
		MipsOutput << str_tp << ":\n";
		MipsOutput << "#为函数局部变量和临时变量退栈空间\n#退栈空间，接着保存s系寄存器和$ra,$fp\n";//为函数局部变量和临时变量退栈空间
		MipsOutput << "\tsub $sp, $sp, " << FuncTab[FuncLoc].InfoSize - FuncTab[FuncLoc].ParaNum * 4 + 40<< '\n';
		for (int w = 0; w < FuncTab[FuncLoc].VariNum - FuncTab[FuncLoc].ParaNum && w < 8; w++) {
			MipsOutput << "\tsw $s" << w << ", " << 40 - w * 4 << "($sp)\n";
		}
		/*MipsOutput << "\tsw $s0, 40($sp)\n\tsw $s1, 36($sp)\n";
		MipsOutput << "\tsw $s2, 32($sp)\n\tsw $s3, 28($sp)\n";
		MipsOutput << "\tsw $s4, 24($sp)\n\tsw $s5, 20($sp)\n";
		MipsOutput << "\tsw $s6, 16($sp)\n\tsw $s7, 12($sp)\n";*/
		MipsOutput << "\tsw $ra, 8($sp)\n\tsw $fp, 4($sp)\n";
		MipsOutput << "#置$fp为被调用者基地址\n\tadd $fp, $sp, " << FuncTab[FuncLoc].InfoSize + 40 << "\n";
	}

}
void CallHandler(int i) {
	string str_tp;
	str_tp = midcode[i].argu1;
	MipsOutput << "\tjal " << str_tp << '\n';
}
void PushHandler(int i) {
	int addr_tp = 0;
	int index_tp, op1 = -1;
	string str_tp;
	int ParaNum = FuncTab[FuncLoc].ParaNum;//当前函数的参数个数
	//如果argu1是数字
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
		else if (index_tp < ParaNum) {//参数从栈加载出来
			MipsOutput << "\tlw $t0, " << -addr_tp << "($fp)\n";
		}
		else if (index_tp >= ParaNum && index_tp <= ParaNum + 7) {
			//MipsOutput << "\tmove $t0, $s" << index_tp - ParaNum << '\n';
			op1 = index_tp - ParaNum;
		}
		else {//是未分配到s系寄存器的变量
			MipsOutput << "\tlw $t0, " << -addr_tp << "($fp)\n";
			//TOTHINK!!!
		}
	}
	if (op1 == -1)
		MipsOutput << "\tsw $t0, 0($sp)\n\tsub $sp, $sp, 4\n";
	else
		MipsOutput << "\tsw $s" << op1 << ", 0($sp)\n\tsub $sp, $sp, 4\n";
}
void FuncRetHandler(int i) {
	int addr_tp = 0;
	int index_tp;
	string str_tp;
	int ParaNum = FuncTab[FuncLoc].ParaNum;//当前函数的参数个数
	//即result = $v0,临时起名的。一定在函数变量表
	index_tp = get_index(midcode[i].result, FuncLoc, &addr_tp);
	if (index_tp >= ParaNum + 0 && index_tp <= ParaNum + 7) {
		MipsOutput << "\tmove $s" << index_tp - ParaNum << ", $v0\n";
		//MipsOutput << "\tmove $v0, $s" << index_tp << "\n";
	}
	else {
		MipsOutput << "\tsw $v0, " << -addr_tp << "($fp)\n";
	}
}
void VarAssignHandler(int i) {
	int addr_tp = 0;
	int index_tp;
	int op1 = -1;
	string str_tp;
	int ParaNum = FuncTab[FuncLoc].ParaNum;//当前函数的参数个数
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
			//MipsOutput << "\tmove $t0, $s" << index_tp - ParaNum << '\n';
			op1 = index_tp - ParaNum;
		}
		else {
			MipsOutput << "\tlw $t0, " << -addr_tp << "($fp)\n";
		}
	}
	index_tp = get_index(midcode[i].result, FuncLoc, &addr_tp);
	if (index_tp == -1) {
		str_tp = midcode[i].result;
		if (op1 == -1)
			MipsOutput << "\tsw $t0, " << str_tp << '\n';
		else
			MipsOutput << "\tsw $s" << op1 << ", " << str_tp << '\n';
	}
	else if (index_tp < ParaNum) {
		if (op1 == -1)
			MipsOutput << "\tsw $t0, " << -addr_tp << "($fp)\n";
		else
			MipsOutput << "\tsw $s" << op1 << ", " << -addr_tp << "($fp)\n";
	}
	else if (index_tp >= ParaNum + 0 && index_tp <= ParaNum + 7) {
		if (op1 == -1)
			MipsOutput << "\tmove $s" << index_tp - ParaNum << ", $t0" << '\n';
		else
			MipsOutput << "\tmove $s" << index_tp - ParaNum << ", $s" << op1 << '\n';
	}
	else {//需要写进函数运行栈
		if (op1 == -1)
			MipsOutput << "\tsw $t0, " << -addr_tp << "($fp)\n";
		else
			MipsOutput << "\tsw $s" << op1 << ", " << -addr_tp << "($fp)\n";
	}
}
void ArrAssignHandler(int i) {
	//!!!如果是全局数组就是正偏移，局部数组在栈里是负偏移
	int addr_tp = 0;
	int index_tp;
	string str_tp;
	int op1 = -1;
	int op2 = -1;
	int isGlobalArr = 0;
	int ParaNum = FuncTab[FuncLoc].ParaNum;//当前函数的参数个数
	index_tp = get_index(midcode[i].result, FuncLoc, &addr_tp);
	//$t0存数组地址
	if (index_tp == -1) {
		//全局数组
		str_tp = midcode[i].result;
		MipsOutput << "\tla $t0, " << str_tp << '\n';
		isGlobalArr = 1;
	}
	else {
		MipsOutput << "\tla $t0, " << -addr_tp << "($fp)\n";
	}
	//$t1存偏移
	if (str_is_num(midcode[i].argu1)) {
		str_tp = midcode[i].argu1;
		MipsOutput << "\tli $t1, " << str_tp << '\n';
	}
	else {
		index_tp = get_index(midcode[i].argu1, FuncLoc, &addr_tp);
		if (index_tp == -1) {//是全局变量
			str_tp = midcode[i].argu1;
			MipsOutput << "\tlw $t1, " << str_tp << '\n';
		}
		else if (index_tp < ParaNum) {
			MipsOutput << "\tlw $t1, " << -addr_tp << "($fp)\n";
		}
		else if (index_tp >= ParaNum + 0 && index_tp <= ParaNum + 7) {
			//MipsOutput << "\tmove $t1, $s" << index_tp - ParaNum << '\n';
			op1 = index_tp - ParaNum;
		}
		else {
			MipsOutput << "\tlw $t1, " << -addr_tp << "($fp)\n";
		}
	}
	//$t1*4
	MipsOutput << "\tli $t2, 4\n";
	if(op1 == -1)
		MipsOutput << "\tmul $t1, $t1, $t2\n";
	else
		MipsOutput << "\tmul $t1, $s"<< op1 <<", $t2\n";
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
			//MipsOutput << "\tmove $t2, $s" << index_tp - ParaNum << '\n';
			op2 = index_tp - ParaNum;
		}
		else {
			MipsOutput << "\tlw $t2, " << -addr_tp << "($fp)\n";
		}
	}
	if(isGlobalArr)
		MipsOutput << "\tadd $t0, $t0, $t1\n";
	else
		MipsOutput << "\tsub $t0, $t0, $t1\n";
	if (op2 == -1)
		MipsOutput << "\tsw $t2, 0($t0)\n";
	else
		MipsOutput << "\tsw $s" << op2 << ", 0($t0)\n";
}
void ArrAccessHandler(int i) {
	int addr_tp = 0;
	int index_tp;
	string str_tp;
	int op1 = -1, op2 = -1;
	int isGlobalArr = 0;
	int ParaNum = FuncTab[FuncLoc].ParaNum;//当前函数的参数个数
	index_tp = get_index(midcode[i].argu1, FuncLoc, &addr_tp);
	//$t0数组名， $t1下标， $t2左式
	if(index_tp == -1){//是全局数组
		str_tp = midcode[i].argu1;
		MipsOutput << "\tla $t0, " << str_tp << '\n';
		isGlobalArr = 1;
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
		if (index_tp == -1) {//是全局变量
			str_tp = midcode[i].argu2;
			MipsOutput << "\tlw $t1, " << str_tp << '\n';
		}
		else if (index_tp < ParaNum) {
			MipsOutput << "\tlw $t1, " << -addr_tp << "($fp)\n";
		}
		else if (index_tp >= ParaNum + 0 && index_tp <= ParaNum + 7) {
			//MipsOutput << "\tmove $t1, $s" << index_tp - ParaNum << '\n';
			op1 = index_tp - ParaNum;
		}
		else {
			MipsOutput << "\tlw $t1, " << -addr_tp << "($fp)\n";
		}
	}
	//$t1*4
	MipsOutput << "\tli $t2, 4\n";
	if (op1 == -1)
		MipsOutput << "\tmul $t1, $t1, $t2\n";
	else
		MipsOutput << "\tmul $t1, $s" << op1 << ", $t2\n";
	//$t2 = array[$t1]
	if(isGlobalArr)
		MipsOutput << "\tadd $t0, $t0, $t1\n";
	else
		MipsOutput << "\tsub $t0, $t0, $t1\n";
	//MipsOutput << "\tlw $t2, 0($t0)\n";
	index_tp = get_index(midcode[i].result, FuncLoc, &addr_tp);
	if (index_tp == -1) {
		str_tp = midcode[i].result;
		MipsOutput << "\tlw $t2, 0($t0)\n";
		MipsOutput << "\tsw $t2, " << str_tp << '\n';
	}
	else if (index_tp < ParaNum) {
		MipsOutput << "\tlw $t2, 0($t0)\n";
		MipsOutput << "\tsw $t2, " << -addr_tp << "($fp)\n";
	}
	else if (index_tp >= ParaNum + 0 && index_tp <= ParaNum + 7) {
		MipsOutput << "\tlw $s" << index_tp - ParaNum << ", 0($t0)\n";
	}
	else {
		MipsOutput << "\tlw $t2, 0($t0)\n";
		MipsOutput << "\tsw $t2, " << -addr_tp << "($fp)\n";
	}
}
void ConditionHandler(int i) {
	/*
		需要将i+1所以传入指针
		没有UNEQUOP即!=
		EQUOP之后会有BZ 等于0跳转 
		EQUOP之后会有BNZ 不等跳转
		其余的之后只有BNZ
	*/
	int addr_tp = 0;
	int index_tp;
	string str_tp;
	int op1 = -1, op2 = -1;
	int ParaNum = FuncTab[FuncLoc].ParaNum;//当前函数的参数个数
	if (midcode[i].type == LESSOP) {
		//bge $t0, $t1, label 大于等于跳转
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
				//MipsOutput << "\tmove $t0, $s" << index_tp - ParaNum << '\n';
				op1 = index_tp - ParaNum;
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
				//MipsOutput << "\tmove $t1, $s" << index_tp - ParaNum << '\n';
				op2 = index_tp - ParaNum;
			}
			else {
				MipsOutput << "\tlw $t1, " << -addr_tp << "($fp)\n";
			}
		}
		str_tp = midcode[i + 1].argu1;
		if (op1 == -1) {
			if(op2 == -1)
				MipsOutput << "\tbge $t0, $t1, " << str_tp << '\n';
			else
				MipsOutput << "\tbge $t0, $s" << op2 << ", " << str_tp << '\n';
		}
		else {
			if(op2 == -1)
				MipsOutput << "\tbge $s" << op1 << ", $t1, " << str_tp << '\n';
			else
				MipsOutput << "\tbge $s" << op1 << ", $s" << op2 << ", " << str_tp << '\n';
		}
	}
	else if (midcode[i].type == UNMOREOP) {
		//bgt 大于跳转
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
				//MipsOutput << "\tmove $t0, $s" << index_tp - ParaNum << '\n';
				op1 = index_tp - ParaNum;
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
				//MipsOutput << "\tmove $t1, $s" << index_tp - ParaNum << '\n';
				op2 = index_tp - ParaNum;
			}
			else {
				MipsOutput << "\tlw $t1, " << -addr_tp << "($fp)\n";
			}
		}
		str_tp = midcode[i + 1].argu1;
		if (op1 == -1) {
			if (op2 == -1)
				MipsOutput << "\tbgt $t0, $t1, " << str_tp << '\n';
			else
				MipsOutput << "\tbgt $t0, $s" << op2 << ", " << str_tp << '\n';
		}
		else {
			if (op2 == -1)
				MipsOutput << "\tbgt $s" << op1 << ", $t1, " << str_tp << '\n';
			else
				MipsOutput << "\tbgt $s" << op1 << ", $s" << op2 << ", " << str_tp << '\n';
		}
	}
	else if (midcode[i].type == MOREOP) {
		//ble 小于等于跳转
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
				//MipsOutput << "\tmove $t0, $s" << index_tp - ParaNum << '\n';
				op1 = index_tp - ParaNum;
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
				//MipsOutput << "\tmove $t1, $s" << index_tp - ParaNum << '\n';
				op2 = index_tp - ParaNum;
			}
			else {
				MipsOutput << "\tlw $t1, " << -addr_tp << "($fp)\n";
			}
		}
		str_tp = midcode[i + 1].argu1;
		if (op1 == -1) {
			if (op2 == -1)
				MipsOutput << "\tble $t0, $t1, " << str_tp << '\n';
			else
				MipsOutput << "\tble $t0, $s" << op2 << ", " << str_tp << '\n';
		}
		else {
			if (op2 == -1)
				MipsOutput << "\tble $s" << op1 << ", $t1, " << str_tp << '\n';
			else
				MipsOutput << "\tble $s" << op1 << ", $s" << op2 << ", " << str_tp << '\n';
		}
	}
	else if (midcode[i].type == UNLESSOP) {
		//blt 小于跳转
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
				//MipsOutput << "\tmove $t0, $s" << index_tp - ParaNum << '\n';
				op1 = index_tp - ParaNum;
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
				//MipsOutput << "\tmove $t1, $s" << index_tp - ParaNum << '\n';
				op2 = index_tp - ParaNum;
			}
			else {
				MipsOutput << "\tlw $t1, " << -addr_tp << "($fp)\n";
			}
		}
		str_tp = midcode[i + 1].argu1;
		if (op1 == -1) {
			if (op2 == -1)
				MipsOutput << "\tblt $t0, $t1, " << str_tp << '\n';
			else
				MipsOutput << "\tblt $t0, $s" << op2 << ", " << str_tp << '\n';
		}
		else {
			if (op2 == -1)
				MipsOutput << "\tblt $s" << op1 << ", $t1, " << str_tp << '\n';
			else
				MipsOutput << "\tblt $s" << op1 << ", $s" << op2 << ", " << str_tp << '\n';
		}
	}
	//没有UNEQUOP
	else if (midcode[i].type == EQUOP) {
		if (midcode[i + 1].type == BZ) {
			//argu1等于0跳转
			if (str_is_num(midcode[i].argu1)) {
				str_tp = midcode[i].argu1;
				MipsOutput << "\tli $t0, " << str_tp << '\n';
				MipsOutput << "\tbeq $t0, $0, ";
				str_tp = midcode[i + 1].argu1;
				MipsOutput << str_tp << '\n';
			}
			else {
				index_tp = get_index(midcode[i].argu1, FuncLoc, &addr_tp);
				if (index_tp == -1) {//是全局变量
					str_tp = midcode[i].argu1;
					MipsOutput << "\tlw $t0, " << str_tp << '\n';
					MipsOutput << "\tbeq $t0, $0, ";
					str_tp = midcode[i + 1].argu1;
					MipsOutput << str_tp << '\n';
				}
				else if (index_tp >= 0 && index_tp <= 7) {
					//MipsOutput << "\tmove $t0, $s" << index_tp - ParaNum << '\n';
					MipsOutput << "\tbeq $s" << index_tp - ParaNum << ", $0, ";
					str_tp = midcode[i + 1].argu1;
					MipsOutput << str_tp << '\n';
				}
				else {//在栈里
					MipsOutput << "\tlw $t0, " << -addr_tp << "($fp)\n";
					MipsOutput << "\tbeq $t0, $0, ";
					str_tp = midcode[i + 1].argu1;
					MipsOutput << str_tp << '\n';
				}
			}
		}
		else if (midcode[i + 1].type == BNZ) {
			//argu1和2不等跳转，argu2是数字
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
					//MipsOutput << "\tmove $t0, $s" << index_tp - ParaNum << '\n';
					op1 = index_tp - ParaNum;
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
					//MipsOutput << "\tmove $t1, $s" << index_tp - ParaNum << '\n';
					op2 = index_tp - ParaNum;
				}
				else {
					MipsOutput << "\tlw $t1, " << -addr_tp << "($fp)\n";
				}
			}

			/*str_tp = midcode[i].argu2;
			MipsOutput << "\tli $t1, " << str_tp;*/
			str_tp = midcode[i + 1].argu1;
			if (op1 == -1) {
				if (op2 == -1)
					MipsOutput << "\tbne $t0, $t1, " << str_tp << '\n';
				else
					MipsOutput << "\tbne $t0, $s" << op2 << ", " << str_tp << '\n';
			}
			else {
				if (op2 == -1)
					MipsOutput << "\tbne $s" << op1 << ", $t1, " << str_tp << '\n';
				else
					MipsOutput << "\tbne $s" << op1 << ", $s" << op2 << ", " << str_tp << '\n';
			}
		}
	}
	else if (midcode[i].type == UNEQUOP) {
		//beq $t0, $t1, label 大于等于跳转
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
				//MipsOutput << "\tmove $t0, $s" << index_tp - ParaNum << '\n';
				op1 = index_tp - ParaNum;
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
				//MipsOutput << "\tmove $t1, $s" << index_tp - ParaNum << '\n';
				op2 = index_tp - ParaNum;
			}
			else {
				MipsOutput << "\tlw $t1, " << -addr_tp << "($fp)\n";
			}
		}
		str_tp = midcode[i + 1].argu1;
		if (op1 == -1) {
			if (op2 == -1)
				MipsOutput << "\tbeq $t0, $t1, " << str_tp << '\n';
			else
				MipsOutput << "\tbeq $t0, $s" << op2 << ", " << str_tp << '\n';
		}
		else {
			if (op2 == -1)
				MipsOutput << "\tbeq $s" << op1 << ", $t1, " << str_tp << '\n';
			else
				MipsOutput << "\tbeq $s" << op1 << ", $s" << op2 << ", " << str_tp << '\n';
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
	int ParaNum = FuncTab[FuncLoc].ParaNum;//当前函数的参数个数
	MipsOutput << "#读取int\n\tli $v0, 5\n";
	MipsOutput << "\tsyscall \n";
	index_tp = get_index(midcode[i].argu1, FuncLoc, &addr_tp);
	if (index_tp == -1) {//是全局变量
		str_tp = midcode[i].argu1;
		MipsOutput << "\tsw $v0, " << str_tp << '\n';
	}
	else if (index_tp < ParaNum) {
		MipsOutput << "\tsw $v0, " << -addr_tp << "($fp)\n";
	}
	else if (index_tp >= ParaNum + 0 && index_tp <= ParaNum + 7) {
		MipsOutput << "\tmove $s" << index_tp - ParaNum << ", $v0\n";
	}
	else {
		MipsOutput << "\tsw $v0, " << -addr_tp << "($fp)\n";
	}
}
void ScanCharHandler(int i) {
	int addr_tp = 0;
	int index_tp;
	string str_tp;
	int ParaNum = FuncTab[FuncLoc].ParaNum;//当前函数的参数个数
	MipsOutput << "#读取char\n\tli $v0, 12\n";
	MipsOutput << "\tsyscall \n";
	index_tp = get_index(midcode[i].argu1, FuncLoc, &addr_tp);
	if (index_tp == -1) {//是全局变量
		str_tp = midcode[i].argu1;
		MipsOutput << "\tsw $v0, " << str_tp << '\n';
	}
	else if (index_tp < ParaNum) {
		MipsOutput << "\tsw $v0, " << -addr_tp << "($fp)\n";
	}
	else if (index_tp >= ParaNum + 0 && index_tp <= ParaNum + 7) {
		MipsOutput << "\tmove $s" << index_tp - ParaNum << ", $v0\n";
	}
	else {
		MipsOutput << "\tsw $v0, " << -addr_tp << "($fp)\n";
	}
}
void PrintIntHandler(int i) {
	int addr_tp = 0;
	int index_tp;
	string str_tp;
	int ParaNum = FuncTab[FuncLoc].ParaNum;//当前函数的参数个数
	if (str_is_num(midcode[i].argu1)) {
		str_tp = midcode[i].argu1;
		MipsOutput << "#打印int\n\tli $a0, " << str_tp << '\n';
		MipsOutput << "\tli $v0, 1\n\tsyscall\n";
	}
	else {
		index_tp = get_index(midcode[i].argu1, FuncLoc, &addr_tp);
		if (index_tp == -1) {
			str_tp = midcode[i].argu1;
			MipsOutput << "#打印int\n\tlw $a0, " << str_tp << '\n';
			MipsOutput << "\tli $v0, 1\n\tsyscall\n";
		}
		else if (index_tp < ParaNum) {
			MipsOutput << "#打印int\n\tlw $a0, " << -addr_tp << "($fp)\n";
			MipsOutput << "\tli $v0, 1\n\tsyscall\n";
		}
		else if (index_tp >= ParaNum + 0 && index_tp <= ParaNum + 7) {
			MipsOutput << "#打印int\n\tmove $a0, $s" << index_tp - ParaNum << '\n';
			MipsOutput << "\tli $v0, 1\n\tsyscall\n";
		}
		else {
			MipsOutput << "#打印int\n\tlw $a0, " << -addr_tp << "($fp)\n";
			MipsOutput << "\tli $v0, 1\n\tsyscall\n";
		}
	}
}
void PrintCharHandler(int i) {
	int addr_tp = 0;
	int index_tp;
	string str_tp;
	int ParaNum = FuncTab[FuncLoc].ParaNum;//当前函数的参数个数
	if (str_is_num(midcode[i].argu1)) {
		str_tp = midcode[i].argu1;
		MipsOutput << "#打印char\n\tli $a0, " << str_tp << '\n';
		MipsOutput << "\tli $v0, 11\n\tsyscall\n";
	}
	else {
		index_tp = get_index(midcode[i].argu1, FuncLoc, &addr_tp);
		if (index_tp == -1) {
			str_tp = midcode[i].argu1;
			MipsOutput << "#打印char\n\tlw $a0, " << str_tp << '\n';
			MipsOutput << "\tli $v0, 11\n\tsyscall\n";
		}
		else if (index_tp < ParaNum) {
			MipsOutput << "#打印char\n\tlw $a0, " << -addr_tp << "($fp)\n";
			MipsOutput << "\tli $v0, 11\n\tsyscall\n";
		}
		else if (index_tp >= ParaNum + 0 && index_tp <= ParaNum + 7) {
			MipsOutput << "#打印char\n\tmove $a0, $s" << index_tp - ParaNum << '\n';
			MipsOutput << "\tli $v0, 11\n\tsyscall\n";
		}
		else {
			MipsOutput << "#打印char\n\tlw $a0, " << -addr_tp << "($fp)\n";
			MipsOutput << "\tli $v0, 11\n\tsyscall\n";
		}
	}
}
void PrintStrHandler(int i) {
	string str_tp;
	str_tp = midcode[i].argu1;
	MipsOutput << "#打印字符串\n\tla $a0, " << str_tp << '\n';
	MipsOutput << "\tli $v0, 4\n\tsyscall\n";
}
void RetExprHandler(int i) {
	int addr_tp = 0;
	int index_tp;
	string str_tp;
	int ParaNum = FuncTab[FuncLoc].ParaNum;//当前函数的参数个数
	//用$v0保存返回值
	if (str_is_num(midcode[i].argu1)) {
		str_tp = midcode[i].argu1;
		MipsOutput << "\tli $v0, " << str_tp << '\n';
	}
	else {
		index_tp = get_index(midcode[i].argu1, FuncLoc, &addr_tp);
		if (index_tp == -1) {
			str_tp = midcode[i].argu1;
			MipsOutput << "\tlw $v0, " << str_tp << '\n';
		}
		else if (index_tp < ParaNum) {
			MipsOutput << "\tlw $v0, " << -addr_tp << "($fp)\n";
		}
		else if (index_tp >= ParaNum + 0 && index_tp <= ParaNum + 7) {
			MipsOutput << "\tmove $v0, $s" << index_tp - ParaNum << '\n';
		}
		else {
			MipsOutput << "\tlw $v0, " << -addr_tp << "($fp)\n";
		}
	}
	//return之前恢复栈指针到调用者的位置
	/*MipsOutput << "\tadd $sp, $sp, 4\n\tlw $fp, 0($sp)\n\tadd $sp, $sp, 4\n\tlw $ra, 0($sp)\n";
	MipsOutput << "\tadd $sp, $sp, 4\n\tlw $s7, 0($sp)\n\tadd $sp, $sp, 4\n\tlw $s6, 0($sp)\n";
	MipsOutput << "\tadd $sp, $sp, 4\n\tlw $s5, 0($sp)\n\tadd $sp, $sp, 4\n\tlw $s4, 0($sp)\n";
	MipsOutput << "\tadd $sp, $sp, 4\n\tlw $s3, 0($sp)\n\tadd $sp, $sp, 4\n\tlw $s2, 0($sp)\n";
	MipsOutput << "\tadd $sp, $sp, 4\n\tlw $s1, 0($sp)\n\tadd $sp, $sp, 4\n\tlw $s0, 0($sp)\n";*/
	/*MipsOutput << "\tlw $fp, 4($sp)\n\tlw $ra, 8($sp)\n\tlw $s7, 12($sp)\n\tlw $s6, 16($sp)\n";
	MipsOutput << "\tlw $s5, 20($sp)\n\tlw $s4, 24($sp)\n\tlw $s3, 28($sp)\n\tlw $s2, 32($sp)\n";
	MipsOutput << "\tlw $s1, 36($sp)\n\tlw $s0, 40($sp)\n";*/
	MipsOutput << "\tlw $fp, 4($sp)\n\tlw $ra, 8($sp)\n";
	for (int w = 0; w < FuncTab[FuncLoc].VariNum - FuncTab[FuncLoc].ParaNum && w < 8; w++) {
		MipsOutput << "\tlw $s" << w << ", " << 40 - w * 4 << "($sp)\n";
	}
	MipsOutput << "\tadd $sp, $sp, " << FuncTab[FuncLoc].InfoSize + 40 << '\n';
	MipsOutput << "\tjr $ra\n";
}
void RetNullHandler(int i) {
	if (FuncLoc == funcnum - 1)
		//main函数返回
		MipsOutput << "\tli $v0, 10\n\tsyscall\n";
	else {
		/*MipsOutput << "\tadd $sp, $sp, 4\n\tlw $fp, 0($sp)\n\tadd $sp, $sp, 4\n\tlw $ra, 0($sp)\n";
		MipsOutput << "\tadd $sp, $sp, 4\n\tlw $s7, 0($sp)\n\tadd $sp, $sp, 4\n\tlw $s6, 0($sp)\n";
		MipsOutput << "\tadd $sp, $sp, 4\n\tlw $s5, 0($sp)\n\tadd $sp, $sp, 4\n\tlw $s4, 0($sp)\n";
		MipsOutput << "\tadd $sp, $sp, 4\n\tlw $s3, 0($sp)\n\tadd $sp, $sp, 4\n\tlw $s2, 0($sp)\n";
		MipsOutput << "\tadd $sp, $sp, 4\n\tlw $s1, 0($sp)\n\tadd $sp, $sp, 4\n\tlw $s0, 0($sp)\n";*/
		/*MipsOutput << "\tlw $fp, 4($sp)\n\tlw $ra, 8($sp)\n\tlw $s7, 12($sp)\n\tlw $s6, 16($sp)\n";
		MipsOutput << "\tlw $s5, 20($sp)\n\tlw $s4, 24($sp)\n\tlw $s3, 28($sp)\n\tlw $s2, 32($sp)\n";
		MipsOutput << "\tlw $s1, 36($sp)\n\tlw $s0, 40($sp)\n";*/
		MipsOutput << "\tlw $fp, 4($sp)\n\tlw $ra, 8($sp)\n";
		for (int w = 0; w < FuncTab[FuncLoc].VariNum - FuncTab[FuncLoc].ParaNum && w < 8; w++) {
			MipsOutput << "\tlw $s" << w << ", " << 40 - w * 4 << "($sp)\n";
		}
		MipsOutput << "\tadd $sp, $sp, " << FuncTab[FuncLoc].InfoSize + 40<< '\n';
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
		将字符串写进data段，并把PRINTSTR的argu1改名
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
		将函数信息表、变量表初始化
		变量只有函数内定义的变量和$开头的临时变量
	*/
	for (i = 0; i < midcodec; i++) {
		//遇到函数开始统计
		if (midcode[i].type >= 10 && midcode[i].type <= 12) {
			ParaLoc = 0;
			VariLoc = 0;
			VariAddr = 0;//40是因为要预留空间存$ra和$fp和s系寄存器
			strcpy_s(FuncTab[FuncLoc].func_name, midcode[i].argu1);
			for (j = i + 1; j < midcodec; j++) {
				//遇到下一个函数跳出
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
	FuncLoc = -1;//开始顺序处理中间代码FuncLoc归为-1
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
			PrintStrHandler(i);
			break;
		case RETEXPR:
			//return 表达式
			RetExprHandler(i);
			break;
		case RETNULL:
			//return;
			RetNullHandler(i);
			break;
		case PRINTNEWLINE:
			//MipsOutput << "\tli $a0, 10\n\tli $v0, 11\n\tsyscall\n";
			break;
		default:
			break;
		}
	}
	MipsOutput.close();
}