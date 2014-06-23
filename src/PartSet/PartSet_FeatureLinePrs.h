// File:        PartSet_FeatureLinePrs.h
// Created:     04 Jun 2014
// Author:      Natalia ERMOLAEVA

#ifndef PartSet_FeatureLinePrs_H
#define PartSet_FeatureLinePrs_H

#include "PartSet.h"

#include "PartSet_FeaturePrs.h"
#include "PartSet_Constants.h"

#include <gp_Pnt.hxx>

class GeomDataAPI_Point2D;
class GeomAPI_Lin2d;
class Handle_V3d_View;

/*!
 \class PartSet_FeatureLinePrs
 * \brief The class to define the line feature manipulation. It is created for
 * the feature create operation to move out the feature properties set and use one operation
 * for any type of features.
*/
class PARTSET_EXPORT PartSet_FeatureLinePrs : public PartSet_FeaturePrs
{
public:
  /// Returns the feature type processed by this presentation
  /// \return the feature kind
  static std::string getKind();

  /// Constructor
  /// \param theSketch the sketch feature
  PartSet_FeatureLinePrs(FeaturePtr theSketch);
  /// Destructor
  virtual ~PartSet_FeatureLinePrs() {};

  /// Sets the point to the feature in an attribute depending on the selection mode
  /// \param theX the 2D point horizontal coordinate
  /// \param theY the 2D point vertical coordinate
  /// \param theMode the selection mode
  /// \return the new selection mode
  virtual PartSet_SelectionMode setPoint(double theX, double theY,
                                         const PartSet_SelectionMode& theMode);

  /// Sets the feature to to a feature attribute depending on the selection mode
  /// \param theFeature a feature instance
  /// \param theMode the selection mode
  /// \return whether the feature is set
  virtual PartSet_SelectionMode setFeature(FeaturePtr theFeature, const PartSet_SelectionMode& theMode);

  /// Returns the feature attribute name for the selection mode
  /// \param theMode the current operation selection mode. The feature attribute depends on the mode
  virtual std::string getAttribute(const PartSet_SelectionMode& theMode) const;

  /// Returns the next selection mode after the attribute
  /// \param theAttribute the feature attribute name
  /// \return next attribute selection mode
  virtual PartSet_SelectionMode getNextMode(const std::string& theAttribute) const;

  /// Project the point on a feature
  /// \param theFeature the feature to be projected on
  /// \param theMode the selection mode
  /// \param thePoint the clicked point
  /// \param theView the viewer
  /// \param theX the output horizontal coordinate
  /// \param theY the output vertical coordinate
  void projectPointOnLine(FeaturePtr theFeature, const PartSet_SelectionMode& theMode,
                          const gp_Pnt& thePoint, Handle_V3d_View theView,
                          double& theX, double& theY);

  /// Find a point in the line with given coordinates
  /// \param theFeature the line feature
  /// \param theX the horizontal point coordinate
  /// \param theY the vertical point coordinate
  virtual boost::shared_ptr<GeomDataAPI_Point2D> findPoint(FeaturePtr theFeature, double theX,
                                                           double theY);

  /// Creates a lin 2d object on a base of the line feature
  /// \param theFeature the line feature
  static boost::shared_ptr<GeomAPI_Lin2d> createLin2d(FeaturePtr theFeature);
  /// \brief Get the line point 2d coordinates.
  /// \param theFeature the line feature
  /// \param theAttribute the start or end attribute of the line
  /// \param theX the horizontal coordinate
  /// \param theY the vertical coordinate
  static void getLinePoint(FeaturePtr theFeature, const std::string& theAttribute,
                           double& theX, double& theY);

protected:
  /// Returns the feature point in the selection mode position.
  /// \param theMode the current operation selection mode. The feature attribute depends on the mode
  virtual boost::shared_ptr<GeomDataAPI_Point2D> featurePoint(const PartSet_SelectionMode& theMode);
};

#endif
