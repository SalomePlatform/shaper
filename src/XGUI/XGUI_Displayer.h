// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        XGUI_Displayer.h
// Created:     20 Apr 2014
// Author:      Natalia ERMOLAEVA

#ifndef XGUI_Displayer_H
#define XGUI_Displayer_H

#include "XGUI.h"

#include <GeomAPI_AISObject.h>
#include <TopoDS_Shape.hxx>
#include <AIS_InteractiveObject.hxx>
#include <AIS_InteractiveContext.hxx>
#include <NCollection_List.hxx>

#include <ModelAPI_Result.h>

#include <ModuleBase_Definitions.h>
#include <ModuleBase_ViewerPrs.h>

#include <SelectMgr_AndFilter.hxx>

#include <QString>
#include <QMap>

class ModelAPI_Feature;
class XGUI_Workshop;

/**\class XGUI_Displayer
 * \ingroup GUI
 * \brief Displayer. Provides mechanizm of display/erase of objects in the viewer
 */
class XGUI_EXPORT XGUI_Displayer
{
 public:
   enum DisplayMode { NoMode = -1, Wireframe, Shading };

  /// Constructor
  /// \param theViewer the viewer
  XGUI_Displayer(XGUI_Workshop* theWorkshop);
  /// Destructor
  virtual ~XGUI_Displayer();

  /// Returns the feature visibility state.
  /// \param theFeature a feature instance
  bool isVisible(ObjectPtr theObject) const;

  /// Display the feature. Obtain the visualized object from the feature.
  /// \param theFeature a feature instance
  /// \param isUpdateViewer the parameter whether the viewer should be update immediatelly
  /// Returns true if the Feature succesfully displayed
  void display(ObjectPtr theObject, bool isUpdateViewer = true);

  /// Display the given AIS object. To hide this object use corresponde erase method
  void displayAIS(AISObjectPtr theAIS, bool isUpdate = true);

  /**
   * Add presentations which corresponds to the given features to current selection
   * \param theFeatures a list of features to be selected
   * isUpdateViewer the parameter whether the viewer should be update immediatelly
   */
  void setSelected(const QObjectPtrList& theFeatures, bool isUpdateViewer = true);


  /// Un select all objects
  void clearSelected();

  /// Erase the feature and a shape.
  /// \param theFeature a feature instance
  /// \param isUpdateViewer the parameter whether the viewer should be update immediatelly
  void erase(ObjectPtr theObject, const bool isUpdateViewer = true);

  /// Erase the given AIS object displayed by corresponded display method
  void eraseAIS(AISObjectPtr theAIS, const bool isUpdate = true);

  /// Erase all presentations
  /// \param isUpdateViewer the parameter whether the viewer should be update immediatelly
  void eraseAll(const bool isUpdateViewer = true);

  /// Deactivates selection of sub-shapes
  /// \param isUpdateViewer the parameter whether the viewer should be update immediatelly
  void closeLocalContexts(const bool isUpdateViewer = true);

  void addSelectionFilter(const Handle(SelectMgr_Filter)& theFilter);

  void removeSelectionFilter(const Handle(SelectMgr_Filter)& theFilter);

  void removeFilters();

  /**
   * Sets a flag to the displayer whether the internal viewer can be updated by 
   * the updateViewer method call. If it is not enabled, this method do nothing
   * \param isEnabled a boolean value
   */
  bool enableUpdateViewer(const bool isEnabled);

  /// Updates the viewer
  void updateViewer();

  /// Searches the interactive object by feature
  /// \param theFeature the object or presentable feature
  /// \return theIO an interactive object
  AISObjectPtr getAISObject(ObjectPtr theFeature) const;

  /// Searches the feature by interactive object
  /// \param theIO an interactive object
  /// \return feature the feature or NULL if it not visualized
  ObjectPtr getObject(const AISObjectPtr& theIO) const;
  ObjectPtr getObject(const Handle(AIS_InteractiveObject)& theIO) const;

  /// Deactivates the given object (not allow selection)
  void deactivate(ObjectPtr theFeature);

  /// Activates the given object (it can be selected)
  /// \param theModes - modes on which it has to be activated (can be empty)
  void activate(ObjectPtr theFeature, const QIntList& theModes);

  /// Returns the modes of activation
  /// \param theFeature the feature or NULL if it not visualized
  /// \param theModes - modes on which it is activated (can be empty)
  void getModesOfActivation(ObjectPtr theObject, QIntList& theModes);

  /// Activates the given object with default modes
  void activate(ObjectPtr theFeature);

  /// Returns true if the given object can be selected
  bool isActive(ObjectPtr theObject) const;

  /// Activates in local context displayed outside of the context.
  /// \param theModes - modes on which it has to be activated (can be empty)
  void activateObjects(const QIntList& theModes);

  /// Activates in local context displayed outside of the context.
  void deactivateObjects();

  /// Sets display mode for the given object if this object is displayed
  void setDisplayMode(ObjectPtr theObject, DisplayMode theMode, bool toUpdate = true);

  /// Returns current display mode for the given object.
  /// If object is not dis played then returns NoMode.
  DisplayMode displayMode(ObjectPtr theObject) const;


  /// Displays only objects listed in the list
  void showOnly(const QObjectPtrList& theList);

  /// Returns number of displayed objects
  int objectsCount() const { return myResult2AISObjectMap.size(); }

  QObjectPtrList displayedObjects() const { return myResult2AISObjectMap.keys(); }

  /// Returns true if the given object can be shown in shaded mode
  bool canBeShaded(ObjectPtr theObject) const;

 protected:
  /// Returns currently installed AIS_InteractiveContext
  Handle(AIS_InteractiveContext) AISContext() const;

  /// Returns the viewer context top filter. If there is no a filter, it is created and set into
  /// The context should have only this filter inside. Other filters should be add to the filter
  Handle(SelectMgr_AndFilter) GetFilter();

  /// Display the feature and a shape. This shape would be associated to the given feature
  /// \param theFeature a feature instance
  /// \param theAIS AIS presentation
  /// \param isUpdateViewer the parameter whether the viewer should be update immediatelly
  /// Returns true if the Feature succesfully displayed
  void display(ObjectPtr theObject, AISObjectPtr theAIS, bool isShading,
               bool isUpdateViewer = true);

  /// Display the shape and activate selection of sub-shapes
  /// \param theFeature a feature instance
  /// \param theAIS an AIS object
  /// \param isUpdateViewer the parameter whether the viewer should be update immediatelly
  /// \returns true if the presentation is created
  //bool redisplay(ObjectPtr theObject,
  //               AISObjectPtr theAIS, 
  //               const bool isUpdateViewer = true);

  /** Redisplay the shape if it was displayed
   * \param theFeature a feature instance
   * \param isUpdateViewer the parameter whether the viewer should be update immediatelly
   */
  void redisplay(ObjectPtr theObject, bool isUpdateViewer = true);

  /// Opens local context. Does nothing if it is already opened.
  void openLocalContext();

 protected:
  XGUI_Workshop* myWorkshop;

  Handle(SelectMgr_AndFilter) myAndFilter;

  typedef QMap<ObjectPtr, AISObjectPtr> ResultToAISMap;
  ResultToAISMap myResult2AISObjectMap;

  // A flag of initialization of external objects selection
  //bool myUseExternalObjects;
  // Selection modes installed for external objects in local context
  QIntList myActiveSelectionModes;

  bool myEnableUpdateViewer;  /// the enable update viewer flag
};

#endif
