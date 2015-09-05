#include "ipdialog.h"
#include "ui_ipdialog.h"
#include <QSignalMapper>
#include <QKeyEvent>
#include <QDebug>

IPDialog::IPDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::IPDialog)
{
    ui->setupUi(this);
    for(int i=0;i<10;i++)button[i] = new QPushButton(QString("%1").arg(i), this);
    button[10] = new QPushButton(QString("."), this);
    button[11] = new QPushButton(QString("Del"), this);
    button[12] = new QPushButton(QString("Enter"), this);
    for(int i=0;i<5;i++)ui->digitLayout1->addWidget(button[i]);
    for(int i=5;i<10;i++)ui->digitLayout2->addWidget(button[i]);
    for(int i=10;i<13;i++)ui->digitLayout3->addWidget(button[i]);

    QSignalMapper *map = new QSignalMapper(this);
    for(int i=0;i<12;i++)
    {
        connect(button[i], SIGNAL(clicked(bool)), map, SLOT(map()));
        map->setMapping(button[i], button[i]->text());
    }
    connect(button[12], SIGNAL(clicked(bool)), this, SLOT(accept()));
    connect(map, SIGNAL(mapped(QString)), this, SLOT(input(QString)));
}

void IPDialog::input(QString str)
{
    if(str == "Del" && ui->lineEdit->text().length()!=0)
        ui->lineEdit->backspace();
    ui->lineEdit->insert(str);
}

void IPDialog::accept()
{
    ip = ui->lineEdit->text();
    port = ui->spinBox->value();
    QDialog::accept();
}

IPDialog::~IPDialog()
{
    delete ui;
}
