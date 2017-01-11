#include "settingwindow.h"
#include "ui_setting.h"
#include "encrypt.h"
#include <QTableWidgetItem>
#include <QtWinExtras/QtWin>

#define PATH    "C:/list.txt"

SettingWindow::SettingWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SettingWindow)
{
    ui->setupUi(this);

    rowcount=0;

    search_all = false;

    init_UI();

    readFileList();

    connect(ui->pushButton_lock_file, SIGNAL(clicked(bool)),
            this, SLOT(on_actionLockFile_triggered()) );

    connect(ui->pushButton_unlock_file, SIGNAL(clicked(bool)),
            this, SLOT(on_actionunlockFile_triggered()) );

    connect(ui->pushButton_lock_dir, SIGNAL(clicked(bool)),
            this, SLOT(on_actionLockDir_triggered()) );

    connect(ui->pushButton_unlock_dir, SIGNAL(clicked(bool)),
            this, SLOT(on_actionUnlockDir_triggered()) );

    connect(ui->pushButton_lock_ext, SIGNAL(clicked(bool)),
            this, SLOT(on_actionLockExt_triggered()) );

    connect(ui->pushButton_unlock_ext, SIGNAL(clicked(bool)),
            this, SLOT(on_actionUnlockExt_triggered()) );

    connect(ui->checkBox_select_all, SIGNAL(stateChanged(int)),
            this, SLOT(select_all()) );
}

SettingWindow::~SettingWindow()
{
    saveFileList();
    delete ui;
}

void SettingWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
         dragPosition = event->globalPos() - frameGeometry().topLeft();
         event->accept();
    }
}

void SettingWindow::mouseMoveEvent(QMouseEvent *event)
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


