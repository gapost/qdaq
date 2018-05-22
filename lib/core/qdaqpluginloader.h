#ifndef QDAQPLUGINLOADER_H
#define QDAQPLUGINLOADER_H

#include <QStringList>
#include <QDir>
#include <QPluginLoader>
#include <QLibraryInfo>
#include <QApplication>

template<typename PluginInterfacePtr>
struct QDaqPluginLoader {

public:

    static QStringList findPlugins()
    {
        QStringList S;
        int idx = 0;

        // first look at app path
        QDir pluginsDir = QDir(qApp->applicationDirPath());
#if defined(Q_OS_WIN)
        if (pluginsDir.dirName().toLower() == "debug" || pluginsDir.dirName().toLower() == "release")
            pluginsDir.cdUp();
#elif defined(Q_OS_MAC)
        if (pluginsDir.dirName() == "MacOS") {
            pluginsDir.cdUp();
            pluginsDir.cdUp();
            pluginsDir.cdUp();
        }
#endif
        pluginsDir.cd("plugins");

        foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
            idx++;
            //S += QString("  %1. %2: ").arg(idx).arg(fileName);
            QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
            QObject *plugin = loader.instance();
            if (plugin) {
                PluginInterfacePtr ifc = qobject_cast<PluginInterfacePtr>(plugin);
                if (ifc) S.push_back(fileName);
            }
        }

        // now look at Qt plugin folder
        pluginsDir = QDir(QLibraryInfo::location(QLibraryInfo::PluginsPath));
        pluginsDir.cd("qdaq");

        foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
            idx++;
            //S += QString("  %1. %2: ").arg(idx).arg(fileName);
            QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
            QObject *plugin = loader.instance();
            if (plugin) {
                PluginInterfacePtr ifc = qobject_cast<PluginInterfacePtr>(plugin);
                if (ifc) S.push_back(fileName);
            }
        }

        return S;
    }

    static QObject* loadPlugin(const QString& pluginName)
    {
        // find a plugin with this name

        // first look at app path
        QDir pluginsDir = QDir(qApp->applicationDirPath());
#if defined(Q_OS_WIN)
        if (pluginsDir.dirName().toLower() == "debug" || pluginsDir.dirName().toLower() == "release")
            pluginsDir.cdUp();
#elif defined(Q_OS_MAC)
        if (pluginsDir.dirName() == "MacOS") {
            pluginsDir.cdUp();
            pluginsDir.cdUp();
            pluginsDir.cdUp();
        }
#endif
        pluginsDir.cd("plugins");

        foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
            if (fileName==pluginName) {
                QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
                QObject *plugin = loader.instance();
                if (plugin && qobject_cast<PluginInterfacePtr>(plugin))
                    return plugin;
                }
        }

        // now look at Qt plugin folder
        pluginsDir = QDir(QLibraryInfo::location(QLibraryInfo::PluginsPath));
        pluginsDir.cd("qdaq");

        foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
            if (fileName==pluginName) {
                QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
                QObject *plugin = loader.instance();
                if (plugin && qobject_cast<PluginInterfacePtr>(plugin))
                    return plugin;
                }
        }

        return 0;
    }

};

#endif // QDAQPLUGINLOADER_H
