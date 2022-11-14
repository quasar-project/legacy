#include "config.h"

Config::Config(QString fn) : QSettings(fn, QSettings::IniFormat)
{
         /*
         * Заполнять стандартные параметры рекомендуется так:
         * group_t <Название группы>;
         * <Название группы>["<Название параметра>"] = "<Значение>";
         * checkValuesSimple(<Название группы>);
         */

    group_t utility;
    utility["version"] = "1111";
    checkValuesSimple(utility);

    group_t network;
    network["type"] = "UDP";
    network["ip"] = "127.0.0.1";
    network["port"] = "25565";
    network["updateTime"] = "0.5";
    checkValuesSimple(network);

    group_t map;
    map["predict_line_range"] = "4";
    map["capture_time"] = "15";
    map["diagram_length"] = "3";
    map["diagram_theta_azimuth"] = "12.5";
    map["diagram_drift_angle"] = "11";
    map["antenna_position"] = "r";
    map["vehicle_type"] = "helicopter";
    checkValuesSimple(map);
}

void Config::checkValues(QString group, group_t *list){

    QMapIterator<QString, QString> i(*list);

    beginGroup(group);
    while (i.hasNext()) {
        i.next();
        if(!value(i.key()).isValid()){
            setValue(i.key(), i.value());
        }
    }
    endGroup();
    list->clear();
}
