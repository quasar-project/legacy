#pragma once

#include <QtCore/QObject>
#include <QtCore/QHash>
#include <LPVL/Global>
#include <vector>

#define THEME GUI::Theme::get()

namespace GUI
{
    class ThemePrivate;
    class Theme : public QObject
    {
        Q_OBJECT
        LPVL_DECLARE_SINGLETON(Theme)
        Q_DECLARE_PRIVATE(Theme)

        public:
            virtual ~Theme() = default;

            Q_INVOKABLE QString color(QString key);
            Q_INVOKABLE QString colorText(const QString& text, const QString& theme_color_name);

            signals:
                void colorsChanged();

        protected:
            ThemePrivate* const d_ptr;

        private:
            explicit Theme(QObject* parent = nullptr);
    };
} // namespace GUI;