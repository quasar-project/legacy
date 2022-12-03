#ifndef COREUI_H
#define COREUI_H

#include <QMainWindow>
#include <QSslSocket>
#include <QTimer>
#include <QQmlEngine>
#include <QQmlContext>

#include "udpremote.h"
#include "tcpremote.h"
#include "settingsdialog.h"
#include "imageprocessing.h"
#include "imagemanager.h"
#include "messageparser.h"
#include "debug.h"

#include "style.h"
#include "linkerqml.h"
#include "sconfig.h"
#include "tcpdownloader.h"
#include "tilesmanager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class CoreUI; }
QT_END_NAMESPACE

class CoreUI : public QMainWindow
{
    Q_OBJECT

public:
    CoreUI(QWidget *parent = nullptr);
    ~CoreUI();
    //static public methods
    static CoreUI* getDebugPointer(void);

    //public methods
    void debugStreamUpdate(QString _text, int msgtype);

    //public getters
    void getConfig(QString s1, QString s2, QString s3, float f1, float f2, float f3, float f4, float f5, float f6, QString s4, QString s5, bool b1, bool b2, bool b3);
    bool getReady(void);
    QQuickItem* getMapPointer(void);

public slots:
    //utility public slots
    void Connected();
    void Disconnected();

    //gui public slots
    void setPushButton_goLeftEnabled(bool state);
    void setPushButton_goRightEnabled(bool state);
    void updateLoaderLabel(void);
    void updateDirectory(void);
    void updateImageManagerLabels(int total, int current);
    void updateImageMetaLabels(QString filename, float lat, float lon, float dx, float dy, float x0, float y0, float angle, float driftAngle, QString hexSum, QString datetime, bool match);
    void enableImageBar(bool b);
    void updateProgress(float f);
    void updateTelemetryLabels(float lat, float lon, float speed, float elevation, int satcount);

private:
    //object pointers
    Ui::CoreUI *ui;
    static CoreUI* debugPointer;
    UDPRemote *udpRemote;
    TCPRemote *tcpRemote;
    ImageProcessing *imageProcessing;
    LinkerQML *linker;
    TCPDownloader *downloader;
    QQuickItem* qml;

    //timers
    QTimer *timer;
    QTimer *udpTimeout;
    QTimer *uiTimer1;

    //global flags
    bool uiReady = false;
    bool connected = false;
    bool autoUpdate = true;

    //global variables
    double _conckc = 0;
    std::array<double, 5> telemetry; //lat, lon, speed, elevation, sats

    //private methods
    void InitializeUI();
    void InitializeConnections();
    void SendRemoteCommand(QString command);

private slots:
    //utility slots
    void ReadUDPData(QByteArray data);
    void Halftime();

    //menu slots
    void on_formImage_triggered();
    void on_openSettings_triggered();

    //gui slots
    void on_checkBox_drawTooltip_stateChanged(int arg1);
    void on_checkBox_drawTrack_stateChanged(int arg1);
    void on_checkBox_stateChanged(int arg1);
    void on_pushButton_panGPS_clicked();
    void on_pushButton_goLeft_clicked();
    void on_pushButton_goRight_clicked();
    void on_checkBox_autoUpdate_stateChanged(int arg1);
    void on_pushButton_expandImageInfo_clicked();
    void on_pushButton_update_clicked();
    void on_pushButton_panImage_clicked();
    void on_pushButton_showImage_clicked();
    void on_pushButton_clearTrack_clicked();
    void on_pushButton_showAllImages_clicked();
    void on_pushButton_reconnect_clicked();
    void on_pushButton_clearCache_clicked();
};
#endif // COREUI_H
