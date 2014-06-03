// File:        XGUI_Displayer.h
// Created:     20 Apr 2014
// Author:      Natalia ERMOLAEVA

#ifndef XGUI_Displayer_H
#define XGUI_Displayer_H

#include "XGUI.h"

#include <QString>
#include <boost/shared_ptr.hpp>

#include <TopoDS_Shape.hxx>
#include <AIS_InteractiveObject.hxx>
#include <AIS_InteractiveContext.hxx>
#include <NCollection_List.hxx>

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
  bool IsVisible(boost::shared_ptr<ModelAPI_Feature> theFeature);

  /// Display the feature. Obtain the visualized object from the feature.
  /// \param theFeature a feature instance
  /// \param isUpdateViewer the parameter whether the viewer should be update immediatelly
  void Display(boost::shared_ptr<ModelAPI_Feature> theFeature, const bool isUpdateViewer = true);

  /// Display the feature and a shape. This shape would be associated to the given feature
  /// \param theFeature a feature instance
  /// \param theShape a shape
  /// \param isUpdateViewer the parameter whether the viewer should be update immediatelly
  //void Display(boost::shared_ptr<ModelAPI_Feature> theFeature, const TopoDS_Shape& theShape,
  //             const bool isUpdateViewer = true);
  
  /// Returns a list of viewer selected presentations
  /// \param theShapeTypeToSkip the shapes with this type will be skipped during the result list build
  /// \return list of presentations
  std::list<XGUI_ViewerPrs> GetSelected(const int theShapeTypeToSkip = -1);

  /// Returns a list of viewer highlited presentations
  /// \param theShapeTypeToSkip the shapes with this type will be skipped during the result list build
  /// \return list of presentations
  std::list<XGUI_ViewerPrs> GetHighlighted(const int theShapeTypeToSkip = -1);

  /// Display the shape and activate selection of sub-shapes
  /// \param theFeature a feature instance
  /// \param theAIS an AIS object
  /// \param isUpdateViewer the parameter whether the viewer should be update immediatelly
  /// \returns true if the presentation is created
  bool Redisplay(boost::shared_ptr<ModelAPI_Feature> theFeature,
                 Handle(AIS_InteractiveObject) theAIS,
                 const int theSelectionMode, const bool isUpdateViewer = true);

  /// Redisplay the shape and activate selection of sub-shapes
  /// \param theFeature a feature instance
  /// \param isUpdateViewer the parameter whether the viewer should be update immediatelly
  //void Redisplay(Handle(AIS_InteractiveObject) theAIS, const bool isUpdateViewer = true);

  /// Display the shape and activate selection of sub-shapes
  /// \param theFeature a feature instance
  /// \param theShape a shape
  /// \param theMode a list of local selection modes
  /// \param isUpdateViewer the parameter whether the viewer should be update immediatelly
  void ActivateInLocalContext(boost::shared_ptr<ModelAPI_Feature> theFeature,
                              const std::list<int>& theModes, const bool isUpdateViewer = true);

  /// Stop the current selection and color the given features to the selection color
  /// \param theFeatures a list of features to be disabled
  /// \param theToStop the boolean state whether it it stopped or non stopped
  /// \param isUpdateViewer the parameter whether the viewer should be update immediatelly
  void StopSelection(const std::list<XGUI_ViewerPrs>& theFeatures, const bool isStop,
                     const bool isUpdateViewer);

  /// Set the features are selected
  /// \param theFeatures a list of features to be selected
  /// \param isUpdateViewer the parameter whether the viewer should be update immediatelly
  void SetSelected(const std::list<XGUI_ViewerPrs>& theFeatures, const bool isUpdateViewer);

  /// Erase the feature and a shape.
  /// \param theFeature a feature instance
  /// \param isUpdateViewer the parameter whether the viewer should be update immediatelly
  void Erase(boost::shared_ptr<ModelAPI_Feature> theFeature, const bool isUpdateViewer = true);

  /// Erase all presentations
  /// \param isUpdateViewer the parameter whether the viewer should be update immediatelly
  void EraseAll(const bool isUpdateViewer = true);

  /// Erase AIS interactive objects, which has an empty feature in the internal map
  /// \param isUpdateViewer the parameter whether the viewer should be update immediatelly
  void EraseDeletedFeatures(const bool isUpdateViewer = true);

  /// Deactivates selection of sub-shapes
  /// \param isUpdateViewer the parameter whether the viewer should be update immediatelly
  void CloseLocalContexts(const bool isUpdateViewer = true);

  /// Updates the viewer
  void UpdateViewer();

  /// Searches the interactive object by feature
  /// \param theFeature the feature or NULL if it not visualized
  /// \return theIO an interactive object
  Handle(AIS_InteractiveObject) GetAISObject(boost::shared_ptr<ModelAPI_Feature> theFeature) const;

protected:
  /// Searches the feature by interactive object
  /// \param theIO an interactive object
  /// \return feature the feature or NULL if it not visualized
  boost::shared_ptr<ModelAPI_Feature> GetFeature(Handle(AIS_InteractiveObject) theIO) const;
  /// Deactivate local selection
  /// \param isUpdateViewer the state wether the viewer should be updated immediatelly
  void CloseAllContexts(const bool isUpdateViewer);

  /// Returns currently installed AIS_InteractiveContext
  Handle(AIS_InteractiveContext) AISContext() const;

protected:
  XGUI_Workshop* myWorkshop;

  typedef std::map<boost::shared_ptr<ModelAPI_Feature>, Handle(AIS_InteractiveObject) > FeatureToAISMap;
  FeatureToAISMap myFeature2AISObjectMap;
};


#endif
