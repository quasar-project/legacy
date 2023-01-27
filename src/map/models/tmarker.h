#ifndef TMARKER_H
#define TMARKER_H

#include <QObject>
#include <QColor>

enum MarkerIcon : short int {
    GoogleDefaultMarker,
    Flag,
    SARImage,
    AutocaptureMark
};

class TMarker : public QObject
{
    Q_OBJECT
public:
    explicit TMarker(QObject *parent = nullptr);
    QColor color = QColor("#dae1e5");
    QString name = "Новый маркер";
    short int icon = MarkerIcon::GoogleDefaultMarker;
    qreal latitude = 0;
    qreal longitude = 0;
    bool autocapture = false;
    bool scalable = false;
    bool save = true;
    
    qreal zoomLevel = 0;
    qreal anchorX = 0;
    qreal anchorY = 0;
    QString iconPath = "qrc:/map-resources/markers/default.png";

    void update();

signals:

};

#endif // TMARKER_H