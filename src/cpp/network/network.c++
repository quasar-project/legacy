#include "network.h"
#include "telemetry/telemetry.h"
#include "telemetry/telemetrysocket.h"
#include "execd/execdsocket.h"
#include "execd/feedbacksocket.h"
#include "tcpsocket.h"
#include <QtCore/QDebug>
#include <QtCore/QTimer>

namespace Network {

Network::Network(QObject* parent)
    : QObject{parent}
    , m_telemetry(new Telemetry(this))
    , m_remoteData(new RemoteData(this))
    , execdSocket(new ExecdSocket(this))
    , feedbackSocket(new FeedbackSocket(this))
    , tcpSocket(new TCPSocket(this))
    , m_network_delay_timer(new QTimer(this))
{
    telemetrySocket = new TelemetrySocket(this, m_telemetry);

    qDebug() << "[NETWORK] Beginning network setup";
    m_network_delay_timer->start(100);
    QObject::connect(telemetrySocket, &TelemetrySocket::ping, this, [this](){
        setNetworkDelay(0);
    });
    QObject::connect(m_network_delay_timer, &QTimer::timeout, this, [this](){
        setNetworkDelay(networkDelay() + 0.1);
    });
    QObject::connect(execdSocket, &ExecdSocket::ping, this, [this](){
        setNetworkDelay(0);
    });
    QObject::connect(telemetrySocket, &TelemetrySocket::socketMetrics, this, &Network::telemetrySocketMetrics);
    //! @todo execdsock
    QObject::connect(feedbackSocket, &FeedbackSocket::socketMetrics, this, &Network::feedbackSocketMetrics);

    QObject::connect(feedbackSocket, &FeedbackSocket::diskSpaceReceived, this, [this](long free, long total) {
        float space = free / (float)total;
        remoteData()->setStorageSpace(space);
    });
}

void Network::startTelemetrySocket(const QString& address, float frequency)
{
    telemetrySocket->setFrequency(frequency);
    telemetrySocket->start(address);
}

void Network::stopTelemetrySocket()
{
    telemetrySocket->stop();
}


void Network::startExecdSocket(const QString& execd_address, const QString& feedback_address)
{
    execdSocket->start(execd_address);
    feedbackSocket->start(feedback_address);
}

void Network::stopExecdSocket()
{
    execdSocket->stop();
    feedbackSocket->stop();
}

void Network::executeCommand(const QString& command)
{

}

void Network::startTCPSocket(const QString &address)
{
    tcpSocket->startServer(address);
}

void Network::stopTCPSocket()
{
    tcpSocket->stopServer();
}

void Network::setNetworkDelay(const float& other) {
    if (qFuzzyCompare(m_networkDelay, other)) return;
    m_networkDelay = other;
    emit networkDelayChanged();

    if(networkDelay() >= DISCONNECT_DELAY_THRESHOLD)
        setConnected(0);
    else if(networkDelay() >= SEMICONNECT_DELAY_THRESHOLD)
        setConnected(1);
    else
        setConnected(2);
}
}
