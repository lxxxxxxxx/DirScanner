// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 DIRSCANNER_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// DIRSCANNER_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#ifdef DIRSCANNER_EXPORTS
#define DIRSCANNER_API __declspec(dllexport)
#else
#define DIRSCANNER_API __declspec(dllimport)
#endif

#include <Windows.h>
#include <string>
#include <map>
#include <mutex>

typedef struct _ScanInfo {
	std::string fileAttr; // "file" or "dir"
	std::string path;
	std::string filename;
	uint64_t fileSize;
	uint64_t totalSize;
	_ScanInfo(): fileSize(0), totalSize(0){
	}
}ScanInfo;

enum CallbackStage {
	CALLBACK_STAGE_ON_ONE_BEGIN,   //开始扫描文件
	CALLBACK_STAGE_ON_ONE_FINISH,  //扫描文件结束
	CALLBACK_STAGE_ON_FINISH,      //所有文件扫描结束
};

typedef int(*CallbackFunc)(ScanInfo);

// 此类是从 dll 导出的
class DIRSCANNER_API CDirScanner {
public:
	CDirScanner();
	CDirScanner(std::string root);

	void RegistCallback(CallbackStage stage, CallbackFunc func);
	void UnRegistCallback(CallbackStage stage);

	void SetRootPath(std::string root);
	std::string GetRootPath();


	bool ScanFile(std::string path, std::string filename);
	bool ScanDir(std::string dir);

	bool StartScan();
	bool StopScan();

	void AddTotalSize(uint64_t size);
	uint64_t GetTotalSize();

	


private:
	std::string m_rootPath;
	std::map<CallbackStage, CallbackFunc> m_callbacks;
	HANDLE m_exitEvent;

	uint64_t m_totalSize;
	std::mutex m_sizeMutex;
};

extern DIRSCANNER_API int nDirScanner;

DIRSCANNER_API int fnDirScanner(void);
