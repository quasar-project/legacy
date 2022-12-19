#include "imagemanager.h"

ImageManager *ImageManager::_instance = nullptr;
QVector<TImage *> ImageManager::imageList = {};
ImageManager::ImageManager(QObject *parent)
    : QObject{parent}
{
}

ImageManager *ImageManager::initialize(QObject *parent)
{
    if (_instance != NULL)
        return _instance;
    _instance = new ImageManager(parent);
    return _instance;
}

// returns true if occurence is found within an image vector
bool ImageManager::checkForOccurence(QString filename)
{
    for (TImage *i : imageList)
        if (i->cachedJPEGfilename == filename)
            return true;
    return false;
}

void ImageManager::newImage(QString filenamePath, QByteArray data)
{
    QStringList _cutFilename;
    _cutFilename = filenamePath.split("/");
    QString filename = _cutFilename.last();

    TImage *image = new TImage(initialize());
    image->index = imageList.length();
    image->cachedJPEGfilename = filenamePath;

    // save thread from disktools

    ImageProcess imageProcess;
    if (!imageProcess.decode(data, *image))
    {
        delete image;
        Debug::Log("!![IMGMANAGER] Image discarded due to decoding error");
        return;
    }
    imageProcess.assignUIStrings(*image, filename);
    image->image = imageProcess.dataToImage(data, ImageMode::GeometricAlphaMask, *image);
    image->base64 = imageProcess.imageToBase64(image->image);

    // append to vector when all functions which changing image is called
    imageList.append(image);
    LinkerQML::addModel(*image);
    // any logging only after it
}

bool ImageManager::removeImage(qint32 index)
{
    QMessageBox askForClearTrack;
    askForClearTrack.setWindowTitle("Удаление РЛИ");
    askForClearTrack.setIcon(QMessageBox::Information);
    askForClearTrack.setText("Вы уверены, что хотите удалить выбранное изображение с карты?");
    askForClearTrack.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    askForClearTrack.setDefaultButton(QMessageBox::Yes); // maybe cancel(r)
    int ret = askForClearTrack.exec();                   // не ставить шорт, иначе будет выход за границы буфера (енумы qt имеют неадекватные значения)
    switch (ret)
    {
    case QMessageBox::Yes:
        imageList.remove(index);
        Debug::Log("[IMGMANAGER] Image " + QString::number(index) + " removed from map. List now contains = " + QString::number(imageList.length()));
        return true;
        break;
    case QMessageBox::Cancel:
        return false;
        break;
    default:
        break;
    }
}