#include "mainwindow.h"
#include "ui_mainwindow.h"

extern HINSTANCE hInstLibrary;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setting = new SettingWindow();

    logged = false;

    init_loginUI();

    init_systray();

    load_dll();

    this->hInstLibrary = hInstLibrary;

    if( setting )
    connect(setting, SIGNAL(policy_changed(QString,QString,QString)),
            this, SLOT(policy_changed(QString,QString,QString)) );
}


MainWindow::~MainWindow()
{
    if( stop_service )
        stop_service();

    if( hInstLibrary )
        FreeLibrary(hInstLibrary);

    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *)
{

}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
         dragPosition = event->globalPos() - frameGeometry().topLeft();
         event->accept();
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        QPoint pos = event->globalPos() - dragPosition;
        if(frameGeometry().topLeft().y()+pos.y() < 0)
            pos.setY(0);
        move(pos);
        event->accept();
    }
}

void MainWindow::init_loginUI()
{
    this->setWindowFlags(Qt::FramelessWindowHint);

    ui->frame->setMinimumHeight(100);


    ui->pushButton_wdsec->setIcon(QIcon(":icon/security.ico"));
    ui->pushButton_wdsec->setText("WdSec");
    ui->pushButton_wdsec->setStyleSheet("QPushButton{color: lightgray;border:0px;}");

    ui->pushButton_mini->setFlat(true);
    ui->pushButton_mini->setIcon(QIcon(":icon/line.png"));

    ui->pushButton_close->setFlat(true);
    ui->pushButton_close->setIcon(QIcon(":icon/close.png"));

    ui->lineEdit_tel->setMinimumHeight(25);
    ui->lineEdit_code->setMinimumHeight(25);

    ui->pushButton_login->setMinimumHeight(30);
    ui->pushButton_login->setMinimumWidth(100);

    ui->pushButton_login->setStyleSheet("QPushButton{background: rgb(42, 120, 192);color: lightgray;}"
                                        "QPushButton:hover{background: rgb(42, 120, 255);color: white;}"
                                        "QPushButton:pressed{background: rgb(42, 120, 255);color: white;}");

    ui->pushButton_mini->setStyleSheet("QPushButton:hover{background: lightgray;color: white;}"
                                        "QPushButton:pressed{background: lightgray;color: white;}");

    ui->pushButton_close->setStyleSheet("QPushButton:hover{background: lightgray;color: white;}"
                                        "QPushButton:pressed{background: lightgray;color: white;}");

    ui->pushButton_get_code->setStyleSheet("QPushButton{background: rgb(58, 58, 58);color: lightgray;}"
                                           "QPushButton:hover{background: lightgray;color: black;}"
                                        "QPushButton:pressed{background: lightgray;color: black;}");

    ui->lineEdit_tel->setStyleSheet("QLineEdit{background: rgb(58, 58, 58);color: white;border:1px solid lightgray;}");

    ui->lineEdit_code->setStyleSheet("QLineEdit{background: rgb(58, 58, 58);color: white;border:1px solid lightgray;}");

    ui->label_tel->setStyleSheet("QLabel{color:lightgray}");
    ui->label_code->setStyleSheet("QLabel{color:lightgray}");

    QPixmap pixmap(":images/theme.jpg");
    QPalette palette;
    palette.setBrush(QPalette::Background, QBrush(pixmap));
    this->setAutoFillBackground(true);
    this->setPalette(palette);

    ui->frame->setMaximumWidth(180);

    connect(&networkAccessManager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(serviceRequestFinished(QNetworkReply*)));

    connect(this, SIGNAL(verify_code_success()),
            this, SLOT(login_success()) );

    connect(&timer_1, SIGNAL(timeout()),
            this, SLOT(timer_out_1()) );

    connect(&timer_60, SIGNAL(timeout()),
            this, SLOT(timer_out_60()) );
}

