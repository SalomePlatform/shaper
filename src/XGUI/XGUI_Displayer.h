// File:        XGUI_Displayer.h
// Created:     20 Apr 2014
// Author:      Natalia ERMOLAEVA

#ifndef XGUI_Displayer_H
#define XGUI_Displayer_H

#include "XGUI.h"

#include <QString>
#include <boost/shared_ptr.hpp>

#include <TopoDS_Shape.hxx>

class XGUI_Viewer;
class ModelAPI_Feature;

/**\class XGUI_Displayer
 * \ingroup GUI
 * \brief Displayer. Provides mechanizm of display/erase of objects in the viewer
 */
class XGUI_EXPORT XGUI_Displayer
{
public:
  /// Constructor
  /// \param theViewer the viewer
  XGUI_Displayer(XGUI_Viewer* theViewer);
  /// Destructor
  virtual ~XGUI_Displayer();

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
  
  /// Display the shape and activate selection of sub-shapes
  /// \param theFeature a feature instance
  /// \param theShape a shape
  /// \param theMode a local selection mode
  /// \param isUpdateViewer the parameter whether the viewer should be update immediatelly
  void LocalSelection(boost::shared_ptr<ModelAPI_Feature> theFeature, const TopoDS_Shape& theShape,
                      const int theMode, const bool isUpdateViewer = true);

  /// Erase the feature and a shape.
  /// \param theFeature a feature instance
  /// \param theFeature a shape
  /// \param isUpdateViewer the parameter whether the viewer should be update immediatelly
  void Erase(boost::shared_ptr<ModelAPI_Feature> theFeature, const TopoDS_Shape& theShape,
             const bool isUpdateViewer = true);

  /// Deactivates selection of sub-shapes
  /// \param isUpdateViewer the parameter whether the viewer should be update immediatelly
  void GlobalSelection(const bool isUpdateViewer = true);

protected:
  XGUI_Viewer* myViewer; ///< the viewer where the objects should be visualized
};

#endif