void SettingWindow::init_UI()
{
    this->setWindowFlags(Qt::FramelessWindowHint);
    QPixmap pixmap(":images/bkg.jpg");
    QPalette palette;
    palette.setBrush(QPalette::Background, QBrush(pixmap));
    this->setAutoFillBackground(true);
    this->setPalette(palette);

    this->setWindowTitle("策略配置");

    ui->pushButton_lock_dir->setMinimumHeight(50);
    ui->pushButton_lock_ext->setMinimumHeight(50);
    ui->pushButton_lock_file->setMinimumHeight(50);
    ui->pushButton_unlock_dir->setMinimumHeight(50);
    ui->pushButton_unlock_ext->setMinimumHeight(50);
    ui->pushButton_unlock_file->setMinimumHeight(50);
    ui->pushButton_add_proc->setMinimumHeight(50);
    ui->pushButton_del_proc->setMinimumHeight(50);

    ui->comboBox_filter->setMinimumWidth(100);
    ui->comboBox_filetype->setMinimumWidth(150);

    ui->comboBox_filetype->addItem(QIcon(":/icon/file_add.ico"),"未加密文件");
    ui->comboBox_filetype->addItem(QIcon(":/icon/file_del.ico"),"已加密文件");
    ui->comboBox_filetype->addItem(QIcon(":/icon/folder_del.ico"),"已保护文件夹");
    ui->comboBox_filetype->addItem(QIcon(":/icon/dir_lock.ico"),"已锁定后缀");
    ui->comboBox_filetype->addItem(QIcon(":/icon/process_delete.ico"),"未授权的软件");
    ui->comboBox_filetype->addItem(QIcon(":/icon/process_accept.ico"),"已授权的软件");
    connect(ui->comboBox_filetype, SIGNAL(currentIndexChanged(QString)),
            this, SLOT(settable()) );

    ui->comboBox_filter->addItem("*.*");
    ui->comboBox_filter->addItem(".txt");
    ui->comboBox_filter->addItem(".doc");
    ui->comboBox_filter->addItem(".xls");
    ui->comboBox_filter->addItem(".ppt");
    ui->comboBox_filter->addItem(".png");
    ui->comboBox_filter->addItem(".jpg");
    connect(ui->comboBox_filter, SIGNAL(currentTextChanged(QString)),
            this, SLOT(settable()) );

    ui->pushButton_setting->setIcon(QIcon(":icon/security.ico"));
    ui->pushButton_setting->setFlat(true);
    ui->pushButton_mini->setFlat(true);
    ui->pushButton_close->setFlat(true);

    ui->pushButton_close->setIcon(QIcon(":icon/close.png"));
    ui->pushButton_mini->setIcon(QIcon(":icon/line.png"));

    ui->GoTopushButton->setMinimumHeight(30);
    ui->DirSelectpushButton->setMinimumHeight(30);

    ui->pushButton_lock_file->setIconSize(QSize(25,25));
    ui->pushButton_unlock_file->setIconSize(QSize(25,25));
    ui->pushButton_lock_dir->setIconSize(QSize(25,25));
    ui->pushButton_unlock_dir->setIconSize(QSize(25,25));
    ui->pushButton_lock_ext->setIconSize(QSize(25,25));
    ui->pushButton_unlock_ext->setIconSize(QSize(25,25));
    ui->pushButton_add_proc->setIconSize(QSize(25,25));
    ui->pushButton_del_proc->setIconSize(QSize(25,25));

    ui->pushButton_setting->setStyleSheet("QPushButton{color: lightgray;border:0px;}");

    ui->pushButton_mini->setStyleSheet("QPushButton:hover{background: lightgray;color: white;}"
                                        "QPushButton:pressed{background: lightgray;color: white;}");

    ui->pushButton_close->setStyleSheet("QPushButton:hover{background: lightgray;color: white;}"
                                        "QPushButton:pressed{background: lightgray;color: white;}");

    ui->pushButton_lock_file->setStyleSheet("QPushButton{background:rgb(240,240,240);color:gray;border:0px;}"
                                        "QPushButton:hover{background: rgb(150,150,150);color: white;}"
                                        "QPushButton:pressed{background: rgb(150,150,150);color: white;}"
                                            "QPushButton{text-align : center;}");

    ui->pushButton_unlock_file->setStyleSheet("QPushButton{background:rgb(240,240,240);color:gray;border:0px;}"
                                              "QPushButton:hover{background: rgb(150,150,150);color: white;}"
                                              "QPushButton:pressed{background: rgb(150,150,150);color: white;}"
                                              "QPushButton{text-align : center;}");

    ui->pushButton_lock_dir->setStyleSheet("QPushButton{background:rgb(240,240,240);color:gray;border:0px;}"
                                        "QPushButton:hover{background: rgb(150,150,150);color: white;}"
                                        "QPushButton:pressed{background: rgb(150,150,150);color: white;}"
                                           "QPushButton{text-align : center;}");

    ui->pushButton_unlock_dir->setStyleSheet("QPushButton{background:rgb(240,240,240);color:gray;border:0px;}"
                                              "QPushButton:hover{background: rgb(150,150,150);color: white;}"
                                              "QPushButton:pressed{background: rgb(150,150,150);color: white;}"
                                             "QPushButton{text-align : center;}");

    ui->pushButton_lock_ext->setStyleSheet("QPushButton{background:rgb(240,240,240);color:gray;border:0px;}"
                                           "QPushButton:hover{background: rgb(150,150,150);color: white;}"
                                           "QPushButton:pressed{background: rgb(150,150,150);color: white;}"
                                              "QPushButton{text-align : center;}");

    ui->pushButton_unlock_ext->setStyleSheet("QPushButton{background:rgb(240,240,240);color:gray;border:0px;}"
                                             "QPushButton:hover{background: rgb(150,150,150);color: white;}"
                                             "QPushButton:pressed{background: rgb(150,150,150);color: white;}"
                                                "QPushButton{text-align : center;}");

    ui->pushButton_add_proc->setStyleSheet("QPushButton{background:rgb(240,240,240);color:gray;border:0px;}"
                                           "QPushButton:hover{background: rgb(150,150,150);color: white;}"
                                           "QPushButton:pressed{background: rgb(150,150,150);color: white;}"
                                              "QPushButton{text-align : center;}");

    ui->pushButton_del_proc->setStyleSheet("QPushButton{background:rgb(240,240,240);color:gray;border:0px;}"
                                             "QPushButton:hover{background: rgb(150,150,150);color: white;}"
                                             "QPushButton:pressed{background: rgb(150,150,150);color: white;}"
                                                "QPushButton{text-align : center;}");

    ui->label->setStyleSheet("QLabel{color:white}");

    ui->GoTopushButton->setStyleSheet("QPushButton{border:0px;}");

    ui->DirSelectpushButton->setStyleSheet("QPushButton{color: white;border:0px;}"
                                           "QPushButton:hover{background: lightgray;color: white;}"
                                           "QPushButton:pressed{background: lightgray;color: lightblue;}"
                                              "QPushButton{text-align : left;}");

    ui->pathlineEdit->setStyleSheet("QLineEdit{color:black;border:1px solid lightgray;}");

    ui->comboBox_filetype->setStyleSheet("QComboBox{color:black;border:0px;}");

    ui->comboBox_filter->setStyleSheet("QComboBox{color:black;border:1px;}");

    ui->checkBox_search_all->setStyleSheet("QCheckBox{color:white;border:0px;}");

    ui->checkBox_select_all->setStyleSheet("QCheckBox{color:white;border:0px;}");

    ui->tableView->setStyleSheet("QTableView{background:lightgray;color:black;border:0px}");
}

