#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <windows.h>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    status = false;

    //////////////////////////////////////////////////
    port = 8010;
    portEX = 8020;
    ui->portLineEdit->setText(QString::number(port));
    ui->portEXlineEdit->setText(QString::number(portEX));
    ui->srvIPLineEdit->setText("127.0.0.1");
    serverIP =new QHostAddress();
    ui->sendButton->setEnabled(false);
    ui->fileButton->setEnabled(false);
    ui->toNameCombo->setEnabled(false);
    wf = 0;
    rcvTotal = -1;
    rcv = false;
    //////////////////////////////////////////////////
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    this->setFocus();
    if(status)
    {
        QString msg="LEAVE#"+userName+"#ALL#"+userName+":Leave Chat Room";
        tcpSocket->write(msg.toUtf8(),msg.toUtf8().length());
        tcpSocketEX->write(msg.toUtf8(),msg.toUtf8().length());//(*)
        tcpSocket->disconnectFromHost();
        tcpSocketEX->disconnectFromHost();//(*)
    }
    event->accept();
}

void MainWindow::on_enterButton_clicked()
{
    if(!status)
    {
        QString ip = ui->srvIPLineEdit->text();
        if(!serverIP->setAddress(ip))
        {
            QMessageBox::information(this,tr("error"),tr("server ip address error!"));
            return;
        }

        if(ui->userNameEdit->text()==""||ui->userName->text().contains('*'))
        {
            QMessageBox::information(this,tr("error"),tr("User name error!"));
            return;
        }

        userName=ui->userNameEdit->text();
        port = ui->portLineEdit->text().toInt();
        portEX = ui->portEXlineEdit->text().toInt();

        tcpSocket = new QTcpSocket(this);
        connect(tcpSocket,SIGNAL(connected()),this,SLOT(slotConnected()));
        connect(tcpSocket,SIGNAL(disconnected()),this,SLOT(slotDisconnected()));
        connect(tcpSocket,SIGNAL(readyRead()),this,SLOT(dataReceived()));
        tcpSocket->connectToHost(*serverIP,port);

        tcpSocketEX = new QTcpSocket(this);
        connect(tcpSocketEX,SIGNAL(connected()),this,SLOT(slotConnectedEX()));
        connect(tcpSocketEX,SIGNAL(disconnected()),this,SLOT(slotDisconnectedEX()));
        connect(tcpSocketEX,SIGNAL(readyRead()),this,SLOT(dataReceivedEX()));
        tcpSocketEX->connectToHost(*serverIP,portEX);

        status=true;
    }
    else
    {
        QString msg="LEAVE#"+userName+"#ALL#"+userName+":Leave Chat Room";
        tcpSocket->write(msg.toUtf8(),msg.toUtf8().length());
        tcpSocket->disconnectFromHost();
        tcpSocketEX->write(msg.toUtf8(),msg.toUtf8().length());
        tcpSocketEX->disconnectFromHost();
        status=false;
    }
}

void MainWindow::on_sendButton_clicked()
{
    toName = ui->toNameCombo->currentText();
    if(ui->sendLineEdit->text()=="")
    {
        return ;
    }
    QString msg="SEND#"+userName+"#"+toName+"#"+userName+":"+ui->sendLineEdit->text();
    ui->listWidget->addItem(userName+":"+ui->sendLineEdit->text());
    ui->listWidget->scrollToBottom();
    tcpSocket->write(msg.toUtf8(),msg.toUtf8().length());
    ui->sendLineEdit->clear();
}

void MainWindow::on_fileButton_clicked()
{
    if(ui->fileLineEdit->text()=="")
    {
        return ;
    }

    toName = ui->toNameCombo->currentText();
    FILE *rf;
    QString fileName = ui->fileLineEdit->text();
    QString msg="FILE#"+userName+"#"+toName+"#"+fileName+"*";
    QByteArray contentB;
    rf = fopen(fileName.toLatin1(), "rb");
    if (rf != 0)
    {
        ui->debugLabel->setText("sending...");
        ui->fileButton->setEnabled(false);
        while (true)
        {
            char buffer[5120];
            size_t rlength = fread(buffer, 1, sizeof(buffer), rf);
            if (rlength == 0)
            {
                msg+=QString::number(contentB.length());
                ui->listEXWidget->addItem(userName+":"+fileName+" "+QString::number(contentB.length())+"Byte");
                ui->listEXWidget->scrollToBottom();
                tcpSocketEX->write(msg.toUtf8(),msg.toUtf8().length());
                tcpSocketEX->flush();////////////////////////////////////////////////////////////////////////////////////
                Sleep(30);
                tcpSocketEX->write(contentB.data(),contentB.length());
                break;
            }
            else
            {
                QByteArray temp(buffer,rlength);//这个是靠谱的
                contentB+=temp;//这个是靠谱的
            }
        }
        fclose(rf);
    }
    ui->debugLabel->setText("sent");
    ui->fileButton->setEnabled(true);
}

