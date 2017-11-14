#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>

#include <connection.h>
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
private slots:
    void on_connectButton_clicked();
    void threadFinished();

private:
    Ui::MainWindow *ui;

    QThread *thread;
    Connection *connection;

    bool connected = false;

signals:
    void closeConnection();
};

#endif // MAINWINDOW_H
