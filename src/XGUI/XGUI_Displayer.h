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
  void Display(boost::shared_ptr<ModelAPI_Feature> theFeature, const TopoDS_Shape& theShape,
               const bool isUpdateViewer = true);
  
  /// Returns the feature, that was displayed with this shape
  /// \param theShape a shape
  boost::shared_ptr<ModelAPI_Feature> GetFeature(const TopoDS_Shape& theShape);

  /// Returns a list of viewer presentations
  /// \param theShapes list of shapes to find corresponded features
  /// \return list of presentations
  std::list<XGUI_ViewerPrs> GetViewerPrs(const NCollection_List<TopoDS_Shape>& theShapes);

  /// Display the shape and activate selection of sub-shapes
  /// \param theFeature a feature instance
  /// \param theShape a shape
  /// \param theMode a local selection mode
  /// \param isUpdateViewer the parameter whether the viewer should be update immediatelly
  void RedisplayInLocalContext(boost::shared_ptr<ModelAPI_Feature> theFeature,
                             const TopoDS_Shape& theShape,
                             const std::list<int>& theMode, const bool isUpdateViewer = true);

  /// Erase the feature and a shape.
  /// \param theFeature a feature instance
  /// \param isUpdateViewer the parameter whether the viewer should be update immediatelly
  void Erase(boost::shared_ptr<ModelAPI_Feature> theFeature, const bool isUpdateViewer = true);

  /// Erase all presentations
  /// \param isUpdateViewer the parameter whether the viewer should be update immediatelly
  void EraseAll(const bool isUpdateViewer = true);

  /// Deactivates selection of sub-shapes
  /// \param isUpdateViewer the parameter whether the viewer should be update immediatelly
  void CloseLocalContexts(const bool isUpdateViewer = true);

  /// Updates the viewer
  void UpdateViewer();

protected:
  /// Deactivate local selection
  /// \param isUpdateViewer the state wether the viewer should be updated immediatelly
  void closeAllContexts(const bool isUpdateViewer);

  /// Returns currently installed AIS_InteractiveContext
  Handle(AIS_InteractiveContext) AISContext() const;

protected:
  XGUI_Workshop* myWorkshop;

  typedef std::map<boost::shared_ptr<ModelAPI_Feature>, Handle(AIS_InteractiveObject) > FeatureToAISMap;
  FeatureToAISMap myFeature2AISObjectMap;
};

#endif
