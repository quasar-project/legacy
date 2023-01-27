#ifndef UDPEMULATOR_H
#define UDPEMULATOR_H

#include <QObject>
#include <QUdpSocket>
#include <QJsonDocument>

#include "network/messageparser.h"
#include "config/sconfig.h"

class UDPEmulator : public QObject
{
    Q_OBJECT
public:
    explicit UDPEmulator(QObject *parent = nullptr);
    ~UDPEmulator();
    QByteArray jsonEncode(float latitude, float longitude, float speed, float elevation, int satellites, float pitch, float roll);
    struct EmulatorTelemetry
    {
        qreal latitude = 0;
        qreal longitude = 0;
        qreal speed = 0;
        qreal elevation = 0;
        qreal pitch = 0;
        qreal roll = 0;
        int sats = 6;
    }; EmulatorTelemetry emulatorTelemetry;

signals:

private:
    QUdpSocket* m_socket = nullptr;

private slots:
    void readSlot();

};

#endif // UDPEMULATOR_H