void SettingWindow::readFileList()
{
    QFile file(PATH);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug()<<"open list file failed";
    }
    else
    {
        QTextStream in(&file);
        QString sss;
        int flag=0;
        while(!in.atEnd())
        {
           sss=in.readLine();
           if(sss=="<PROCESS>")
           {
               flag=3;
               continue;
           }
           else if(sss=="<EXT>")
           {
               flag=2;
               continue;
           }
           else if(sss=="<FILE>")
           {
               flag=1;
               continue;
           }
           else if(sss == "<DIR>")
           {
               flag=0;
               continue;
           }

           sss.replace("\\","/");


           if(flag == 3)
               ProList<<sss;
           else if(flag == 2)
               ExtList<<sss;
           else if(flag == 1)
               FileList<<sss;
           else
               DirList<<sss;
        }
        file.close();
    }

}

void SettingWindow::saveFileList()
{
    QFile file(PATH);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug()<<"open list file failed";
    }
    else
    {
        QTextStream out(&file);
        out<<"<DIR>"<<endl;
        for(int i=0;i<DirList.size();i++)
        {
            if(DirList[i].endsWith('/'))
                DirList[i].remove(DirList[i].length()-1,1);
            DirList[i].replace("/","\\");
            out<<DirList[i]<<endl;
        }
        out<<"<FILE>"<<endl;
        for(int i=0;i<FileList.size();i++)
        {
            FileList[i].replace("/","\\");
            out<<FileList[i]<<endl;
            FileList[i].replace("\\","/");
        }
        out<<"<EXT>"<<endl;
        for(int i=0;i<ExtList.size();i++)
        {
            ExtList[i].replace("/","\\");
            out<<ExtList[i]<<endl;
            ExtList[i].replace("\\","/");
        }
        out<<"<PROCESS>";
        for(int i=0;i<ProList.size();i++)
        {
            out<<endl<<ProList[i];
        }
        file.close();
    }

}

void SettingWindow::filtFolder()
{
    if(ui->comboBox_filetype->currentIndex() == 2)
    {

    }

    QString path = ui->pathlineEdit->text();
    QDir dir( path );
    if(!dir.exists() || path.isEmpty() )
    {
        return;
    }
    QFileInfoList list = dir.entryInfoList();

    int file_count = list.count();
    if(file_count <= 0)
    {
        return;
    }

    QString filters;

    if( ui->comboBox_filter->currentText() != "*.*" )
        filters = ui->comboBox_filter->currentText();

    QDirIterator *iter;
    if(search_all)
       iter =  new QDirIterator(dir,QDirIterator::Subdirectories);
    else
       iter =  new QDirIterator(dir,QDirIterator::NoIteratorFlags);

    unlockinfolist.clear();
    lockinfolist.clear();
    waitlist.clear();

    while (iter->hasNext())
    {
        iter->next();
        QFileInfo info=iter->fileInfo();
        if (info.isFile() && info.fileName().contains(filters))
        {
            int state = FileState(info);
            int index = ui->comboBox_filetype->currentIndex();
            if(state==index)
                unlockinfolist<<info;
        }
    }
    delete iter;
}

