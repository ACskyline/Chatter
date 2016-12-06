#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHostAddress>
#include <QTcpSocket>
#include <QCloseEvent>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    void closeEvent(QCloseEvent *event);
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_enterButton_clicked();
    void on_sendButton_clicked();
    void slotConnected();
    void slotDisconnected();
    void dataReceived();
    void slotConnectedEX();
    void slotDisconnectedEX();
    void dataReceivedEX();
    void on_fileButton_clicked();

private:
    Ui::MainWindow *ui;
    bool status;
    int port;
    int portEX;//(*)
    QHostAddress *serverIP;
    QString userName;
    FILE *wf;
    bool rcv;
    int rcvTotal;
    QString rcvName;
    QString toName;
    QTcpSocket *tcpSocket;
    QTcpSocket *tcpSocketEX;//(*)
    QList<QString> NameList;
};

#endif // MAINWINDOW_H