void MainWindow::init_systray()
{
    systray = new SystemTray;
    if( !systray )
        return;

    systray->m_systemTray = new QSystemTrayIcon(this);
    systray->m_trayMenu = new QMenu();

    //init top widget
    systray->m_topWidget = new QWidget();
    systray->m_topWidget->setMinimumHeight(50);
    systray->m_topWidgetAction = new QWidgetAction(systray->m_trayMenu);
    systray->m_topLabel = new QLabel(QStringLiteral(""));
    QPixmap pixmap(":images/backgroud.jpg");
    QPalette palette;
    palette.setBrush(QPalette::Background, QBrush(pixmap));
    systray->m_topWidget->setAutoFillBackground(true);
    systray->m_topWidget->setPalette(palette);

    QVBoxLayout* m_topLayout = new QVBoxLayout();
    m_topLayout->addWidget(systray->m_topLabel, 0, Qt::AlignLeft|Qt::AlignVCenter);

    m_topLayout->setSpacing(5);
    m_topLayout->setContentsMargins(5, 5, 5, 5);

    systray->m_topWidget->setLayout(m_topLayout);
    systray->m_topWidget->installEventFilter(this);
    systray->m_topWidgetAction->setDefaultWidget(systray->m_topWidget);
    systray->m_trayMenu->addAction(systray->m_topWidgetAction);


    //init action 
    systray->m_mdWidget = new QWidget();
    systray->m_mdWidgetAction = new QWidgetAction(systray->m_trayMenu);
    systray->m_showBtn = new QPushButton(QIcon(":menu/display.ico"), QStringLiteral("显示窗口"));
    systray->m_logoutBtn = new QPushButton(QIcon(":menu/logout.ico"), QStringLiteral("注销"));
    connect(systray->m_logoutBtn, SIGNAL(clicked(bool)),
            this, SLOT(logout()) );

    systray->m_setBtn = new QPushButton(QIcon(":menu/setting.ico"), QStringLiteral("配置"));
    connect(systray->m_setBtn, SIGNAL(clicked(bool)),
            this, SLOT(showWindow()) );

    systray->m_showBtn->setMinimumHeight(30);
    systray->m_logoutBtn->setMinimumHeight(30);
    systray->m_setBtn->setMinimumHeight(30);

    QVBoxLayout* m_mdLayout = new QVBoxLayout();
    //m_mdLayout->addWidget(systray->m_showBtn);
    m_mdLayout->addWidget(systray->m_logoutBtn);
    m_mdLayout->addWidget(systray->m_setBtn);

    m_mdLayout->setSpacing(5);
    m_mdLayout->setContentsMargins(5, 5, 5, 5);

    systray->m_mdWidget->setLayout(m_mdLayout);
    systray->m_mdWidget->installEventFilter(this);
    systray->m_mdWidgetAction->setDefaultWidget(systray->m_mdWidget);
    systray->m_trayMenu->addAction(systray->m_mdWidgetAction);
    systray->m_trayMenu->addSeparator();

    //init buttom widget
    systray->m_bottomWidget = new QWidget();
    systray->m_bottomWidgetAction = new QWidgetAction(systray->m_trayMenu);

    systray->m_aboutBtn = new QPushButton(QIcon(":menu/about.ico"), QStringLiteral("关于"));
    systray->m_aboutBtn->setObjectName(QStringLiteral("TrayButton"));
    systray->m_aboutBtn->setFixedSize(60, 25);
    connect(systray->m_aboutBtn, SIGNAL(clicked(bool)),
            this, SLOT(about()) );

    systray->m_exitBtn = new QPushButton(QIcon(":menu/out.ico"), QStringLiteral("退出"));
    systray->m_exitBtn->setObjectName(QStringLiteral("TrayButton"));
    systray->m_exitBtn->setFixedSize(60, 25);
    connect(systray->m_exitBtn, SIGNAL(clicked(bool)),
            this, SLOT(quit()) );

    QHBoxLayout* m_bottomLayout = new QHBoxLayout();
    m_bottomLayout->addWidget(systray->m_aboutBtn, 0, Qt::AlignCenter);
    m_bottomLayout->addWidget(systray->m_exitBtn, 0, Qt::AlignCenter);

    m_bottomLayout->setSpacing(5);
    m_bottomLayout->setContentsMargins(2,2,2,2);

    systray->m_bottomWidget->setLayout(m_bottomLayout);
    systray->m_bottomWidgetAction->setDefaultWidget(systray->m_bottomWidget);
    systray->m_trayMenu->addAction(systray->m_bottomWidgetAction);


    //init menu event
    systray->m_systemTray->setContextMenu(systray->m_trayMenu);
    systray->m_systemTray->setIcon(QIcon(":icon/security.ico"));
    systray->m_systemTray->setToolTip("wdsec");
    systray->m_systemTray->show();

    connect(systray->m_systemTray, &QSystemTrayIcon::activated,
            this, &MainWindow::activeTray);//点击托盘，执行相应的动作

    systray->m_trayMenu->setStyleSheet("QMenu{background:white;border:0px solid lightgray;}"
    "QMenu::separator{height:1px;background: lightgray;margin:2px 0px 2px 0px;\
    }");

    systray->m_aboutBtn->setStyleSheet("QPushButton{border:0px;}"
        "QPushButton:hover{background: lightgray;color: rgb(42, 120, 192);}"
        "QPushButton:pressed{background: lightgray;color: rgb(42, 120, 192);}");

    systray->m_exitBtn->setStyleSheet("QPushButton{border:0px;}"
        "QPushButton:hover{background: lightgray;color: rgb(42, 120, 192);}"
        "QPushButton:pressed{background: lightgray;color: rgb(42, 120, 192);}");

    systray->m_logoutBtn->setStyleSheet("QPushButton{border:0px;}"
        "QPushButton:hover{background: lightgray;color: rgb(42, 120, 192);}"
        "QPushButton:pressed{background: lightgray;color: rgb(42, 120, 192);}"
                                        "QPushButton{text-align : left;}");

    systray->m_setBtn->setStyleSheet("QPushButton{border:0px;}"
        "QPushButton:hover{background: lightgray;color: rgb(42, 120, 192);}"
        "QPushButton:pressed{background: lightgray;color: rgb(42, 120, 192);}"
                                     "QPushButton{text-align : left;}");
}

