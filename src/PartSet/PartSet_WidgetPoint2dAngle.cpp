// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_WidgetPoint2dAngle.h
// Created:     23 June 2014
// Author:      Vitaly Smetannikov

#include "PartSet_WidgetPoint2dAngle.h"

#include <ModuleBase_ParamSpinBox.h>

#include <ModuleBase_Tools.h>

#include <GeomAPI_Pnt2d.h>
#include <GeomDataAPI_Point2D.h>

#include <ModelAPI_Data.h>
#include <ModelAPI_AttributeDouble.h>

#include <QMouseEvent>

#include <GeomAPI_Dir2d.h>
#include <GeomAPI_XY.h>

#define PI 3.1415926535897932

PartSet_WidgetPoint2dAngle::PartSet_WidgetPoint2dAngle(QWidget* theParent,
                                                       ModuleBase_IWorkshop* theWorkshop,
                                                       const Config_WidgetAPI* theData,
                                                       const std::string& theParentId)
: PartSet_WidgetPoint2dDistance(theParent, theWorkshop, theData, theParentId)
{
}

PartSet_WidgetPoint2dAngle::~PartSet_WidgetPoint2dAngle()
{
}

double PartSet_WidgetPoint2dAngle::computeValue(const std::shared_ptr<GeomAPI_Pnt2d>& theFirstPnt,
                                                const std::shared_ptr<GeomAPI_Pnt2d>& theCurrentPnt)
{
  std::shared_ptr<GeomAPI_Dir2d> aHorizontalDir(new GeomAPI_Dir2d(1, 0));
  std::shared_ptr<GeomAPI_XY> aStartPnt = theFirstPnt->xy();
  std::shared_ptr<GeomAPI_XY> aEndPnt = theCurrentPnt->xy();

  std::shared_ptr<GeomAPI_Dir2d> aLineDir(new GeomAPI_Dir2d(aEndPnt->decreased(aStartPnt)));

  double aValue = aHorizontalDir->angle(aLineDir);
  aValue *= 180.0 / PI;

  return aValue;
}