void SettingWindow::write_log(QString type, QString path, QString filename)
{
    if(path.endsWith('/'))
        path.remove(path.length()-1,1);
    path.replace("/","\\");

    emit policy_changed(type, path, filename);
}

int SettingWindow::FileState(QFileInfo strPath)
{
    /*0--明文  1--密文  2--待加密*/
    if(QDir::match(FileList,strPath.absoluteFilePath()))
        return 1;

    for(int i=0;i<DirList.size();i++)
    {
        if(strPath.path().indexOf(DirList[i])!=-1)
            return 1;
    }
    return 0;
}

void SettingWindow::settable()
{
    ui->tableView->clearSpans();
    int index = ui->comboBox_filetype->currentIndex();
    switch(index)
    {
    case 0:
        show_file();
        ui->pushButton_lock_file->setEnabled(true);
        ui->pushButton_unlock_file->setEnabled(false);
        ui->pushButton_add_proc->setEnabled(false);
        ui->pushButton_del_proc->setEnabled(false);
        break;
    case 1:
        show_file();
        ui->pushButton_lock_file->setEnabled(false);
        ui->pushButton_unlock_file->setEnabled(true);
        ui->pushButton_add_proc->setEnabled(false);
        ui->pushButton_del_proc->setEnabled(false);
        break;
    case 2:
        show_dir();
        ui->pushButton_lock_file->setEnabled(false);
        ui->pushButton_unlock_file->setEnabled(false);
        ui->pushButton_add_proc->setEnabled(false);
        ui->pushButton_del_proc->setEnabled(false);
        break;
    case 3:
        show_ext();
        ui->pushButton_lock_file->setEnabled(false);
        ui->pushButton_unlock_file->setEnabled(false);
        ui->pushButton_add_proc->setEnabled(false);
        ui->pushButton_del_proc->setEnabled(false);
        break;
    case 4:
        show_process_unauth();
        ui->pushButton_lock_file->setEnabled(false);
        ui->pushButton_unlock_file->setEnabled(false);
        ui->pushButton_add_proc->setEnabled(true);
        ui->pushButton_del_proc->setEnabled(false);
        break;
    case 5:
        show_process_auth();
        ui->pushButton_lock_file->setEnabled(false);
        ui->pushButton_unlock_file->setEnabled(false);
        ui->pushButton_add_proc->setEnabled(false);
        ui->pushButton_del_proc->setEnabled(true);
        break;
    }
}

void SettingWindow::GetCheck(QStringList& list)
{
    list.clear();
    for(int i=0;i<rowcount;i++)
    {
      if(ui->tableView->model()->index(i,4).data(Qt::CheckStateRole) == Qt::Checked )
      {
          list<<unlockinfolist[i].absoluteFilePath();
      }
    }
}

void SettingWindow::on_GoTopushButton_clicked()
{
    QString path = ui->pathlineEdit->text();
    QDir dir( path );
    if(!dir.exists() || path.isEmpty() )
    {
        return;
    }
    settable();
}

void SettingWindow::on_DirSelectpushButton_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this);
    if(!dir.isEmpty())
    {
        ui->pathlineEdit->setText(dir);
        settable();
    }
}

