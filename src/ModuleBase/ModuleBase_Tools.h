// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_Tools.h
// Created:     11 July 2014
// Author:      Vitaly Smetannikov

#ifndef ModuleBase_Tools_H
#define ModuleBase_Tools_H

#include "ModuleBase.h"
#include "ModuleBase_Definitions.h"

#include <ModelAPI_Feature.h>
#include <TopAbs_ShapeEnum.hxx>

#include <QPixmap>

class QWidget;
class QLayout;
class QDoubleSpinBox;
class ModuleBase_ParamSpinBox;

namespace ModuleBase_Tools {

/*
 * Methods to adjust margins and spacings.
 */
MODULEBASE_EXPORT void adjustMargins(QWidget* theWidget);
MODULEBASE_EXPORT void adjustMargins(QLayout* theLayout);

MODULEBASE_EXPORT void zeroMargins(QWidget* theWidget);
MODULEBASE_EXPORT void zeroMargins(QLayout* theLayout);


/**
 * \ingroup GUI
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

/// Sets programmatically the value to the spin box without emitting any signals(e.g. valueChanged)
/// \param theSpin an X or Y coordinate widget
/// \param theValue a new value
MODULEBASE_EXPORT void setSpinValue(QDoubleSpinBox* theSpin, double theValue);

/// Sets programmatically the value to the spin box without emitting any signals(e.g. valueChanged)
/// \param theSpin an X or Y coordinate widget
/// \param theValue a new value
MODULEBASE_EXPORT void setSpinValue(ModuleBase_ParamSpinBox* theSpin, double theValue);

/// Sets programmatically the value to the spin box without emitting any signals(e.g. valueChanged)
/// \param theSpin an ModuleBase_ParamSpinBox that accepts text
/// \param theText a new value
MODULEBASE_EXPORT void setSpinText(ModuleBase_ParamSpinBox* theSpin, const QString& theText);

/// Converts the object to the feature or a result and generate information string
/// \param theObj an object
/// \param isUseAttributesInfo a flag whether the attribute values information is used
/// \return a string
MODULEBASE_EXPORT QString objectInfo(const ObjectPtr& theObj, const bool isUseAttributesInfo = false);

/// Converts string value (name of shape type) to shape enum value
/// \param theType - string with shape type name
/// \return TopAbs_ShapeEnum value
MODULEBASE_EXPORT TopAbs_ShapeEnum shapeType(const QString& theType);

/*!
Check types of objects which are in the given list
\param theObjects the list of objects
\param hasResult will be set to true if list contains Result objects
\param hasFeature will be set to true if list contains Feature objects
\param hasParameter will be set to true if list contains Parameter objects
*/
MODULEBASE_EXPORT void checkObjects(const QObjectPtrList& theObjects, bool& hasResult, bool& hasFeature, bool& hasParameter);
}

#endif
