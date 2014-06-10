// File:        PartSet_FeatureArcPrs.h
// Created:     04 Jun 2014
// Author:      Natalia ERMOLAEVA

#ifndef PartSet_FeatureArcPrs_H
#define PartSet_FeatureArcPrs_H

#include "PartSet.h"

#include "PartSet_FeaturePrs.h"
#include "PartSet_Constants.h"

#include <gp_Pnt.hxx>

class GeomDataAPI_Point2D;
class Handle_V3d_View;

/*!
 \class PartSet_FeatureArcPrs
 * \brief The class to define the circle arc feature manipulation. It is created for
 * the feature create operation to move out the feature properties set and use one operation
 * for any type of features.
*/
class PARTSET_EXPORT PartSet_FeatureArcPrs : public PartSet_FeaturePrs
{
public:
  /// Returns the feature type processed by this presentation
  /// \return the feature kind
  static std::string getKind();

  /// Constructor
  /// \param theSketch the sketch feature
  PartSet_FeatureArcPrs(FeaturePtr theSketch);
  /// Destructor
  virtual ~PartSet_FeatureArcPrs() {};

  /// Sets the point to the feature in an attribute depending on the selection mode
  /// \param theX the 2D point horizontal coordinate
  /// \param theY the 2D point vertical coordinate
  /// \param theMode the selection mode
  /// \return the new selection mode
  virtual PartSet_SelectionMode setPoint(double theX, double theY,
                                         const PartSet_SelectionMode& theMode);

  /// Returns the feature attribute name for the selection mode
  /// \param theMode the current operation selection mode. The feature attribute depends on the mode
  virtual std::string getAttribute(const PartSet_SelectionMode& theMode) const;

  /// Returns the next selection mode after the attribute
  /// \param theAttribute the feature attribute name
  /// \return next attribute selection mode
  virtual PartSet_SelectionMode getNextMode(const std::string& theAttribute) const;

  void projectPointOnArc(gp_Pnt& thePoint, Handle_V3d_View theView,
                         double& theX, double& theY);

protected:
  /// Returns the feature point in the selection mode position.
  /// \param theMode the current operation selection mode. The feature attribute depends on the mode
  virtual boost::shared_ptr<GeomDataAPI_Point2D> featurePoint(const PartSet_SelectionMode& theMode);
};

#endif
