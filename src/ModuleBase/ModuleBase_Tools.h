// File:        ModuleBase_Tools.h
// Created:     11 July 2014
// Author:      Vitaly Smetannikov

#ifndef ModuleBase_Tools_H
#define ModuleBase_Tools_H

#include "ModuleBase.h"

#include <QPixmap>

class QWidget;
class QLayout;

namespace ModuleBase_Tools {

/*
 * Methods to adjust margins and spacings.
 */
MODULEBASE_EXPORT void adjustMargins(QWidget* theWidget);
MODULEBASE_EXPORT void adjustMargins(QLayout* theLayout);

MODULEBASE_EXPORT void zeroMargins(QWidget* theWidget);
MODULEBASE_EXPORT void zeroMargins(QLayout* theLayout);


/**
 * Methods to modify a resource pixmap
 */
//! Create composite pixmap. 
//! Pixmap \a theAdditionalIcon is drawn over pixmap \a dest with coordinates
//! specified relatively to the upper left corner of \a theIcon.

//! \param theAdditionalIcon resource text of the additional pixmap
//! \param theIcon resource text of the background pixmap
//! \return resulting pixmap
MODULEBASE_EXPORT QPixmap composite(const QString& theAdditionalIcon, const QString& theIcon);

//! Generates the pixmap lighter than the resources pixmap. 
//! Pixmap \a theIcon is lighted according to the given value.
//! If the lighter value is greater than 100, this functions returns a lighter pixmap.
//! Setting lighter value to 150 returns a color that is 50% brighter. If the factor is less than 100,
//! the return pixmap is darker. If the factor is 0 or negative, the return pixmap is unspecified.

//! \param resource text of the pixmap
//! \param theLighterValue a lighter factor
//! \return resulting pixmap
MODULEBASE_EXPORT QPixmap lighter(const QString& theIcon, const int theLighterValue = 200);
}

#endif
