#include "disktools.h"

DiskTools *DiskTools::_instance = nullptr;
DiskTools::DiskTools(QObject *parent)
    : QObject{parent}
{
}
DiskTools *DiskTools::initialize(QObject *parent)
{
    if (_instance != nullptr)
        return _instance;
    _instance = new DiskTools(parent);
    return _instance;
}

void DiskTools::fetchDirectory()
{
    bool mode = SConfig::getHashBoolean("Mode");
    QStringList initialFileList;
    if (mode == false)
    {
        QDir initialDirectory(SConfig::getHashString("ViewPath"));
        initialDirectory.setFilter(QDir::Files | QDir::NoSymLinks | QDir::NoDot | QDir::NoDotDot);
        initialDirectory.setNameFilters(QStringList("*.jpg"));
        for (QString entryString : initialDirectory.entryList())
        {
            initialFileList.append(entryString.prepend(SConfig::getHashString("ViewPath") + "/"));
        }
    } else {
        QDir initialDirectory(SConfig::getHashString("FlightPath"));

        initialDirectory.setFilter(QDir::Files | QDir::NoSymLinks | QDir::NoDot | QDir::NoDotDot);
        initialDirectory.setNameFilters(QStringList("*.jpg"));
        for (QString entryString : initialDirectory.entryList())
        {
            initialFileList.append(entryString.prepend(SConfig::getHashString("FlightPath") + "/"));
        }
    }
    if (!initialFileList.empty())
    {
        for (QString filename : initialFileList)
        {
            if(!ImageManager::checkForOccurence(filename))
            {
                ImageManager::newImage(filename, convertToRawData(filename));
            } else 
            {
                Debug::Log("?[DISK] Occurence found, skipping...");
            }
        }
    } else {
        Debug::Log("![DISK] Directory is empty, throwing warning window...");
        QMessageBox warningDialogue;
        warningDialogue.setWindowTitle("Изображения не найдены!");
        warningDialogue.setIcon(QMessageBox::Warning);
        warningDialogue.setText("В выбранном каталоге или кэше не найдены изображения!");
        warningDialogue.exec();
    }
}

void DiskTools::clearCache(void)
{
    CacheManager::clearImageCache();
}

QByteArray DiskTools::convertToRawData(QString path)
{
    QFile imageFile(path);
    if(imageFile.open(QIODevice::ReadOnly))
    {
        QByteArray rawData = imageFile.readAll();
        return rawData;
    } else {
        Debug::Log("!!Error opening the image file for converting to raw bytes data");
        return QByteArray();
    }
}
