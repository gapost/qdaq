#ifndef RTGLOBAL_H
#define RTGLOBAL_H

#include <qglobal.h>

// RTLAB_VERSION is (major << 16) + (minor << 8) + patch.

#define QDAQ_VERSION       GIT_VERSION

#if defined(_MSC_VER) /* MSVC Compiler */
/* template-class specialization 'identifier' is already instantiated */
#pragma warning(disable: 4660)
#endif // _MSC_VER

//#define RTLAB_EXPORT  __declspec(dllexport)
//#define RTLAB_IMPORT  __declspec(dllexport)

//#if defined(RTLAB_BUILD_BASE)     // create a DLL library
//#define RTLAB_BASE_EXPORT  Q_DECL_EXPORT
//#else                        // use a DLL library
//#define RTLAB_BASE_EXPORT  Q_DECL_IMPORT
//#endif

//#if defined(RTLAB_BUILD_GUI)     // create a DLL library
//#define RTLAB_GUI_EXPORT  Q_DECL_EXPORT
//#else                        // use a DLL library
//#define RTLAB_GUI_EXPORT  Q_DECL_IMPORT
//#endif

#define RTLAB_BASE_EXPORT
#define RTLAB_GUI_EXPORT

#if defined(QDAQ_LIBRARY)
#  define QDAQ_EXPORT Q_DECL_EXPORT
#else
#  define QDAQ_EXPORT Q_DECL_IMPORT
#endif

namespace QDaq {

inline const char* Version() { return QDAQ_VERSION; }
inline const char* QtVersion() { return QT_VERSION_STR; }

}



#endif // RTGLOBAL_H
