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

#include <GeomAPI_ICustomPrs.h>

#include <SelectMgr_AndFilter.hxx>

#include <QString>
#include <QMap>
#include <QObject>
#include <QColor>

class ModuleBase_ViewerPrs;
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
  /// \param theUpdateViewer the parameter whether the viewer should be update immediatelly
  /// \return true if the object visibility state is changed
  bool display(ObjectPtr theObject, bool theUpdateViewer = true);

  /// Display the given AIS object. This object is not added to the displayer internal map of objects
  /// So, it can not be obtained from displayer. This is just a wrap method of OCC display in order
  /// to perform the display with correct flags.
  /// \param theAIS AIOS object to display
  /// \param toActivateInSelectionModes boolean value whether the presentation should be
  /// activated in the current selection modes
  /// \param theUpdateViewer the parameter whether the viewer should be update immediatelly
  /// \return true if the object visibility state is changed
  bool displayAIS(AISObjectPtr theAIS, const bool toActivateInSelectionModes,
                  bool theUpdateViewer = true);

  /// Redisplay the shape if it was displayed
  /// \param theObject an object instance
  /// \param theUpdateViewer the parameter whether the viewer should be update immediatelly
  /// \return true if the object visibility state is changed
  bool redisplay(ObjectPtr theObject, bool theUpdateViewer = true);

  /// Sends and flushes a signal to redisplay all visualized objects.
  void redisplayObjects();

  /// Add presentations to current selection. It unhighlight and deselect the current selection.
  /// The shape and result components are processed in the values. If the presentation shape is not
  /// empty, select it, otherwise select the result.
  /// \param theValues a list of presentation to be selected
  /// \param theUpdateViewer the parameter whether the viewer should be update immediatelly
  void setSelected(const  QList<std::shared_ptr<ModuleBase_ViewerPrs>>& theValues, bool theUpdateViewer = true);

  /// Unselect all objects
  void clearSelected();

  /// Erase the feature and a shape.
  /// \param theObject an object instance
  /// \param theUpdateViewer the parameter whether the viewer should be update immediatelly
  /// \return true if the object visibility state is changed
  bool erase(ObjectPtr theObject, const bool theUpdateViewer = true);

  /// Erase the given AIS object displayed by corresponded display method
  /// \param theAIS instance of AIS object
  /// \param theUpdateViewer the parameter whether the viewer should be update immediatelly
  /// \return true if the object visibility state is changed
  bool eraseAIS(AISObjectPtr theAIS, const bool theUpdateViewer = true);

  /// Erase all presentations
  /// \param theUpdateViewer the parameter whether the viewer should be update immediatelly
  /// \return true if the object visibility state is changed
  bool eraseAll(const bool theUpdateViewer = true);

  /// Deactivates selection of sub-shapes
  /// \param theUpdateViewer the parameter whether the viewer should be update immediatelly
  void closeLocalContexts(const bool theUpdateViewer = true);

  /// Remove default selection filters of the module from the current viewer
  void deactivateSelectionFilters();

  /// \brief Add selection filter
  /// \param theFilter a filter instance
  void addSelectionFilter(const Handle(SelectMgr_Filter)& theFilter);

  /// \brief Remove selection filter
  /// \param theFilter a filter instance
  void removeSelectionFilter(const Handle(SelectMgr_Filter)& theFilter);

  /// Returns true if the selection filter is set to the viewer
  /// \param theFilter a selection filter
  virtual bool hasSelectionFilter(const Handle(SelectMgr_Filter)& theFilter);

  /// Remove all selection filters
  void removeFilters();

  /// Sets a flag to the displayer whether the internal viewer can be updated by 
  /// the updateViewer method call. If it is not enabled, this method do nothing.
  /// This state maintain recurse, if the update is blocked twice or three times, the
  /// viewer will not be updated until it is unblocked necessary times(twice or three in the example).
  /// \param isEnabled a boolean value
  bool enableUpdateViewer(const bool isEnabled);

  /// Returns true if the viewer update is not blocked
  bool isUpdateEnabled() const;

  /// Updates the viewer
  void updateViewer() const;

  /// Activate interactive context
  /// \param theIO an interactive object
  /// \param theMode activation mode
  /// \param theUpdateViewer update viewer flag
  void activateAIS(const Handle(AIS_InteractiveObject)& theIO, const int theMode,
                   const bool theUpdateViewer) const;

  /// Activate interactive context. It is necessary to call ClearOutdatedSelection after deactivation
  /// \param theIO an interactive object
  /// \param theMode a mode to deactivate. When theMode=-1 then all modes will be deactivated
  void deactivateAIS(const Handle(AIS_InteractiveObject)& theIO, const int theMode = -1) const;

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

  /// Deactivates the given objects (not allow selection)
  /// \param theObjList - list of objects which has to be deactivated.
  /// \param theUpdateViewer update viewer flag
  void deactivateObjects(const QObjectPtrList& theObjList,
                         const bool theUpdateViewer = true);

  /// Returns the modes of activation
  /// \param theObject the feature or NULL if it not visualized
  /// \param theModes - modes on which it is activated (can be empty)
  void getModesOfActivation(ObjectPtr theObject, QIntList& theModes);

  /// Returns true if the given object can be selected
  /// \param theObject object to check
  bool isActive(ObjectPtr theObject) const;

  /// Activates in local context displayed outside of the context.
  /// \param theModes - modes on which it has to be activated (can be empty)
  /// \param theObjList - list of objects which has to be activated.
  /// \param theUpdateViewer an update viewer flag
  void activateObjects(const QIntList& theModes, const QObjectPtrList& theObjList,
                       const bool theUpdateViewer = true);

  /// Sets display mode for the given object if this object is displayed
  void setDisplayMode(ObjectPtr theObject, DisplayMode theMode, bool theUpdateViewer = true);

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

  /// Set color on presentation of an object if it is displayed
  /// \param theObject an object 
  /// \param theColor a color which has to be set
  /// \param theUpdateViewer update viewer flag
  /// \return previously defined color on the object
  QColor setObjectColor(ObjectPtr theObject, const QColor& theColor, bool theUpdateViewer = true);

  /// Returns Trihedron object if it is displayed
  Handle(AIS_InteractiveObject) getTrihedron() const;
  
  /// Set trihedron active (used in selection) or non active
  void activateTrihedron(bool theIsActive);

  /// Displays/erases thrihedron in current modes. It will be activated or deactivated
  /// depending on the trihedron visible state and displayer active trihedron state
  void displayTrihedron(bool theToDisplay) const;

  /// Returns true if the trihedron should be activated in current selection modes
  bool isTrihedronActive() const { return myIsTrihedronActive; }

  /// Returns list of currently active selection modes
  /// Selection modes will be returned according to TopAbs_ShapeEnum
  QIntList activeSelectionModes() const;


  /// Converts shape type (TopAbs_ShapeEnum) to selection mode
  /// \param theShapeType a shape type from TopAbs_ShapeEnum
  static int getSelectionMode(int theShapeType);

  /// Return true if the object is visible. If the object is feature, it returns true
  /// if all results of the feature are shown
  /// \param theDisplayer a displayer
  /// \param theObject an object
  /// \return a boolean value
  static bool isVisible(XGUI_Displayer* theDislayer, const ObjectPtr& theObject);

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
  /// \param theUpdateViewer the parameter whether the viewer should be update immediatelly
  /// \return true if the object visibility state is changed
  bool display(ObjectPtr theObject, AISObjectPtr theAIS, bool isShading,
               bool theUpdateViewer = true);

