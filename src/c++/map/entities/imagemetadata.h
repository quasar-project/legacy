#pragma once

#include <cstdint>

//! @brief Пространство имен для классов и функций, связанных с картографией.
namespace Map
{
    /*!
     *  @brief Заголовок метаданных телескопического РЛИ.
     *  @details Сериализована в Big Endian.
     */
    struct ImageMetaHeader
    {
        uint8_t JPEG_HEADER_SIZE = 20;          //!< Размер заголовка JPEG до начала метаданных.
        uint16_t meta_marker = 0xFFE1;          //!< Маркер начала метаданных. Big-endian.
        uint16_t meta_size;                     //!< Размер метаданных. Big-endian.
    };

    /*!
     *  @brief Тело метаданных телескопического РЛИ.
     *  @details Сериализована в Little Endian.
     */
    struct ImageMetadata
    {
        double latitude = 0;                    //!< Широта БПЛА в момент съемки в градусах.
        double longitude = 0;                   //!< Долгота БПЛА в момент съемки в градусах.
        float dx = 0;                           //!< Шаг по горизонтальной дальности в м/пиксель.
        float dy = 0;                           //!< Шаг по путевой дальности в м/пиксель.
        float x0 = 0;                           //!< Ближняя граница по горизонтальной дальности в м.
        float y0 = 0;                           //!< Смещение кадра по путевой дальности в м.
        float angle = 0;                        //!< Путевой азимут в градусах.
        float drift_angle = 0;                  //!< Угол сноса отн. путевого угла в градусах.
        float lx = 0;                           //!< Размер РЛИ по горизонтали в пикселях.
        float ly = 0;                           //!< Размер РЛИ по вертикали в пикселях.
        float div = 0;                          //!< Ширина диаграммы направленности антенны.
        float velocity = 0;                     //!< Скорость БПЛА в момент съемки в м/с.
        float altitude = 0;                     //!< Высота БПЛА отн. уровня моря в момент съемки в м.
        float kr = 0;                           //!< Коэффициент частотной интерполяции.
        float time_shift = 0;                   //!< Время сдвига в с.
        float time_duration = 0;                //!< Время синтезирования в с.
        float reserved_1 = 0;                   //!< Зарезервированно.
        float reserved_2 = 0;                   //!< Зарезервированно.
        uint8_t mode = 0;                       //!< Режим съемки.
        uint8_t image_type = 0;                 //!< Тип РЛИ (0 - телескопическое).
        uint32_t reserved_3 = 0;                //!< Зарезервированно.
        uint32_t reserved_4 = 0;                //!< Зарезервированно.
        uint16_t crc16 = 0;                     //!< Контрольная сумма CRC16.
    };

    //! @brief Заголовок пакета полосового РЛИ.
    struct StripHeaderMetadata
    {
        uint16_t marker = 0;                    //!< Идентификатор протокола. Равен 0xDEFA.
        uint16_t version = 0;                   //!< Версия протокола. По маске 0xF000 - мажорная, по 0x0FFF - минорная.
        uint16_t size = 0;                      //!< Размер пакета в байтах, включая заголовок.
        uint16_t cnt = 0;                       //!< Счётчик.
        uint16_t id = 0;                        //!< Идентификатор РЛС. 0x55 - РЛС "Квазар".
        uint16_t type = 0;                      //!< Идентификатор типа сообщения. 0x1 - полосовое РЛИ. 0x2 - таблица обнаружений.
    } __attribute__((packed));

    //! @brief Навигационные данные пакета полосового РЛИ.
    struct StripNavigationMetadata
    {
        float pitch = 0;                        //!< Тангаж в градусах.
        float roll = 0;                         //!< Крен в градусах.
        float elevation = 0;                    //!< Высота БПЛА в метрах.
        double latitude = 0;                    //!< Широта в градусах в проекции WGS-84.
        double longitude = 0;                   //!< Долгота в градусах в проекции WGS-84.
        float velocity = 0;                     //!< Скорость БПЛА в м/с.
        float course = 0;                       //!< Курс БПЛА в градусах.
        float track_ang = 0;                    //!< Сумма курса БПЛА и угла сноса (в градусах).
    } __attribute__((packed));

    //! @brief Данные форматирования пакета полосового РЛИ.
    struct StripFormatMetadata
    {
        float dx = 0;                           //!< Дискрета по дальности в метрах.
        float dy = 0;                           //!< Дискрета по азимуту в метрах.
        float course = 0;                       //!< Угол азимута в градусах.
        float roll = 0;                         //!< Угол наклона в градусах.
        float x0 = 0;                           //!< Дальность до первого отсчёта в метрах.
        uint8_t word_size = 0;                  //!< Количество байт в элементе разрешения.
        uint8_t polarization = 0;               //!< Поляризация: 0 - н/о, 1 - Н/Н, 2 - H/V, 3 - V/H, 4 - V/V, 5 - HV/H, 6 - HV/V.
        uint16_t y = 0;                         //!< Смещение по Y в метрах.
        uint16_t nx = 0;                        //!< Общее количество дискрет по дальности.
        uint16_t ny = 0;                        //!< Общее количество дискрет по азимуту.
        float k = 0;                            //!< Коэффициент дискреты.
    } __attribute__((packed));
} // namespace Map;
