#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "server.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_createButton_clicked();
    void updateServer(QString);
    void updateServerEX(QString);
private:
    Ui::MainWindow *ui;
    int port;
    int portEX;
    Server *server;
    Server *serverEX;
};

#endif // MAINWINDOW_H
