#include "stripsocket.h"

namespace Networking
{

StripSocket::StripSocket(QObject* parent)
    : AbstractUDPSocket(parent)
{
    QObject::connect(this, &StripSocket::received, this, &StripSocket::processResult, Qt::DirectConnection);
}

void StripSocket::start(const QString& address)
{
    this->connect(address);
    qInfo().noquote() << "[STRIPLFS] Started listening to SAR on" << address;
}

void StripSocket::stop()
{
    this->disconnect();
    qInfo() << "[STRIPLFS] Socket disconnected";
}

void StripSocket::processResult(QByteArray data)
{

}

} // Networking
