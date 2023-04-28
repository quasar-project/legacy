#pragma once

#include <definitions.h++>
#include "telemetry/telemetry.h++"
#include <QtCore/QObject>

class QTimer;

namespace Network
{
    class TelemetrySocket;
    class ExecdSocket;
    class FeedbackSocket;
    class TCPSocket;

    class Network : public QObject
    {
        Q_OBJECT
        Q_PROPERTY(Telemetry* telemetry READ telemetry WRITE setTelemetry NOTIFY telemetryChanged)
        Q_PROPERTY(float networkDelay READ networkDelay WRITE setNetworkDelay NOTIFY networkDelayChanged)
        Q_PROPERTY(int connected READ connected WRITE setConnected NOTIFY connectedChanged)

        constexpr __global float DISCONNECT_DELAY_THRESHOLD = 10.0f;
        constexpr __global float SEMICONNECT_DELAY_THRESHOLD = 3.0f;

        static Network* instance;
        QTimer* m_network_delay_timer;

        Telemetry* m_telemetry;
        float m_networkDelay = DISCONNECT_DELAY_THRESHOLD + 0.1f;
        int m_connected = 0;

        public:
            static Network* get(QObject* parent = nullptr);

            TelemetrySocket* telemetrySocket;
            ExecdSocket* execdSocket;
            FeedbackSocket* feedbackSocket;
            TCPSocket* tcpSocket;

            __qml void startTelemetrySocket(const QString& address, float frequency);
            __qml void stopTelemetrySocket();

            __qml void startExecdSocket(const QString& execd_address, const QString& feedback_address);
            __qml void stopExecdSocket();
            __qml void executeCommand(const QString& command);

            __qml void startTCPSocket(const QString& address);
            __qml void stopTCPSocket();

            Telemetry* telemetry() const;
            void setTelemetry(Telemetry* other);

            float networkDelay() const;
            void setNetworkDelay(float other);

            int connected() const;
            void setConnected(int other);

            signals:
                __signal telemetryChanged();
                __signal networkDelayChanged();
                __signal connectedChanged();

                __signal __profile telsock(const QString& data);
                __signal __profile execdsock(const QString& data);
                __signal __profile feedbacksock(const QString& data);

        private:
            explicit Network(QObject* parent = nullptr);
    };
} // namespace Network;
