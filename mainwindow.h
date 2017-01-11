#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <Qsystemtrayicon>
#include <QWidgetAction>
#include <QCloseEvent>
#include <QMouseEvent>
#include <QPushButton>
#include <QAction>
#include <QPoint>
#include <QLabel>
#include <Qmenu>
#include <QTimer>

#include "settingwindow.h"
#include "load_dll.h"

namespace Ui {
class MainWindow;
}

typedef struct __SystemTray
{
    QSystemTrayIcon *m_systemTray;

    QMenu* m_trayMenu;

    //菜单上部分
    QWidget* m_topWidget;
    QWidgetAction* m_topWidgetAction;
    QLabel* m_topLabel;

    //菜单中间部分
    QWidget* m_mdWidget;
    QWidgetAction* m_mdWidgetAction;
    QPushButton* m_showBtn;
    QPushButton* m_logoutBtn;
    QPushButton* m_setBtn;

    //菜单下面部分
    QWidget* m_bottomWidget;
    QWidgetAction* m_bottomWidgetAction;
    QPushButton* m_aboutBtn;
    QPushButton* m_exitBtn;

}SystemTray, *PSystemTray;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

    ~MainWindow();

    void closeEvent(QCloseEvent *);

    void mousePressEvent(QMouseEvent *event);

    void mouseMoveEvent(QMouseEvent *event);

    void send_message(const QString tel, QString code);

private:

    void init_loginUI();

    void init_systray();

signals:
    void verify_code_success();

private slots:
    void serviceRequestFinished(QNetworkReply *reply);

    void login_success();

    void logout();

    void about();

    void quit();

    void policy_changed(QString type, QString path, QString filename);

    void on_pushButton_get_code_clicked();

    void on_pushButton_login_clicked();

    void timer_out_1();

    void timer_out_60();

    void on_pushButton_mini_clicked();

    void on_pushButton_close_clicked();

    void activeTray(QSystemTrayIcon::ActivationReason reason);

    void showWindow();//显示窗体

    void showMessage(QString title,QString message);//消息框

    void showMenu();//显示菜单

private:
    Ui::MainWindow *ui;

    SettingWindow* setting;

    QPoint dragPosition;

    QNetworkRequest httpRequest;

    QNetworkAccessManager networkAccessManager;

    QTimer timer_60, timer_1;

    bool verify, logged;

    int cur;

    HINSTANCE hInstLibrary;

    PSystemTray systray;
};

#endif // MAINWINDOW_H
