#pragma once

#include <QtCore/QObject>
#include "definitions.h++"
#include "configdef.h++"


namespace Config
{
    class Config : public QObject
    {
        Q_OBJECT
        Q_PROPERTY(network_t network READ network WRITE setNetwork NOTIFY networkChanged)
        Q_PROPERTY(map_t map READ map WRITE setMap NOTIFY mapChanged)
        Q_PROPERTY(image_t image READ image WRITE setImage NOTIFY imageChanged)

        network_t m_network;
        map_t m_map;
        image_t m_image;

        public:
            explicit Config(QObject *parent = nullptr);
            virtual ~Config() = default;

            network_t network() const;
            void setNetwork(const network_t& other);
            map_t map() const;
            void setMap(const map_t& other);
            image_t image() const;
            void setImage(const image_t& other);

            signals:
                __signal networkChanged();
                __signal mapChanged();
                __signal imageChanged();
    };
} // namespace Config;
