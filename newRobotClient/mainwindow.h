#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QInputDialog>
#include <QAction>
#include <QMessageBox>
#include <QMap>
#include <QRegExp>
#include <QFile>

#include <connection.h>
#include <datahandler.h>
#include <controlform.h>


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
    void createConnection();

    QString ipAddress;
    QString *logDataToSave = new QString(""); //Variable holds all the data we will save into the file
    qint16 port;
public slots:
    void changeConnectionStatus(Connection::connectionStatus status, QString statusText);
    void updateUiValues();
    void updateLog(QString data);
private slots:
    //UI
    void connectClicked();
    void showHelp();
    void showAbout();

    void threadFinished();
    void movementWindowAction();

    void sendHalt();
    void sendReturn();
    void sendRelease();
    void sendSpeed();
    void sendRobotRegister();
    void sendGoToWaypoint();

    void batteryLevelWarning();
    void saveLog();

    void movementControl();

private:
    Ui::MainWindow *ui;

    ControlForm* movementControlWindow; //MovementControlWindow, not used
    QThread *thread;
    Connection *connection;
    DataHandler *handler;

    bool connected = false;
    bool movementWindowOpen = false; //Is MovementControlWindow open, not used
signals:
    void closeConnection();
};

#endif // MAINWINDOW_H
