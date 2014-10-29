// File:        XGUI_Displayer.h
// Created:     20 Apr 2014
// Author:      Natalia ERMOLAEVA

#ifndef XGUI_Displayer_H
#define XGUI_Displayer_H

#include "XGUI.h"

#include <QString>
#include <boost/shared_ptr.hpp>

#include <GeomAPI_AISObject.h>

#include <TopoDS_Shape.hxx>
#include <AIS_InteractiveObject.hxx>
#include <AIS_InteractiveContext.hxx>
#include <NCollection_List.hxx>

#include <ModelAPI_Result.h>

#include <ModuleBase_Definitions.h>
#include <ModuleBase_ViewerPrs.h>

#include <map>
#include <vector>
#include <list>

class XGUI_Viewer;
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
  void display(AISObjectPtr theAIS, bool isUpdate = true);

  /// Stop the current selection and color the given features to the selection color
  /// \param theFeatures a list of features to be disabled
  /// \param theToStop the boolean state whether it it stopped or non stopped
  /// \param isUpdateViewer the parameter whether the viewer should be update immediatelly
  void stopSelection(const QList<ObjectPtr>& theFeatures, const bool isStop,
                     const bool isUpdateViewer);

  /**
   * Add presentations which corresponds to the given features to current selection
   * \param theFeatures a list of features to be selected
   * isUpdateViewer the parameter whether the viewer should be update immediatelly
   */
  void setSelected(const QList<ObjectPtr>& theFeatures, bool isUpdateViewer = true);


  /// Un select all objects
  void clearSelected();

  /// Erase the feature and a shape.
  /// \param theFeature a feature instance
  /// \param isUpdateViewer the parameter whether the viewer should be update immediatelly
  void erase(ObjectPtr theObject, const bool isUpdateViewer = true);

  /// Erase the given AIS object displayed by corresponded display method
  void erase(AISObjectPtr theAIS, const bool isUpdate = true);

  /// Erase all presentations
  /// \param isUpdateViewer the parameter whether the viewer should be update immediatelly
  void eraseAll(const bool isUpdateViewer = true);

  /// Erase AIS interactive objects, which has an empty feature in the internal map
  /// \param isUpdateViewer the parameter whether the viewer should be update immediatelly
  void eraseDeletedResults(const bool isUpdateViewer = true);

  /// Opens local context. Does nothing if it is already opened.
  void openLocalContext();

  /// Deactivates selection of sub-shapes
  /// \param isUpdateViewer the parameter whether the viewer should be update immediatelly
  void closeLocalContexts(const bool isUpdateViewer = true);

  /*
  * Set modes of selections. Selection mode has to be defined by TopAbs_ShapeEnum.
  * It doesn't manages a local context
  * \param theModes - list of selection modes. If the list is empty then all selectoin modes will be cleared.
  */
  void setSelectionModes(const QIntList& theModes);

  void addSelectionFilter(const Handle(SelectMgr_Filter)& theFilter);

  void removeSelectionFilter(const Handle(SelectMgr_Filter)& theFilter);

  /// Updates the viewer
  void updateViewer();

  /// Searches the interactive object by feature
  /// \param theFeature the feature or NULL if it not visualized
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

  /// Returns true if the given object can be selected
  bool isActive(ObjectPtr theObject) const;

  /// Activates in local context displayed outside of the context.
  /// \param theModes - modes on which it has to be activated (can be empty)
  void activateObjectsOutOfContext(const QIntList& theModes);

  /// Activates in local context displayed outside of the context.
  void deactivateObjectsOutOfContext();

  /// Sets display mode for the given object if this object is displayed
  void setDisplayMode(ObjectPtr theObject, DisplayMode theMode, bool toUpdate = true);

  /// Returns current display mode for the given object.
  /// If object is not displayed then returns NoMode.
  DisplayMode displayMode(ObjectPtr theObject) const;


  int objectsCount() const { return myResult2AISObjectMap.size(); }

 protected:
  /// Deactivate local selection
  /// \param isUpdateViewer the state wether the viewer should be updated immediatelly
  void closeAllContexts(const bool isUpdateViewer);

  /// Returns currently installed AIS_InteractiveContext
  Handle(AIS_InteractiveContext) AISContext() const;

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

 protected:
  XGUI_Workshop* myWorkshop;

  typedef std::map<ObjectPtr, AISObjectPtr> ResultToAISMap;
  ResultToAISMap myResult2AISObjectMap;
};

#endif
