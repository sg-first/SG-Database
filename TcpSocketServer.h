//#pragma once
#ifndef TEST_H
#define TEST_H
#include <QTcpServer>
#include <QObject>
#include <QDebug>
#include <QTcpSocket>
#include <QMap>
#include <queue>
#include <string>
#include <vector>
#include <iostream>
#include "dbProcess.h"
#include "Tcp_tools.h"



class TcpSocketServer:public QTcpServer//,public QObject
{
    Q_OBJECT
public:
    QMap<QString,QTcpSocket*> *tcp;//=new QMap<QString,QTcpSocket*>;//改成全局变量试一试
    //QMap<QString,ip_port*> *socket_ip_port;//=new QMap<QString,ip_port*>;//存客户端的ip和port
    TcpSocketServer();
    virtual void incomingConnection(qintptr handle);
    void show_map();
public slots:
    void response_handle(processObject);
};

class ServerResponseThread : public QThread//,public QObject
{
    Q_OBJECT
public:
    ServerResponseThread();

    ServerResponseThread(QMap<QString,QTcpSocket*>*);
    virtual void run() override;
    //QMap<QString,QTcpSocket*>* tcp2;
signals:
    void db_response_signal(processObject);
};

#endif
