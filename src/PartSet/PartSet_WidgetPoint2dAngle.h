// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_WidgetPoint2dAngle.h
// Created:     30 Apr 2015
// Author:      Natalia Ermolaeva

#ifndef PartSet_WidgetPoint2dAngle_H
#define PartSet_WidgetPoint2dAngle_H

#include "PartSet.h"
#include "PartSet_WidgetPoint2dDistance.h"

#include <ModelAPI_Feature.h>

class ModuleBase_IWorkshop;

class GeomAPI_Pnt2d;

/**
* \ingroup Modules
* Implementation of model widget for widget which provides input of an anble between two points
* The XML definion is the same as the parent one.
*/ 
class PARTSET_EXPORT PartSet_WidgetPoint2dAngle : public PartSet_WidgetPoint2dDistance
{
Q_OBJECT
 public:
  /// Constructor
  /// \param theParent the parent object
  /// \param theWorkshop a current workshop
  /// \param theData the widget configuation. The attribute of the model widget is obtained from
  /// \param theParentId is Id of a parent of the current attribute
  PartSet_WidgetPoint2dAngle(QWidget* theParent, ModuleBase_IWorkshop* theWorkshop,
                             const Config_WidgetAPI* theData, const std::string& theParentId);

  virtual ~PartSet_WidgetPoint2dAngle();

protected:
  /// Compute an angle between points
  /// \param theFirstPnt a point value of the out point attribute
  /// \param theCurrentPnt a point of the current widget
  /// \return a double value
  virtual double computeValue(const std::shared_ptr<GeomAPI_Pnt2d>& theFirstPnt,
                              const std::shared_ptr<GeomAPI_Pnt2d>& theCurrentPnt);
};

#endif
