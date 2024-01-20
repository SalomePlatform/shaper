// Copyright (C) 2014-2024  CEA, EDF
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#ifndef PartSet_CustomPrs_H
#define PartSet_CustomPrs_H

#include "PartSet.h"

#include "PartSet_OperationPrs.h"

#include <ModuleBase_IModule.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_Result.h>
#include <ModelAPI_Feature.h>

#include <Events_Listener.h>

#include <GeomAPI_ICustomPrs.h>
#include <GeomAPI_AISObject.h>
#include <GeomAPI_Shape.h>

class ModuleBase_IWorkshop;
class XGUI_Workshop;

/**
 * This is the module custom presentation, which manage an AIS presentation, that can be filled
 * by a feature and visualized in the viewer additionally to usual workshop objects.
*/
class PartSet_CustomPrs : public Events_Listener
{
public:
  /// Returns yellow color
  static const std::string OPERATION_PARAMETER_COLOR() { return "255, 255, 0"; }
  /// Returns green color
  static const std::string OPERATION_RESULT_COLOR() { return "0,225, 0"; }
  /// Returns color between white and color of highlight
  static const std::string OPERATION_HIGHLIGHT_COLOR() { return "128, 0, 0"; }

  /// Returns color lighter than sketch feature entity : pink
  static const std::string OPERATION_REMOVE_FEATURE_COLOR() { return "255, 174, 201"; }

  /// Returns color equal to default color of construction plugin : green
  static const std::string OPERATION_SKETCH_PLANE() { return "170,255,0"; }
public:
  /// Constructor
  /// \param theWorkshop a reference to workshop
  PARTSET_EXPORT PartSet_CustomPrs(ModuleBase_IWorkshop* theWorkshop);
  PARTSET_EXPORT virtual ~PartSet_CustomPrs() {};

  /// Returns true if the presentation is active
  /// \param theFlag an object AIS presentation type
  bool isActive(const ModuleBase_IModule::ModuleBase_CustomizeFlag& theFlag);

  /// Initializes the operation presentation by the parameter object and display the presentation
  /// \param theObject an operation feature source to fill the presentation
  /// \param theFlag an object AIS presentation type
  /// \param theUpdateViewer the parameter whether the viewer should be update immediatelly
  /// \returns true if the presentation is displayed
  bool activate(const FeaturePtr& theObject,
                const ModuleBase_IModule::ModuleBase_CustomizeFlag& theFlag,
                const bool theUpdateViewer);

  /// Initializes the operation presentation by empty object and erase the presentation
  /// \param theUpdateViewer the parameter whether the viewer should be update immediatelly
  /// \param theFlag an object AIS presentation type
  /// \returns true if the presentation has been displayed and now it is erased
  bool deactivate(const ModuleBase_IModule::ModuleBase_CustomizeFlag& theFlag,
                  const bool theUpdateViewer);

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

  /// Disable displaying of custom mode
  /// \param theMode a mode to disable
  void disableCustomMode(ModuleBase_IModule::ModuleBase_CustomizeFlag theMode) {
    myDisabledMode = theMode;
    erasePresentation(theMode, false);
  }

  /// Enables disabled custom mode
  void enableCustomModes() { myDisabledMode = -1; }

  //! Redefinition of Events_Listener method to listen a moment that the presentation becomes empty
  virtual void processEvent(const std::shared_ptr<Events_Message>& theMessage);

private:
  /// Creates the AIS operation presentation
  /// \param theFlag an object AIS presentation type
  void initPresentation(const ModuleBase_IModule::ModuleBase_CustomizeFlag& theFlag);

  /// Gets an AIS presentation by the flag or create new if there is no a presentation by the flag
  /// \param theFlag an object AIS presentation type
  /// \param theToCreate a flag if a new presentation should be created
  /// \return wrap of AIS prsentation
  AISObjectPtr getPresentation(const ModuleBase_IModule::ModuleBase_CustomizeFlag& theFlag,
                               const bool theToCreate);

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
  /// \param returns whether the presentation is erased
  bool erasePresentation(const ModuleBase_IModule::ModuleBase_CustomizeFlag& theFlag,
                         const bool theUpdateViewer);

  /// Nullify the operation presentation. For example, it can be useful when the viewer/context
  /// is closed. If this is not performed and the presentation is assigned in another context,
  /// it caused erroneus case because the presentation has linkage to the previous context.
  /// \param theFlag an object AIS presentation type
  void clearPresentation(const ModuleBase_IModule::ModuleBase_CustomizeFlag& theFlag);

  /// Returns presentation color according to the flag
  /// \param theFlag an object AIS presentation type
  /// \return theShapeColor a color
  Quantity_Color getShapeColor(const ModuleBase_IModule::ModuleBase_CustomizeFlag& theFlag);

  /// Removes error shapes presentation
  void clearErrorShape();

private:
  ModuleBase_IWorkshop* myWorkshop; /// current workshop
  FeaturePtr myFeature; /// Reference to a feature object
  bool myPresentationIsEmpty; /// Boolean state about empty presentation
  /// map of presentation type to AIS object
  QMap<ModuleBase_IModule::ModuleBase_CustomizeFlag, AISObjectPtr> myPresentations;
  /// State whether the presentation is activated/deactivated
  QMap<ModuleBase_IModule::ModuleBase_CustomizeFlag, bool> myIsActive;

  int myDisabledMode;

  Handle(AIS_Shape) myErrorShapes;
};

#endif
