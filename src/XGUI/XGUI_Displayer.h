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

#include <GeomAPI_ICustomPrs.h>

#include <SelectMgr_AndFilter.hxx>

#include <QString>
#include <QMap>
#include <QObject>

class ModelAPI_Feature;
class XGUI_Workshop;

/**\class XGUI_Displayer
 * \ingroup GUI
 * \brief Displayer. Provides mechanizm of display/erase of objects in the viewer
 */
class XGUI_EXPORT XGUI_Displayer: public QObject
{
  Q_OBJECT
 public:
   /// \enum DisplayMode display mode
   enum DisplayMode { 
     /// Mode is not defined
     NoMode = -1, 
     /// Wireframe display mode
     Wireframe, 
     /// Shading display mode
     Shading      
   };

  /// Constructor
  /// \param theWorkshop a workshop instance
  XGUI_Displayer(XGUI_Workshop* theWorkshop);

  /// Destructor
  virtual ~XGUI_Displayer();

  /// Returns the feature visibility state.
  /// \param theObject an object instance
  bool isVisible(ObjectPtr theObject) const;

  /// Display the feature. Obtain the visualized object from the feature.
  /// \param theObject an object to display
  /// \param isUpdateViewer the parameter whether the viewer should be update immediatelly
  /// Returns true if the Feature succesfully displayed
  void display(ObjectPtr theObject, bool isUpdateViewer = true);

  /// Display the given AIS object. To hide this object use corresponde erase method
  /// \param theAIS AIOS object to display
  /// \param isUpdate the parameter whether the viewer should be update immediatelly
  void displayAIS(AISObjectPtr theAIS, bool isUpdate = true);

  /**
   * Add presentations which corresponds to the given features to current selection
   * \param theFeatures a list of features to be selected
   * \param isUpdateViewer the parameter whether the viewer should be update immediatelly
   */
  void setSelected(const QObjectPtrList& theFeatures, bool isUpdateViewer = true);


  /// Unselect all objects
  void clearSelected();

  /// Erase the feature and a shape.
  /// \param theObject an object instance
  /// \param isUpdateViewer the parameter whether the viewer should be update immediatelly
  void erase(ObjectPtr theObject, const bool isUpdateViewer = true);

  /// Erase the given AIS object displayed by corresponded display method
  /// \param theAIS instance of AIS object
  /// \param isUpdate the parameter whether the viewer should be update immediatelly
  void eraseAIS(AISObjectPtr theAIS, const bool isUpdate = true);

  /// Erase all presentations
  /// \param isUpdateViewer the parameter whether the viewer should be update immediatelly
  void eraseAll(const bool isUpdateViewer = true);

  /// Deactivates selection of sub-shapes
  /// \param isUpdateViewer the parameter whether the viewer should be update immediatelly
  void closeLocalContexts(const bool isUpdateViewer = true);

  /// \brief Add selection filter
  /// \param theFilter a filter instance
  void addSelectionFilter(const Handle(SelectMgr_Filter)& theFilter);

  /// \brief Remove selection filter
  /// \param theFilter a filter instance
  void removeSelectionFilter(const Handle(SelectMgr_Filter)& theFilter);

  /// Remove all selection filters
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
  /// \param theObject the object or presentable feature
  /// \return theIO an interactive object
  AISObjectPtr getAISObject(ObjectPtr theObject) const;

  /// Searches the feature by interactive object
  /// \param theIO an interactive object
  /// \return feature the feature or NULL if it not visualized
  ObjectPtr getObject(const AISObjectPtr& theIO) const;

  /// Searches the feature by interactive object
  /// \param theIO an interactive object
  /// \return corresponded object or NULL if it not found
  ObjectPtr getObject(const Handle(AIS_InteractiveObject)& theIO) const;

  /// Deactivates the given object (not allow selection)
  /// \param theObject object to deactivate
  void deactivate(ObjectPtr theObject);

  /// Activates the given object (it can be selected)
  /// \param theObject object to activate
  /// \param theModes - modes on which it has to be activated (can be empty)
  //void activate(ObjectPtr theObject, const QIntList& theModes);

