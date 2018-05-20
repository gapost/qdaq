#ifndef FOPDT_GLOBAL_H
#define FOPDT_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(FOPDT_LIBRARY)
#  define FOPDTSHARED_EXPORT Q_DECL_EXPORT
#else
#  define FOPDTSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // FOPDT_GLOBAL_H
