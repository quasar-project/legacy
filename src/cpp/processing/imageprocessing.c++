#include "imageprocessing.h"
#include "map/imagemodel.h"
#include "map/entities/stripimage.h"
#include "config/paths.h"
#include "config/config.h"

#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QtEndian>
#include <QtGui/QImage>
#include <QtGui/QPainter>
#include <QtGui/QPainterPath>
#include <QtConcurrent/QtConcurrent>
#include <cmath>
#include <ccl/ccl_core.h>

using namespace Processing;

ImageProcessing::ImageProcessing(QObject* parent)
    : QObject{parent}
    , m_model(new Map::ImageModel(this))
{
}

void ImageProcessing::asyncProcess(const QString& filename)
{
    Map::Image image;

    image.filename = filename;
    image.path.first = Config::Paths::imageCache() + "/lod0/" + filename;
    QByteArray data = fileToByteArray(image.path.first);

    char* data_ptr = data.data();
    uint16_t* marker = reinterpret_cast<uint16_t*>(data_ptr + image.header.JPEG_HEADER_SIZE);
    if(*marker != image.header.meta_marker and *marker != qFromBigEndian<uint16_t>(image.header.meta_marker))
    {
        qCritical().noquote().nospace() << "[PROCESSING] Marker mismatch: expected 0x"
                                        << Qt::hex << image.header.meta_marker << " or 0x"
                                        << qFromBigEndian<uint16_t>(image.header.meta_marker)
                                        << ", received 0x" << *marker << Qt::dec;
        return;
    }

    // заполнение структуры метаданными
    uint16_t *meta_size = reinterpret_cast<uint16_t *>(data_ptr + image.header.JPEG_HEADER_SIZE
                                                       + sizeof(uint16_t));
    *meta_size = qToBigEndian(*meta_size) - sizeof(uint16_t);
    memcpy(&image.meta, (data_ptr + image.header.JPEG_HEADER_SIZE + sizeof(uint32_t)), *meta_size);

    // проверка контрольной суммы
    char *crc_data = (char *) &image.meta;
    uint16_t crc16 = ccl::crc16(crc_data, sizeof(Map::ImageMetadata) - sizeof(uint16_t));

    image.valid = crc16 == image.meta.crc16;
    if(not image.valid)
        qWarning().noquote().nospace() << "[PROCESSING] Image CRC16 seems to be incorrect: expected 0x"
                                       << Qt::hex << crc16 << ", received 0x" << image.meta.crc16 << Qt::dec;

    // геометрические преобразования
    if(CONFIG(useRadians))
    {
        image.meta.angle = ccl::rad2deg(image.meta.angle)
                           + CONFIG(angleCorrection);
        image.meta.drift_angle = CONFIG(useDriftAngle)
                                     ? ccl::rad2deg(image.meta.drift_angle)
                                     : 0;
        image.meta.div = ccl::rad2deg(image.meta.div);
    }
    else
        image.meta.angle += CONFIG(angleCorrection);

    QImage image_data(image.path.first);

    if(CONFIG(overrideImageHeight))
        image.meta.ly = image_data.height();

    QString target_filename = filename;
    target_filename.chop(3);
    target_filename += "png";

    if(not CONFIG(cutImage))
    {
        bool ret = image_data.save(Config::Paths::imageCache() + "/lod0/" + target_filename);
        if(ret)
            qInfo() << "[PROCESSING] Image saved successfully without correction";
        else
            qCritical() << "[PROCESSING] Failed to save image";
    }
    else
    {
        QImage sample(image_data.width(), image_data.height(), QImage::Format_ARGB32_Premultiplied);
        sample.fill(QColor(254, 254, 254));
        image_data.setAlphaChannel(sample);

        QPainter painter;
        QPainterPath path;
        QBrush brush(Qt::transparent, Qt::SolidPattern);
        QPolygon p1, p2;

        brush.setStyle(Qt::SolidPattern);
        brush.setColor(Qt::transparent);
        painter.begin(&image_data);
        painter.setCompositionMode(QPainter::CompositionMode_Source);
        painter.setPen(QPen(Qt::transparent, 10));
        painter.setRenderHint(QPainter::Antialiasing);

        const int top[8] = {
            0, (int)((image.meta.ly / 2) - 2
               * image.meta.x0
               * tan(ccl::deg2rad((image.meta.div - CONFIG(thetaAzimuthCorrection))
               / 2))),
            0, 0,
            (int)image.meta.lx, 0,
            (int)image.meta.lx, (int)((image.meta.ly / 2) -
                                (2 * image.meta.x0 + image.meta.lx)
                                * tan(ccl::deg2rad((image.meta.div - CONFIG(thetaAzimuthCorrection))
                                / 2)))
        };

        const int bottom[8] = {
            0, (int)((image.meta.ly / 2) + 2
               * image.meta.x0
               * tan(ccl::deg2rad((image.meta.div - CONFIG(thetaAzimuthCorrection))
               / 2))),
            0, (int)image.meta.ly,
            (int)image.meta.lx, (int)image.meta.ly,
            (int)image.meta.lx, (int)((image.meta.ly / 2)
                                + (2 * image.meta.x0 + image.meta.lx)
                                * tan(ccl::deg2rad((image.meta.div - CONFIG(thetaAzimuthCorrection))
                                / 2)))
        };

        p1.setPoints(4, top);
        p2.setPoints(4, bottom);
        painter.drawPolygon(p1);
        painter.drawPolygon(p2);
        path.addPolygon(p1);
        path.addPolygon(p2);

        painter.fillPath(path, brush);
        painter.end();

        bool ret = image_data.save(Config::Paths::imageCache() + "/lod0/" + target_filename);
        if(ret)
            qInfo() << "[PROCESSING] Image saved successfully with correction";
        else
            qCritical() << "[PROCESSING] Failed to save image";
    }

    image.path.first = Config::Paths::imageCache() + "/lod0/" + target_filename;
    QFile::remove(Config::Paths::imageCache() + "/lod0/" + filename);

    image.opacity = INITIAL_OPACITY;
    image.shown = INITIAL_VISIBILITY;
    image.mercator_zoom_level = ccl::mqi_zoom_level(image.meta.latitude, image.meta.dx);

    model()->add(image);
}