void MainWindow::login_success()
{
    if(start_service)
        start_service();
    this->hide();
    showMessage("通知","登录成功");
    logged = true;
}

void MainWindow::logout()
{
    if(stop_service)
        stop_service();
    this->show();
    ui->lineEdit_code->clear();
    logged = false;
}

void MainWindow::about()
{
    systray->m_trayMenu->hide();
    showMessage("关于","wdsec为您保驾护航");
}

void MainWindow::quit()
{
    if(stop_service)
        stop_service();
    this->close();
}

void MainWindow::policy_changed(QString type, QString path, QString filename)
{
    path = path.toLower();
    filename = filename.toLower();

    qDebug()<<type<<path<<filename;
    if(type == "1")
    {
        if(filename == "process")
        {
            if(add_process)
            add_process((char*)path.toStdString().c_str());
        }
        else
        {
            if(add_path)
            add_path((WCHAR*)path.toStdWString().c_str(),
                     (WCHAR*)filename.toStdWString().c_str());
        }
    }
    else
    {
        if(filename == "process")
        {
            if(del_process)
            del_process((char*)path.toStdString().c_str());
        }
        else
        {
            if(del_path)
            del_path((WCHAR*)path.toStdWString().c_str(),
                     (WCHAR*)filename.toStdWString().c_str());
        }
    }
}


void MainWindow::on_pushButton_get_code_clicked()
{
    QString tel = ui->lineEdit_tel->text();

    if( tel.isEmpty() )
        return;

    verify = false;
    //send_message(tel,"");

    ui->pushButton_get_code->setEnabled(false);
    ui->pushButton_get_code->setText("60秒");
    cur = 60;
    timer_1.start(1000);
    timer_60.start(60000);
}

void MainWindow::on_pushButton_login_clicked()
{
    QString code = ui->lineEdit_code->text();
    QString tel = ui->lineEdit_tel->text();

    if( tel.isEmpty() )
        return;

    verify = true;
    //send_message(tel, code);
    login_success();
}

void MainWindow::timer_out_1()
{
    cur--;
    if( cur > 0 )
        ui->pushButton_get_code->setText(QString::number(cur)+"秒");
}

void MainWindow::timer_out_60()
{
    timer_1.stop();
    timer_60.stop();
    ui->pushButton_get_code->setEnabled(true);
    ui->pushButton_get_code->setText("获取");
}

void MainWindow::activeTray(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
        {
        case QSystemTrayIcon::Context:
            showMenu();
            break;
        case QSystemTrayIcon::DoubleClick:
            showWindow();
            break;
        case QSystemTrayIcon::Trigger:
            showWindow();
            break;
    }
}

void MainWindow::showWindow()
{
    if( logged )
        if( setting )
        setting->show();
    else
        this->show();
    this->setFocus();
}

void MainWindow::showMessage(QString title,QString message)
{
    systray->m_systemTray->showMessage(title,//消息窗口标题
            message,//消息内容
            QSystemTrayIcon::Information,//消息窗口图标
            5000);//消息窗口显示时长
}

void MainWindow::showMenu()
{
    systray->m_trayMenu->show();
}

void MainWindow::on_pushButton_mini_clicked()
{
    this->showMinimized();
}

void MainWindow::on_pushButton_close_clicked()
{
    quit();
}