void MainWindow::slotConnected()
{
    ui->sendButton->setEnabled(true);
    ui->toNameCombo->setEnabled(true);
    ui->enterButton->setText(tr("leave"));
    QString msg="ENTER#" + userName + "#ALL#" +userName + ":Enter Chat Room";
    tcpSocket->write(msg.toUtf8(),msg.toUtf8().length());
}

void MainWindow::slotDisconnected()
{
    ui->sendButton->setEnabled(false);
    ui->enterButton->setText(tr("enter"));
    ui->toNameCombo->clear();
    ui->toNameCombo->setEnabled(false);
    NameList.clear();
}

//ENTER#FROM#TO#CONTENT#NAME1*NAME2*NAME3...名字中不得出现*号
//LEAVE#FROM#TO#CONTENT#NAME1*NAME2*NAME3...名字中不得出现*号
//SEND#FROM#TO#CONTENT
void MainWindow::dataReceived()
{
    while(tcpSocket->bytesAvailable()>0)
    {
        QByteArray datagram;
        datagram.resize(tcpSocket->bytesAvailable());
        tcpSocket->read(datagram.data(),datagram.size());
        QString msg=QString::fromUtf8(datagram.data(),datagram.length());

        ///////////////////////////////////////////////////////
        QString content;

        if(msg.startsWith("SEND"))
        {
            QString from = msg.section('#',1,1);
            QString to = msg.section('#',2,2);
            if(from==userName||to==userName||to=="ALL")
            {
                content = msg.section('#',3);
                ui->listWidget->addItem(content);
                ui->listWidget->scrollToBottom();
            }

        }
        else if(msg.startsWith("ENTER")||msg.startsWith("LEAVE"))
        {
            QString from = msg.section('#',1,1);
            QString to = msg.section('#',2,2);
            if(from==userName||to==userName||to=="ALL")
            {
                content = msg.section('#',3,3);
                QString names = msg.section("#",4);

                NameList.clear();
                ui->toNameCombo->clear();
                ui->toNameCombo->addItem("ALL");

                while(names.isEmpty()==false)//将在线用户名暂存到内存
                {
                    QString temp = names.section("*",0,0);
                    NameList.append(temp);
                    names = names.section("*",1);
                }

                for(int i = 0;i<NameList.count();i++)//从内存更新到comboBox
                {
                    ui->toNameCombo->addItem(NameList.at(i));
                }

                ui->listWidget->addItem(content);
                ui->listWidget->scrollToBottom();
            }

        }


        ///////////////////////////////////////////////////////
    }
}

void MainWindow::slotConnectedEX()
{
    ui->fileButton->setEnabled(true);
    QString msg="ENTER#" + userName + "#ALL#" +userName + ":Enter File Room";
    tcpSocketEX->write(msg.toUtf8(),msg.toUtf8().length());
}

void MainWindow::slotDisconnectedEX()
{
    ui->fileButton->setEnabled(false);
}


//FILE#FROM#TO#FILENAME*LENGTH
void MainWindow::dataReceivedEX()
{
    while(tcpSocketEX->bytesAvailable()>0)
    {
        if(rcv)//开始接收数据
        {
            ui->debugLabel->setText("receiving...");
            ui->fileButton->setEnabled(false);
            if(rcvTotal>0)
            {
                QByteArray datagram;
                datagram.resize(tcpSocketEX->bytesAvailable());
                tcpSocketEX->read(datagram.data(),datagram.size());
                size_t wlength = 0;
                while((int)wlength<datagram.size())
                {
                    wlength += fwrite(datagram.data()+wlength, 1, datagram.length()-wlength, wf);
                }
                rcvTotal-=datagram.length();
                ui->debugLabel->setText("receiving..."+QString::number(rcvTotal)+"Byte left");
            }
            if(rcvTotal<=0)
            {
                fclose(wf);
                ui->debugLabel->setText("received");
                ui->fileButton->setEnabled(true);
                rcvTotal=-1;
                rcv=false;
            }
        }
        else//开始接收信息
        {
            QByteArray datagram;
            datagram.resize(tcpSocketEX->bytesAvailable());
            tcpSocketEX->read(datagram.data(),datagram.size());
            QString msg=QString::fromUtf8(datagram.data(),datagram.length());
            //
            if(msg.startsWith("FILE"))
            {
                QString from = msg.section('#',1,1);
                QString to = msg.section('#',2,2);
                if(from!=userName&&(to==userName||to=="ALL"))
                {
                    QString content = msg.section('#',3);
                    rcvName = content.section('*',0,0);
                    rcvTotal = content.section('*',1,1).toInt();
                    rcvName = "new"+rcvName;
                    wf = fopen(rcvName.toLatin1(), "wb+");
                    ui->listEXWidget->addItem(from+":"+rcvName+" "+QString::number(rcvTotal)+"Byte");
                    ui->listEXWidget->scrollToBottom();
                    rcv= true;//准备接受数据
                }
            }
        }
    }
}
