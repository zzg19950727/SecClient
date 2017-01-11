#include "mainwindow.h"
#include <QCryptographicHash>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QDateTime>
#include <QSplitter>
#include <time.h>

QString Random()
{
    QString str = "aaaaaaaaaaaaaaaaaaaaa";
    srand( time(NULL) );
    for(int i=0; i<str.length(); i++)
    {
        int r = rand()%26;
        str[i] = ('a'+r);
    }
    return str;
}

void MainWindow::send_message(const QString tel, QString code)
{
    QString time = QString::number(QDateTime::currentMSecsSinceEpoch()/1000);
    QString nonce = Random();
    QString AppSecret = "3dfa3efcb1ca";
    QString str = AppSecret+nonce+time;
    QString data = "mobile="+tel;
    QString url = "https://api.netease.im/sms/sendcode.action";

    if(!code.isEmpty())
    {
        data += "&code="+code;
        url = "https://api.netease.im/sms/verifycode.action";
    }

    QCryptographicHash *hash=new QCryptographicHash(QCryptographicHash::Sha1);
    hash->addData(str.toUtf8());
    QByteArray sha1=hash->result();
    QString tmp = sha1.toHex().toStdString().c_str();

    httpRequest.setRawHeader("AppKey","1a6f9e7e4d0dc6c6bd93795705c1a862");
    httpRequest.setRawHeader("CurTime",time.toUtf8());
    httpRequest.setRawHeader("CheckSum",tmp.toUtf8());
    httpRequest.setRawHeader("Nonce",nonce.toUtf8());
    httpRequest.setRawHeader("Content-Type","application/x-www-form-urlencoded;charset=utf-8");

    httpRequest.setUrl(QUrl(url));
    networkAccessManager.post(httpRequest, data.toUtf8());
}

void MainWindow::serviceRequestFinished(QNetworkReply *reply)
{
    if( !verify )
        return;

    QByteArray data = reply->readAll();

    if(reply->error() == QNetworkReply::NoError)
    {
        QJsonParseError jsonError;
        QJsonObject json = QJsonDocument::fromJson(data, &jsonError).object();
        if(jsonError.error == QJsonParseError::NoError)
        {
           int code = json.value("code").toInt();
           if(code == 200)
               emit verify_code_success();
        }
    }
    reply->deleteLater();
}
