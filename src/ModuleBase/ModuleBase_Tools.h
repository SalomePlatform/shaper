// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_Tools.h
// Created:     11 July 2014
// Author:      Vitaly Smetannikov

#ifndef ModuleBase_Tools_H
#define ModuleBase_Tools_H

#include "ModuleBase.h"
#include "ModuleBase_Definitions.h"

#include <ModelAPI_Attribute.h>
#include <ModelAPI_Feature.h>

#include <TopAbs_ShapeEnum.hxx>
#include <TopoDS_Shape.hxx>
#include <Prs3d_Drawer.hxx>

#include <Quantity_Color.hxx>

#include <QPixmap>

class QWidget;
class QLayout;
class QDoubleSpinBox;
class ModuleBase_ParamIntSpinBox;
class ModuleBase_ParamSpinBox;
class ModuleBase_IWorkshop;

class GeomAPI_Shape;

namespace ModuleBase_Tools {

/*
 * Methods to adjust margins and spacings.
 */
MODULEBASE_EXPORT void adjustMargins(QWidget* theWidget);
MODULEBASE_EXPORT void adjustMargins(QLayout* theLayout);

MODULEBASE_EXPORT void zeroMargins(QWidget* theWidget);
MODULEBASE_EXPORT void zeroMargins(QLayout* theLayout);

/*
 * Calls the same-named Qt method for the given widget.
 * It sets the top-level widget containing this widget to be the active window.
 * An active window is a visible top-level window that has the keyboard input focus.
 * \param theWidget a widget to be activated
 * \param theIndo a debug information
 */
MODULEBASE_EXPORT void activateWindow(QWidget* theWidget, const QString& theInfo = QString());

/*
 * Calls the same-named Qt method for the given widget.
 * Gives the keyboard input focus to this widget (or its focus proxy) if this widget or
 * one of its parents is the active window.
 * \param theWidget a widget to be activated
 * \param theIndo a debug information
 */
MODULEBASE_EXPORT void setFocus(QWidget* theWidget, const QString& theInfo = QString());


//! Sets or removes the shadow effect to the widget
//! \param theWidget a widget to be styled
//! \param isSetEffect if true, the shadow effect is set, overwise cleared
//! \return resulting pixmap
MODULEBASE_EXPORT void setShadowEffect(QWidget* theWidget, const bool isSetEffect);

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

/// Sets programmatically the value to the spin box without emitting any signals(e.g. valueChanged)
/// \param theSpin a ModuleBase_ParamIntSpinBox object
/// \param theValue a new value
MODULEBASE_EXPORT void setSpinValue(ModuleBase_ParamIntSpinBox* theSpin, int theValue);

/// Sets programmatically the value to the spin box without emitting any signals(e.g. valueChanged)
/// \param theSpin a SpinBox that accepts text
/// \param theText a new value
MODULEBASE_EXPORT void setSpinText(ModuleBase_ParamIntSpinBox* theSpin, const QString& theText);

/// Converts the object to the feature or a result and generate information string
/// \param theObj an object
/// \param isUseAttributesInfo a flag whether the attribute values information is used
/// \return a string
MODULEBASE_EXPORT QString objectInfo(const ObjectPtr& theObj, const bool isUseAttributesInfo = false);

/// Converts string value (name of shape type) to shape enum value
/// \param theType - string with shape type name
/// \return TopAbs_ShapeEnum value
MODULEBASE_EXPORT TopAbs_ShapeEnum shapeType(const QString& theType);

/// Checks whether the object is a sub result. It gets the feature of the object,
/// obtains all results of the feature and check if the object is a sub result
/// \return boolean result
MODULEBASE_EXPORT bool isSubResult(ObjectPtr theObject);

/*!
Check types of objects which are in the given list
\param theObjects the list of objects
\param hasResult will be set to true if list contains Result objects
\param hasFeature will be set to true if list contains Feature objects
\param hasParameter will be set to true if list contains Parameter objects
\param hasCompositeOwner will be set to true if list contains Sub-Feature objects
*/
MODULEBASE_EXPORT void checkObjects(const QObjectPtrList& theObjects, bool& hasResult,
                           bool& hasFeature, bool& hasParameter, bool& hasCompositeOwner);

/*! Sets the default coeffient into the driver calculated accordingly the shape type.
It provides 1.e-4 for a shape withe Edge shape type
\param theShape a shape to define the deviation coeffient, 
\param theDrawer a drawer
*/
MODULEBASE_EXPORT void setDefaultDeviationCoefficient(const TopoDS_Shape& theShape,
                                                      const Handle(Prs3d_Drawer)& theDrawer);

/*! Obtains the color from the property manager and converts it to the OCCT color
\param theSection a property section
\param theName a property item name
\param theDefault a default color value
\return quantity color
*/
MODULEBASE_EXPORT Quantity_Color color(const std::string& theSection,
                                       const std::string& theName,
                                       const std::string& theDefault);


/// Returns the object from the attribute
/// \param theObj an object
MODULEBASE_EXPORT ObjectPtr getObject(const AttributePtr& theAttribute);

/// Returns the object from the attribute
/// \param theObj an object
MODULEBASE_EXPORT TopAbs_ShapeEnum getCompoundSubType(const TopoDS_Shape& theShape);

/// Returns list of parameters accessible in the active part and partset
/// \theParameters a list of parameter names
MODULEBASE_EXPORT void getParameters(QStringList& theParameters);

/// Returns list of parameters accessible in the active part and partset
/// \theParameters a list of parameter names
MODULEBASE_EXPORT std::string findGreedAttribute(ModuleBase_IWorkshop* theWorkshop,
                                                 const FeaturePtr& theFeature);

/// Set the object to the attribute depending on the attribute type. If it is a list,
/// the values are appended if they are not in the list yet.
/// \param theAttribute an attribute where the object and shape are set
/// \param theObject an object
/// \param theShape a shape
/// \param theWorkshop to find an attribute for the given shape for attribute reference
/// \param theTemporarily if it is true, do not store and name the added in the data framework
///        It is useful for attribute selection
MODULEBASE_EXPORT void setObject(const AttributePtr& theAttribute, const ObjectPtr& theObject,
                                 const std::shared_ptr<GeomAPI_Shape>& theShape,
                                 ModuleBase_IWorkshop* theWorkshop,
                                 const bool theTemporarily = false);

/// Returns the shape of the attribute. If the attribute is AttributeRefAttrPtr, the shape is found
/// using current module of the given workshop.
/// \param theAttribute an attribute where the shape is set
/// \param theWorkshop to find a shape for the given attribute
MODULEBASE_EXPORT std::shared_ptr<GeomAPI_Shape> getShape(const AttributePtr& theAttribute,
                                                          ModuleBase_IWorkshop* theWorkshop);

/// Flush updated signal for the object. The viewer update is blocked in the process
/// \param theObject parameter of the signal
MODULEBASE_EXPORT void flushUpdated(ObjectPtr theObject);

/// Sends a message about block/unblock viewer updating
/// \param theValue a boolean value
MODULEBASE_EXPORT void blockUpdateViewer(const bool theValue);

/// Generates a wrapped string to be less than value with '\n' separators
/// \param theValue a boolean value
/// \param theWidget a widget to know the font
/// \param theMaxLineInPixels a maximum line width in pixels
MODULEBASE_EXPORT QString wrapToolTipByWords(const QString& theValue, QWidget* theWidget,
                                             int theMaxLineInPixels = 150);
}

#endif
