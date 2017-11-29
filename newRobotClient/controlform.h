#ifndef CONTROLFORM_H
#define CONTROLFORM_H

#include <QWidget>
#include <QDebug>

#include <datahandler.h>

namespace Ui {
class ControlForm;
}

class ControlForm : public QWidget
{
    Q_OBJECT

public:
    explicit ControlForm(QWidget *parent = 0, DataHandler* handler = nullptr);
    ~ControlForm();

private slots:
    void on_upButton_clicked();

private:
    Ui::ControlForm *ui;
    DataHandler* handler;
};

#endif // CONTROLFORM_H
