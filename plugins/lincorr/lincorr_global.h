#ifndef LINCORR_GLOBAL_H
#define LINCORR_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(LINCORR_LIBRARY)
#  define LINCORRSHARED_EXPORT Q_DECL_EXPORT
#else
#  define LINCORRSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // LINCORR_GLOBAL_H
