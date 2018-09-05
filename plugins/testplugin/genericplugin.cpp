#include "genericplugin.h"


GenericPlugin::GenericPlugin(QObject *parent) :
    QGenericPlugin(parent)
{
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(testplugin, GenericPlugin)
#endif // QT_VERSION < 0x050000
