// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        SketchShapePlugin_PageGroupBox.h
// Created:     13 Dec 2015
// Author:      Natalia ERMOLAEVA

#ifndef SKETCHSHAPEPLUGIN_WIDGET_CHECK_GROUP_BOX_H_
#define SKETCHSHAPEPLUGIN_WIDGET_CHECK_GROUP_BOX_H_

#include <ModuleBase_WidgetCheckGroupBox.h>

class QWidget;

/*!
 * \ingroup GUI
 * Represent a property panel's list of ModuleBase_ModelWidgets.
 */
class SketchShapePlugin_WidgetCheckGroupBox : public ModuleBase_WidgetCheckGroupBox
{
  //Q_OBJECT
public:
  /// Constructs a page that looks like a QGroupBox
  SketchShapePlugin_WidgetCheckGroupBox(QWidget* theParent, const Config_WidgetAPI* theData,
                                 const std::string& theParentId);
  /// Destructs the page
  virtual ~SketchShapePlugin_WidgetCheckGroupBox() {}

  //! Switch On/Off highlighting of the widget
  //! Set highlight to the parent group box if there is such parent
  virtual void setHighlighted(bool isHighlighted);
};

#endif /* SKETCHSHAPEPLUGIN_WIDGET_CHECK_GROUP_BOX_H_ */
