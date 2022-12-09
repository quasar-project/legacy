#include "aboutdialog.h"
#include "ui_aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent, QString softwareBuildVersion) :
    QDialog(parent),
    softwareVersion(softwareBuildVersion),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
    ui->softVersion->setText("Версия программного обеспечения " + Style::StyleText(softwareVersion, Colors::Accent, Format::Bold));
}

AboutDialog::~AboutDialog()
{
    delete ui;
}
