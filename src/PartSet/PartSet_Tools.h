// File:        PartSet_Tools.h
// Created:     28 Apr 2014
// Author:      Natalia ERMOLAEVA

#ifndef PartSet_Tools_H
#define PartSet_Tools_H

#include "PartSet.h"

#include <gp_Pnt.hxx>

#include <QPoint>

#include <boost/shared_ptr.hpp>

class Handle_V3d_View;
class ModelAPI_Feature;

/*!
 \class PartSet_Tools
 * \brief The operation for the sketch feature creation
*/
class PARTSET_EXPORT PartSet_Tools
{
public:
  /// Converts the 2D screen point to the 3D point on the view according to the point of view
  /// \param thePoint a screen point
  /// \param theView a 3D view
  static gp_Pnt ConvertClickToPoint(QPoint thePoint, Handle_V3d_View theView);

  /// \brief Converts the 3D point to the projected coodinates on the sketch plane.
  /// \param thePoint the 3D point in the viewer
  /// \param theSketch the sketch feature
  /// \param theX the X coordinate
  /// \param theY the Y coordinate
  static void ConvertTo2D(const gp_Pnt& thePoint, boost::shared_ptr<ModelAPI_Feature> theSketch,
                          Handle(V3d_View) theView, double& theX, double& theY);
};

#endif
