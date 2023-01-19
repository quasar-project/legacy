#ifndef COREUI_H
#define COREUI_H

#include <QMainWindow>
#include <QScreen>
#include <QSslSocket>
#include <QTimer>
#include "qqml.h"
#include <QQuickStyle>
#include <QQmlEngine>
#include <QQmlContext>
#include <QRect>
#include <QScopedPointer>
#include <QSet>

//qml types
#include "backend/fmousekeyhandler.h"
#include "backend/recallhandler.h"
#include "backend/flightprediction.h"

#include "udpremote.h"
#include "messageparser.h"
#include "smath.h"

#include "settingsdialog.h"
#include "aboutdialog.h"
#include "passworddialog.h"

#include "style.h"
#include "thememanager.h"
#include "linkerqml.h"

#include "tcpdownloader.h"
#include "tilesmanager.h"
#include "runtimedata.h"
#include "backend/signallinker.h"
#include "applicationheader.h"
#include "flightemulator.h"

#include <plugin.h>
#include <pluginHostAPI.h>

QT_BEGIN_NAMESPACE
namespace Ui { class CoreUI; }
QT_END_NAMESPACE

enum CommandType {
    TelemetryCommand,
    FormCommand
};

class CoreUI : public QMainWindow
{
    Q_OBJECT

public:
    CoreUI(QWidget *parent = nullptr);
    ~CoreUI();
    //static public methods
    static CoreUI* getDebugPointer(void);
    static QRect screenResolution;

    //public methods
    void debugStreamUpdate(QString _text, int msgtype);

    //public getters
    void getConfig(QString s1, QString s2, QString s3, float f1, float f2, float f3, float f4, float f5, float f6, QString s4, QString s5, bool b1, bool b2, bool b3);
    bool getReady(void);
    QQuickItem* getMapPointer(void);
    bool eventFilter(QObject* obj, QEvent* event);
    //void keyPressEvent(QKeyEvent *event);

public slots:
    //utility public slots
    void Connected();
    void Disconnected();

    //gui public slots
    void updateProgress(float f);
    void reconnectSlot();
    void disconnectSlot();

private:
    //object pointers
    Ui::CoreUI *ui;
    static CoreUI* debugPointer;
    UDPRemote *telemetryRemote;
    UDPRemote *formRemote;
    UDPRemote *consoleListenerRemote;
    LinkerQML *linker;
    TCPDownloader *downloader;
    QQuickItem* qml;
    FlightEmulator* flightEmulator;
    PluginHostAPI *HostAPI;

    //timers
    QTimer *timer;
    QTimer *udpTimeout;
    QTimer *uiTimer1;

    //global flags
    bool uiReady = false;
    bool autoUpdate = true;
    bool formingContinuous = false;

    //global variables
    double _conckc = 0;
    QSet<int> pressedKeys;

    //plugin system
    struct Plugins 
    {
        bool terminalLoaded = false;

        QWidget* terminal = nullptr;
    }; Plugins plugins;

    //private methods
    void InitializeDockwidgets();
    void SendRemoteCommand(QString command, CommandType type);

private slots:
    //header
    void MinimizeSlot();
    void CloseSlot();
    void SettingsSlot();
    void InfoSlot();
    void EmulatorSlot();
    void DebugSlot();

    //utility slots
    void ReadTelemetry(QByteArray data);
    void ReadForm(QByteArray data);
    void ReadSARConsole(QByteArray data);
    void Halftime(void);
    void toggleConsoleSlot(void);
    void SendClearCommand(void);

    //gui slots
    void FormSingleImage();
    void FormContinuousImages();
    void on_spinBox_sarLowerBound_valueChanged(int arg1);
    void on_spinBox_sarUpperBound_valueChanged(int arg1);
    void on_doubleSpinBox_sarTime_valueChanged(double arg1);
    void on_doubleSpinBox_sarDX_valueChanged(double arg1);
    void on_checkBoxEnableManualGPS_stateChanged(int arg1);
    void on_doubleSpinBox_height_valueChanged(double arg1);
    void on_doubleSpinBox_velocity_valueChanged(double arg1);
    void on_pushButton_sendCustomCommand_clicked();
};
#endif // COREUI_H
