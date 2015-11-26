// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#ifndef ModuleBase_IWidgetCreator_H
#define ModuleBase_IWidgetCreator_H

#include "ModuleBase.h"

#include <list>
#include <string>

class ModuleBase_ModelWidget;

class QWidget;

/** 
* \ingroup GUI
* Interface to WidgetCreator which can create specific widgets by type
*/
class MODULEBASE_EXPORT ModuleBase_IWidgetCreator
{
public:
  /// Default constructor
  ModuleBase_IWidgetCreator();

  /// Virtual destructor
  ~ModuleBase_IWidgetCreator();

  /// Returns a list of possible widget types, which this creator can process
  /// \param theTypes
  virtual void widgetTypes(std::list<std::string>& theTypes) = 0;

   /// Create widget by its type
   /// \param theType a type
   /// \param theParent a parent widget
  virtual ModuleBase_ModelWidget* createWidgetByType(const std::string& theType,
                                                     QWidget* theParent = NULL) = 0;
};


#endif