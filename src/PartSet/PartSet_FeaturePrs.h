// File:        PartSet_FeaturePrs.h
// Created:     04 Jun 2014
// Author:      Natalia ERMOLAEVA

#ifndef PartSet_FeaturePrs_H
#define PartSet_FeaturePrs_H

#include "PartSet.h"

#include "PartSet_Constants.h"

class GeomDataAPI_Point2D;

/*!
 \class PartSet_FeaturePrs
 * \brief The abstract class to define the specific feature manipulation. It is created for
 * the feature create operation to move out the feature properties set and use one operation
 * for any type of features.
*/
class PARTSET_EXPORT PartSet_FeaturePrs
{
public:
  /// Constructor
  /// \param theSketch the sketch feature
  PartSet_FeaturePrs(FeaturePtr theSketch);
  /// Destructor
  virtual ~PartSet_FeaturePrs();

  /// Initializes some fields of feature accorging to the source feature
  /// Saves the fiature as the presentation internal feature
  /// \param theFeature the presentation feature
  /// \param theSourceFeature the feature, which attributes are used to initialize the feature
  virtual void init(FeaturePtr theFeature, FeaturePtr theSourceFeature);

  /// Returns the operation sketch feature
  /// \returns the sketch instance
  FeaturePtr sketch() const;

  /// Sets the point to the feature in an attribute depending on the selection mode
  /// \param theX the 2D point horizontal coordinate
  /// \param theY the 2D point vertical coordinate
  /// \param theMode the selection mode
  /// \return the new selection mode
  PartSet_SelectionMode setPoint(double theX, double theY, const PartSet_SelectionMode& theMode);

  /// Creates constrains of the current 
  /// \param theX the horizontal coordnate of the point
  /// \param theY the vertical coordnate of the point
  /// \param theMode the current operation selection mode. The feature attribute depends on the mode
  void setConstraints(double theX, double theY, const PartSet_SelectionMode& theMode);

  /// Returns the feature attribute name for the selection mode
  /// \param theMode the current operation selection mode. The feature attribute depends on the mode
  std::string getAttribute(const PartSet_SelectionMode& theMode) const;

  /// Returns the next selection mode after the attribute
  /// \param theAttribute the feature attribute name
  /// \return next attribute selection mode
  PartSet_SelectionMode getNextMode(const std::string& theAttribute) const;

  /// \brief Save the point to the line.
  /// \param theFeature the line feature
  /// \param theX the horizontal coordinate
  /// \param theY the vertical coordinate
  /// \param theAttribute the start or end attribute of the line
  static void setLinePoint(FeaturePtr, double theX, double theY,
                           const std::string& theAttribute);

protected:
  /// Returns pointer to the root document.
  boost::shared_ptr<ModelAPI_Document> document() const;

    /// Returns the operation feature
  /// \return the feature
  FeaturePtr feature() const;

  /// Creates a constraint on two points
  /// \param thePoint1 the first point
  /// \param thePoint1 the second point
  void createConstraint(boost::shared_ptr<GeomDataAPI_Point2D> thePoint1,
                        boost::shared_ptr<GeomDataAPI_Point2D> thePoint2);

  /// \brief Get the line point 2d coordinates.
  /// \param theFeature the line feature
  /// \param theAttribute the start or end attribute of the line
  /// \param theX the horizontal coordinate
  /// \param theY the vertical coordinate
  void getLinePoint(FeaturePtr theFeature, const std::string& theAttribute,
                    double& theX, double& theY);
  /// Find a point in the line with given coordinates
  /// \param theFeature the line feature
  /// \param theX the horizontal point coordinate
  /// \param theY the vertical point coordinate
  boost::shared_ptr<GeomDataAPI_Point2D> findLinePoint(FeaturePtr theFeature,
                                                       double theX, double theY);

private:
  FeaturePtr mySketch; ///< the sketch of the feature
  FeaturePtr myFeature; ///< the feature
};

#endif
