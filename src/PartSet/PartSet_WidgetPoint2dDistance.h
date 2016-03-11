// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_WidgetPoint2dDistance.h
// Created:     23 June 2014
// Author:      Vitaly Smetannikov

#ifndef PartSet_WidgetPoint2dDistance_H
#define PartSet_WidgetPoint2dDistance_H

#include "PartSet.h"
#include <ModuleBase_WidgetDoubleValue.h>

#include <ModelAPI_CompositeFeature.h>

class GeomAPI_Pnt2d;
class ModuleBase_IWorkshop;
class ModuleBase_IViewWindow;
class QMouseEvent;

/**
* \ingroup Modules
* Implementation of model widget for widget which provides input of a distance between two points
* In XML file can be defined as following:
* \code
* <point2ddistance id="CircleRadius" 
*   first_point="CircleCenter" 
*   label="Radius:" 
*   min="0" 
*   step="1.0" 
*   default="0" 
*   icon=":icons/radius.png" 
*   tooltip="Set radius">
*   <validator id="GeomValidators_Positive"/>
* </point2ddistance>
* \endcode
*/ 
class PARTSET_EXPORT PartSet_WidgetPoint2dDistance : public ModuleBase_WidgetDoubleValue
{
Q_OBJECT
 public:
  /// Constructor
  /// \param theParent the parent object
  /// \param theWorkshop a current workshop
  /// \param theData the widget configuation. The attribute of the model widget is obtained from
  PartSet_WidgetPoint2dDistance(QWidget* theParent, ModuleBase_IWorkshop* theWorkshop,
                                const Config_WidgetAPI* theData);

  virtual ~PartSet_WidgetPoint2dDistance();

  /// The methiod called when widget is deactivated
  virtual void deactivate();

  /// \returns the sketch instance
  CompositeFeaturePtr sketch() const { return mySketch; }

  /// Set sketch instance
  void setSketch(CompositeFeaturePtr theSketch) { mySketch = theSketch; }

  /// Returns true if the event is processed.
  virtual bool processEnter();

public slots:
   /// Process of mouse move
   /// \param theWnd a pointer to a window
   /// \param theEvent a mouse event
  void onMouseMove(ModuleBase_IViewWindow* theWnd, QMouseEvent* theEvent);

  protected slots:
   /// Process of mouse release
   /// \param theWnd a pointer to a window
   /// \param theEvent a mouse event
  void onMouseRelease(ModuleBase_IViewWindow* theWnd, QMouseEvent* theEvent);

protected:
  /// Store current value in cashed value
  void storeCurentValue();

  /// Restore cashed value in the model attribute
  /// \return boolean state if the restored feature shoud be hidden
  bool restoreCurentValue();

  /// Fills the widget with default values
  /// \return true if the widget current value is reset
  virtual bool resetCustom();

  /// The methiod called when widget is activated
  virtual void activateCustom();

  /// Set the second point which defines a value in the widget as a distance with a first point defined by feature
  void setPoint(FeaturePtr theFeature, const std::shared_ptr<GeomAPI_Pnt2d>& thePnt);

  /// Compute the distance between points
  /// \param theFirstPnt a point value of the out point attribute
  /// \param theCurrentPnt a point of the current widget
  /// \return a double value
  virtual double computeValue(const std::shared_ptr<GeomAPI_Pnt2d>& theFirstPnt,
                              const std::shared_ptr<GeomAPI_Pnt2d>& theCurrentPnt);

protected:
  /// A reference to workshop
  ModuleBase_IWorkshop* myWorkshop;
  
  /// A name of the first point
  std::string myFirstPntName;

  /// Reference to sketch
  CompositeFeaturePtr mySketch;

  bool myValueIsCashed; /// boolean state if the value is cashed during value state change
  bool myIsFeatureVisibleInCash; /// boolean value if the feature was visible when cash if filled
  double myValueInCash; /// the cashed X value during value state change
};

#endif
