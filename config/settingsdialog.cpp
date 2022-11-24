#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent), uiS(new Ui::SettingsDialog)
{
    uiS->setupUi(this);

    uiS->i_networktype->setText(SConfig::NETWORKTYPE);
    uiS->i_networkip->setText(SConfig::NETWORKADDRESS);
    uiS->i_networkport->setText(SConfig::NETWORKPORT);
    uiS->i_updateTime->setValue(SConfig::UPDATETIME);
    uiS->i_predictRange->setValue(SConfig::PREDICTRANGE);
    uiS->i_driftAngle->setValue(SConfig::DRIFTANGLE);
    uiS->i_azimuth->setValue(SConfig::AZIMUTH);
    uiS->i_captureRange->setValue(SConfig::CAPTURERANGE);
    uiS->i_captureTime->setValue(SConfig::CAPTURETIME);
    uiS->i_showImages->setChecked(SConfig::SHOWIMAGEONSTART);
    uiS->i_connectOnStart->setChecked(SConfig::CONNECTONSTART);
    uiS->i_debugConsole->setChecked(SConfig::DEBUGCONSOLE);
    uiS->i_useLoader->setChecked(SConfig::USELOADER);
    if(SConfig::ANTENNAPOSITION == "r") {
        uiS->i_antennaRightB->setChecked(true);
        uiS->i_antennaLeftB->setChecked(false); }
    else {
        uiS->i_antennaRightB->setChecked(false);
        uiS->i_antennaLeftB->setChecked(true); }
}

SettingsDialog::~SettingsDialog() { delete uiS; }
void SettingsDialog::on_pushButton_clicked() {
    QString pathNotNullCheck = QFileDialog::getExistingDirectory(this,
                                                                tr("Выберите папку с выходными изображениями РЛС"),
                                                                QStandardPaths::displayName(QStandardPaths::HomeLocation));
    if(pathNotNullCheck!=NULL) { SConfig::PATH = pathNotNullCheck; }
}
void SettingsDialog::on_buttonBox_rejected() { reject(); }

void SettingsDialog::on_buttonBox_accepted()
{
    SConfig::NETWORKTYPE      =           uiS->i_networktype->text();
    SConfig::NETWORKADDRESS   =           uiS->i_networkip->text();
    SConfig::NETWORKPORT      =           uiS->i_networkport->text();
    SConfig::UPDATETIME       =           uiS->i_updateTime->value();
    SConfig::PREDICTRANGE     =           uiS->i_predictRange->value();
    SConfig::DRIFTANGLE       =           uiS->i_driftAngle->value();
    SConfig::CAPTURERANGE     =           uiS->i_captureRange->value();
    SConfig::AZIMUTH          =           uiS->i_azimuth->value();
    SConfig::CAPTURETIME      =           uiS->i_captureTime->value();
    SConfig::ANTENNAPOSITION  =           (uiS->i_antennaLeftB->isChecked()) ? "l" : "r";
    //SConfig::C_PATH         =           SConfig::C_PATH   присваивается в слоте кнопки
    SConfig::SHOWIMAGEONSTART =           (uiS->i_showImages->isChecked()) ? true : false;
    SConfig::CONNECTONSTART   =           (uiS->i_connectOnStart->isChecked()) ? true : false;
    SConfig::DEBUGCONSOLE     =           (uiS->i_debugConsole->isChecked()) ? true : false;
    SConfig::USELOADER        =           (uiS->i_useLoader->isChecked()) ? true : false;
    accept();
}


