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
  bool aCanSetFocus = true;
  CompositeFeaturePtr aComposite = std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(myFeature);
  if (aComposite.get())
    aCanSetFocus = aComposite->numberOfSubs() == 0;

  // disable widget controls
  if (!aCanSetFocus) {
    disableControls();
  }

  return aCanSetFocus;
}

void PartSet_WidgetMultiSelectorComposite::disableControls()
{
  QList<QWidget*> aMyControls = getControls();
  foreach(QWidget*  eachControl, aMyControls) {
    eachControl->setFocusPolicy(Qt::NoFocus);
    eachControl->setEnabled(false);
  }
}
