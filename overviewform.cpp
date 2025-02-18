#include "overviewform.h"
#include "ui_overviewform.h"

OverviewForm::OverviewForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::OverviewForm)
{
    ui->setupUi(this);
}

OverviewForm::~OverviewForm()
{
    delete ui;
}
