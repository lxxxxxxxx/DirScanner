// DirScanner.cpp : 定义 DLL 的导出函数。
//

#include "framework.h"
#include "DirScanner.h"

#include <iostream>
#include <thread>


// 这是导出变量的一个示例
DIRSCANNER_API int nDirScanner = 0;

// 这是导出函数的一个示例。
DIRSCANNER_API int fnDirScanner(void)
{
	return 0;
}

// 这是已导出类的构造函数。
CDirScanner::CDirScanner():m_totalSize(0)
{
	m_exitEvent = CreateEventA(NULL, TRUE, FALSE, NULL);
	return;
}
CDirScanner::CDirScanner(std::string root) :m_rootPath(root) , m_totalSize(0) {
	m_exitEvent = CreateEventA(NULL, TRUE, FALSE, NULL);
}

void CDirScanner::RegistCallback(CallbackStage stage, CallbackFunc func) {
	m_callbacks[stage] = func;
}

void CDirScanner::UnRegistCallback(CallbackStage stage) {
	if (m_callbacks.count(stage)) {
		m_callbacks.erase(stage);
	}
}

void CDirScanner::SetRootPath(std::string root) {
	m_rootPath = root;
}
std::string CDirScanner::GetRootPath() {
	return m_rootPath;
}

bool CDirScanner::ScanFile(std::string path,std::string filename) {
	std::string realFilePath = path +"\\"+ filename;

	WIN32_FIND_DATAA finddata;
	HANDLE file = FindFirstFileA(realFilePath.c_str(), &finddata);

	if (INVALID_HANDLE_VALUE == file) {
		return false;
	}

	//扫描文件后调用回调
	ScanInfo info;
	info.fileAttr = "file";
	info.path = path;
	info.filename = std::string(finddata.cFileName);
	info.fileSize = (finddata.nFileSizeHigh * (MAXDWORD + 1)) + finddata.nFileSizeLow;
	info.totalSize = GetTotalSize();
	if (m_callbacks.count(CALLBACK_STAGE_ON_ONE_BEGIN)) {
		m_callbacks[CALLBACK_STAGE_ON_ONE_BEGIN](info);
	}

	//do scan

	//扫描文件后调用回调
	AddTotalSize(info.fileSize);  //增加总大小
	info.fileAttr = "file";
	info.path = path;
	info.filename = std::string(finddata.cFileName);
	info.fileSize = (finddata.nFileSizeHigh * (MAXDWORD + 1)) + finddata.nFileSizeLow;
	info.totalSize = GetTotalSize();
	if (m_callbacks.count(CALLBACK_STAGE_ON_ONE_FINISH)) {
		m_callbacks[CALLBACK_STAGE_ON_ONE_FINISH](info);
	}

	return true;
}

bool CDirScanner::ScanDir(std::string dir) {
	std::string path = dir;
	if (*(path.rbegin()) != '\\') {
		path += "\\";
	}
	path += "*";
	WIN32_FIND_DATAA finddata;
	HANDLE file = FindFirstFileA(path.c_str(), &finddata);

	//扫描目录前调用回调
	ScanInfo info;
	info.fileAttr = "dir";
	info.path = dir;
	info.filename = std::string(finddata.cFileName);
	info.fileSize = (finddata.nFileSizeHigh * (MAXDWORD + 1)) + finddata.nFileSizeLow;
	info.totalSize = GetTotalSize();
	if (m_callbacks.count(CALLBACK_STAGE_ON_ONE_BEGIN)) {
		m_callbacks[CALLBACK_STAGE_ON_ONE_BEGIN](info);
	}

	while (file != INVALID_HANDLE_VALUE) {
		//std::cout << "scaning " << finddata.cFileName << std::endl;
		if (!strcmp(finddata.cFileName,".") || !strcmp(finddata.cFileName,"..")) {
			std::cout << "jump . or .." << std::endl;
			if (!FindNextFileA(file, &finddata)) {
				break;
			}
			continue;
		}

		
		if (finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {  //扫描目录
			//std::cout << "begin to scan dir :" << realPath << std::endl;
			std::string realPath = dir + "\\" + std::string(finddata.cFileName);
			ScanDir(realPath);
		}
		else {
			//std::cout << "begin to scan file :" << realPath << std::endl;
			ScanFile(dir, finddata.cFileName);
		}
		if (!FindNextFileA(file, &finddata)) {
			break;
		}
	}

	//扫描目录后
	AddTotalSize(info.fileSize);  //增加总大小
	info.fileAttr = "dir";
	info.path = dir;
	info.filename = std::string(finddata.cFileName);
	info.fileSize = (finddata.nFileSizeHigh * (MAXDWORD + 1)) + finddata.nFileSizeLow;
	info.totalSize = GetTotalSize();
	if (m_callbacks.count(CALLBACK_STAGE_ON_ONE_FINISH)) {
		m_callbacks[CALLBACK_STAGE_ON_ONE_FINISH](info);
	}
	return false;
}

bool CDirScanner::StartScan() {
	std::thread([this]() {
		WIN32_FIND_DATAA finddata;
		HANDLE file = FindFirstFileA(m_rootPath.c_str(), &finddata);

		if (file == INVALID_HANDLE_VALUE) {
			std::cout << "file invalid:" << m_rootPath << std::endl;
			return false;
		}

		if (finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {  //扫描目录
			std::cout << "begin to scan dir :" << m_rootPath << std::endl;
			ScanDir(m_rootPath);
		}
		else {
			std::cout << "begin to scan file :" << m_rootPath << std::endl;
			ScanFile(m_rootPath,"");
		}
		FindClose(file);
		return true;
	}).detach();
	return true;
}

bool CDirScanner::StopScan() {
	return true;
}

void CDirScanner::AddTotalSize(uint64_t size) {
	std::lock_guard<std::mutex> autolock(m_sizeMutex);
	m_totalSize += size;
}

uint64_t CDirScanner::GetTotalSize() {
	std::lock_guard<std::mutex> autolock(m_sizeMutex);
	return m_totalSize;
}