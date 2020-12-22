#include <iostream>
#include <QDebug>
#include <QTcpSocket>
#include "TcpSocketServer.h"
using namespace std;
TcpSocketServer::TcpSocketServer()
{
    //初始换原始数据
    m_oData << tr("You've been leading a dog's life. Stay off the furniture.")
        << tr("You've got to think about tomorrow.")
        << tr("You will be surprised by a loud noise.")
        << tr("You will feel hungry again in another hour.")
        << tr("You might have mail.")
        << tr("You cannot kill time without injuring eternity.")
        << tr("Computers are not intelligent. They only think they are.");
}

void TcpSocketServer::incomingConnection(qintptr handle)
{
    //获取服务端数据
    QString sWriteData = m_oData.at(qrand() % m_oData.size());
    qDebug() << "TcpSocketServer::incomingConnection" << handle;
    ServerHandleThread* pThread = new ServerHandleThread(handle, sWriteData);
    connect(pThread, &ServerHandleThread::finished, pThread, &ServerHandleThread::deleteLater);
    pThread->start();
}

ServerHandleThread::ServerHandleThread(qintptr handle, const QString& sWriteData)
:m_sWriteData(sWriteData), m_nHandle(handle)
{

}

void ServerHandleThread::run()
{
    //1. 建立与客户端通信的TCP套接字
    QTcpSocket oTcpSocket;
    oTcpSocket.waitForReadyRead();

    if (!oTcpSocket.setSocketDescriptor(m_nHandle))
    {
        qDebug() << "oTcpSocket.setSocketDescriptor error";
        return;
    }

    //2. 向客户端写数据
    /*qDebug() << "TcpSocketServer::readDataFromClient" << &oTcpSocket;
    qDebug() << "TcpSocketServer::writeDataToClient" << m_sWriteData;
    oTcpSocket.write(m_sWriteData.toUtf8());
    oTcpSocket.disconnectFromHost();
    oTcpSocket.waitForDisconnected();*/

    //2.从客户端读数据
    for(int i=0;i<3;i++)//之后改成死循环
    {
        oTcpSocket.waitForReadyRead();

        QString sReadData = oTcpSocket.readAll();//读取到的string
        qDebug()<<sReadData;//试试能不能读入数据



        //3.将string发送给数据库


        //4.从数据库获取结果
        QString sWrite="Answer";

        //5.将结果发送给客户端
        oTcpSocket.write(sWrite.toUtf8());
        oTcpSocket.flush();
    }
}
QString ServerHandleThread::read(QTcpSocket oTcpSocket)
{
    oTcpSocket.waitForReadyRead();
    QString sReadData = oTcpSocket.readAll();
    qDebug()<<sReadData;//试试能不能读入数据
}
void testTcpSocketServer()
{
    cout<<"start the databaseServer"<<endl;
    //1. 建立服务器端套接字
    TcpSocketServer* m_pTcpServer = new TcpSocketServer();
    //2. 启动服务端
    if (!m_pTcpServer->listen(QHostAddress::Any, 8888))
    {
        qDebug() << "m_pTcpServer->listen() error";
    }
}
