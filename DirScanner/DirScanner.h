#ifdef DIRSCANNER_EXPORTS
#define DIRSCANNER_API __declspec(dllexport)
#else
#define DIRSCANNER_API __declspec(dllimport)
#endif

#include <Windows.h>
//#include <map>
#include <vector>
#include <mutex>
#include "NotifyInterface.h"

typedef int(*CallbackFunc)(ScanInfo);
//typedef std::map<CallbackStage, CallbackFunc> CallbackMap;

// 此类是从 dll 导出的
class DIRSCANNER_API CDirScanner {
public:
	CDirScanner();
	//带扫描路径的构造函数
	CDirScanner(std::string root);
	//设置扫描路径
	void SetRootPath(std::string path);
	//获取扫描路径
	std::string GetRootPath();

	////注册回调：包括开始扫描文件，扫描文件完成，扫描所有文件完成
	//void RegistCallback(CallbackStage stage, CallbackFunc func);
	////解注册回调
	//void UnRegistCallback(CallbackStage stage);

	//注册订阅者
	void Attach(SubscriberBase *subscriber);
	//解注册订阅者
	void UnAttach(SubscriberBase *subscriber);
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

	//通知订阅制
	void Notify(CallbackStage stage, ScanInfo info);

private:
	std::string m_rootPath;
	//CallbackMap m_callbacks;
	std::vector<SubscriberBase*> m_subscriber;
	HANDLE      m_exitEvent;
	uint64_t    m_totalSize;
	std::mutex  m_sizeMutex;
};