// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_WidgetShapeSelector.h
// Created:     27 Nov 2014
// Author:      Vitaly Smetannikov


#ifndef PartSet_WidgetEditor_H
#define PartSet_WidgetEditor_H

#include "PartSet.h"

#include <ModuleBase_WidgetEditor.h>

class ModuleBase_IWorkshop;

/**
* \ingroup Modules
* Customosation of ModuleBase_WidgetEditor in order to do not show the editor value if the mouse
* cursor is not over the OCC window
*/
class PARTSET_EXPORT PartSet_WidgetEditor : public ModuleBase_WidgetEditor
{
Q_OBJECT
 public:
  /// Constructor
  /// \param theParent the parent object
  /// \param theWorkshop instance of workshop interface
  /// \param theData the widget configuation. The attribute of the model widget is obtained from
  /// \param theParentId is Id of a parent of the current attribute
  PartSet_WidgetEditor(QWidget* theParent, ModuleBase_IWorkshop* theWorkshop,
                       const Config_WidgetAPI* theData, const std::string& theParentId);

  virtual ~PartSet_WidgetEditor() {}

  /// Activates the editor control only in case if the mouse over the OCC window, otherwise
  /// set focus to the usual double value control
  /// \return the state whether the widget can accept the focus
  virtual bool focusTo();

private:
  ModuleBase_IWorkshop* myWorkshop; // the current workshop
};

#endif