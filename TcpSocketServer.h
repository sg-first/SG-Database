#pragma once

#include <QTcpServer>
#include <QThread>
#include <QDebug>
#include <QTcpSocket>
#include <QMap>
#include <queue>
#include "data.h"
using namespace std;
QString get_userid(QString S)
{
     //userid定位8位数字
    return S.mid(0,8);
}
QString get_content(QString S)
{
    return S.mid(8);
}


class TcpSocketServer:public QTcpServer
{
public:
    QMap<QString,QTcpSocket*> *tcp=new QMap<QString,QTcpSocket*>;
    TcpSocketServer();
    virtual void incomingConnection(qintptr handle);
    void show_map();
};

class ServerResponseThread : public QThread
{
public:
    ServerResponseThread(QMap<QString,QTcpSocket*>*);
    virtual void run() override;
    QMap<QString,QTcpSocket*>* tcp;

};

TcpSocketServer::TcpSocketServer()
{
    //开启响应监听线程
    ServerResponseThread *SRT=new ServerResponseThread(tcp);
    SRT->start();
}
void TcpSocketServer::show_map()
{

    qDebug()<<"输出map";
    QMap<QString, QTcpSocket*>::const_iterator i;
    for( i=tcp->constBegin(); i!=tcp->constEnd(); ++i)
        qDebug() << i.key() <<"        " << i.value();

}
//每当有连接请求调用此函数
void TcpSocketServer::incomingConnection(qintptr handle)
{
    //获取服务端数据
    QTcpSocket *oTcpSocket=new QTcpSocket();
    oTcpSocket->setSocketDescriptor(handle);
    qDebug() << handle << " " << oTcpSocket->socketDescriptor();

    //connect(oTcpSocket, &oTcpSocket::finished, pThread, &ServerHandleThread::deleteLater);
    oTcpSocket->waitForReadyRead();

    QString sReadData = oTcpSocket->readAll();//读取到的string
    qDebug()<<"接收检测："<<sReadData;//试试能不能读入数据
    QString userid=get_userid(sReadData);
    QString content=get_content(sReadData);
    Request_Queue.push(sReadData);//收到的String加入到请求队列

    //添加该socket到map
    tcp->insert(userid,oTcpSocket);

    //输出map
    show_map();

}

void testTcpSocketServer()
{
    //1. 建立服务器端套接字
    TcpSocketServer* m_pTcpServer = new TcpSocketServer();
    //2. 启动服务端
    if (!m_pTcpServer->listen(QHostAddress::Any, 8888))
    {
        qDebug() << "m_pTcpServer->listen() error";
    }
}


ServerResponseThread::ServerResponseThread(QMap<QString,QTcpSocket*>* tcp)
{
    this->tcp=tcp;//获得map
}
void ServerResponseThread::run()
{
    while(1)
    {
        //qDebug()<<"ServerResponseThread在执行";
        QString response;
        QString userid;
        QString content;
        //如果响应队列不为空
        if(Response_Queue.size()!=0)
        {
            response=Response_Queue.front();
            Response_Queue.pop();
            userid=get_userid(response);
            content=get_content(response);
            //在map中查找对应的socket
            if(tcp->contains(userid))
            {
                QMap<QString ,QTcpSocket *>::iterator it;
                it=tcp->find(userid);
                QTcpSocket *socket=it.value();//获得了对应的socket
                qDebug()<<"查看回复内容："<<content;
                socket->write(content.toUtf8());//写回结果
                socket->flush();
            }
            else
            {
                qDebug()<<"tcp map中不含这个键值对";
            }

        }
    }
}
