#ifndef RTGLOBAL_H
#define RTGLOBAL_H

#include <qglobal.h>

// RTLAB_VERSION is (major << 16) + (minor << 8) + patch.



#if defined(_MSC_VER) /* MSVC Compiler */
/* template-class specialization 'identifier' is already instantiated */
#pragma warning(disable: 4660)
#endif // _MSC_VER

#if defined(QDAQ_LIBRARY)
#  define QDAQ_EXPORT Q_DECL_EXPORT
#else
#  define QDAQ_EXPORT Q_DECL_IMPORT
#endif

namespace QDaq {

const char* Version();

const char* QtVersion();

}



#endif // RTGLOBAL_H