void ImageProcessing::asyncStripProcess(const QString& filename)
{
    Map::StripImage image;

    image.filename = filename;
    image.path.first = Config::Paths::imageCache() + "/lod0/" + filename;
    QByteArray data = fileToByteArray(image.path.first);

    char* data_ptr = data.data();
    memcpy(&image.header, data_ptr, sizeof(Map::StripHeaderMetadata));
    qDebug() << "Image header:" << image.header.marker << image.header.version << image.header.size << image.header.cnt
             << image.header.id << image.header.type;
}

QByteArray ImageProcessing::fileToByteArray(const QString& path)
{
    QFile file(path);
    if(not file.open(QIODevice::ReadOnly))
    {
        qCritical().noquote() << "[PROCESSING] Error opening file |" << path << "| for converting to raw bytes data";
        return QByteArray();
    }

    return file.readAll();
}

Map::ImageModel* ImageProcessing::model()
{
    return this->m_model;
}

void ImageProcessing::processImage(const QString& filename)
{
    qDebug() << "[PROCESSING] Received image to process" << filename;

    this->asyncProcess(filename);

    //! @todo busy
}

void ImageProcessing::processStripImage(const QString& filename)
{
    qDebug() << "[PROCESSING] Received binary to process" << filename;

    this->asyncStripProcess(filename);

    //! @todo busy
}

bool ImageProcessing::exists(const QString& name)
{
    for(Map::Image image : *model()->direct())
    {
        if(image.filename == name)
            return true;

        if(image.filename.chopped(4) == name.chopped(4))
            return true;
    }
    return false;
}

bool ImageProcessing::busy() const { return m_busy; }
void ImageProcessing::setBusy(bool other) {
    if (m_busy == other)
        return;
    m_busy = other;
    emit busyChanged();
}
