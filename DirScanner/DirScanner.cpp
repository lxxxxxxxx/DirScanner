// DirScanner.cpp : 定义 DLL 的导出函数。
//
#include <iostream>
#include <thread>
#include <fstream>

#include "framework.h"
#include "DirScanner.h"

CDirScanner::CDirScanner() :m_totalSize(0) {
	m_exitEvent = CreateEventA(NULL, TRUE, FALSE, NULL);
}

CDirScanner::CDirScanner(std::string root) :m_rootPath(root) , m_totalSize(0) {
	m_exitEvent = CreateEventA(NULL, TRUE, FALSE, NULL);
}

void CDirScanner::SetRootPath(std::string path) {
	m_rootPath = path;
}

std::string CDirScanner::GetRootPath() {
	return m_rootPath;
}

//void CDirScanner::RegistCallback(CallbackStage stage, CallbackFunc func) {
//	m_callbacks[stage] = func;
//}
//
//void CDirScanner::UnRegistCallback(CallbackStage stage) {
//	if (m_callbacks.count(stage)) {
//		m_callbacks.erase(stage);
//	}
//}

void CDirScanner::Attach(SubscriberBase *subscriber) {
	m_subscriber.push_back(subscriber);
}

void CDirScanner::UnAttach(SubscriberBase *subscriber) {
	int i = 0;
	for (; i < m_subscriber.size(); i++) {
		if (m_subscriber[i] == subscriber) {
			break;
		}
	}
	m_subscriber.erase(m_subscriber.begin() + i);

}

bool CDirScanner::StartScan() {
	ResetEvent(m_exitEvent);
	//启动主扫描线程
	std::thread([this]() {
		WIN32_FIND_DATAA finddata;
		HANDLE file = FindFirstFileA(m_rootPath.c_str(), &finddata);

		if (file == INVALID_HANDLE_VALUE) {
			std::cout << "file invalid:" << m_rootPath << std::endl;
			return false;
		}
		if (finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {  //扫描目录
			ScanDir(m_rootPath);
		}
		else {//扫描文件
			ScanFile(m_rootPath, "");
		}
		FindClose(file);

		Sleep(500);
		ScanInfo info;
		info.totalSize = GetTotalSize();
		Notify(CALLBACK_STAGE_ON_FINISH, info);


		//if (m_callbacks.count(CALLBACK_STAGE_ON_FINISH)) {
		//	ScanInfo info;
		//	info.totalSize = GetTotalSize();
		//	m_callbacks[CALLBACK_STAGE_ON_FINISH](info);
		//}
		return true;
	}).detach();
	return true;
}

bool CDirScanner::StopScan() {
	SetEvent(m_exitEvent);
	return true;
}

bool CDirScanner::Break() {
	return WaitForSingleObject(m_exitEvent, 0) == WAIT_OBJECT_0;
}

void CDirScanner::ReadFile(std::string path) {
	std::fstream file;
	file.open(path, std::ios::beg | std::ios::binary | std::ios::in);

	if (file.is_open()) {
		//获取文件长度
		file.seekg(0, std::ios::end);
		int len = file.tellg();
		file.seekg(0, std::ios::beg);

		char buf[1024] = { 0 };
		while (!file.eof() && !Break()) {
			file.read(buf, 1024);
			//do something with buf...
			//std::cout << buf;
		}
		if (Break()) {

		}
	}
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

	Notify(CALLBACK_STAGE_ON_ONE_BEGIN, info);

	//if (m_callbacks.count(CALLBACK_STAGE_ON_ONE_BEGIN)) {
	//	m_callbacks[CALLBACK_STAGE_ON_ONE_BEGIN](info);
	//}

	//do scan
	ReadFile(path + "\\" + finddata.cFileName);

	//扫描文件后调用回调
	AddTotalSize(info.fileSize);  //增加总大小
	info.fileAttr = "file";
	info.path = path;
	info.filename = std::string(finddata.cFileName);
	info.fileSize = (finddata.nFileSizeHigh * (MAXDWORD + 1)) + finddata.nFileSizeLow;
	info.totalSize = GetTotalSize();

	Notify(CALLBACK_STAGE_ON_ONE_FINISH, info);

	//if (m_callbacks.count(CALLBACK_STAGE_ON_ONE_FINISH)) {
	//	m_callbacks[CALLBACK_STAGE_ON_ONE_FINISH](info);
	//}

	FindClose(file);

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

	Notify(CALLBACK_STAGE_ON_ONE_BEGIN, info);

	//if (m_callbacks.count(CALLBACK_STAGE_ON_ONE_BEGIN)) {
	//	m_callbacks[CALLBACK_STAGE_ON_ONE_BEGIN](info);
	//}

	while (file != INVALID_HANDLE_VALUE && !Break()) {
		if (!strcmp(finddata.cFileName,".") || !strcmp(finddata.cFileName,"..")) {
			if (!FindNextFileA(file, &finddata)) {
				break;
			}
			continue;
		}
		if (finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {  //扫描目录
			//启动扫描目录线程
			std::string realPath = dir + "\\" + std::string(finddata.cFileName);
			std::thread([this, realPath]() {ScanDir(realPath); }).detach();
		}
		else {
			//启动扫描文件线程
			std::string name = finddata.cFileName;
			std::thread([this, dir,name](){ScanFile(dir, name); }).detach();
		}
		if (!FindNextFileA(file, &finddata)) {
			break;
		}
	}
	FindClose(file);

	if (Break()) {
		std::cout << "broken." << std::endl;
	}

	//扫描目录后
	AddTotalSize(info.fileSize);  //增加总大小
	info.fileAttr = "dir";
	info.path = dir;
	info.filename = std::string(finddata.cFileName);
	info.fileSize = (finddata.nFileSizeHigh * (MAXDWORD + 1)) + finddata.nFileSizeLow;
	info.totalSize = GetTotalSize();

	Notify(CALLBACK_STAGE_ON_ONE_FINISH, info);

	//if (m_callbacks.count(CALLBACK_STAGE_ON_ONE_FINISH)) {
	//	m_callbacks[CALLBACK_STAGE_ON_ONE_FINISH](info);
	//}
	return false;
}

void CDirScanner::AddTotalSize(uint64_t size) {
	std::lock_guard<std::mutex> autolock(m_sizeMutex);
	m_totalSize += size;
}

uint64_t CDirScanner::GetTotalSize() {
	std::lock_guard<std::mutex> autolock(m_sizeMutex);
	return m_totalSize;
}

//通知订阅制
void CDirScanner::Notify(CallbackStage stage, ScanInfo info) {
	switch (stage) {
	case CALLBACK_STAGE_ON_ONE_BEGIN: {
		for (auto it : m_subscriber) {
			it->OnScanOneStart(info);
		}
		break;
	}
	case CALLBACK_STAGE_ON_ONE_FINISH: {
		for (auto it : m_subscriber) {
			it->OnScanOneFinish(info);
		}
		break;
	}
	case CALLBACK_STAGE_ON_FINISH: {
		for (auto it : m_subscriber) {
			it->OnScanAllFinish(info);
		}
		break;
	}
	default:
		break;
	}
}
