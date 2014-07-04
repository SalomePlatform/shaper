// File:        XGUI_Displayer.h
// Created:     20 Apr 2014
// Author:      Natalia ERMOLAEVA

#ifndef XGUI_Displayer_H
#define XGUI_Displayer_H

#include "XGUI.h"
#include <ModuleBase_Definitions.h>

#include <QString>
#include <boost/shared_ptr.hpp>

#include <GeomAPI_AISObject.h>

#include <TopoDS_Shape.hxx>
#include <AIS_InteractiveObject.hxx>
#include <AIS_InteractiveContext.hxx>
#include <NCollection_List.hxx>

#include <ModelAPI_Feature.h>

#include <XGUI_ViewerPrs.h>

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
  /// Constructor
  /// \param theViewer the viewer
  XGUI_Displayer(XGUI_Workshop* theWorkshop);
  /// Destructor
  virtual ~XGUI_Displayer();

  /// Set AIS_InteractiveContext object in case if it was changed
  /// or can not be initialized in constructor
  void setAISContext(const Handle(AIS_InteractiveContext)& theAIS);

  /// Returns the feature visibility state.
  /// \param theFeature a feature instance
  bool isVisible(FeaturePtr theFeature);

  /// Display the feature. Obtain the visualized object from the feature.
  /// \param theFeature a feature instance
  /// \param isUpdateViewer the parameter whether the viewer should be update immediatelly
  /// Returns true if the Feature succesfully displayed
  bool display(FeaturePtr theFeature, bool isUpdateViewer = true);

  /// Display the feature and a shape. This shape would be associated to the given feature
  /// \param theFeature a feature instance
  /// \param theAIS AIS presentation
  /// \param isUpdateViewer the parameter whether the viewer should be update immediatelly
  /// Returns true if the Feature succesfully displayed
  bool display(FeaturePtr theFeature, boost::shared_ptr<GeomAPI_AISObject> theAIS, bool isUpdateViewer = true);
  
  /// Returns a list of viewer selected presentations
  /// \param theShapeTypeToSkip the shapes with this type will be skipped during the result list build
  /// \return list of presentations
  std::list<XGUI_ViewerPrs> getSelected(const int theShapeTypeToSkip = -1);

  /**
  * Returns list of features currently selected in 3d viewer
  */
  QFeatureList selectedFeatures() const;

  /// Returns a list of viewer highlited presentations
  /// \param theShapeTypeToSkip the shapes with this type will be skipped during the result list build
  /// \return list of presentations
  std::list<XGUI_ViewerPrs> getHighlighted(const int theShapeTypeToSkip = -1);

  /// Display the shape and activate selection of sub-shapes
  /// \param theFeature a feature instance
  /// \param theAIS an AIS object
  /// \param isUpdateViewer the parameter whether the viewer should be update immediatelly
  /// \returns true if the presentation is created
  bool redisplay(FeaturePtr theFeature,
                 boost::shared_ptr<GeomAPI_AISObject> theAIS, 
                 const bool isUpdateViewer = true);

  /** Redisplay the shape if it was displayed
  * \param theFeature a feature instance
  * \param isUpdateViewer the parameter whether the viewer should be update immediatelly
  */
  bool redisplay(FeaturePtr theFeature, bool isUpdateViewer = true);

  /// Redisplay the shape and activate selection of sub-shapes
  /// \param theFeature a feature instance
  /// \param isUpdateViewer the parameter whether the viewer should be update immediatelly
  //void redisplay(Handle(AIS_InteractiveObject) theAIS, const bool isUpdateViewer = true);

  /// Display the shape and activate selection of sub-shapes
  /// \param theFeature a feature instance
  /// \param theShape a shape
  /// \param theMode a list of local selection modes
  /// \param isUpdateViewer the parameter whether the viewer should be update immediatelly
  void activateInLocalContext(FeaturePtr theFeature,
                              const std::list<int>& theModes, const bool isUpdateViewer = true);

  /// Stop the current selection and color the given features to the selection color
  /// \param theFeatures a list of features to be disabled
  /// \param theToStop the boolean state whether it it stopped or non stopped
  /// \param isUpdateViewer the parameter whether the viewer should be update immediatelly
  void stopSelection(const QFeatureList& theFeatures, const bool isStop,
                     const bool isUpdateViewer);

  /**
  * Add presentations which corresponds to the given features to current selection
  * \param theFeatures a list of features to be selected
  * isUpdateViewer the parameter whether the viewer should be update immediatelly
  */
  void setSelected(const QFeatureList& theFeatures, bool isUpdateViewer = true);

  /// Erase the feature and a shape.
  /// \param theFeature a feature instance
  /// \param isUpdateViewer the parameter whether the viewer should be update immediatelly
  void erase(FeaturePtr theFeature, const bool isUpdateViewer = true);

  /// Erase all presentations
  /// \param isUpdateViewer the parameter whether the viewer should be update immediatelly
  //void EraseAll(const bool isUpdateViewer = true);

  /// Erase AIS interactive objects, which has an empty feature in the internal map
  /// \param isUpdateViewer the parameter whether the viewer should be update immediatelly
  void eraseDeletedFeatures(const bool isUpdateViewer = true);

  /// Deactivates selection of sub-shapes
  /// \param isUpdateViewer the parameter whether the viewer should be update immediatelly
  void closeLocalContexts(const bool isUpdateViewer = true);

  /// Updates the viewer
  void updateViewer();

  /// Searches the interactive object by feature
  /// \param theFeature the feature or NULL if it not visualized
  /// \return theIO an interactive object
  boost::shared_ptr<GeomAPI_AISObject> getAISObject(FeaturePtr theFeature) const;

protected:
  /// Searches the feature by interactive object
  /// \param theIO an interactive object
  /// \return feature the feature or NULL if it not visualized
  FeaturePtr getFeature(Handle(AIS_InteractiveObject) theIO) const;
  /// Deactivate local selection
  /// \param isUpdateViewer the state wether the viewer should be updated immediatelly
  void closeAllContexts(const bool isUpdateViewer);

  /// Returns currently installed AIS_InteractiveContext
  Handle(AIS_InteractiveContext) AISContext() const;

protected:
  XGUI_Workshop* myWorkshop;

  typedef std::map<FeaturePtr, boost::shared_ptr<GeomAPI_AISObject> > FeatureToAISMap;
  FeatureToAISMap myFeature2AISObjectMap;
};


#endif
