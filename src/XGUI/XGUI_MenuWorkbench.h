// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        XGUI_MenuWorkbench.hxx
// Created:     13 Apr 2016
// Author:      Natalia ERMOLAEVA

#ifndef XGUI_MENUWORKBENCH_H_
#define XGUI_MENUWORKBENCH_H_

#include "XGUI.h"

#include <string>
#include <list>
#include <memory>

class XGUI_MenuGroup;

/**
* \ingroup GUI
* A class for management of menu actions (features). The actions should be arranged like they are
* in XML file. It listens the read feature of XML and fills internal structure of menu workbenches
* and groups of feature. After, it creates menues and tools in the module.
*/
class XGUI_EXPORT XGUI_MenuWorkbench
{
 public:
  /// Constructor
  XGUI_MenuWorkbench(const std::string& theName);
  /// Destructor
  virtual ~XGUI_MenuWorkbench() {}

  /// Returns a name of the workbench
  /// \return workbench name
  std::string getName() const { return myName; }

  /// Finds or creates a group for the given name
  /// \param theGroupName a name defined in XML
  /// \return an instance of group
  std::shared_ptr<XGUI_MenuGroup> findGroup(const std::string& theGroupName);

  /// Returns container of existing groups
  /// \returns list
  const std::list<std::shared_ptr<XGUI_MenuGroup> >& groups() const;

private:
  std::string myName; /// a name of the workbench
  std::list<std::shared_ptr<XGUI_MenuGroup> > myGroups; /// container of existing groups
};

#endif /* XGUI_MENUWORKBENCH_H_ */

