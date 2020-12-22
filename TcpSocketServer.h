#pragma once
//
//服务器端-多线程处理客户端连接

#include <QTcpServer>
#include <QThread>
#include <QDebug>
#include <QTcpSocket>
class TcpSocketServer : public QTcpServer
{
public:
    TcpSocketServer();
    //This virtual function is called by QTcpServer when a new connection is available.
    //The socketDescriptor argument is the native socket descriptor for the accepted connection.
    virtual void incomingConnection(qintptr handle);

private:
    QStringList m_oData;
};

//处理线程
class ServerHandleThread : public QThread
{
    Q_OBJECT

public:

    ServerHandleThread(qintptr handle, const QString& sWriteData);
    QString read(QTcpSocket);//信息分类处理
    //bool Login();//如果read收到了Login标志，调用此函数
    virtual void run();
private:
    qintptr m_nHandle;
    QString m_sWriteData;
};

void testTcpSocketServer();
