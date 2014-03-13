#include <stdio.h>
#include <windows.h>
#include <tchar.h>

void monitor();

//c盘下自动创建testFileChange文件，在次文件夹下所有变动讲被记录并在terminal中打出
int main(void){
	monitor();
	return 1;
}

void monitor(){
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
}
