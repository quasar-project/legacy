#include "colortheme.h"
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include "config/settings.h"
#include "config/paths.h"
#include "config/internalconfig.h"

namespace GUI
{
  internal::ColorThemeWrapper::ColorThemeWrapper(QObject* parent)
    : QObject(parent)
  {}

  QString internal::ColorThemeWrapper::color(internal::ColorThemeWrapper::Color key) const noexcept
  {
    if(m_activeTheme.empty())
      return "red";
    if(not m_activeTheme.count(key))
      return "purple";
    return m_activeTheme.at(key);
  }

  void internal::ColorThemeWrapper::set(const std::map<Color, QString>& theme_dict) noexcept
  {
    m_activeTheme = theme_dict;
  }

  ColorTheme* ColorTheme::get() { static ColorTheme instance; return &instance; }

  ColorTheme::ColorTheme(QObject* parent)
    : QObject(parent)
    , m_wrapper(new internal::ColorThemeWrapper(this))
  {
    this->scanThemes();
    this->apply();

    connect(Config::Settings::get(), &Config::Settings::themeChanged, this, &ColorTheme::apply);
  }

  void ColorTheme::set(const std::map<internal::ColorThemeWrapper::Color, QString>& dict) noexcept
  {
    m_wrapper->set(dict);
    emit activeChanged();
  }

  void ColorTheme::scanThemes() noexcept
  {
    QDir folder(Config::Paths::themes(), "*.json", QDir::Name | QDir::IgnoreCase, QDir::Files);
    QList<QFileInfo> files = folder.entryInfoList();

    m_themeList.clear();
    for(const auto& file : files)
    {
      QFile json_file(file.absoluteFilePath());
      if(not json_file.open(QIODevice::ReadOnly))
      {
        qWarning() << "[COLORSCHEME] Error opening" << json_file.fileName();
        continue;
      }

      QJsonDocument json = QJsonDocument::fromJson(json_file.readAll());
      if(not json.isObject())
      {
        qWarning() << "[COLORSCHEME] Parsing error at JSON file:" << json_file.fileName();
        continue;
      }

      QJsonObject object = json.object();
      if(not object.contains("theme_name"))
      {
        qWarning() << "[COLORSCHEME] Theme file does not contain theme name:" << json_file.fileName();
        continue;
      }

      if(not ICFG<bool>("THEMES_ALLOW_BETA_THEMES") and object.value("theme_name").toString().contains("beta"))
        continue;

      m_themeList.push_back(object.value("theme_name").toString());
      m_files.insert({object.value("theme_name").toString(), file.absoluteFilePath()});
    }

    emit themeListChanged();
    qInfo() << "[COLORSCHEME] Total themes found:" << m_themeList.size();
  }

  void ColorTheme::apply() noexcept
  {
    if(m_themeList.empty())
    {
      qCritical() << "[COLORSCHEME] Themes not found. Reinstall application";
      return;
    }

    if(not m_themeList.contains(Config::Settings::get()->value<QString>("application/theme")))
    {
      qWarning() << "[COLORSCHEME] Selected theme isn't present in total found list";
      this->setActiveThemeName(m_themeList.first());
    }

    this->setActiveThemeName(Config::Settings::get()->value<QString>("application/theme"));

    if(not m_files.count(activeThemeName()))
      setActiveThemeName(m_files.begin()->first);
    QFile file(m_files.at(activeThemeName()));
    if(not file.open(QIODevice::ReadOnly))
    {
      qCritical() << "[COLORSCHEME] Something went wrong in applying theme";
      return;
    }

    QJsonDocument json = QJsonDocument::fromJson(file.readAll());
    QJsonObject object = json.object();
    auto keys = object.keys();
    std::map<internal::ColorThemeWrapper::Color, QString> result;
    for(const auto& key : keys)
      if(m_wrapper->keyMap.count(key))
        result.insert({m_wrapper->keyMap.at(key), object.value(key).toString()});

    this->set(result);
  }

  QString ColorTheme::activeThemeName() const { return m_activeThemeName; }
  void ColorTheme::setActiveThemeName(const QString& other)
  {
    if(other == m_activeThemeName)
      return;
    m_activeThemeName = other;
    emit activeThemeNameChanged();
  }
} // GUI