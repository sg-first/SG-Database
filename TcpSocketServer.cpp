#include "TcpSocketServer.h"

//槽函数，处理分发响应内容
void TcpSocketServer::response_handle(processObject p)
{
    cout<<"response_handle called"<<endl;
    processObject response=processObject();
    QString userid;
    QString content;
    response=p;
    dbProcess::correspondQueue.pop();
    userid=QString::fromStdString(response.getUser());
    content=QString::fromStdString(response.getResult());
    //在map中查找对应的socket
    if(tcp->contains(userid))
    {
        QMap<QString ,QTcpSocket *>::iterator it;
        it=tcp->find(userid);
        QTcpSocket *socket=it.value();//获得了对应的socket
        qDebug()<<"查看回复内容："<<content;
        qDebug()<<"查看这个socket"<<socket;
        socket->write(content.toUtf8());//写回结果
        socket->flush();
    }
    else
    {
        qDebug()<<"tcp map中不含这个键值对";
    }

}
//线程函数
//void db_listener_thread_function()
//{

//    qDebug()<<"db_listener_thread_function线程在执行";
//    while(1)
//    {
//        processObject response=processObject();
//        QString userid;
//        QString content;
//        //如果响应队列不为空
//        if(dbProcess::correspondQueue.size()!=0)
//        {
//            response=dbProcess::correspondQueue.front();
//            dbProcess::correspondQueue.pop();
//            userid=QString::fromStdString(response.getUser());
//            content=QString::fromStdString(response.getResult());
//            //在map中查找对应的socket
////            if(tcp->contains(userid))
////            {
////                QMap<QString ,QTcpSocket *>::iterator it;
////                it=tcp->find(userid);
////                QTcpSocket *socket=it.value();//获得了对应的socket
////                qDebug()<<"查看回复内容："<<content;
////                qDebug()<<"查看这个socket"<<socket;
////                socket->write(content.toUtf8());//写回结果
////                socket->flush();
////            }
//            if(socket_ip_port->contains(userid))
//            {
//                qDebug()<<"find the match one in map"<<endl;
//                QMap<QString,ip_port*>::iterator it;
//                it=socket_ip_port->find(userid);
//                QString::fromStdString(it.value()->ip);

//                //QTcpSocket *socket=new QTcpSocket();
//                QTcpSocket *socket = new QTcpSocket();

//                //3. 与服务器端建立连接

//                socket->connectToHost(QString::fromStdString(it.value()->ip), it.value()->port);
//                socket->waitForConnected(10000);//等候连接建立

//                qDebug()<<"查看回复内容："<<content;
//                qDebug()<<"查看这个socket"<<socket;
//                socket->write(content.toUtf8());//写回结果
//                socket->flush();
//            }
//            else
//            {
//                qDebug()<<"tcp map中不含这个键值对";
//            }

//        }
//    }
//}

TcpSocketServer::TcpSocketServer()
{
    tcp=new QMap<QString,QTcpSocket*>;
    //socket_ip_port=new QMap<QString,ip_port*>;
    //开启响应监听线程
    //ServerResponseThread *SRT=new ServerResponseThread(tcp);
    //SRT->start();

//    //开启C++11线程函数
//    std::thread t1(db_listener_thread_function);
//    t1.join();
}
//void show_socket_ip_port_map()
//{
//    qDebug()<<"show_socket_ip_port_map:";
//    QMap<QString,ip_port*>::const_iterator i;
//    for( i=socket_ip_port->constBegin(); i!=socket_ip_port->constEnd(); ++i)
//        qDebug() << i.key() <<"        " << QString::fromStdString(i.value()->ip)<<"        "<<i.value()->port;
//}
//void TcpSocketServer::show_map()
//{
//    qDebug()<<"输出map";
//    QMap<QString, QTcpSocket*>::const_iterator i;
//    for( i=tcp->constBegin(); i!=tcp->constEnd(); ++i)
//        qDebug() << i.key() <<"        " << i.value();
//}

//每当有连接请求调用此函数
void TcpSocketServer::incomingConnection(qintptr handle)
{
    //获取服务端数据
    QTcpSocket *oTcpSocket=new QTcpSocket();
    oTcpSocket->setSocketDescriptor(handle);
    qDebug() << handle << " " << oTcpSocket->socketDescriptor();

    oTcpSocket->waitForReadyRead();

    QString sReadData = oTcpSocket->readAll();//读取到的string
    qDebug()<<"接收检测："<<sReadData;//试试能不能读入数据


    processObject sReadData_Object=Tcp_tools::string_to_processObject(sReadData.toStdString());
    QString userid=QString::fromStdString(sReadData_Object.getUser());

    dbProcess::processQueue.push(sReadData_Object);//把封装的Object加入队列

    //添加该socket到map
    tcp->insert(userid,oTcpSocket);

    qDebug()<<"查看QHostAddress"<<(oTcpSocket->peerAddress()).toString();
    qDebug()<<"查看port"<<QString::number(oTcpSocket->peerPort());
    //qDebug()<<"查看本机ip"<<QHostAddress(QHostAddress::LocalHost).toString();
    //添加ip和port到字典中
//    string ip=(oTcpSocket->peerAddress()).toString().toStdString().substr(7);
//    //ip=ip.substr(7);
//    cout<<"CLIENT's ip="<<ip<<endl;
//    int port=oTcpSocket->peerPort();
//    cout<<"CLIENT's port="<<port<<endl;
//    ip_port* I_P=new ip_port(ip,port);
//    socket_ip_port->insert(userid,I_P);//添加到map socket_ip_port
    //输出map
    //show_socket_ip_port_map();

    //输出map
    //show_map();
    //close();
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
ServerResponseThread::ServerResponseThread()
{

}

ServerResponseThread::ServerResponseThread(QMap<QString,QTcpSocket*>* tcp)
{

}
void ServerResponseThread::run()
{
    //this->tcp2=tcp;//获得map
    while(1)
    {
        //qDebug()<<"ServerResponseThread在执行";
        processObject response=processObject();
        QString userid;
        QString content;
        //如果响应队列不为空(有回复)
        if(dbProcess::correspondQueue.size()!=0)
        {
            response=dbProcess::correspondQueue.front();
            dbProcess::correspondQueue.pop();

            emit db_response_signal(response);//发送信号,参数为响应内容

//                    response=dbProcess::correspondQueue.front();
//                    dbProcess::correspondQueue.pop();
//            userid=QString::fromStdString(response.getUser());
//            content=QString::fromStdString(response.getResult());
//            //在map中查找对应的socket
//            if(tcp2->contains(userid))
//            {
//                QMap<QString ,QTcpSocket *>::iterator it;
//                it=tcp2->find(userid);
//                QTcpSocket *socket=it.value();//获得了对应的socket
//                qDebug()<<"查看回复内容："<<content;
//                socket->write(content.toUtf8());//写回结果
//                socket->flush();
//            }
//            else
//            {
//                qDebug()<<"tcp map中不含这个键值对";
//            }

        }
    }
}
