// File:        PartSet_ConstraintLengthPrs.h
// Created:     16 Jun 2014
// Author:      Natalia ERMOLAEVA

#ifndef PartSet_ConstraintLengthPrs_H
#define PartSet_ConstraintLengthPrs_H

#include "PartSet.h"

#include "PartSet_FeaturePrs.h"
#include "PartSet_Constants.h"

class GeomDataAPI_Point2D;
class Handle_AIS_InteractiveObject;

/*!
 \class PartSet_ConstraintLengthPrs
 * \brief The class to define the circle feature manipulation. It is created for
 * the feature create operation to move out the feature properties set and use one operation
 * for any type of features.
*/
class PARTSET_EXPORT PartSet_ConstraintLengthPrs : public PartSet_FeaturePrs
{
public:
  /// Returns the feature type processed by this presentation
  /// \return the feature kind
  static std::string getKind();

  /// Constructor
  /// \param theSketch the sketch feature
  PartSet_ConstraintLengthPrs(FeaturePtr theSketch);
  /// Destructor
  virtual ~PartSet_ConstraintLengthPrs() {};

  /// Sets the feature to to a feature attribute depending on the selection mode
  /// \param theFeature a feature instance
  /// \param theMode the selection mode
  /// \return whether the feature is set
  virtual PartSet_SelectionMode setFeature(FeaturePtr theFeature, const PartSet_SelectionMode& theMode);

  /// Sets the point to the feature in an attribute depending on the selection mode
  /// \param theX the 2D point horizontal coordinate
  /// \param theY the 2D point vertical coordinate
  /// \param theMode the selection mode
  /// \return the new selection mode
  virtual PartSet_SelectionMode setPoint(double theX, double theY,
                                         const PartSet_SelectionMode& theMode);

  /// Creates an AIS presentation if the previous is null or update the given one
  /// \param theFeature a feature
  /// \param theSketch a feature sketch
  /// \param thePrevious a previuos AIS presentation
  /// \return a created/changed AIS object with the feature parameters
  static Handle_AIS_InteractiveObject createPresentation(FeaturePtr theFeature,
                                                         FeaturePtr theSketch,
                                                         Handle_AIS_InteractiveObject thePreviuos);

  /// Returns the feature attribute name for the selection mode
  /// \param theMode the current operation selection mode. The feature attribute depends on the mode
  virtual std::string getAttribute(const PartSet_SelectionMode& theMode) const;

  /// Returns the next selection mode after the attribute
  /// \param theAttribute the feature attribute name
  /// \return next attribute selection mode
  virtual PartSet_SelectionMode getNextMode(const std::string& theAttribute) const;

  /// Return the distance between the feature and the point
  /// \param theFeature feature object
  /// \param theX the horizontal coordinate of the point
  /// \param theX the vertical coordinate of the point
  virtual double distanceToPoint(FeaturePtr theFeature, double theX, double theY);

  /// Find a point in the line with given coordinates
  /// \param theFeature the line feature
  /// \param theX the horizontal point coordinate
  /// \param theY the vertical point coordinate
  virtual boost::shared_ptr<GeomDataAPI_Point2D> findPoint(FeaturePtr theFeature, double theX,
                                                           double theY);
protected:
  /// Returns the feature point in the selection mode position.
  /// \param theMode the current operation selection mode. The feature attribute depends on the mode
  virtual boost::shared_ptr<GeomDataAPI_Point2D> featurePoint(const PartSet_SelectionMode& theMode);
};

#endif
