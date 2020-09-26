﻿// ScanTest.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "DirScanner.h"


int OnBginScanOne(ScanInfo info) {
	std::cout << "--- begin scan "<<info.fileAttr<<" : " << info.path << "\\" << info.filename
		<< ",size : " << info.fileSize
		<< ",attr : " << info.fileAttr
		<< ",total : " << info.totalSize << std::endl;

	return 0;
}

int OnFinishScanOne(ScanInfo info) {
	std::cout << "   --- finish scan " << info.fileAttr << " : " << info.path << "\\" << info.filename
		<< ",size : " << info.fileSize
		<< ",attr : " << info.fileAttr
		<< ",total : " << info.totalSize << std::endl;

	return 0;
}

int OnAllFinish(ScanInfo info) {
	std::cout << "+++ scan all file finished,total size : " << info.totalSize << std::endl;
	return 0;
}



int main()
{
	CDirScanner scanner("C:\\lx\\program\\c++\\ludashi\\DirScanner");
	scanner.RegistCallback(CALLBACK_STAGE_ON_ONE_BEGIN, OnBginScanOne);
	scanner.RegistCallback(CALLBACK_STAGE_ON_ONE_FINISH, OnFinishScanOne);
	scanner.RegistCallback(CALLBACK_STAGE_ON_FINISH, OnAllFinish);

	scanner.StartScan();

	char c = getchar();

}
