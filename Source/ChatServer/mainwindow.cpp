#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(tr("LKAA STUDIO"));
    /////////////////////////////////////////////
    port=8010;
    portEX=8020;
    ui->portLineEdit->setText(QString::number(port));
    ui->portEXLineEdit->setText(QString::number(portEX));
    /////////////////////////////////////////////
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_createButton_clicked()
{
    port = ui->portLineEdit->text().toInt();
    server = new Server(this,port);
    connect(server,SIGNAL(updateServer(QString)),this,SLOT(updateServer(QString)));

    portEX = ui->portEXLineEdit->text().toInt();
    serverEX = new Server(this,portEX);
    connect(serverEX,SIGNAL(updateServer(QString)),this,SLOT(updateServerEX(QString)));

    ui->createButton->setEnabled(false);
    ui->portLineEdit->setEnabled(false);
    ui->portEXLineEdit->setEnabled(false);
}

void MainWindow::updateServer(QString msg)
{
    ui->listWidget->addItem(msg);
    ui->listWidget->scrollToBottom();
}

void MainWindow::updateServerEX(QString msg)
{
    ui->listEXWidget->addItem(msg);
    ui->listEXWidget->scrollToBottom();
}
