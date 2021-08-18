#include "radioactive.h"
#include "ui_radioactive.h"

radioactive::radioactive(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::radioactive)
{
    ui->setupUi(this);
}

radioactive::~radioactive()
{
    delete ui;
}

