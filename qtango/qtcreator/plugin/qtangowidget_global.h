#pragma once

#include <QtGlobal>

#if defined(QTANGOWIDGET_LIBRARY)
#  define QTANGOWIDGETSHARED_EXPORT Q_DECL_EXPORT
#else
#  define QTANGOWIDGETSHARED_EXPORT Q_DECL_IMPORT
#endif
