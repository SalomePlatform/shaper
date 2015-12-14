// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        SketchShapePlugin_PageGroupBox.h
// Created:     25 Nov 2015
// Author:      Natalia ERMOLAEVA

#ifndef SKETCHSHAPEPLUGIN_PAGEGROUPBOX_H_
#define SKETCHSHAPEPLUGIN_PAGEGROUPBOX_H_

#include <ModuleBase_PageGroupBox.h>

class QWidget;

/*!
 * \ingroup GUI
 * Represent a property panel's list of ModuleBase_ModelWidgets.
 */
class SketchShapePlugin_PageGroupBox : public ModuleBase_PageGroupBox
{
  //Q_OBJECT
public:
  /// Constructs a page that looks like a QGroupBox
  SketchShapePlugin_PageGroupBox(QWidget* theParent = 0);
  /// Destructs the page
  virtual ~SketchShapePlugin_PageGroupBox() {}

  //! Switch On/Off highlighting of the widget
  //! Set highlight to the parent group box if there is such parent
  void setHighlightedGroupBox(bool isHighlighted);
};

#endif /* SKETCHSHAPEPLUGIN_PAGEGROUPBOX_H_ */
