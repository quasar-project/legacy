#ifndef TILESMANAGER_H
#define TILESMANAGER_H

#include <QObject>
#include <QCoreApplication>
#include <QFile>
#include <QSaveFile>
#include <QDir>
#include <QTextStream>

#include "disk/cachemanager.h"

class TilesManager : public QObject
{
    Q_OBJECT
public:
    static TilesManager* get();
    static void InitializeConfig();

private:
    explicit TilesManager();
    static TilesManager* _instance;
    static QString _street;
    static QString _satellite;
    static QString _terrain;
    static QString _transit;
    static QString _cycle;
    static QString _hiking;
    static QString _nighttransit;

signals:

};

#endif // TILESMANAGER_H
