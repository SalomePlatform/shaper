// File:        PartSet_FeaturePointPrs.h
// Created:     04 Jun 2014
// Author:      Natalia ERMOLAEVA

#ifndef PartSet_FeaturePointPrs_H
#define PartSet_FeaturePointPrs_H

#include "PartSet.h"

#include "PartSet_Constants.h"

class GeomDataAPI_Point2D;

/*!
 \class PartSet_FeaturePointPrs
 * \brief The abstract class to define the specific feature manipulation. It is created for
 * the feature create operation to move out the feature properties set and use one operation
 * for any type of features.
*/
class PARTSET_EXPORT PartSet_FeaturePointPrs
{
public:
  /// Constructor
  /// \param theSketch the sketch feature
  PartSet_FeaturePointPrs(FeaturePtr theSketch);
  /// Destructor
  virtual ~PartSet_FeaturePointPrs();
};

#endif
