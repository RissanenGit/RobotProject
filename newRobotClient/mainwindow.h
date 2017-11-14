#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>

#include <connection.h>
#include <datahandler.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    void connectSignals();


public slots:
    void changeStatusLabel(QString status);
    void changeConnectionStatus(Connection::connectionStatus status);
    void updateUiValues();
private slots:
    void on_connectButton_clicked();
    void on_connectButton_2_clicked();

    void threadFinished();

private:
    Ui::MainWindow *ui;

    QThread *thread;
    Connection *connection;

    DataHandler *handler;

    bool connected = false;

signals:
    void closeConnection();
};

#endif // MAINWINDOW_H