private:
  /// Activates the interactive object in the local context.
  /// \param theIO an interactive object
  /// \param theModes - modes on which it has to be activated (can be empty)
  /// \return a flag is object activated or not
  bool activate(const Handle(AIS_InteractiveObject)& theIO, const QIntList& theModes,
                const bool theUpdateViewer) const;

  /// Deactivates the given object (not allow selection)
  /// \param theObject object to deactivate
  void deactivate(ObjectPtr theObject, const bool theUpdateViewer);

  /// Find a trihedron in a list of displayed presentations and deactivate it.
  /// \param theUpdateViewer an update viewer flag
  void deactivateTrihedron(const bool theUpdateViewer) const;

  /// Opens local context. Does nothing if it is already opened.
  void openLocalContext();

  /// Update the object presentable properties such as color, lines width and other
  /// If the object is result with the color attribute value set, it is used,
  /// otherwise the customize is applyed to the object's feature if it is a custom prs
  /// \param theObject an object instance
  /// \return the true state if there is changes and the presentation is customized
  bool customizeObject(ObjectPtr theObject);

  /// Append the objects in the internal map. Checks whether the map already contains the object
  /// \param theObject an object to display
  /// \param theAIS AIOS object to display
  void appendResultObject(ObjectPtr theObject, AISObjectPtr theAIS);

  /// Returns an information about alredy displayed objects
  /// \return a string representation
  std::string getResult2AISObjectMapInfo() const;

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

  /// Number of blocking of the viewer update. The viewer is updated only if it equals zero
  int myViewerBlockedRecursiveCount;

  // Flag: use trihedgon for selection or not
  bool myIsTrihedronActive;

  /// A flag that update was requested but not done
  mutable bool myNeedUpdate;
};

#endif
