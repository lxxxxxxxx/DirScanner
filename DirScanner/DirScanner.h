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
typedef std::map<CallbackStage, CallbackFunc> CallbackMap;

// 此类是从 dll 导出的
class DIRSCANNER_API CDirScanner {
public:
	//带扫描路径的构造函数
	CDirScanner(std::string root);
	//注册回调：包括开始扫描文件，扫描文件完成，扫描所有文件完成
	void RegistCallback(CallbackStage stage, CallbackFunc func);
	//解注册回调
	void UnRegistCallback(CallbackStage stage);
	//开始扫描
	bool StartScan();
	//停止扫描
	bool StopScan();


private:
	//停止扫描循环
	bool Break();
	//读取文件：核心处理函数
	void ReadFile(std::string path);
	//扫描文件
	bool ScanFile(std::string path, std::string filename);
	//扫描目录
	bool ScanDir(std::string dir);
	//累计文件大小
	void AddTotalSize(uint64_t size);
	//获取累计大小
	uint64_t GetTotalSize();

private:
	std::string m_rootPath;
	CallbackMap m_callbacks;
	HANDLE      m_exitEvent;
	uint64_t    m_totalSize;
	std::mutex  m_sizeMutex;
};