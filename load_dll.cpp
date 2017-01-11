#include "load_dll.h"
#include <Windows.h>
#include <QDebug>

#define START_FUNC      "start_service"
#define STOP_FUNC       "stop_service"
#define ADD_PATH_FUNC   "MyAddProtectPath"
#define DEL_PATH_FUNC   "MyDelProtectPath"
#define ADD_PRO_FUNC    "MyAddProtectProcess"
#define DEL_PRO_FUNC    "MyDelProtectProcess"
#define GET_PRO_NAME    "GetProcessName"

SERVICE start_service, stop_service;

COMMAND add_path, del_path;

PROCESS add_process, del_process;

NAME GetProcessName;

HINSTANCE hInstLibrary;

void load_dll()
{
    start_service = NULL;
    stop_service = NULL;
    add_path = NULL;
    del_path = NULL;
    add_process = NULL;
    del_process = NULL;
    GetProcessName = NULL;
    LPCSTR start_func = START_FUNC;

    hInstLibrary = LoadLibrary(L"MessageDriver.dll");
    if (hInstLibrary == NULL)
    {
         qDebug()<<"载入dll失败";
         goto ERR;
    }


    start_service = (SERVICE)GetProcAddress(hInstLibrary,start_func);
    if(start_service == NULL)
    {
        DWORD err = GetLastError();
        qDebug()<<"转换start service失败"<<err;
        goto ERR;
    }

    stop_service = (SERVICE)GetProcAddress(hInstLibrary,STOP_FUNC);
    if(stop_service == NULL)
    {
        DWORD err = GetLastError();
        qDebug()<<"转换stop service失败"<<err;
        goto ERR;
    }

    add_path = (COMMAND)GetProcAddress(hInstLibrary,ADD_PATH_FUNC);
    if(add_path == NULL)
    {
        DWORD err = GetLastError();
        qDebug()<<"转换add path失败"<<err;
        goto ERR;
    }

    del_path = (COMMAND)GetProcAddress(hInstLibrary,DEL_PATH_FUNC);
    if(del_path == NULL)
    {
        DWORD err = GetLastError();
        qDebug()<<"转换del path失败"<<err;
        goto ERR;
    }

    add_process = (PROCESS)GetProcAddress(hInstLibrary,ADD_PRO_FUNC);
    if(add_process == NULL)
    {
        DWORD err = GetLastError();
        qDebug()<<"转换add process失败"<<err;
        goto ERR;
    }

    del_process = (PROCESS)GetProcAddress(hInstLibrary,DEL_PRO_FUNC);
    if(del_process == NULL)
    {
        DWORD err = GetLastError();
        qDebug()<<"转换del process失败"<<err;
        goto ERR;
    }

    GetProcessName = (NAME)GetProcAddress(hInstLibrary,GET_PRO_NAME);
    if(GetProcessName == NULL)
    {
        DWORD err = GetLastError();
        qDebug()<<"转换get process name失败"<<err;
        goto ERR;
    }
    return;
ERR:
    if(hInstLibrary)
        FreeLibrary(hInstLibrary);
    return;
}
