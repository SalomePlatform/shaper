// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        SketchShapePlugin_PageGroupBox.h
// Created:     13 Dec 2015
// Author:      Natalia ERMOLAEVA

#include <SketchShapePlugin_WidgetCheckGroupBox.h>

#include <SketchShapePlugin_PageGroupBox.h>

SketchShapePlugin_WidgetCheckGroupBox::SketchShapePlugin_WidgetCheckGroupBox(QWidget* theParent,
                                                               const Config_WidgetAPI* theData,
                                                               const std::string& theParentId)
  : ModuleBase_WidgetCheckGroupBox(theParent, theData, theParentId)
{
}

void SketchShapePlugin_WidgetCheckGroupBox::setHighlighted(bool isHighlighted)
{
  SketchShapePlugin_PageGroupBox* aShapeGroupBox = 0;
  QWidget* aParent = qobject_cast<QWidget*>(parent());
  while (aParent) {
    aShapeGroupBox = dynamic_cast<SketchShapePlugin_PageGroupBox*>(aParent);
    if (aShapeGroupBox)
      break;
    aParent = qobject_cast<QWidget*>(aParent->parent());
  }

  if (aShapeGroupBox)
    aShapeGroupBox->setHighlightedGroupBox(isHighlighted);
}