  /// Returns the modes of activation
  /// \param theObject the feature or NULL if it not visualized
  /// \param theModes - modes on which it is activated (can be empty)
  void getModesOfActivation(ObjectPtr theObject, QIntList& theModes);

  /// Activates the given object with default modes
  /// \param theObject object to activate
  //void activate(ObjectPtr theObject);

  /// Returns true if the given object can be selected
  /// \param theObject object to check
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
  /// \param theObject object to check
  DisplayMode displayMode(ObjectPtr theObject) const;

  /// Displays only objects listed in the list
  /// \param theList list of objects
  void showOnly(const QObjectPtrList& theList);

  /// Returns number of displayed objects
  int objectsCount() const { return myResult2AISObjectMap.size(); }

  /// Returns list of displayed objects
  QObjectPtrList displayedObjects() const { return myResult2AISObjectMap.keys(); }

  /// Returns list of displayed objects
  QList<AISObjectPtr> displayedPresentations() const { return myResult2AISObjectMap.values(); }

  /// Returns true if the given object can be shown in shaded mode
  /// \param theObject object to check
  bool canBeShaded(ObjectPtr theObject) const;


signals:
  /// Signal on object display
  /// \param theObject a data object
  /// \param theAIS a presentation object
  void objectDisplayed(ObjectPtr theObject, AISObjectPtr theAIS);

  /// Signal on before object erase
  /// \param theObject a data object
  /// \param theAIS a presentation object
  void beforeObjectErase(ObjectPtr theObject, AISObjectPtr theAIS);

 protected:
  /// Returns currently installed AIS_InteractiveContext
  Handle(AIS_InteractiveContext) AISContext() const;

  /// Returns the viewer context top filter. If there is no a filter, it is created and set into
  /// The context should have only this filter inside. Other filters should be add to the filter
  Handle(SelectMgr_AndFilter) GetFilter();

  /// Display the feature and a shape. This shape would be associated to the given feature
  /// \param theObject an object instance
  /// \param theAIS AIS presentation
  /// \param isShading flag to show in shading mode
  /// \param isUpdateViewer the parameter whether the viewer should be update immediatelly
  /// \return true if the object is succesfully displayed
  void display(ObjectPtr theObject, AISObjectPtr theAIS, bool isShading,
               bool isUpdateViewer = true);

  /** Redisplay the shape if it was displayed
   * \param theObject an object instance
   * \param isUpdateViewer the parameter whether the viewer should be update immediatelly
   */
  void redisplay(ObjectPtr theObject, bool isUpdateViewer = true);

  /// Activates the interactive object in the local context.
  /// \param theIO an interactive object
  /// \param theModes - modes on which it has to be activated (can be empty)
  void activate(const Handle(AIS_InteractiveObject)& theIO, const QIntList& theModes) const;

  /// Opens local context. Does nothing if it is already opened.
  void openLocalContext();

  /** Update the object presentable properties such as color, lines width and other
   * If the object is result with the color attribute value set, it is used,
   * otherwise the customize is applyed to the object's feature if it is a custom prs
   * \param theObject an object instance
   * \return the true state if there is changes and the presentation is customized
   */
  bool customizeObject(ObjectPtr theObject);

 protected:
   /// Reference to workshop
  XGUI_Workshop* myWorkshop;

  /// A container for selection filters
  Handle(SelectMgr_AndFilter) myAndFilter;

  /// A default custom presentation, which is used if the displayed feature is not a custom presentation
  GeomCustomPrsPtr myCustomPrs;

  /// Definition of a type of map which defines correspondance between objects and presentations
  typedef QMap<ObjectPtr, AISObjectPtr> ResultToAISMap;

  /// A map of displayed objects
  ResultToAISMap myResult2AISObjectMap;

  /// Selection modes installed for external objects in local context
  QIntList myActiveSelectionModes;

  /// the enable update viewer flag
  bool myEnableUpdateViewer;  
};

#endif
