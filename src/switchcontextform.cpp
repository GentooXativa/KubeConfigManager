#include "switchcontextform.h"
#include "ui_switchcontextform.h"

SwitchContextForm::SwitchContextForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SwitchContextForm)
{
    ui->setupUi(this);
}

SwitchContextForm::~SwitchContextForm()
{
    delete ui;
}
