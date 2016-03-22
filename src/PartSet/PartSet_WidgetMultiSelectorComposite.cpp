// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_WidgetMultiSelectorComposite.cpp
// Created:     17 Mar 2016
// Author:      Natalia Ermolaeva

#include "PartSet_WidgetMultiSelectorComposite.h"

PartSet_WidgetMultiSelectorComposite::PartSet_WidgetMultiSelectorComposite(QWidget* theParent,
                                                         ModuleBase_IWorkshop* theWorkshop,
                                                         const Config_WidgetAPI* theData)
: ModuleBase_WidgetMultiSelector(theParent, theWorkshop, theData)
{
}

PartSet_WidgetMultiSelectorComposite::~PartSet_WidgetMultiSelectorComposite()
{
}

bool PartSet_WidgetMultiSelectorComposite::focusTo()
{
  bool aHasSubObjects = hasSubObjects();

  // disable widget controls
  if (aHasSubObjects)
    disableControls();

  return !aHasSubObjects;
}

void PartSet_WidgetMultiSelectorComposite::setEditingMode(bool isEditing)
{
  ModuleBase_ModelWidget::setEditingMode(isEditing);
  bool aHasSubObjects = hasSubObjects();
  if (aHasSubObjects)
    disableControls();
}

bool PartSet_WidgetMultiSelectorComposite::hasSubObjects() const
{
  bool aHasSubObjects = false;

  bool aCanSetFocus = true;
  CompositeFeaturePtr aComposite = std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(myFeature);
  if (aComposite.get())
    aHasSubObjects = aComposite->numberOfSubs() > 0;
  return aHasSubObjects;
}

void PartSet_WidgetMultiSelectorComposite::disableControls()
{
  QList<QWidget*> aMyControls = getControls();
  foreach(QWidget*  eachControl, aMyControls) {
    eachControl->setFocusPolicy(Qt::NoFocus);
    eachControl->setEnabled(false);
  }
}
