// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_CustomPrs.hxx
// Created:     30 Jun 2015
// Author:      Natalia ERMOLAEVA

#ifndef PartSet_CustomPrs_H
#define PartSet_CustomPrs_H

#include "PartSet.h"

#include "PartSet_OperationPrs.h"

#include <ModelAPI_Object.h>
#include <ModelAPI_Result.h>
#include <ModelAPI_Feature.h>

#include <GeomAPI_ICustomPrs.h>
#include <GeomAPI_AISObject.h>
#include <GeomAPI_Shape.h>

class ModuleBase_IWorkshop;

/**
* Interface of a class which can provide specific customization of
* object presentation
*/
class PartSet_CustomPrs
{
public:
  PARTSET_EXPORT PartSet_CustomPrs(ModuleBase_IWorkshop* theWorkshop);
  PARTSET_EXPORT virtual ~PartSet_CustomPrs() {};

  /// Returns true if the presentation is active
  bool isActive() const;

  /// Initializes the presentation by the parameter object
  void activate(const FeaturePtr& theObject);

  void deactivate();

  /// Modifies the given presentation in the custom way.
  void customize(const ObjectPtr& theObject);

private:
  /// Returns the AIS presentation
  Handle(PartSet_OperationPrs) getPresentation() const;

  /// Displays the internal presentation in the viewer of workshop
  void displayPresentation();
  /// Erases the internal presentation from the viewer of workshop
  void erasePresentation();
  /// Sets color, point size and width of the presentation
  void customizePresentation();

private:
  ModuleBase_IWorkshop* myWorkshop; /// current workshop
  AISObjectPtr myOperationPrs; /// the AIS presentation, which is displayed/erased in the viewer
};

#endif
