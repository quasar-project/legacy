#ifndef IMAGEMANAGER_H
#define IMAGEMANAGER_H

#include <QObject>

#include "models/timage.h"
#include "imageprocess.h"
#include "disktools.h"

class ImageManager : public QObject
{
    Q_OBJECT
public:
    static ImageManager* initialize(QObject *parent = nullptr);
    static void newImage(QString filename, QByteArray rawData);

signals:

private:
    explicit ImageManager(QObject *parent = nullptr);
    static ImageManager* _instance;
};

#endif // IMAGEMANAGER_H
