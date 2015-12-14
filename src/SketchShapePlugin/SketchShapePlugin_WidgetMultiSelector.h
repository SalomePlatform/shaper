// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        SketchShapePlugin_PageGroupBox.h
// Created:     13 Dec 2015
// Author:      Natalia ERMOLAEVA

#ifndef SKETCHSHAPEPLUGIN_WIDGET_MULTI_SELECTOR_H_
#define SKETCHSHAPEPLUGIN_WIDGET_MULTI_SELECTOR_H_

#include <ModuleBase_WidgetMultiSelector.h>

class ModuleBase_IWorkshop;
class QWidget;

/*!
 * \ingroup GUI
 * Represent a property panel's list of ModuleBase_ModelWidgets.
 */
class SketchShapePlugin_WidgetMultiSelector : public ModuleBase_WidgetMultiSelector
{
  //Q_OBJECT
public:
  /// Constructs a multi selector widget, which can not be highlighted itself,
  /// the parent SketchShapePlugin_GroupBox is highlighted instead of it
  SketchShapePlugin_WidgetMultiSelector(QWidget* theParent, ModuleBase_IWorkshop* theWorkshop,
                                        const Config_WidgetAPI* theData,
                                        const std::string& theParentId);
  /// Destructs the page
  virtual ~SketchShapePlugin_WidgetMultiSelector() {}

  //! Switch On/Off highlighting of the widget
  //! Set highlight to the parent group box if there is such parent
  virtual void setHighlighted(bool isHighlighted);
};

#endif /* SKETCHSHAPEPLUGIN_PAGEGROUPBOX_H_ */
