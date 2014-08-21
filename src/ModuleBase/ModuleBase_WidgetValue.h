// File:        ModuleBase_WidgetValue.h
// Created:     25 Apr 2014
// Author:      Natalia ERMOLAEVA

#ifndef ModuleBase_WidgetValue_H
#define ModuleBase_WidgetValue_H

#include <ModuleBase.h>

/**\class ModuleBase_WidgetValue
 * \ingroup GUI
 * \brief Custom widget value. An abstract class to be redefined and to be set in the model widget
 */
class MODULEBASE_EXPORT ModuleBase_WidgetValue
{
 public:
  /// Constructor
  ModuleBase_WidgetValue();
  /// Destructor
  virtual ~ModuleBase_WidgetValue();
};

#endif
