#pragma once

#include <QtCore/QObject>
#include <QtNetwork/QUdpSocket>

class BoardStatus : public QObject
{
  Q_OBJECT

  constexpr uint16_t SEND_PORT = 44000;
  constexpr uint16_t RECEIVE_PORT = 12000;

  // коммутатор питания
  // 1-7 каналов
  // 6-7 - 5В
  // 0-40В
  // 1 банка: 3.6 красный, 4.0 зеленый

  public:
    explicit BoardStatus(QObject* parent = nullptr);

};