// File:        XGUI_ViewerPrs.h
// Created:     20 Apr 2014
// Author:      Natalia ERMOLAEVA

#ifndef XGUI_ViewerPrs_H
#define XGUI_ViewerPrs_H

#include "XGUI.h"

#include <boost/shared_ptr.hpp>
#include <TopoDS_Shape.hxx>

class ModelAPI_Feature;

/**\class XGUI_ViewerPrs
 * \ingroup GUI
 * \brief Presentation. Provides container to have feature and the shape
 */
class XGUI_EXPORT XGUI_ViewerPrs
{
public:
  /// Constructor
  XGUI_ViewerPrs();
  /// Constructor
  XGUI_ViewerPrs(boost::shared_ptr<ModelAPI_Feature> theFeature,
                 const TopoDS_Shape& theShape);
  /// Destructor
  virtual ~XGUI_ViewerPrs();

  /// Sets the feature.
  /// \param theFeature a feature instance
  void setFeature(boost::shared_ptr<ModelAPI_Feature> theFeature);

  /// Sets the shape
  /// \param theShape a shape instance
  void setShape(const TopoDS_Shape& theShape);

  /// Returns the feature.
  /// \return a feature instance
  boost::shared_ptr<ModelAPI_Feature> feature() const;

  /// Returns the shape
  /// \return a shape instance
  const TopoDS_Shape& shape() const;

private:
  boost::shared_ptr<ModelAPI_Feature> myFeature; /// the feature
  TopoDS_Shape myShape; /// the shape
};

#endif
