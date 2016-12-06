#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QObject>
#include "tcpclientsocket.h"

class Server : public QTcpServer
{
    Q_OBJECT
public:
    Server(QObject *parent=0,int port=0);
    QList<TcpClientSocket*> tcpClientSocketList;
    void sendMsg(QString, QString);
    void sendMsgWithout(QString, QString);
    void sendMsgToALL(QString);
    void sendDataToALL(QByteArray);
    void sendData(QString, QByteArray);
    QString upDateNames();
    QString upDateNamesWithout(QString);
    int tot;
signals:
    void updateServer(QString);
public slots:
    void updateClients(QString,QByteArray);
    void slotDisconnected(int);
protected:
    void incomingConnection(int socketDescriptor);
};

#endif // SERVER_H
