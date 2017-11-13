#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
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

public slots:
    void changeStatus(QString status);
    void status(Connection::connectionStatus status);
private slots:
    void on_connectButton_clicked();

private:
    Ui::MainWindow *ui;
    Connection *connection;
    bool connected = false;
};

#endif // MAINWINDOW_H
