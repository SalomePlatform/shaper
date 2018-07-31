// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#ifndef XGUI_Displayer_H
#define XGUI_Displayer_H

#include "XGUI.h"

#include <GeomAPI_AISObject.h>
#include <GeomAPI_ICustomPrs.h>
#include <GeomAPI_Pln.h>

#include <ModelAPI_Result.h>

#include <ModuleBase_Definitions.h>

#include <AIS_InteractiveObject.hxx>
#include <AIS_InteractiveContext.hxx>
#include <NCollection_Map.hxx>
#include <NCollection_DataMap.hxx>
#include <SelectMgr_AndFilter.hxx>
#include <TopoDS_Shape.hxx>

#include <QColor>
#include <QMap>
#include <QObject>
#include <QString>

class ModuleBase_ViewerPrs;
class ModelAPI_Feature;
class XGUI_SelectionActivate;
class XGUI_Workshop;

#ifdef TINSPECTOR
class VInspectorAPI_CallBack;
#endif


class XGUI_TwoSidePresentationMap
{
public:
  ~XGUI_TwoSidePresentationMap() { clear(); }

  /// Add new values pair to the map
  /// \param theObj an object
  /// \param theAIS a corresponded presentation
  bool add(const ObjectPtr& theObj, const AISObjectPtr& theAIS)
  {
    if (myResultToAISMap.contains(theObj))
      return false;
    Handle(AIS_InteractiveObject) anAIS = theAIS->impl<Handle(AIS_InteractiveObject)>();
    myResultToAISMap[theObj] = anAIS;
    myAIStoResultMap[anAIS] = theObj;
    return true;
  }

  /// Removes values by object
  /// \param theObj an object
  bool remove(const ObjectPtr& theObj)
  {
    if (!myResultToAISMap.contains(theObj))
      return false;
    Handle(AIS_InteractiveObject) aAIS = myResultToAISMap[theObj];
    myResultToAISMap.remove(theObj);
    myAIStoResultMap.remove(aAIS);
    return true;
  }

  /// Removes values by presentation
  /// \param theAIS a presentation
  bool remove(const AISObjectPtr& theAIS)
  {
    Handle(AIS_InteractiveObject) anAIS = theAIS->impl<Handle(AIS_InteractiveObject)>();
    if (!myAIStoResultMap.contains(anAIS))
      return false;
    ObjectPtr aObj = myAIStoResultMap[anAIS];
    myResultToAISMap.remove(aObj);
    myAIStoResultMap.remove(anAIS);
    return true;
  }

  /// Removes all values
  void clear()
  {
    myResultToAISMap.clear();
    myAIStoResultMap.clear();
  }

  /// Returns presentation by object
  /// \param theObj an object
  AISObjectPtr value(const ObjectPtr& theObj) const
  {
    if (myResultToAISMap.contains(theObj)) {
      Handle(AIS_InteractiveObject) anAIS = myResultToAISMap[theObj];
      AISObjectPtr anAISObj = AISObjectPtr(new GeomAPI_AISObject());
      anAISObj->setImpl(new Handle(AIS_InteractiveObject)(anAIS));
      return anAISObj;
    }
    return AISObjectPtr();
  }

  /// Returns object by presentation
  /// \param theAIS a presentation
  ObjectPtr value(const AISObjectPtr& theAIS) const
  {
    Handle(AIS_InteractiveObject) anAIS = theAIS->impl<Handle(AIS_InteractiveObject)>();
    if (myAIStoResultMap.contains(anAIS))
      return myAIStoResultMap[anAIS];
    return ObjectPtr();
  }

  /// Returns object by presentation
  /// \param theAIS a presentation
  ObjectPtr value(const Handle(AIS_InteractiveObject)& theAIS) const
  {
    if (myAIStoResultMap.contains(theAIS))
      return myAIStoResultMap[theAIS];
    return ObjectPtr();
  }

  /// Returns number of values
  int size() const { return myResultToAISMap.size(); }

  /// Returns list of objects
  QObjectPtrList objects() const { return myResultToAISMap.keys(); }

  /// returns list of presentations
  QList<Handle(AIS_InteractiveObject)> presentations() const { return myAIStoResultMap.keys(); }

  /// Returns true if the Map contains the object
  /// \param theObj an object
  bool contains(const ObjectPtr& theObj) const { return myResultToAISMap.contains(theObj); }

