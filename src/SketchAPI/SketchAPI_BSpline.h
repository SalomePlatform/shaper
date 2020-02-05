// Copyright (C) 2019-2020  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#ifndef SketchAPI_BSpline_H_
#define SketchAPI_BSpline_H_

#include "SketchAPI.h"
#include "SketchAPI_SketchEntity.h"

#include <GeomDataAPI_Point2DArray.h>

#include <ModelAPI_AttributeDoubleArray.h>

#include <SketchPlugin_BSpline.h>
#include <SketchPlugin_BSplinePeriodic.h>

#include <ModelHighAPI_Double.h>
#include <ModelHighAPI_Integer.h>

class ModelHighAPI_Selection;

/// \class SketchAPI_BSpline
/// \ingroup CPPHighAPI
/// \brief Interface for BSpline feature.
class SketchAPI_BSpline : public SketchAPI_SketchEntity
{
public:
  /// Constructor without values.
  SKETCHAPI_EXPORT
  explicit SketchAPI_BSpline(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Destructor.
  SKETCHAPI_EXPORT
  virtual ~SketchAPI_BSpline();

  INTERFACE_8(SketchPlugin_BSpline::ID(),
              poles, SketchPlugin_BSplineBase::POLES_ID(),
              GeomDataAPI_Point2DArray, /** B-spline poles */,
              weights, SketchPlugin_BSplineBase::WEIGHTS_ID(),
              ModelAPI_AttributeDoubleArray, /** B-spline weights */,
              knots, SketchPlugin_BSplineBase::KNOTS_ID(),
              ModelAPI_AttributeDoubleArray, /** B-spline knots */,
              multiplicities, SketchPlugin_BSplineBase::MULTS_ID(),
              ModelAPI_AttributeIntArray, /** Knots multiplicities */,
              degree, SketchPlugin_BSplineBase::DEGREE_ID(),
              ModelAPI_AttributeInteger, /** B-spline degree */,
              startPoint, SketchPlugin_BSpline::START_ID(),
              GeomDataAPI_Point2D, /** First pole of B-spline */,
              endPoint, SketchPlugin_BSpline::END_ID(),
              GeomDataAPI_Point2D, /** Last pole of B-spline */,
              external, SketchPlugin_BSplineBase::EXTERNAL_ID(),
              ModelAPI_AttributeSelection, /** External */)

  /// Set by poles and weights.
  SKETCHAPI_EXPORT
  void setByDegreePolesAndWeights(const ModelHighAPI_Integer& theDegree,
                                  const std::list<std::shared_ptr<GeomAPI_Pnt2d> >& thePoles,
                                  const std::list<ModelHighAPI_Double>& theWeights);

  /// Initialize by full set of B-spline parameters.
  SKETCHAPI_EXPORT
  void setByParameters(const ModelHighAPI_Integer& theDegree,
                       const std::list<std::shared_ptr<GeomAPI_Pnt2d> >& thePoles,
                       const std::list<ModelHighAPI_Double>& theWeights,
                       const std::list<ModelHighAPI_Double>& theKnots,
                       const std::list<ModelHighAPI_Integer>& theMults);

  /// Set by external.
  SKETCHAPI_EXPORT
  void setByExternal(const ModelHighAPI_Selection& theExternal);

  /// Generate list of construction points coincident with B-spline poles
  SKETCHAPI_EXPORT
  std::list<std::shared_ptr<SketchAPI_SketchEntity> > controlPoles(
      const std::list<int>& regular   = std::list<int>(),
      const std::list<int>& auxiliary = std::list<int>()) const;

  /// Generate control polygon for B-spline curve
  SKETCHAPI_EXPORT
  std::list<std::shared_ptr<SketchAPI_SketchEntity> > controlPolygon(
      const std::list<int>& regular   = std::list<int>(),
      const std::list<int>& auxiliary = std::list<int>()) const;

  /// Insert new pole after the pole with the given index
  SKETCHAPI_EXPORT
  bool insertPole(const int theIndex,
                  const std::shared_ptr<GeomAPI_Pnt2d>& theCoordinates,
                  const ModelHighAPI_Double& theWeight = ModelHighAPI_Double(1.0));

  /// Dump wrapped feature
  SKETCHAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;

protected:
  SketchAPI_BSpline(const std::shared_ptr<ModelAPI_Feature>& theFeature, bool theInitialize);

private:
  /// Initialize start and end points of B-spline and apply internal coincidence
  /// constraint to keep them on the corresponding pole.
  void setStartAndEndPoints();

  /// Compute default B-spline parameters (degree, knots and multiplicities)
  /// basing on hte given poles and weights
  void getDefaultParameters(const std::list<std::shared_ptr<GeomAPI_Pnt2d> >& thePoles,
                            const std::list<ModelHighAPI_Double>& theWeights,
                            ModelHighAPI_Integer& theDegree,
                            std::list<ModelHighAPI_Double>& theKnots,
                            std::list<ModelHighAPI_Integer>& theMults) const;

  /// Check what parameters of B-spline are default
  void checkDefaultParameters(bool& isDefaultDegree,
                              bool& isDefaultWeights,
                              bool& isDefaultKnotsMults) const;

  void dumpControlPolygon(ModelHighAPI_Dumper& theDumper,
                          const FeaturePtr& theBSpline,
                          const std::string& theMethod,
                          const std::map<int, FeaturePtr>& theAuxFeatures) const;
};

/// Pointer on B-spline object.
typedef std::shared_ptr<SketchAPI_BSpline> BSplinePtr;



/// \class SketchAPI_BSplinePeriodic
/// \ingroup CPPHighAPI
/// \brief Interface for BSplinePeriodic feature.
class SketchAPI_BSplinePeriodic : public SketchAPI_BSpline
{
public:
  /// Constructor without values.
  SKETCHAPI_EXPORT
  explicit SketchAPI_BSplinePeriodic(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Destructor.
  SKETCHAPI_EXPORT
  virtual ~SketchAPI_BSplinePeriodic() {}

  static std::string ID() { return SketchPlugin_BSplinePeriodic::ID(); }
  virtual std::string getID() { return SketchPlugin_BSplinePeriodic::ID(); }
};

#endif // SketchAPI_BSpline_H_
