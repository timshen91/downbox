#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <cassert>
#include "../../socket.h"
#include "../../protocol.h"
#include "../../tool.h"

using namespace std;
#define PORT 9999
#define IP_ADDRESS "216.37.108.31"

int monitor();

//Create file in C://
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
	TCPSocket conn;
	if (!conn.init(IP_ADDRESS, PORT)) {
		cout << "fail init socket" << endl;
	}
	else {
		cout << "tcp success" << endl;
	}

	//init monitor
	TCHAR *dir = _T("c:\\Airpocket");
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
	char Namestr[1024];

	ReqCreateFile req;
	assert(SetCurrentDirectory(dir));
	while (TRUE){
		if (ReadDirectoryChangesW(dirHandle, &notify, sizeof(notify), TRUE,
			FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_DIR_NAME
			| FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE,
			&cbBytes, NULL, NULL)) {
			switch (pnotify->Action) {
			case FILE_ACTION_ADDED: {
										//show file_change info in console
										wchar_to_char(pnotify->FileName, Namestr, pnotify->FileNameLength);
										cout << "added   " << Namestr << endl;
										//send file
										conn << (uint8_t)CREATE_FILE;
										req.get<0>().assign(Namestr);
										ifstream fin(Namestr);
										if (fin.fail()) {
											cout << "init file stream fail";
										}
										fin.seekg(0, ifstream::end);
										req.get<1>().resize(fin.tellg());
										fin.seekg(0,ifstream::beg);
										if (fin.tellg() > 0) {
											fin.read(req.get<1>().data(), req.get<1>().size());
											fin.close();
											conn << req;
										}
										else {
											conn << req;
										}
										break;
			}
			case FILE_ACTION_MODIFIED:
				wchar_to_char(pnotify->FileName, Namestr, pnotify->FileNameLength);
				cout << "modified  " << Namestr;
			case FILE_ACTION_REMOVED:
				wchar_to_char(pnotify->FileName, Namestr, pnotify->FileNameLength);
				cout << "removed  " << Namestr << endl;
				break;
			case FILE_ACTION_RENAMED_OLD_NAME:
				wchar_to_char(pnotify->FileName, Namestr, pnotify->FileNameLength);
				cout << "renamed  " << Namestr << endl;
				break;
			case FILE_ACTION_RENAMED_NEW_NAME:
				//FIXME new file name will not show
				wchar_to_char(pnotify->FileName, Namestr, pnotify->FileNameLength);
				cout << "to name  " << Namestr << endl;
				break;
			default:
				printf("unkonw action\n");
			}
		}
	}
	return 0;
}