void SettingWindow::show_file()
{
    filtFolder();

    int count=0;

    //准备数据模型
    QStandardItemModel *model = new QStandardItemModel();
    model->setHorizontalHeaderItem(0, new QStandardItem(QObject::tr("文件名")));
    model->setHorizontalHeaderItem(1, new QStandardItem(QObject::tr("最后访问时间")));
    model->setHorizontalHeaderItem(2, new QStandardItem(QObject::tr("创建时间")));
    model->setHorizontalHeaderItem(3, new QStandardItem(QObject::tr("大小")));
    model->setHorizontalHeaderItem(4, new QStandardItem(QObject::tr("选择")));

    //利用setModel()方法将数据模型与QTableView绑定
    ui->tableView->resizeColumnsToContents();

    ui->tableView->setModel(model);

    //设置选中时为整行选中
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);

    //设置表格的单元为只读属性，即不能编辑
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    for(int i=0;i<unlockinfolist.size();i++)
    {
        model->setItem(count,0,new QStandardItem(unlockinfolist[i].fileName()));
        model->setItem(count,1,new QStandardItem(unlockinfolist[i].lastModified().toString("yyyy-MM-dd")));
        model->setItem(count,2,new QStandardItem(unlockinfolist[i].created().toString("yyyy-MM-dd")));
        model->setItem(count,3,new QStandardItem(QString::number(unlockinfolist[i].size())));
        QStandardItem* item0 = new QStandardItem();

        item0->setCheckable(true);
        model->setItem(count,4,item0);
        count++;
    }
    rowcount=count;
    ui->tableView->setColumnWidth(0, 200);
}

void SettingWindow::show_dir()
{
    int count=0;

    //准备数据模型
    QStandardItemModel *model = new QStandardItemModel();
    model->setHorizontalHeaderItem(0, new QStandardItem(QObject::tr("文件夹")));
    model->setHorizontalHeaderItem(1, new QStandardItem(QObject::tr("选择")));

    //利用setModel()方法将数据模型与QTableView绑定
    ui->tableView->resizeColumnsToContents();

    ui->tableView->setModel(model);

    //设置选中时为整行选中
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);

    //设置表格的单元为只读属性，即不能编辑
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    for(int i=0;i<DirList.size();i++)
    {
        model->setItem(count,0,new QStandardItem(DirList[i]));
        QStandardItem* item0 = new QStandardItem();

        item0->setCheckable(true);
        model->setItem(count,1,item0);
        count++;
    }
    rowcount=count;
    ui->tableView->setColumnWidth(0, 200);
}

void SettingWindow::show_ext()
{
    int count=0;

    //准备数据模型
    QStandardItemModel *model = new QStandardItemModel();
    model->setHorizontalHeaderItem(0, new QStandardItem(QObject::tr("文件后缀")));
    model->setHorizontalHeaderItem(1, new QStandardItem(QObject::tr("选择")));

    //利用setModel()方法将数据模型与QTableView绑定
    ui->tableView->resizeColumnsToContents();

    ui->tableView->setModel(model);

    //设置选中时为整行选中
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);

    //设置表格的单元为只读属性，即不能编辑
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    for(int i=0;i<ExtList.size();i++)
    {
        model->setItem(count,0,new QStandardItem(ExtList[i]));
        QStandardItem* item0 = new QStandardItem();

        item0->setCheckable(true);
        model->setItem(count,1,item0);
        count++;
    }
    rowcount=count;
    ui->tableView->setColumnWidth(0, 100);
}

extern QStringList save, processlist;
extern QList<HICON> iconlist;

void SettingWindow::show_process_unauth()
{
    get_all_process(ProList);

    int count=0;

    //准备数据模型
    QStandardItemModel *model = new QStandardItemModel();
    model->setHorizontalHeaderItem(0, new QStandardItem(QObject::tr("软件")));
    model->setHorizontalHeaderItem(1, new QStandardItem(QObject::tr("进程名")));
    model->setHorizontalHeaderItem(2, new QStandardItem(QObject::tr("选择")));

    //利用setModel()方法将数据模型与QTableView绑定
    ui->tableView->resizeColumnsToContents();

    ui->tableView->setModel(model);

    //设置选中时为整行选中
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);

    //设置表格的单元为只读属性，即不能编辑
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    for(int i=0;i<save.size();i++)
    {
        QIcon icon(QtWin::fromHICON(iconlist[i]));
        QStandardItem* item = new QStandardItem();
        item->setIcon( icon );
        item->setText(save[i]);
        model->setItem(count,0,item);

        QStandardItem* item_1 = new QStandardItem();
        item_1->setText(processlist[i]);
        model->setItem(count,1,item_1);

        QStandardItem* item0 = new QStandardItem();
        item0->setCheckable(true);
        model->setItem(count,2,item0);
        count++;
    }
    rowcount=count;

    ui->tableView->setColumnWidth(0,400);
    ui->tableView->setColumnWidth(1,100);
}

