#ifndef SettingWindow_H
#define SettingWindow_H

#include <QMainWindow>
#include <QMessageBox>
#include <QFileDialog>
#include <QFileSystemModel>
#include <QStandardItem>
#include <QStringList>
#include <QDateTime>
#include <QMouseEvent>
#include <QTextStream>
#include <QCloseEvent>
#include <QFile>
#include <QDir>
#include <QDebug>

#include <windows.h>

namespace Ui {
class SettingWindow;
}

class SettingWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SettingWindow(QWidget *parent = 0);

    int FileState(QFileInfo strPath);

    void GetCheck(QStringList &list);

    ~SettingWindow();

    void mousePressEvent(QMouseEvent *event);

    void mouseMoveEvent(QMouseEvent *event);

    void init_UI();

    void readFileList();

    void saveFileList();

    void filtFolder();

    void write_log(QString type, QString path, QString filename);  

    void get_all_process(QStringList& list);

signals:
    void policy_changed(QString type, QString path, QString filename);

private slots:
    void on_GoTopushButton_clicked();

    void on_DirSelectpushButton_clicked();

    void show_file();

    void show_dir();

    void show_ext();

    void show_process_unauth();

    void show_process_auth();

    void settable();

    void on_actionUnlockDir_triggered();

    void on_actionLockDir_triggered();

    void on_actionunlockFile_triggered();

    void on_actionLockFile_triggered();

    void on_actionLockExt_triggered();

    void on_actionUnlockExt_triggered();

    void select_all();

    void on_checkBox_search_all_stateChanged(int);

    void on_pushButton_mini_clicked();

    void on_pushButton_close_clicked();

    void on_pushButton_add_proc_clicked();

    void on_pushButton_del_proc_clicked();

private:
    Ui::SettingWindow *ui;

    QPoint dragPosition;

    int rowcount;

    QStringList FileList,DirList,ExtList,ProList;

    QFileInfoList unlockinfolist,lockinfolist,waitlist;

    bool search_all;
};

#endif // SettingWindow_H
