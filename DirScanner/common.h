#pragma once

#include <string>

typedef struct _ScanInfo {
	std::string fileAttr; // "file" or "dir"
	std::string path;
	std::string filename;
	uint64_t fileSize;
	uint64_t totalSize;
	_ScanInfo() : fileSize(0), totalSize(0) {
	}
}ScanInfo;

enum CallbackStage {
	CALLBACK_STAGE_ON_ONE_BEGIN,   //��ʼɨ���ļ�
	CALLBACK_STAGE_ON_ONE_FINISH,  //ɨ���ļ�����
	CALLBACK_STAGE_ON_FINISH,      //�����ļ�ɨ�����
};