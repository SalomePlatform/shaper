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
  void Display(boost::shared_ptr<ModelAPI_Feature> theFeature);

  /// Display the feature and a shape. This shape would be associated to the given feature
  /// \param theFeature a feature instance
  /// \param theFeature a shape
  void Display(boost::shared_ptr<ModelAPI_Feature> theFeature, const TopoDS_Shape& theShape);

  /// Erase the feature and a shape.
  /// \param theFeature a feature instance
  /// \param theFeature a shape
  void Erase(boost::shared_ptr<ModelAPI_Feature> theFeature, const TopoDS_Shape& theShape);

protected:
  XGUI_Viewer* myViewer; ///< the viewer where the objects should be visualized
};

#endif