void SettingWindow::show_process_auth()
{
    int count=0;

    //准备数据模型
    QStandardItemModel *model = new QStandardItemModel();
    model->setHorizontalHeaderItem(0, new QStandardItem(QObject::tr("进程")));
    model->setHorizontalHeaderItem(1, new QStandardItem(QObject::tr("选择")));

    //利用setModel()方法将数据模型与QTableView绑定
    ui->tableView->resizeColumnsToContents();

    ui->tableView->setModel(model);

    //设置选中时为整行选中
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);

    //设置表格的单元为只读属性，即不能编辑
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    for(int i=0;i<ProList.size();i++)
    {
        QStandardItem* item = new QStandardItem();
        item->setText(ProList[i]);
        model->setItem(count,0,item);

        QStandardItem* item0 = new QStandardItem();
        item0->setCheckable(true);
        model->setItem(count,1,item0);
        count++;
    }
    rowcount=count;

    ui->tableView->setColumnWidth(0,200);
}

void SettingWindow::on_actionUnlockDir_triggered()
{
    if(ui->comboBox_filetype->currentIndex() != 2)
        return;
    QStringList List;
    for(int i=0;i<rowcount;i++)
    {
      if(ui->tableView->model()->index(i,1).data(Qt::CheckStateRole) == Qt::Checked )
      {
          List<<ui->tableView->model()->index(i,0).data().toString();
      }
    }

    for(int i=0; i<List.size(); i++)
    {
           DirList.removeAll(List[i]);
           write_log("0",List[i],"dir");
    }

    DirList.removeDuplicates();

    //QMessageBox::information(this, "通知", "解锁成功！");

    settable();
    saveFileList();
}

void SettingWindow::on_actionLockDir_triggered()
{
    QString path = ui->pathlineEdit->text();
    QDir dir( path );
    if(!dir.exists() || path.isEmpty() )
    {
        return;
    }
    DirList<<ui->pathlineEdit->text();
    write_log("1",ui->pathlineEdit->text(),"dir");
    DirList.removeDuplicates();

    settable();
    saveFileList();
}

void SettingWindow::on_actionunlockFile_triggered()
{
    if(ui->comboBox_filetype->currentIndex() != 1)
        return;
    QStringList List;
    for(int i=0;i<rowcount;i++)
    {
      if(ui->tableView->model()->index(i,4).data(Qt::CheckStateRole) == Qt::Checked )
      {
          write_log("0",unlockinfolist[i].absolutePath(),unlockinfolist[i].fileName());
          if( !decrypt_file(unlockinfolist[i].absoluteFilePath().toStdString().c_str()) )
          {
              QMessageBox::information(this, "通知", "解密失败！");
              write_log("1",unlockinfolist[i].absolutePath(),unlockinfolist[i].fileName());
              continue;
          }
          List<<unlockinfolist[i].absoluteFilePath();
      }
    }


    for(int i=0; i<List.size(); i++)
    {
           FileList.removeAll(List[i]);
    }

    FileList.removeDuplicates();

    //QMessageBox::information(this, "通知", "解锁成功！");

    settable();
    saveFileList();
}

