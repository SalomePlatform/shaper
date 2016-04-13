// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        XGUI_MenuGroup.hxx
// Created:     13 Apr 2016
// Author:      Natalia ERMOLAEVA

#ifndef XGUI_MENUGROUP_H_
#define XGUI_MENUGROUP_H_

#include "XGUI.h"

#include <string>
#include <list>

class Config_FeatureMessage;

/**
* \ingroup GUI
* A class for management of menu actions (features). The actions should be arranged like they are
* in XML file. It listens the read feature of XML and fills internal structure of menu workbenches
* and groups of feature. After, it creates menues and tools in the module.
*/
class XGUI_EXPORT XGUI_MenuGroup
{
 public:
  /// Constructor
  XGUI_MenuGroup(const std::string& theName);
  virtual ~XGUI_MenuGroup() {}

  /// Returns a name of the workbench
  /// \return workbench name
  std::string getName() const { return myName; }

  /// Stores XML information for the feature kind
  /// \param theMessage a container of the feature XML properties
  virtual void setFeatureInfo(const std::shared_ptr<Config_FeatureMessage>& theMessage);

  /// Returns container of existing features
  /// \returns list
  const std::list<std::shared_ptr<Config_FeatureMessage> >& featureInfo() const;

private:
  std::string myName; /// a name of the workbench
  std::list<std::shared_ptr<Config_FeatureMessage> > myFeatureInfo; /// container of existing features
};

#endif /* XGUI_MENUGROUP_H_ */
