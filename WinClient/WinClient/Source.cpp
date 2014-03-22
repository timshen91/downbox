#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include "protocol.h"
#include <iostream>
//static link lib
#pragma comment(lib,"ws2_32.lib")

using namespace std;
#define PORT 9999
#define IP_ADDRESS "216.37.108.31"

int monitor();

//c盘下自动创建testFileChange文件，在次文件夹下所有变动讲被记录并在terminal中打出
int main(void){

	monitor();
	return 1;
}

int monitor(){
	//inti TCP connection
	WSADATA	ws;
	SOCKET client;
	struct sockaddr_in ServerAddr;
	int Ret = 0;
	int AddrLen = 0;
	HANDLE hThread = NULL;

	if (WSAStartup(MAKEWORD(2, 2), &ws) != 0) {
		cout << "init socket failed" << GetLastError() << endl;
		return -1;
	}

	client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (client == INVALID_SOCKET) {
		cout << "client socket create failed" << GetLastError() << endl;
		return -1;
	}

	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_addr.s_addr = inet_addr(IP_ADDRESS);
	ServerAddr.sin_port = htons(PORT);
	memset(ServerAddr.sin_zero,0x00,8);

	Ret = connect(client, (struct sockaddr*)&ServerAddr, sizeof(ServerAddr));
	if (Ret == SOCKET_ERROR) {
		cout << "connect failed" << GetLastError() << endl;
		return -1;
	}
	else {
		cout << "success" << endl;
	}

	//init monitor
	TCHAR *dir = _T("c:\\testFileChange");
	if (!CreateDirectory(dir, NULL)){
		printf("File already exists.\n");
	}
	HANDLE dirHandle = CreateFile(dir, FILE_LIST_DIRECTORY,
		FILE_SHARE_WRITE | FILE_SHARE_READ | FILE_SHARE_DELETE,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_BACKUP_SEMANTICS,
		NULL);

	DWORD cbBytes;

	char notify[2024];
	FILE_NOTIFY_INFORMATION *pnotify = (FILE_NOTIFY_INFORMATION*)notify;
	char str2[1024];

	while (TRUE){
		if (ReadDirectoryChangesW(dirHandle, &notify, sizeof(notify), TRUE,
			FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_DIR_NAME
			| FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE,
			&cbBytes, NULL, NULL)){
			switch (pnotify->Action){
			case FILE_ACTION_ADDED:
				printf("file add %S.", pnotify->FileName);
				printf("\n");
				
				break;
			case FILE_ACTION_MODIFIED:
				WideCharToMultiByte(CP_ACP, 0, pnotify->FileName,
					pnotify->FileNameLength / 2, str2, 99, NULL, NULL);
				str2[pnotify->FileNameLength / 2] = '\0';
				printf("modified----%d---%S", strlen(str2), str2);
				printf("\n");
				break;
			case FILE_ACTION_REMOVED:
				printf("file removed.--%S--", pnotify->FileName);
				printf("\n");
				break;
			case FILE_ACTION_RENAMED_OLD_NAME:
				WideCharToMultiByte(CP_ACP, 0, pnotify->FileName,
					NULL, NULL, NULL, NULL, NULL);
				printf("rename %S", pnotify->FileName);
				printf("\n");
				break;
			case FILE_ACTION_RENAMED_NEW_NAME:
				//FIXME 新文件名并不会显示
				WideCharToMultiByte(CP_ACP, 0, pnotify->FileName,
					NULL, NULL, NULL, NULL, NULL);
				printf(" to %S ", pnotify->FileName);
				printf("\n");
				break;
			default:
				printf("unkonw action\n");
			}
		}
	}
	return 0;
}