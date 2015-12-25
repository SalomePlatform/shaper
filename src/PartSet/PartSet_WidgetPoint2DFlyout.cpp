// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_WidgetPoint2D.cpp
// Created:     25 Apr 2014
// Author:      Natalia ERMOLAEVA

#include "PartSet_WidgetPoint2DFlyout.h"

#include <XGUI_Workshop.h>
#include <XGUI_ModuleConnector.h>
#include <XGUI_Displayer.h>

#include <Config_WidgetAPI.h>
#include <Config_Keywords.h>

#include <QWidget>


PartSet_WidgetPoint2DFlyout::PartSet_WidgetPoint2DFlyout(QWidget* theParent, 
                                                         ModuleBase_IWorkshop* theWorkshop,
                                                         const Config_WidgetAPI* theData,
                                                         const std::string& theParentId)
 : PartSet_WidgetPoint2D(theParent, theWorkshop, theData, theParentId)
{
}

bool PartSet_WidgetPoint2DFlyout::useSelectedShapes() const
{
  return false;
}

bool PartSet_WidgetPoint2DFlyout::focusTo()
{
  bool aCanAcceptFocus = true;
  if (isInternal() && isComputedDefault()) {
    AISObjectPtr anObject = workshop()->displayer()->getAISObject(feature());
    aCanAcceptFocus = anObject.get() && !anObject->isEmptyDistanceGeometry();
  }
  if (aCanAcceptFocus)
    aCanAcceptFocus = PartSet_WidgetPoint2D::focusTo();
  return aCanAcceptFocus;
}

XGUI_Workshop* PartSet_WidgetPoint2DFlyout::workshop() const
{
  XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(myWorkshop);
  return aConnector->workshop();
}
