#pragma once

#include <definitions.h++>
#include <QtCore/QObject>
#include <QtNetwork/QHostAddress>

class QUdpSocket;

namespace Debug
{
    class TelemetrySocketEmulator : public QObject
    {
        Q_OBJECT

        constexpr __global uint32_t MARKER = 0x55AA55AA;

        struct DatagramOut
        {
            static_assert(sizeof(double) == 8); // remove later

            uint32_t marker;
            uint8_t version;
            double latitude;
            double longitude;
            double elevation;
            double velocity_course;
            double velocity_east;
            double velocity_north;
            double velocity_vertical;
            double pitch;
            double roll;
            double yaw;
            double course;
            uint64_t time;
            bool valid;
            uint16_t crc16;
        };

        struct DatagramIn
        {
            uint32_t marker;
            uint8_t init_flag;
            uint16_t port;
            int32_t interval_ms;
            uint16_t crc16;
        };

        QUdpSocket* socket;
        QHostAddress m_hostaddress;
        uint16_t m_port;

        public:
            TelemetrySocketEmulator(QObject* parent = nullptr);
            virtual ~TelemetrySocketEmulator();

            void startTelemetryServer(const QString& address = "127.0.0.1:9955");
            void stop();

        private:
            private slots:
                void read();
    };
} // namespace Debug;
