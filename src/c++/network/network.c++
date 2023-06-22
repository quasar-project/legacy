#include "network.h"
#include <QtCore/QTimer>
#include <QtCore/QVariant>
#include "config/config.h"
#include "config/internalconfig.h"
#include "telemetry/telemetry.h"
#include "telemetry/telemetrysocket.h"
#include "execd/execdsocket.h"
#include "execd/execdargumentlist.h"
#include "execd/feedbacksocket.h"
#include "lfs/tcpsocket.h"
#include "lfs/stripsocket.h"
#include "gui/terminal/vt100terminal.h"
#include "utils/ping.h"
#include "utils/vt100parser.h"
#include "utils/commonutils.h"

namespace Networking
{

Network *Network::get() { static Network instance; return &instance; }

Network::Network(QObject* parent)
    : QObject{parent}
    , m_telemetry(new Telemetry(this))
    , m_remoteData(new RemoteData(this))
    , execdSocket(new ExecdSocket(this))
    , feedbackSocket(new FeedbackSocket(this))
    , tcpSocket(new TCPSocket(this))
    , stripSocket(new StripSocket(this))
    , m_network_delay_timer(new QTimer(this))
    , m_networkDelay(ICFG<float>("NETWORK_DELAY_THRESHOLD_DISCONNECT") + .1f)
    , m_connected(0)
    , m_recording(false)
    , m_de10ping(new Pinger(this))
    , m_jetsonping(new Pinger(this))
    , m_navping(new Pinger(this))
    , m_utl1ping(new Pinger(this))
    , m_utl2ping(new Pinger(this))
{
    telemetrySocket = new TelemetrySocket(this, m_telemetry);

    qDebug() << "[NETWORK] Beginning network setup";

    m_network_delay_timer->start(100);
    QObject::connect(telemetrySocket, &TelemetrySocket::ping, this, [this](){ setNetworkDelay(0); });
    QObject::connect(telemetrySocket, &TelemetrySocket::socketMetrics, this, &Network::telemetrySocketMetrics);
    QObject::connect(m_network_delay_timer, &QTimer::timeout, this, [this](){ setNetworkDelay(networkDelay() + 0.1); });
    QObject::connect(execdSocket, &ExecdSocket::ping, this, [this](){ setNetworkDelay(0); });
    QObject::connect(execdSocket, &ExecdSocket::socketMetrics, this, &Network::execdSocketMetrics);
    QObject::connect(feedbackSocket, &FeedbackSocket::socketMetrics, this, &Network::feedbackSocketMetrics);
    QObject::connect(feedbackSocket, &FeedbackSocket::textReceived, this, &Network::processFeedback);
    QObject::connect(feedbackSocket, &FeedbackSocket::diskSpaceReceived, this, [this](long free, long total) {
        float space = free / (float)total;
        remoteData()->setStorageSpace(space);
    });

    QObject::connect(tcpSocket, &TCPSocket::progressChanged, this, [this](float progress){ setTcpProgress(progress); });
    QObject::connect(tcpSocket, &TCPSocket::receivingFinished, this, [this](){ executeCommand(Enums::RemoteStorageStatus); });
    QObject::connect(tcpSocket, &TCPSocket::socketMetrics, this, &Network::lfsSocketMetrics);

    QObject::connect(m_de10ping, &Pinger::result, this, [this](int result){ remoteData()->setDe10ping(result); });
    QObject::connect(m_jetsonping, &Pinger::result, this, [this](int result){ remoteData()->setJetsonping(result); });
    QObject::connect(m_navping, &Pinger::result, this, [this](int result){ remoteData()->setNavping(result); });
    QObject::connect(m_utl1ping, &Pinger::result, this, [this](int result){ remoteData()->setUtl1ping(result); });
    QObject::connect(m_utl2ping, &Pinger::result, this, [this](int result){ remoteData()->setUtl2ping(result); });

    m_de10ping->start(0, CONFIG(remoteIP));
    m_jetsonping->start(0, CONFIG(jetsonIP));
    m_navping->start(0, CONFIG(navIP));
    m_utl1ping->start(0, CONFIG(utl1IP));
    m_utl2ping->start(0, CONFIG(utl2IP));

    QObject::connect(telemetry(), &Telemetry::seaLevelChanged, this, [this](){
        setArgument("--e0", telemetry()->seaLevel(), Enums::Form);
        setArgument("--e0", telemetry()->seaLevel(), Enums::Reform);
        setArgument("--e0", telemetry()->seaLevel(), Enums::Focus);
    });
}

void Network::processFeedback(QByteArray data) noexcept
{
    QString tstr = data;
    QList<QString> ls;
    const int SPLIT_LENGTH = 70;
    while(tstr.length() > SPLIT_LENGTH)
    {
        ls.push_back(tstr.left(SPLIT_LENGTH));
        tstr.remove(0, SPLIT_LENGTH);
    }

    if(ls.empty())
        GUI::VT100Terminal::get()->append(utils::parse_vt100_string(tstr).result);
    else
        for(const auto& line : ls)
            GUI::VT100Terminal::get()->append(utils::parse_vt100_string(line).result);
}

void Network::begin(const QString& telemetry_request_addr, const QString& telemetry_recv_addr, float telemetry_frequency,
                    const QString& execd_addr, const QString& feedback_addr, const QString& tcp_lfs_addr, const QString& udp_lfs_addr) noexcept
{
    telemetrySocket->setFrequency(telemetry_frequency);
    telemetrySocket->start(telemetry_request_addr, telemetry_recv_addr);
    execdSocket->start(execd_addr);
    feedbackSocket->start(feedback_addr);
    tcpSocket->startServer(tcp_lfs_addr);
    stripSocket->start(udp_lfs_addr);
}

void Network::stop() noexcept
{
    telemetrySocket->stop();
    execdSocket->stop();
    feedbackSocket->stop();
    tcpSocket->stopServer();
    stripSocket->stop();
    this->setNetworkDelay(100);
}

void Network::executeCommand(const Networking::Enums::NetworkCommand command) noexcept
{
    switch(command)
    {
        case Enums::FormImage:
            this->setArgument("--remote", QString(CONFIG(localIP) + ":" + CONFIG(tcpLFSPort)), Enums::Form);
            break;
        case Enums::FocusImage:
            this->setArgument("--remote", QString(CONFIG(localIP) + ":" + CONFIG(tcpLFSPort)), Enums::Focus);
            break;
        case Enums::ReformImage:
            this->setArgument("--remote", QString(CONFIG(localIP) + ":" + CONFIG(tcpLFSPort)), Enums::Reform);
            break;
        case Enums::StartStrip:
            this->setArgument("--remote", QString(CONFIG(localIP) + ":" + CONFIG(udpLFSPort)), Enums::Form);
            break;
        case Enums::StopStrip:
            this->setArgument("--remote", QString(CONFIG(localIP) + ":" + CONFIG(udpLFSPort)), Enums::Form);
            break;

        default: break;
    }

    execdSocket->executeCommand(command);
}

void Network::executeString(const QString &string) noexcept
{
    GUI::VT100Terminal::get()->append("% Выполняется " + string);
    execdSocket->executeCommand(string);
}
QString Network::argument(const QString& key, Enums::ArgumentCategory category) const noexcept
{
    switch (category)
    {
        case Enums::Form: return execdSocket->list()->argument_list[key].value;
        case Enums::Focus: return execdSocket->list()->focus_argument_list[key].value;
        case Enums::Reform: return execdSocket->list()->reform_argument_list[key].value;
        default: return "Argument Category Error"; break;
    }
}

void Network::setArgument(const QString& key, const QVariant& value, Enums::ArgumentCategory category) noexcept
{
    switch (category)
    {
        case Enums::Form: execdSocket->list()->argument_list[key].set(value); break;
        case Enums::Focus: execdSocket->list()->focus_argument_list[key].set(value); break;
        case Enums::Reform: execdSocket->list()->reform_argument_list[key].set(value); break;
        default: qCritical() << "[NETWORK] Invalid category for argument provided"; break;
        }
}

QString Network::stringifyIP(const QString& ip, const QString& port) noexcept { return utils::host_to_string(ip, port); }

Telemetry* Network::telemetry() const { return m_telemetry; }
void Network::setTelemetry(Telemetry* other) {
    if (m_telemetry == other)
        return;
    m_telemetry = other;
    emit telemetryChanged();
}

RemoteData* Network::remoteData() const { return m_remoteData; }
void Network::setRemoteData(RemoteData* other) {
    if (m_remoteData == other)
        return;
    m_remoteData = other;
    emit remoteDataChanged();
}

float Network::networkDelay() const { return m_networkDelay; }
void Network::setNetworkDelay(float other) {
    if (qFuzzyCompare(m_networkDelay, other)) return;
    m_networkDelay = other;
    emit networkDelayChanged();

    if(networkDelay() >= ICFG<float>("NETWORK_DELAY_THRESHOLD_DISCONNECT"))
        setConnected(0);
    else if(networkDelay() >= ICFG<float>("NETWORK_DELAY_THRESHOLD_BADCONNECT"))
        setConnected(1);
    else
        setConnected(2);
}

int Network::connected() const { return m_connected; }
void Network::setConnected(int other) {
    if (m_connected == other) return;
    m_connected = other;
    emit connectedChanged();
}

float Network::tcpProgress() const { return m_tcpProgress; }
void Network::setTcpProgress(float other) {
    if (qFuzzyCompare(m_tcpProgress, other))
        return;
    m_tcpProgress = other;
    emit tcpProgressChanged();
}

bool Network::recording() const { return m_recording; }
void Network::setRecording(bool other) {
    if (m_recording == other)
        return;
    m_recording = other;
    emit recordingChanged();

    stripSocket->setStripStatus(recording());
}

}

