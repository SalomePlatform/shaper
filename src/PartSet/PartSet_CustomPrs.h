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
class XGUI_Workshop;

/**
 * This is the module custom presentation, which manage an AIS presentation, that can be filled
 * by a feature and visualized in the viewer additionally to usual workshop objects.
*/
class PartSet_CustomPrs
{
public:
  PARTSET_EXPORT PartSet_CustomPrs(ModuleBase_IWorkshop* theWorkshop);
  PARTSET_EXPORT virtual ~PartSet_CustomPrs() {};

  /// Returns true if the presentation is active
  bool isActive();

  /// Initializes the operation presentation by the parameter object and display the presentation
  /// \param theObject an operation feature source to fill the presentation
  /// \param theUpdateViewer the parameter whether the viewer should be update immediatelly
  /// \returns true if the presentation is displayed
  bool activate(const FeaturePtr& theObject, const bool theUpdateViewer);

  /// Initializes the operation presentation by empty object and erase the presentation
  /// \param theUpdateViewer the parameter whether the viewer should be update immediatelly
  /// \returns true if the presentation has been displayed and now it is erased
  bool deactivate(const bool theUpdateViewer);

  /// If the presentation is active[displayed], the shapes of the presentation is recomputed
  /// and the presentation is redisplayed.
  /// \param theUpdateViewer the parameter whether the viewer should be update immediatelly
  /// \returns true if the presentation is redisplayed
  bool redisplay(const ObjectPtr& theObject, const bool theUpdateViewer);

  /// Nullify the operation presentation. For example, it can be useful when the viewer/context
  /// is closed. If this is not performed and the presentation is assigned in another context,
  /// it caused erroneus case because the presentation has linkage to the previous context.
  void clearPrs();

private:
  /// Creates the AIS operation presentation
  void initPrs();

  /// Returns the AIS presentation
  Handle(PartSet_OperationPrs) getPresentation();

  //! Returns workshop
  XGUI_Workshop* workshop() const;

  /// Displays the internal presentation in the viewer of workshop
  /// \param theUpdateViewer the parameter whether the viewer should be update immediatelly
  void displayPresentation(const bool theUpdateViewer);

  /// Erases the internal presentation from the viewer of workshop
  /// \param theUpdateViewer the parameter whether the viewer should be update immediatelly
  void erasePresentation(const bool theUpdateViewer);

  /// Sets color, point size and width of the presentation
  /// \param theUpdateViewer the parameter whether the viewer should be update immediatelly
  void customizePresentation(const bool theUpdateViewer);

private:
  bool myIsActive;
  ModuleBase_IWorkshop* myWorkshop; /// current workshop
  AISObjectPtr myOperationPrs; /// the AIS presentation, which is displayed/erased in the viewer
};

#endif
