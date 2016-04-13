// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        XGUI_MenuGroup.hxx
// Created:     13 Apr 2016
// Author:      Natalia ERMOLAEVA

#ifndef XGUI_MENUGROUP_H_
#define XGUI_MENUGROUP_H_

#include "XGUI.h"

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
  XGUI_MenuGroup();
  virtual ~XGUI_MenuGroup() {}
};

#endif /* XGUI_MENUGROUP_H_ */

