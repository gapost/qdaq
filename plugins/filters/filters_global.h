#ifndef FILTERS_GLOBAL_H
#define FILTERS_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(FILTERS_LIBRARY)
#  define FILTERSSHARED_EXPORT Q_DECL_EXPORT
#else
#  define FILTERSSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // FILTERS_GLOBAL_H
