#include "controlform.h"
#include "ui_controlform.h"

ControlForm::ControlForm(QWidget *parent, DataHandler* handler) :
    QWidget(parent),
    ui(new Ui::ControlForm)
{
    ui->setupUi(this);
    this->handler = handler;
}

ControlForm::~ControlForm()
{
    qDebug() << "ControlForm destroyed";
    delete ui;
}

void ControlForm::on_upButton_clicked(){handler->createMessage(DataHandler::messageTypes::Halt);}
