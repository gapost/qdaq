#ifndef PID_GLOBAL_H
#define PID_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(PID_LIBRARY)
#  define PIDSHARED_EXPORT Q_DECL_EXPORT
#else
#  define PIDSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // PID_GLOBAL_H
