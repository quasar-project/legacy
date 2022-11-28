#include "coreui.h"
#include "ui_coreui.h"

/* @TODO
 *  Класс для дебага
 *
*/

CoreUI* CoreUI::debugPointer;
CoreUI::CoreUI(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::CoreUI)
{
    debugPointer = this;
    InitializeUI();
    InitializeConnections();
}

CoreUI::~CoreUI()
{
    udpRemote->Disconnect();
    tcpRemote->Disconnect();
    delete ui;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
CoreUI* CoreUI::getDebugPointer(void)                       { return debugPointer;                                                                                                                           }
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CoreUI::debugStreamUpdate(QString _text, int msgtype)
{
    if(uiReady)
    {
        if(msgtype == 0) { ui->debugConsole->setTextColor(Qt::white); }
        else if (msgtype == 1) { ui->debugConsole->setTextColor(Qt::cyan); }
        else if (msgtype == 2) { ui->debugConsole->setTextColor(Qt::yellow); }
        else if (msgtype == 3) { ui->debugConsole->setTextColor(Qt::red); }
        else if (msgtype == 4) { ui->debugConsole->setTextColor(Qt::darkRed); }
            QFont consoleFont = ui->debugConsole->font();
            consoleFont.setPointSize(7);
        ui->debugConsole->insertPlainText(_text);
        ui->debugConsole->setTextColor(Qt::white);
        ui->debugConsole->setFont(consoleFont);
        QTextCursor c = ui->debugConsole->textCursor();
        c.movePosition(QTextCursor::End);
        ui->debugConsole->setTextCursor(c);
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool CoreUI::getReady(void)                                 { return uiReady;                                                                                                                                }
QQuickItem* CoreUI::getMapPointer(void)                     { return qml;                                                                                                                                    }
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CoreUI::Connected()
{
    connected = true;
    ui->label_c_connectionstatus->setText(Style::StyleText("Соединение с РЛС установлено", Colors::Success, Format::Bold));
    ui->label_c_satcount->setVisible(true); ui->label_c_satcount->setEnabled(true);
    ui->groupBox_Loader->setVisible(true); ui->groupBox_Loader->setVisible(true);
}
void CoreUI::Disconnected()
{
    connected = false;
    ui->label_c_connectionstatus->setText(Style::StyleText("Соединение с РЛС не установлено", Colors::Failure, Format::Bold));
    ui->label_c_satcount->setVisible(false); ui->label_c_satcount->setEnabled(false);
    ui->groupBox_Loader->setVisible(false); ui->groupBox_Loader->setVisible(false);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CoreUI::setPushButton_goLeftEnabled(bool state)        { ui->pushButton_goLeft->setEnabled(state);                                                                                                      }
void CoreUI::setPushButton_goRightEnabled(bool state)       { ui->pushButton_goRight->setEnabled(state);                                                                                                     }
void CoreUI::updateLoaderLabel(void)                        { ui->label_c_loaderStatus->setText("Статус: "+Style::StyleText("ожидание подключения", Colors::Main, Format::Italic)); uiTimer1->stop();        }
void CoreUI::updateDirectory(void)                          { if(autoUpdate) { imageProcessing->PartialScan(); linker->panImage(imageProcessing->getFileCounter()); }                                        }
void CoreUI::updateImageManagerLabels(int total, int current)
{
    ui->label_c_foundImages->setText("Найдено "
                                     +Style::StyleText(QString::number(total), Colors::Accent, Format::Bold)
                                     +" изображений");
    ui->label_c_currentImage->setText("Изображение "
                                      +Style::StyleText(QString::number(current+1), Colors::Main, Format::Bold)
                                      +" из "
                                      +Style::StyleText(QString::number(total), Colors::NoColor, Format::Bold));
}
void CoreUI::updateImageMetaLabels(QString filename, float lat, float lon, float dx, float dy, float x0, float y0, float angle, float driftAngle, QString hexSum, QString datetime, bool match)
{
    ui->label_c_metaFilename->setText(filename);
    ui->label_c_metaLat->setText(QString::number(lat, 'f', 6));
    ui->label_c_metaLon->setText(QString::number(lon, 'f', 6));
    ui->label_c_metaDx->setText(QString::number(dx));
    ui->label_c_metaDy->setText(QString::number(dy));
    ui->label_c_metaX0->setText(QString::number(x0));
    ui->label_c_metaY0->setText(QString::number(y0));
    ui->label_c_metaAngle->setText(QString::number(angle));
    ui->label_c_metaDAngle->setText(QString::number(driftAngle));
    ui->label_c_metaChecksum->setText(hexSum);
    ui->label_c_metaTime->setText(datetime);
    (match) ? ui->label_c_checksumSuccess->setText(Style::StyleText("совпадает", Colors::Success)) : ui->label_c_checksumSuccess->setText(Style::StyleText("не совпадает", Colors::Failure));
}
void CoreUI::enableImageBar(bool b)
{
    ui->pushButton_showAllImages->setEnabled(b);
    ui->pushButton_showImage->setChecked(b);
    ui->layout_imageTop_2->setEnabled(b);
    ui->layout_imageMiddle_2->setEnabled(b);
    ui->metaGBox->setEnabled(b);
}
void CoreUI::updateProgress(float f)
{
    if(f>0) { ui->label_c_loaderStatus->setText("Статус: "+Style::StyleText("приём данных", Colors::Accent, Format::Italic)); }
    if(f>99) {
        ui->label_c_loaderStatus->setText("Статус: "+Style::StyleText("изображение получено", Colors::Success, Format::Italic));
        uiTimer1->start(5000);
    }
    ui->progressBar_loader->setValue((int)f);
}
void CoreUI::updateTelemetryLabels(float lat, float lon, float speed, float elevation, int satcount)
{
    ui->label_c_telemetrylat->setText(Style::StyleText(QString::number(lat, 'f', 7), Colors::Main, Format::Bold)+Style::StyleText("°", Colors::MainFaded, Format::NoFormat));
    ui->label_c_telemetrylon->setText(Style::StyleText(QString::number(lon, 'f', 7), Colors::Main, Format::Bold)+Style::StyleText("°", Colors::MainFaded, Format::NoFormat));
    ui->label_c_telemetryspd->setText(Style::StyleText(QString::number(speed, 'f', 1), Colors::Main, Format::Bold)+Style::StyleText("км/ч", Colors::MainFaded, Format::SuperScript));
    ui->label_c_telemetryelv->setText(Style::StyleText(QString::number(elevation, 'f', 1), Colors::Main, Format::Bold)+Style::StyleText("м", Colors::MainFaded, Format::SuperScript));
    ui->label_c_satcount->setText("Спутники: "+Style::StyleText(QString::number(satcount), Colors::Accent, Format::Bold));
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CoreUI::InitializeUI()
{
            qDebug()<<"[STARTUP] Starting UI initialization...";
    ui->setupUi(this);
    ui->map->show();
    qml = ui->map->rootObject();
    QDateTime localTime(QDateTime::currentDateTimeUtc().toLocalTime());
    uiReady = true;
            qDebug()<<"[STARTUP] UI initialization finished";
            qInfo()<<"[STARTUP] Session started at "+localTime.toString();
    if (QSslSocket::supportsSsl())
        {
                    qInfo() << "[STARTUP] OpenSSL detected: "
                            << QSslSocket::supportsSsl()
                            << ", OpenSSL build version: "
                            << QSslSocket::sslLibraryBuildVersionString()
                            << ", OpenSSL ver.: "
                            << QSslSocket::sslLibraryVersionString();
        }
        else {
                    qCritical() << "[ERROR] OpenSSL detected: "
                                << QSslSocket::supportsSsl()
                                << ", OpenSSL build version: "
                                << QSslSocket::sslLibraryBuildVersionString()
                                << ", OpenSSL ver.: "
                                << QSslSocket::sslLibraryVersionString();
            QMessageBox openSSLDialogue;
            openSSLDialogue.setWindowTitle("Библиотека OpenSSL не обнаружена!");
            openSSLDialogue.setIcon(QMessageBox::Critical);
            openSSLDialogue.setText("Попробуйте переустановить программу.");
            openSSLDialogue.exec();
        }
    ui->label_c_loaderStatus->setText("Статус: "+Style::StyleText("ожидание подключения", Colors::Main, Format::Italic));
}

void CoreUI::InitializeConnections()
{
        qInfo()<<"[STARTUP] Setuping connections...";
        timer = new QTimer(this);
        udpTimeout = new QTimer(this);
        uiTimer1 = new QTimer(this);
    linker = new LinkerQML(qml);
    new SConfig(qml);
    SConfig::loadSettings();
    if(SConfig::TESTMODE)
        qWarning()<<"[STARTUP] Program is running in test mode!";
    udpRemote = new UDPRemote();
    tcpRemote = new TCPRemote();
    downloader = new TCPDownloader(this, DowloaderMode::SaveAtDisconnect);
        connect(downloader, SIGNAL(receivingFinished()), this, SLOT(updateDirectory()));
        connect(downloader, SIGNAL(progressChanged(float)), this, SLOT(updateProgress(float)));
    new Style(false);  //false при сборке релиза
    imageProcessing = new ImageProcessing(linker);
        connect(imageProcessing, SIGNAL(setLeftButton(bool)), this, SLOT(setPushButton_goLeftEnabled(bool)));
        connect(imageProcessing, SIGNAL(setRightButton(bool)), this, SLOT(setPushButton_goRightEnabled(bool)));
        connect(imageProcessing, SIGNAL(updateTopLabels(int,int)), this, SLOT(updateImageManagerLabels(int,int)));
        connect(imageProcessing, SIGNAL(updateMetaLabels(QString,float,float,float,float,float,float,float,float,QString,QString,bool)),
                           this, SLOT(updateImageMetaLabels(QString,float,float,float,float,float,float,float,float,QString,QString,bool)));
        connect(imageProcessing, SIGNAL(enableImageBar(bool)), this, SLOT(enableImageBar(bool)));

    ui->debugConsoleDock->setEnabled(SConfig::DEBUGCONSOLE);
    ui->debugConsoleDock->setVisible(SConfig::DEBUGCONSOLE);

        connect(timer, SIGNAL(timeout()), this, SLOT(Halftime()));
        connect(udpTimeout, SIGNAL(timeout()), this, SLOT(Disconnected));
        connect(uiTimer1, SIGNAL(timeout()), this, SLOT(updateLoaderLabel()));
        connect(udpRemote, SIGNAL(received(QByteArray)), this, SLOT(ReadUDPData(QByteArray)));
        connect(tcpRemote, SIGNAL(received(QByteArray)), this, SLOT(ReadUDPData(QByteArray)));
    if(SConfig::CONNECTONSTART)
    {
        if(SConfig::NETWORKTYPE == "TCP") { tcpRemote->Connect(SConfig::NETWORKADDRESS+":"+SConfig::NETWORKPORT); }
        else
        {
            udpRemote->Connect(SConfig::NETWORKADDRESS+":"+SConfig::NETWORKPORT);
            if(SConfig::NETWORKTYPE != "UDP") { SConfig::NETWORKTYPE = "UDP"; qWarning()<<"[WARNING] Connection type string unrecognized, using UDP by default"; }
            qInfo()<<"[REMOTE] UDP client connected";
        }
    }
    ui->label_c_sarip->setText("Адрес РЛС: "+Style::StyleText(" ("+SConfig::NETWORKTYPE+") ", Colors::MainFadedPlus, Format::Bold)
                                            +Style::StyleText(SConfig::NETWORKADDRESS+":"+SConfig::NETWORKPORT, Colors::MainFaded, Format::Bold));
    ui->label_c_loaderip->setText("Адрес загрузчика: "+Style::StyleText(SConfig::LOADERIP+":"+SConfig::LOADERPORT, Colors::MainFaded, Format::Bold));
    timer->start(SConfig::UPDATETIME*1000);
    udpTimeout->start(3*SConfig::UPDATETIME*1000);
    Disconnected();
        qDebug()<<"[STARTUP] Connections set up successfully";
    //execute any startup code here
    imageProcessing->InitialScan();
}

void CoreUI::SendRemoteCommand(QString command)
{
    if(SConfig::NETWORKTYPE == "TCP"){
        tcpRemote->Send(command.toUtf8());
    } else {
        udpRemote->Send(command.toUtf8());
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CoreUI::ReadUDPData(QByteArray data)
{
    udpTimeout->start(3*SConfig::UPDATETIME*1000);
    DataType dtype = SARMessageParser::checkReceivedDataType(data);
    switch (dtype) {
    case DataType::Telemetry:
        telemetry[4] = (double)0;
        telemetry = SARMessageParser::parseTelemetry(data);
        break;
    default:
        break;
    }
    if((int)telemetry[4] != 0 && _conckc != telemetry[0])
    {
        Connected();
    } else { Disconnected(); }
    _conckc = telemetry[0];
    updateTelemetryLabels(telemetry[0], telemetry[1], telemetry[2], telemetry[3], (int)telemetry[4]);
    linker->getTelemetry((float)telemetry[0], (float)telemetry[1], (float)telemetry[3], (float)telemetry[2]);
}

void CoreUI::Halftime() //вызывается раз в SConfig::UPDATETIME (обычно 0.5 сек)
{
    SendRemoteCommand(SARMessageParser::REQUEST_TELEMETRY);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------