  /// Returns true if the Map contains the presentation
  /// \param theAIS a presentation
  bool contains(const AISObjectPtr& theAIS) const
  {
    Handle(AIS_InteractiveObject) anAIS = theAIS->impl<Handle(AIS_InteractiveObject)>();
    return myAIStoResultMap.contains(anAIS);
  }

private:
  QMap<ObjectPtr, Handle(AIS_InteractiveObject)> myResultToAISMap;
  QMap<Handle(AIS_InteractiveObject), ObjectPtr> myAIStoResultMap;
};


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
     NoMode = -1, ///< Mode is not defined
     Wireframe, ///< Wireframe display mode
     Shading ///< Shading display mode
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

  /// Display the given AIS object.
  /// This object is not added to the displayer internal map of objects
  /// So, it can not be obtained from displayer. This is just a wrap method of OCC display in
  /// order to perform the display with correct flags.
  /// \param theAIS AIOS object to display
  /// \param toActivateInSelectionModes boolean value whether the presentation should be
  /// activated in the current selection modes
  /// \param theDisplayMode mode how the presentation should be displayed
  /// \param theUpdateViewer the parameter whether the viewer should be update immediatelly
  /// \return true if the object visibility state is changed
  bool displayAIS(AISObjectPtr theAIS, const bool toActivateInSelectionModes,
                  const Standard_Integer theDisplayMode = 0, bool theUpdateViewer = true);

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
  void setSelected(const  QList<std::shared_ptr<ModuleBase_ViewerPrs>>& theValues,
                   bool theUpdateViewer = true);

  /// Unselect all objects
  /// \param theUpdateViewer the parameter whether the viewer should be update immediatelly
  /// \param theUpdateViewer the parameter to update viewer
  void clearSelected(const bool theUpdateViewer = true);

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

  /// Remove default selection filters of the module from the current viewer
  /// \param theAddFilterOnly if is not 'true' it will deactivate all fiters in viewer
  void deactivateSelectionFilters(const bool theAddFilterOnly = true);

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
  /// viewer will not be updated until it is unblocked necessary times
  /// (twice or three in the example).
  /// \param isEnabled a boolean value
  bool enableUpdateViewer(const bool isEnabled);

  /// Returns true if the viewer update is not blocked
  bool isUpdateEnabled() const;

  /// Updates the viewer
  void updateViewer() const;

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
  QObjectPtrList displayedObjects() const { return myResult2AISObjectMap.objects(); }

  /// Returns list of displayed objects
  QList<Handle(AIS_InteractiveObject)> displayedPresentations() const
  {
    return myResult2AISObjectMap.presentations();
  }

  /// Returns true if the given object can be shown in shaded mode
  /// \param theObject object to check
  bool canBeShaded(ObjectPtr theObject) const;

  /// Set color on presentation of an object if it is displayed
  /// \param theObject an object
  /// \param theColor a color which has to be set
  /// \param theUpdateViewer update viewer flag
  /// \return previously defined color on the object
  QColor setObjectColor(ObjectPtr theObject, const QColor& theColor, bool theUpdateViewer = true);

  /// Displays/erases thrihedron in current modes. It will be activated or deactivated
  /// depending on the trihedron visible state and displayer active trihedron state
  void displayTrihedron(bool theToDisplay) const;

#ifdef TINSPECTOR
  void setCallBack(const Handle(VInspectorAPI_CallBack)& theCallBack)
    { myVCallBack = theCallBack; }
  Handle(VInspectorAPI_CallBack) getCallBack() const { return myVCallBack; }
#endif

  /// Return true if the object is visible. If the object is feature, it returns true
  /// if all results of the feature are shown
  /// \param theDisplayer a displayer
  /// \param theObject an object
  /// \return a boolean value
  static bool isVisible(XGUI_Displayer* theDisplayer, const ObjectPtr& theObject);


  /// Returns screen plane of active view
  GeomPlanePtr getScreenPlane() const;

  /// Returns scale of active view
  double getViewScale() const;

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

#ifdef _DEBUG
  /// Returns an information about alredy displayed objects
  /// \return a string representation
  std::string getResult2AISObjectMapInfo() const;
#endif

  /// Returns container of visible presentations for the object. For a feature object,
  /// the feature results are processed also. The presentations map is not cleared inside.
  /// \param theObject a feature or result
  /// \param thePresentations result map of presentations
  void getPresentations(const ObjectPtr& theObject,
                        NCollection_Map<Handle(AIS_InteractiveObject)>& thePresentations);

  /// Sets the shapes selected in the context. It contains logic of the similar method
  /// in OCCT but improved for performance. The modification is to iterates by a list
  /// of owners in the context only once.
  /// \param theContext a viewer context. It has opened local context
  /// \param theShapesToBeSelected a map of shapes. Owner's shape is searched in the map and the
  /// owner is selected if it is found there.
  /// Only first owner is processed(according to OCCT logic)
  static void AddOrRemoveSelectedShapes(Handle(AIS_InteractiveContext) theContext,
    const NCollection_DataMap<TopoDS_Shape,
      NCollection_Map<Handle(AIS_InteractiveObject)>>& theShapesToBeSelected);

protected:
  XGUI_SelectionActivate* selectionActivate() const;

protected:
  XGUI_Workshop* myWorkshop; ///< Reference to workshop
#ifdef TINSPECTOR
  Handle(VInspectorAPI_CallBack) myVCallBack;
#endif
  Handle(SelectMgr_AndFilter) myAndFilter; ///< A container for selection filters

  /// A default custom presentation, which is used if the displayed feature is not
  /// a custom presentation
  GeomCustomPrsPtr myCustomPrs;

  /// Definition of a type of map which defines correspondance between objects and presentations
  XGUI_TwoSidePresentationMap myResult2AISObjectMap; ///< A map of displayed objects

  /// Number of blocking of the viewer update. The viewer is updated only if it is zero
  int myViewerBlockedRecursiveCount;

  bool myIsFirstAISContextUse; ///< Flag: first asking of AIS context: trihedron activation
  mutable bool myNeedUpdate; ///< A flag that update was requested but not done
};

#endif
