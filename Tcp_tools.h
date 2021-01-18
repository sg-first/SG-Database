#ifndef TCP_TOOLS_H
#define TCP_TOOLS_H
#include <QString>
#include <string>
#include "dbProcess.h"
#include <QTcpSocket>
using namespace std;

//class ip_port
//{
//public:
//    string ip;
//    int port;
//    ip_port(string ip,int port)
//    {
//        this->ip=ip;
//        this->port=port;
//    }
//};
class Tcp_tools
{
public:


    QString static get_userid(QString S)
    {
         //userid定位8位数字
        return S.mid(0,8);
    }
    QString static get_content(QString S)
    {
        return S.mid(8);
    }

    string static getUserString(string s, int tag)
    {
        int tag2 = tag;
        int index = 0;
        string userstr;
        for (;; tag2--)
        {
            if (s[tag2] == ':')
            {
                index = tag2;
                break;
            }
        }
        userstr.append(s, index+1, tag - index - 1);
        return userstr;
    }
    string static getUserid(string s, int tag)
    {
        int tag3 = tag;
        int index1 = 0;
        string userstr;
        for (;; tag3--)
        {
            if (s[tag3] == ':')
            {
                index1 = tag3;
                break;
            }
        }
        userstr.append(s, index1+1, tag - index1 - 1);
        return userstr;
    }
    processObject static string_to_processObject(string s)
    {
        int blankcount = 1;
        int tag = 0;
        string user = "";
        string passWord = "";
        string JS = "";
        for (int i = 0;i < s.length(); i++)
        {
            if (s[i] == ' '&&blankcount==1)
            {
                ++blankcount;
                tag = i;
                user = getUserString(s, tag);
                continue;
            }
            if (s[i] == ' ' && blankcount == 2)
            {
                ++blankcount;
                tag = i;
                passWord = getUserid(s, tag);
                continue;
            }
            if (blankcount > 2)
            {

                JS.append(s, i,s.length()-i);
                break;
            }

        }
        return processObject(user, passWord, JS);
    }
};


#endif
