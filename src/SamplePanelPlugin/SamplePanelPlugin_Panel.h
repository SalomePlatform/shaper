// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        SamplePanelPlugin_PageGroupBox.h
// Created:     29 Mar 2015
// Author:      Natalia ERMOLAEVA

#ifndef SAMPLEPANELPLUGIN_PANEL_H_
#define SAMPLEPANELPLUGIN_PANEL_H_

#include <QWidget>

/*!
 * \ingroup GUI
 * Represent a property panel's list of ModuleBase_ModelWidgets.
 */
class SamplePanelPlugin_Panel : public QWidget
{
public:
  /// Constructs a panel page
  SamplePanelPlugin_Panel(QWidget* theParent);
  /// Destructs the page
  virtual ~SamplePanelPlugin_Panel() {}
};

#endif /* SAMPLEPANELPLUGIN_PANEL_H_ */
