#include "settingwindow.h"
#include "mainwindow.h"
#include <QStringList>
#include <QString>
#include <QList>
#include <QDebug>

QStringList global, save, processlist;
QList<HICON> iconlist;
extern NAME GetProcessName;

HICON  GetWindowsIcon( HWND  hwnd)
{
     HICON  hIcon = NULL;
    hIcon = ( HICON )::GetClassLong(hwnd,GCL_HICONSM);
     if (hIcon == NULL)
    {
        hIcon = ( HICON )::GetClassLong(hwnd,GCL_HICON);
    }
     if (hIcon == NULL)
    {
        hIcon = ( HICON )::SendMessage(hwnd, WM_GETICON, ICON_SMALL, 0);
    }
     if (hIcon == NULL)
    {
        hIcon = ( HICON )::SendMessage(hwnd, WM_GETICON, ICON_BIG, 0);
    }
     return  hIcon;
}

BOOL CALLBACK lpEnumFunc(HWND hwnd, LPARAM lParam)
{
    if (hwnd == NULL)
    {
        return FALSE;
    }

    if (::IsWindow(hwnd)
            &&(GetWindowLong(hwnd, GWL_HWNDPARENT)==0)
            &&::IsWindowVisible(hwnd)
            &&((GetWindowLong(hwnd, GWL_EXSTYLE)&WS_EX_TOOLWINDOW)!=WS_EX_TOOLWINDOW)
             )
    {
        WCHAR szCap[255] = {0};

        ::GetWindowText(hwnd, szCap, sizeof(szCap));

        QString process = QString::fromWCharArray(szCap);

        if(process.isEmpty() || process=="Program Manager")
            return TRUE;

        QString proname;
        if(GetProcessName)
        {
            WCHAR name[1024];
            GetProcessName(hwnd, name);
            proname = QString::fromWCharArray(name);
            QStringList list = proname.split('\\');
            list.isEmpty()?proname:proname=list.last();
        }

        for(int i=0; i<global.size(); i++)
        {
            if(proname == global[i])
                return TRUE;
        }

        save.push_back(process);
        iconlist.push_back(GetWindowsIcon(hwnd));
        processlist.push_back(proname);
    }
    return TRUE;
}

void SettingWindow::get_all_process(QStringList& list)
{
    global = list;

    save.clear();
    iconlist.clear();
    processlist.clear();

    HWND desktopPtr = GetDesktopWindow();

    EnumChildWindows(desktopPtr, lpEnumFunc, NULL);
}
