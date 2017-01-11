#ifndef LOAD_DLL_H
#define LOAD_DLL_H
#include <Windows.h>

typedef void (*SERVICE)(void);

typedef void (*COMMAND)(WCHAR *path, WCHAR *fileType);

typedef void (*PROCESS) (CHAR* process);

typedef void (*NAME) (HWND nlHandle, WCHAR name[1024]);

extern SERVICE start_service, stop_service;

extern COMMAND add_path, del_path;

extern PROCESS add_process, del_process;

void load_dll();

#endif // LOAD_DLL_H
