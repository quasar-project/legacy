#include "tcpdownloader.h"

TCPDownloader::TCPDownloader(QObject *parent) : QObject(parent)
{
    server = new QTcpServer(this);
    connect(server, SIGNAL(newConnection()), this, SLOT(clientConnected()));
    if(!server->listen(QHostAddress(SConfig::getHashString("LoaderIP")), SConfig::getHashString("LoaderPort").toUInt()))
    {
        Debug::Log("!![SERVER] TCP-IP server has failed to start.");
    } else {
        Debug::Log("?[SERVER] TCP-IP server started.");
    }
    timer = new QTimer();
    timer->setInterval(TCP_TIMEOUT);
    connect(timer, &QTimer::timeout, this, &TCPDownloader::connectionTimeout);
	//socket = new QTcpSocket(this);
}

void TCPDownloader::clientConnected(void)
{
	Debug::Log("?[SERVER] Trying to connect to SAR...");
	socket = server->nextPendingConnection();
	if(!socket)
	{
		Debug::Log("!![SERVER] NextPendingConnection returned nullptr. Aborting connection.");
		return;
	}
    connect(socket, &QTcpSocket::readyRead, this, &TCPDownloader::serverRead);
    connect(socket, &QTcpSocket::disconnected, this, &TCPDownloader::clientDisconnected);
    imageData64.clear();
    imageData.clear();
    readFile = &TCPDownloader::readFileInfo;
    success = false;
    timer->start();
    splitIndex = 0;
    Debug::Log("?[SERVER] SAR connected and ready to send image");
}

void TCPDownloader::clientDisconnected(void)
{
    socket->close();
    timer->stop();
    (fileSize == imageData.size()) ? Debug::Log("?[SERVER] Image fully received from SAR") : Debug::Log("![SERVER] Something went wrong in receiving SAR image");
    ImageManager::newImage(CacheManager::getTcpDowloaderCache() + "/" + filename, imageData); 
    emit receivingFinished();
}

void TCPDownloader::serverRead(void)
{
    (this->*readFile) (socket->readAll());
}

void TCPDownloader::readFileInfo(QByteArray data)
{
    readFile = &TCPDownloader::readFileBody;

    filename = QString(data);
    uint8_t i = data.indexOf('\0') + 1;

    memcpy(&fileSize, data.mid(i, sizeof(uint32_t)).data(), sizeof(uint32_t));

    i+=sizeof(uint32_t);

    Debug::Log("[SERVER] Name:" + filename);
    Debug::Log("[SERVER] Filesize:" + QString::number(fileSize / (1024)) + " kB");

    data.remove(0, i);

    timer->stop();
    timer->start();
    (this->*readFile)(data);
}

void TCPDownloader::readFileBody(QByteArray data)
{
    timer->stop();
    timer->start();
    if(data.size()){
        imageData.append(data);
        imageData64.append(data.toBase64());
    }
    progress();
}

float TCPDownloader::progress(){
    float f = (float)imageData.size() / fileSize;
    emit progressChanged(f*100);
    return f;
}

void TCPDownloader::connectionTimeout(void){
    socket->close();
}
