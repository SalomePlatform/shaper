// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        SamplePanelPlugin_WidgetCreator.h
// Created:     29 Mar 2015
// Author:      Natalia ERMOLAEVA

#ifndef SamplePanelPlugin_WidgetCreator_H
#define SamplePanelPlugin_WidgetCreator_H

#include "SamplePanelPlugin.h"

#include <ModuleBase_IWidgetCreator.h>

#include <string>
#include <set>

class QWidget;

/** 
* \ingroup GUI
* Interface to WidgetCreator which can create specific widgets by type
*/
class SamplePanelPlugin_WidgetCreator : public ModuleBase_IWidgetCreator
{
public:
  /// Default constructor
  SamplePanelPlugin_WidgetCreator();

  /// Virtual destructor
  ~SamplePanelPlugin_WidgetCreator() {}

  /// Returns a container of possible page types, which this creator can process
  /// \param theTypes a list of type names
  virtual void panelTypes(std::set<std::string>& theTypes);

  /// Create panel control by its type.
  /// \param theType a panel type
  /// \param theParent a parent widget
  /// \param theFeature a feature to fill the panel
  /// \return created widget or null
  virtual QWidget* createPanelByType(const std::string& theType,
                                     QWidget* theParent,
                                     const FeaturePtr& theFeature);
private:
  std::set<std::string> myPanelTypes; /// types of panels
};

typedef std::shared_ptr<SamplePanelPlugin_WidgetCreator> SamplePanelWidgetCreatorPtr;

#endif
