#ifndef QDAQINTERFACESPLUGIN_H
#define QDAQINTERFACESPLUGIN_H

#include <QScriptExtensionPlugin>


class QDaqInterfacesPlugin : public QScriptExtensionPlugin
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QScriptExtensionInterface" FILE "interfaces-plugin.json")
#endif // QT_VERSION >= 0x050000

public:
    QDaqInterfacesPlugin(QObject *parent = 0);

    QStringList keys() const;


    void initialize(const QString &key, QScriptEngine *e);
};

#endif // QDAQINTERFACESPLUGIN_H
