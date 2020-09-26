
//�漰��ͷ�ļ���windows.h, stdio.h,
//�漰�ĺ�����Ҫ��FindFirstFile��FindNextFile����

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

//�����ļ�
HANDLE WINAPI FindFirstFile(
	__in               LPCTSTR lpFileName,//�ļ�����·��
	__out             LPWIN32_FIND_DATA lpFindFileData
);

//������һ���ļ�
BOOL WINAPI FindNextFile(
	__in               HANDLE hFindFile,
	__out             LPWIN32_FIND_DATA lpFindFileData
);

typedef struct _WIN32_FIND_DATA {
	DWORD dwFileAttributes;   //�ļ�����
	FILETIME ftCreationTime;   //����ʱ��
	FILETIME ftLastAccessTime;   //�������ʱ��
	FILETIME ftLastWriteTime; //����޸�ʱ��
	DWORD nFileSizeHigh; //��λ��С
	DWORD nFileSizeLow;   //��λ��С
	DWORD dwReserved0;   //����
	DWORD dwReserved1;   //����
	TCHAR cFileName[MAX_PATH];//�ļ�������ȫ·�� 
	TCHAR cAlternateFileName[14];
}WIN32_FIND_DATA, *PWIN32_FIND_DATA, *LPWIN32_FIND_DATA;

void FindFiles(CString &strPath)

{
	WIN32_FIND_DATA findData;

	CString strTemp = strPath;
	strTemp.Format(_T("%s\\*.*"), strPath);//����ָ��Ŀ¼�µ�ֱ�ӵ������ļ���Ŀ¼

	HANDLE hFile = FindFirstFile(strTemp, &findData);
	while (hFile != INVALID_HANDLE_VALUE)
	{
		if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)//�����Ŀ¼
		{
			if (findData.cFileName[0] != _T('.'))//�ų�.��..�ļ���
			{

				strTemp = strPath + CString("\\") + findData.cFileName;//�������·��
				FindFiles(strTemp);//�ݹ���ҵ�ǰĿ¼���ļ�
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