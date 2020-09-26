
//涉及的头文件有windows.h, stdio.h,
//涉及的函数主要是FindFirstFile、FindNextFile函数

#include <Windows.h>
#include <stdio.h>

void list_current_dir()
{
	WIN32_FIND_DATAA FileData;
	HANDLE hSearch = NULL;
	BOOL finished = FALSE;

	printf("%d\n", sizeof(FILETIME));
	printf("%d\n", sizeof(WIN32_FIND_DATAA));

	hSearch = FindFirstFileA("*", &FileData);   // find for all files
	if (hSearch == INVALID_HANDLE_VALUE)
		return;    // No file found

	while (!finished)
	{
		//puts(FileData.cFileName);      // print file name
		printf("%s\n", FileData.cFileName);
		finished = !FindNextFileA(hSearch, &FileData);    // find next file
	}
	FindClose(hSearch);   // finish searching
}

//查找文件
HANDLE WINAPI FindFirstFile(
	__in               LPCTSTR lpFileName,//文件名或路径
	__out             LPWIN32_FIND_DATA lpFindFileData
);

//查找下一个文件
BOOL WINAPI FindNextFile(
	__in               HANDLE hFindFile,
	__out             LPWIN32_FIND_DATA lpFindFileData
);

typedef struct _WIN32_FIND_DATA {
	DWORD dwFileAttributes;   //文件属性
	FILETIME ftCreationTime;   //创建时间
	FILETIME ftLastAccessTime;   //最近访问时间
	FILETIME ftLastWriteTime; //最近修改时间
	DWORD nFileSizeHigh; //高位大小
	DWORD nFileSizeLow;   //低位大小
	DWORD dwReserved0;   //保留
	DWORD dwReserved1;   //保留
	TCHAR cFileName[MAX_PATH];//文件名，非全路径 
	TCHAR cAlternateFileName[14];
}WIN32_FIND_DATA, *PWIN32_FIND_DATA, *LPWIN32_FIND_DATA;

void FindFiles(CString &strPath)

{
	WIN32_FIND_DATA findData;

	CString strTemp = strPath;
	strTemp.Format(_T("%s\\*.*"), strPath);//查找指定目录下的直接的所有文件和目录

	HANDLE hFile = FindFirstFile(strTemp, &findData);
	while (hFile != INVALID_HANDLE_VALUE)
	{
		if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)//如果是目录
		{
			if (findData.cFileName[0] != _T('.'))//排除.和..文件夹
			{

				strTemp = strPath + CString("\\") + findData.cFileName;//获得完整路径
				FindFiles(strTemp);//递归查找当前目录的文件
			}
		}
		else
		{
			strTemp = strPath + CString("\\") + findData.cFileName;
		}

		if (!FindNextFile(hFile, &findData))
		{
			break;
		}
	}

}