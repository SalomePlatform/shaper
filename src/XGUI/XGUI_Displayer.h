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
  void display(boost::shared_ptr<GeomAPI_AISObject> theAIS, bool isUpdate = true);

  /// Redisplay the shape and activate selection of sub-shapes
  /// \param theFeature a feature instance
  /// \param isUpdateViewer the parameter whether the viewer should be update immediatelly
  //void redisplay(Handle(AIS_InteractiveObject) theAIS, const bool isUpdateViewer = true);

  /// Display the shape and activate selection of sub-shapes
  /// \param theFeature a feature instance
  /// \param theShape a shape
  /// \param theMode a list of local selection modes
  /// \param isUpdateViewer the parameter whether the viewer should be update immediatelly
  void activateInLocalContext(ObjectPtr theFeature, const std::list<int>& theModes,
                              const bool isUpdateViewer = true);

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

  /// Erase the feature and a shape.
  /// \param theFeature a feature instance
  /// \param isUpdateViewer the parameter whether the viewer should be update immediatelly
  void erase(ObjectPtr theObject, const bool isUpdateViewer = true);

  /// Erase the given AIS object displayed by corresponded display method
  void erase(boost::shared_ptr<GeomAPI_AISObject> theAIS, const bool isUpdate = true);

  /// Erase all presentations
  /// \param isUpdateViewer the parameter whether the viewer should be update immediatelly
  void eraseAll(const bool isUpdateViewer = true);

  /// Erase AIS interactive objects, which has an empty feature in the internal map
  /// \param isUpdateViewer the parameter whether the viewer should be update immediatelly
  void eraseDeletedResults(const bool isUpdateViewer = true);

  void openLocalContext();

  /// Deactivates selection of sub-shapes
  /// \param isUpdateViewer the parameter whether the viewer should be update immediatelly
  void closeLocalContexts(const bool isUpdateViewer = true);

  /// Updates the viewer
  void updateViewer();

  /// Searches the interactive object by feature
  /// \param theFeature the feature or NULL if it not visualized
  /// \return theIO an interactive object
  boost::shared_ptr<GeomAPI_AISObject> getAISObject(ObjectPtr theFeature) const;

  /// Searches the feature by interactive object
  /// \param theIO an interactive object
  /// \return feature the feature or NULL if it not visualized
  ObjectPtr getObject(Handle(AIS_InteractiveObject) theIO) const;

  /// Deactivates the given object (not allow selection)
  void deactivate(ObjectPtr theFeature);

  /// Activates the given object (it can be selected)
  void activate(ObjectPtr theFeature);

  /// Returns true if the given object can be selected
  bool isActive(ObjectPtr theObject) const;

  /// Activates in local context displayed outside of the context.
  /// \param theModes - selection modes to activate
  /// \param theFilter - filter for selection
  void activateObjectsOutOfContext(const std::list<int>& theModes, 
                                   Handle(SelectMgr_Filter) theFilter);

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
  void display(ObjectPtr theObject, boost::shared_ptr<GeomAPI_AISObject> theAIS, bool isShading,
               bool isUpdateViewer = true);

  /// Display the shape and activate selection of sub-shapes
  /// \param theFeature a feature instance
  /// \param theAIS an AIS object
  /// \param isUpdateViewer the parameter whether the viewer should be update immediatelly
  /// \returns true if the presentation is created
  //bool redisplay(ObjectPtr theObject,
  //               boost::shared_ptr<GeomAPI_AISObject> theAIS, 
  //               const bool isUpdateViewer = true);

  /** Redisplay the shape if it was displayed
   * \param theFeature a feature instance
   * \param isUpdateViewer the parameter whether the viewer should be update immediatelly
   */
  void redisplay(ObjectPtr theObject, bool isUpdateViewer = true);

 protected:
  XGUI_Workshop* myWorkshop;

  typedef std::map<ObjectPtr, boost::shared_ptr<GeomAPI_AISObject> > ResultToAISMap;
  ResultToAISMap myResult2AISObjectMap;
};

#endif
