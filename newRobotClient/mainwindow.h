#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QInputDialog>
#include <QAction>
#include <QMessageBox>

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

    QString ipAddress;
    qint16 port;
public slots:
    void changeConnectionStatus(Connection::connectionStatus status, QString statusText);
    void updateUiValues();
    void updateLog(QString data);
private slots:
    //UI
    void connectClicked();

    void threadFinished();
    void sendHalt();
    void sendReturn();
    void sendRelease();
    void sendSpeed();
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
