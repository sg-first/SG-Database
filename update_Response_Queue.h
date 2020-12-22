#include <QThread>
#include "data.h"
#pragma once

class update_Respnse_Queue:public QThread
{

public:
    virtual void run() override;
};
void update_Respnse_Queue::run()
{
    for(int i=0;i<10;i++)
    {
        sleep(5000);
        QString S="响应";
        S.append(QString(i));
        Response_Queue.push(S);
    }

}
