#ifndef QDAQ_WIDGETS_GLOBAL_H
#define QDAQ_WIDGETS_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(QDAQ_WIDGETS_LIBRARY)
#  define QDAQ_WIDGETS_EXPORT Q_DECL_EXPORT
#else
#  define QDAQ_WIDGETS_EXPORT Q_DECL_IMPORT
#endif

#endif // SRC_GLOBAL_H