void SettingWindow::on_actionLockFile_triggered()
{
    if(ui->comboBox_filetype->currentIndex() != 0)
        return;
    QStringList List;
    for(int i=0;i<rowcount;i++)
    {
      if(ui->tableView->model()->index(i,4).data(Qt::CheckStateRole) == Qt::Checked )
      {
          List<<unlockinfolist[i].absoluteFilePath();
          if( !encrypt_file(unlockinfolist[i].absoluteFilePath().toStdString().c_str()) )
          {
              List.removeAll(unlockinfolist[i].absoluteFilePath());
              continue;
          }
          write_log("1",unlockinfolist[i].absolutePath(),unlockinfolist[i].fileName());
      }
    }


    for(int i=0; i<List.size(); i++)
    {
           FileList<<List[i];
    }

    FileList.removeDuplicates();

    //QMessageBox::information(this, "通知", "锁定成功！");

    settable();
    saveFileList();
}

void SettingWindow::on_actionLockExt_triggered()
{
    QString ext = ui->comboBox_filter->currentText();
    if( ext == "*.*" || ext.isEmpty() )
        return;
    if(ext[0] != '.' || ext.length()<2)
    {
        QMessageBox::information(this, "通知", "未知的文件类型！");
        return;
    }

    ExtList<<ext;
    write_log("1",ext,"ext");

    //QMessageBox::information(this, "通知", "锁定成功！");

    settable();
    saveFileList();
}

void SettingWindow::on_actionUnlockExt_triggered()
{
    if(ui->comboBox_filetype->currentIndex() != 3)
        return;
    QStringList List;

    for(int i=0;i<rowcount;i++)
    {
      if(ui->tableView->model()->index(i,1).data(Qt::CheckStateRole) == Qt::Checked )
      {
          List<<ui->tableView->model()->index(i,0).data().toString();

      }
    }

    for(int i=0; i<List.size(); i++)
    {
           ExtList.removeAll(List[i]);
           write_log("0",List[i],"ext");
    }

    ExtList.removeDuplicates();

    //QMessageBox::information(this, "通知", "解锁成功！");

    settable();
    saveFileList();
}

void SettingWindow::select_all()
{
    Qt::CheckState state;
    if( ui->checkBox_select_all->checkState()==Qt::Checked )
        state = Qt::Unchecked;
    else
        state = Qt::Checked;

    if(ui->comboBox_filetype->currentIndex()<2)
    {
        for(int i=0;i<rowcount;i++)
        {
            ;
        }
    }
    else
    {
        for(int i=0;i<rowcount;i++)
        {
            ;
        }
    }
}


void SettingWindow::on_checkBox_search_all_stateChanged(int)
{
    if(ui->checkBox_search_all->checkState() == Qt::Checked)
        search_all = true;
    else
        search_all = false;
}

void SettingWindow::on_pushButton_mini_clicked()
{
    this->showMinimized();
}

void SettingWindow::on_pushButton_close_clicked()
{
    this->hide();
}

void SettingWindow::on_pushButton_add_proc_clicked()
{
    if(ui->comboBox_filetype->currentIndex() != 4)
        return;
    QStringList List;

    for(int i=0;i<rowcount;i++)
    {
      if(ui->tableView->model()->index(i,2).data(Qt::CheckStateRole) == Qt::Checked )
      {
          List<<ui->tableView->model()->index(i,1).data().toString();

      }
    }

    for(int i=0; i<List.size(); i++)
    {
           ProList.push_back(List[i]);
           write_log("1",List[i],"process");
    }

    ProList.removeDuplicates();

    settable();
    saveFileList();
}

void SettingWindow::on_pushButton_del_proc_clicked()
{
    if(ui->comboBox_filetype->currentIndex() != 5)
        return;
    QStringList List;

    for(int i=0;i<rowcount;i++)
    {
      if(ui->tableView->model()->index(i,1).data(Qt::CheckStateRole) == Qt::Checked )
      {
          List<<ui->tableView->model()->index(i,0).data().toString();

      }
    }

    for(int i=0; i<List.size(); i++)
    {
           ProList.removeAll(List[i]);
           write_log("0",List[i],"process");
    }

    ProList.removeDuplicates();

    settable();
    saveFileList();
}
