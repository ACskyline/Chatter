#include "tcpclientsocket.h"

TcpClientSocket::TcpClientSocket(QObject *parent)
{
    name = "_";
    fileTo = "_";
    connect(this,SIGNAL(readyRead()),this,SLOT(dataReceived()));
    connect(this,SIGNAL(disconnected()),this,SLOT(slotDisconnected()));
}

void TcpClientSocket::dataReceived()
{
    while(bytesAvailable()>0)
    {
        QByteArray datagram;
        datagram.resize(bytesAvailable());
        read(datagram.data(),datagram.size());
        emit updateClients(name,datagram);
    }
}

void TcpClientSocket::slotDisconnected()
{
    emit disconnected(this->socketDescriptor());
}
