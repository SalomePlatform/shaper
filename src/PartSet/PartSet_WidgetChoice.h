// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_WidgetChoice.h
// Created:     17 Nov 2015
// Author:      Vitaly Smetannikov

#ifndef PartSet_WidgetChoice_H
#define PartSet_WidgetChoice_H

#include "PartSet.h"
#include <ModuleBase_WidgetChoice.h>

/**
* \ingroup GUI
* Implementation of a proxy of choice widget in order to geat access to it on moment 
* of creation in module
*/
class PARTSET_EXPORT PartSet_WidgetChoice : public ModuleBase_WidgetChoice
{
Q_OBJECT
 public:
  /// Constructor
  /// \param theParent the parent object
  /// \param theData the widget configuation. The attribute of the model widget is obtained from
  /// \param theParentId is Id of a parent of the current attribute
  PartSet_WidgetChoice(QWidget* theParent, const Config_WidgetAPI* theData, 
    const std::string& theParentId) 
    : ModuleBase_WidgetChoice(theParent, theData, theParentId) {}
};

#endif