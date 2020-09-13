#ifndef FILTERSPLUGIN_H
#define FILTERSPLUGIN_H

#include <QScriptExtensionPlugin>


class QDaqFiltersPlugin : public QScriptExtensionPlugin
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QScriptExtensionInterface" FILE "filters-plugin.json")
#endif // QT_VERSION >= 0x050000

public:
    QDaqFiltersPlugin(QObject *parent = 0);

    QStringList keys() const;


    void initialize(const QString &key, QScriptEngine *e);
};

#endif // FILTERSPLUGIN_H
