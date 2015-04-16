// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_WidgetMultiSelector.cpp
// Created:     15 Apr 2015
// Author:      Natalia Ermolaeva

#include "PartSet_WidgetMultiSelector.h"

#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

#include <ModuleBase_Definitions.h>
#include <Config_WidgetAPI.h>

#include <PartSet_Tools.h>
#include <PartSet_ExternalObjectsMgr.h>
#include <SketchPlugin_Feature.h>

#include <SketchPlugin_ConstraintRigid.h>

#include <XGUI_Workshop.h>

PartSet_WidgetMultiSelector::PartSet_WidgetMultiSelector(QWidget* theParent,
                                                         ModuleBase_IWorkshop* theWorkshop,
                                                         const Config_WidgetAPI* theData,
                                                         const std::string& theParentId)
: ModuleBase_WidgetShapeSelector(theParent, theWorkshop, theData, theParentId)
{
  myExternalObjectMgr = new PartSet_ExternalObjectsMgr(theData->getProperty("use_external"), false);
}

PartSet_WidgetMultiSelector::~PartSet_WidgetMultiSelector()
{
  delete myExternalObjectMgr;
}

//********************************************************************
void PartSet_WidgetMultiSelector::restoreAttributeValue(const bool theValid)
{
  ModuleBase_WidgetShapeSelector::restoreAttributeValue(theValid);
  myExternalObjectMgr->removeExternal(sketch(), myFeature);
}
