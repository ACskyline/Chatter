#include "server.h"

Server::Server(QObject *parent,int port)
    :QTcpServer(parent)
{
    listen(QHostAddress::Any,port);
}

void Server::incomingConnection(int socketDescriptor)
{
    tot = 0;
    TcpClientSocket *tcpClientSocket=new TcpClientSocket(this);
    connect(tcpClientSocket,SIGNAL(updateClients(QString,QByteArray)),this,SLOT(updateClients(QString,QByteArray)));
    connect(tcpClientSocket,SIGNAL(disconnected(int)),this,SLOT(slotDisconnected(int)));
    tcpClientSocket->setSocketDescriptor(socketDescriptor);
    tcpClientSocketList.append(tcpClientSocket);
}

void Server::updateClients(QString fileFrom,QByteArray data)
{
    QString msg = QString::fromUtf8(data);
    ////////////////////////////////
    if(msg.startsWith("ENTER"))//ENTER#FROM#TO#NAME1*NAME2*NAME3...
    {
        emit updateServer(msg);
        tcpClientSocketList.last()->name = msg.section('#',1,1);
        msg+=upDateNames();
        sendMsgToALL(msg);
    }
    else if(msg.startsWith("LEAVE"))//LEAVE#FROM#TO#NAME1*NAME2*NAME3...
    {
        emit updateServer(msg);
        msg+=upDateNamesWithout(msg.section('#',1,1));
        sendMsgToALL(msg);
    }
    else if(msg.startsWith("SEND"))
    {
        emit updateServer(msg);
        QString to = msg.section('#',2,2);
        QString from = msg.section('#',1,1);
        if(to=="ALL")
        {
            sendMsgWithout(from,msg);
        }
        else
        {
            sendMsg(to,msg);
        }
    }
    else if(msg.startsWith("FILE"))
    {
        tot= 0;
        emit updateServer(msg);
        QString to = msg.section('#',2,2);
        QString from = msg.section('#',1,1);
        for(int i=0;i<tcpClientSocketList.count();i++)
        {
            if(tcpClientSocketList.at(i)->name==from)
            {
                tcpClientSocketList.at(i)->fileTo=to;
            }
        }
        if(to=="ALL")
        {
            sendMsgWithout(from,msg);
        }
        else
        {
            sendMsg(to,msg);
        }
    }
    else
    {
        tot+=data.length();
        emit updateServer(QString::number(tot));
        for(int i=0;i<tcpClientSocketList.count();i++)
        {
            if(tcpClientSocketList.at(i)->name==fileFrom)
            {
                if(tcpClientSocketList.at(i)->fileTo=="ALL")
                {
                    sendDataToALL(data);
                }
                else
                {
                    sendData(tcpClientSocketList.at(i)->fileTo,data);
                }
            }
        }
    }
    ////////////////////////////////
}

void Server::slotDisconnected(int descriptor)
{
    for(int i=0;i<tcpClientSocketList.count();i++)
    {
        QTcpSocket *item = tcpClientSocketList.at(i);
        if(item->socketDescriptor()==descriptor)
        {
            tcpClientSocketList.removeAt(i);
            break;
        }
    }
}

void Server::sendMsg(QString name, QString msg)
{
    for(int i=0;i<tcpClientSocketList.count();i++)
    {
        if(tcpClientSocketList.at(i)->name==name)
        {
            QTcpSocket *item = tcpClientSocketList.at(i);
            item->write(msg.toUtf8(),msg.toUtf8().length());
        }
    }
}

void Server::sendMsgToALL(QString msg)
{
    for(int i=0;i<tcpClientSocketList.count();i++)
    {
        QTcpSocket *item = tcpClientSocketList.at(i);
        item->write(msg.toUtf8(),msg.toUtf8().length());
    }
}

void Server::sendMsgWithout(QString name, QString msg)
{
    for(int i=0;i<tcpClientSocketList.count();i++)
    {
        if(tcpClientSocketList.at(i)->name!=name)
        {
            QTcpSocket *item = tcpClientSocketList.at(i);
            item->write(msg.toUtf8(),msg.toUtf8().length());
        }
    }
}

void Server::sendData(QString name, QByteArray data)
{
    for(int i=0;i<tcpClientSocketList.count();i++)
    {
        if(tcpClientSocketList.at(i)->name==name)
        {
            QTcpSocket *item = tcpClientSocketList.at(i);
            item->write(data.data(),data.length());
        }
    }
}

void Server::sendDataToALL(QByteArray data)
{
    for(int i=0;i<tcpClientSocketList.count();i++)
    {
        QTcpSocket *item = tcpClientSocketList.at(i);
        item->write(data.data(),data.length());
    }
}

QString Server::upDateNames()
{
    QString names = "#";
    for(int i=0;i<tcpClientSocketList.count();i++)
    {
        names+=tcpClientSocketList.at(i)->name + "*";
    }
    return names;
}

QString Server::upDateNamesWithout(QString exclude)
{
    QString names = "#";
    for(int i=0;i<tcpClientSocketList.count();i++)
    {
        if(tcpClientSocketList.at(i)->name==exclude)
        {
            continue;
        }
        else
        {
            names+=tcpClientSocketList.at(i)->name + "*";
        }
    }
    return names;
}
