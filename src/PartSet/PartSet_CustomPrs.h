// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_CustomPrs.hxx
// Created:     30 Jun 2015
// Author:      Natalia ERMOLAEVA

#ifndef PartSet_CustomPrs_H
#define PartSet_CustomPrs_H

#include "PartSet.h"

#include "PartSet_OperationPrs.h"

#include <ModuleBase_IModule.h>
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
  /// Returns yellow color
  static const std::string OPERATION_PARAMETER_COLOR() { return "255, 255, 0"; }
  /// Returns green color
  static const std::string OPERATION_RESULT_COLOR() { return "0,225, 0"; }
  /// Returns color between white and color of highlight
  /// highlight color is equal Quantity_Color(0.5,1,1) or QColor(122, 255,255) // 188
  static const std::string OPERATION_HIGHLIGHT_COLOR() { return "188, 255, 255"; }

public:
  /// Constructor
  /// \param theWorkshop a reference to workshop
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
  /// \param theObject an object to redisplay
  /// \param theFlag an object AIS presentation type
  /// \param theUpdateViewer the parameter whether the viewer should be update immediatelly
  /// \returns true if the presentation is redisplayed
  bool redisplay(const ObjectPtr& theObject,
                 const ModuleBase_IModule::ModuleBase_CustomizeFlag& theFlag,
                 const bool theUpdateViewer);

  /// Nullify all internal presentations. For example, it can be useful when the viewer/context
  /// is closed. If this is not performed and the presentation is assigned in another context,
  /// it caused erroneus case because the presentation has linkage to the previous context.
  void clearPrs();

private:
  /// Creates the AIS operation presentation
  /// \param theFlag an object AIS presentation type
  void initPresentation(const ModuleBase_IModule::ModuleBase_CustomizeFlag& theFlag);

  /// Gets an AIS presentation by the flag
  /// \param theFlag an object AIS presentation type
  /// \return the AIS prsentation
  Handle(PartSet_OperationPrs) getPresentation(
                            const ModuleBase_IModule::ModuleBase_CustomizeFlag& theFlag);

  //! Returns workshop
  XGUI_Workshop* workshop() const;

  /// Displays the internal presentation in the viewer of workshop
  /// \param theFlag an object AIS presentation type
  /// \param theUpdateViewer the parameter whether the viewer should be update immediatelly
  /// \param returns whether the presentation is displayed/redisplayed
  bool displayPresentation(const ModuleBase_IModule::ModuleBase_CustomizeFlag& theFlag,
                           const bool theUpdateViewer);

  /// Erases the internal presentation from the viewer of workshop
  /// \param theFlag an object AIS presentation type
  /// \param theUpdateViewer the parameter whether the viewer should be update immediatelly
  void erasePresentation(const ModuleBase_IModule::ModuleBase_CustomizeFlag& theFlag,
                         const bool theUpdateViewer);

  /// Nullify the operation presentation. For example, it can be useful when the viewer/context
  /// is closed. If this is not performed and the presentation is assigned in another context,
  /// it caused erroneus case because the presentation has linkage to the previous context.
  void clearPresentation(const ModuleBase_IModule::ModuleBase_CustomizeFlag& theFlag);

private:
  bool myIsActive;
  ModuleBase_IWorkshop* myWorkshop; /// current workshop
  /// map of presentation type to AIS object
  QMap<ModuleBase_IModule::ModuleBase_CustomizeFlag, AISObjectPtr> myPresentations;
};

#endif
