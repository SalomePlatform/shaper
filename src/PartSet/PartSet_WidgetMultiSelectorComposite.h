// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_WidgetMultiSelectorComposite.h
// Created:     17 Mar 2016
// Author:      Natalia Ermolaeva

#ifndef PartSet_WidgetMultiSelectorComposite_H
#define PartSet_WidgetMultiSelectorComposite_H

#include "PartSet.h"

#include <ModuleBase_WidgetMultiSelector.h>

#include <ModelAPI_CompositeFeature.h>

/**
* \ingroup Modules
* Customosation of ModuleBase_WidgetMultiSelector in order to provide 
* working with sketch specific objects and creation of external objects.
*/
class PARTSET_EXPORT PartSet_WidgetMultiSelectorComposite: public ModuleBase_WidgetMultiSelector
{
Q_OBJECT
 public:
  /// Constructor
  /// \param theParent the parent object
  /// \param theWorkshop instance of workshop interface
  /// \param theData the widget configuation. The attribute of the model widget is obtained from
  PartSet_WidgetMultiSelectorComposite(QWidget* theParent, ModuleBase_IWorkshop* theWorkshop,
                              const Config_WidgetAPI* theData);

  virtual ~PartSet_WidgetMultiSelectorComposite();

  /// Set focus to the first control of the current widget. The focus policy of the control is checked.
  /// If the widget has the NonFocus focus policy, it is skipped.
  /// \return the state whether the widget can accept the focus
  virtual bool focusTo();

private:
  /// Disable controls by setting them focus policy NoFocus and disabling them
  void disableControls();
};

#endif