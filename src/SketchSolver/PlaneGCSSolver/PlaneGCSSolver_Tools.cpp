// Copyright (C) 2014-2023  CEA, EDF
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

#include <PlaneGCSSolver_Tools.h>
#include <PlaneGCSSolver_EdgeWrapper.h>
#include <PlaneGCSSolver_PointArrayWrapper.h>
#include <PlaneGCSSolver_PointWrapper.h>
#include <PlaneGCSSolver_ScalarArrayWrapper.h>
#include <PlaneGCSSolver_ScalarWrapper.h>
#include <PlaneGCSSolver_ConstraintWrapper.h>

#include <SketchSolver_Constraint.h>
#include <SketchSolver_ConstraintAngle.h>
#include <SketchSolver_ConstraintCoincidence.h>
#include <SketchPlugin_ConstraintCoincidenceInternal.h>
#include <SketchSolver_ConstraintCollinear.h>
#include <SketchSolver_ConstraintDistance.h>
#include <SketchSolver_ConstraintEqual.h>
#include <SketchSolver_ConstraintFixed.h>
#include <SketchSolver_ConstraintLength.h>
#include <SketchSolver_ConstraintMiddle.h>
#include <SketchSolver_ConstraintMirror.h>
#include <SketchSolver_ConstraintPerpendicular.h>
#include <SketchSolver_ConstraintTangent.h>
#include <SketchSolver_ConstraintMultiRotation.h>
#include <SketchSolver_ConstraintMultiTranslation.h>
#include <SketchSolver_ConstraintOffset.h>

#include <SketchPlugin_Arc.h>
#include <SketchPlugin_BSpline.h>
#include <SketchPlugin_BSplinePeriodic.h>
#include <SketchPlugin_Circle.h>
#include <SketchPlugin_ConstraintAngle.h>
#include <SketchPlugin_ConstraintCoincidence.h>
#include <SketchPlugin_ConstraintCollinear.h>
#include <SketchPlugin_ConstraintDistance.h>
#include <SketchPlugin_ConstraintDistanceHorizontal.h>
#include <SketchPlugin_ConstraintDistanceVertical.h>
#include <SketchPlugin_ConstraintEqual.h>
#include <SketchPlugin_ConstraintLength.h>
#include <SketchPlugin_ConstraintMiddle.h>
#include <SketchPlugin_ConstraintMirror.h>
#include <SketchPlugin_ConstraintRigid.h>
#include <SketchPlugin_ConstraintPerpendicular.h>
#include <SketchPlugin_ConstraintTangent.h>
#include <SketchPlugin_Ellipse.h>
#include <SketchPlugin_EllipticArc.h>
#include <SketchPlugin_Line.h>
#include <SketchPlugin_MultiRotation.h>
#include <SketchPlugin_MultiTranslation.h>
#include <SketchPlugin_Offset.h>
#include <SketchPlugin_Point.h>

#include <GeomAPI_BSpline2d.h>
#include <GeomAPI_Circ2d.h>
#include <GeomAPI_Dir2d.h>
#include <GeomAPI_Ellipse2d.h>
#include <GeomAPI_Lin2d.h>
#include <GeomAPI_Pnt2d.h>

#include <ModelAPI_AttributeDoubleArray.h>
#include <ModelAPI_AttributeIntArray.h>

#include <cmath>


#define GCS_EDGE_WRAPPER(x)   std::dynamic_pointer_cast<PlaneGCSSolver_EdgeWrapper>(x)
#define GCS_POINT_WRAPPER(x)  std::dynamic_pointer_cast<PlaneGCSSolver_PointWrapper>(x)
#define GCS_SCALAR_WRAPPER(x) std::dynamic_pointer_cast<PlaneGCSSolver_ScalarWrapper>(x)



static ConstraintWrapperPtr
  createConstraintCoincidence(std::shared_ptr<PlaneGCSSolver_PointWrapper> thePoint1,
                              std::shared_ptr<PlaneGCSSolver_PointWrapper> thePoint2);
static ConstraintWrapperPtr
  createConstraintPointOnEntity(const SketchSolver_ConstraintType& theType,
                                std::shared_ptr<PlaneGCSSolver_PointWrapper> thePoint,
                                std::shared_ptr<PlaneGCSSolver_EdgeWrapper> theEntity,
                                std::shared_ptr<PlaneGCSSolver_ScalarWrapper> theValue);
static ConstraintWrapperPtr
  createConstraintPointsCollinear(std::shared_ptr<PlaneGCSSolver_PointWrapper> thePoint1,
                                  std::shared_ptr<PlaneGCSSolver_PointWrapper> thePoint2,
                                  std::shared_ptr<PlaneGCSSolver_PointWrapper> thePoint3);
static ConstraintWrapperPtr
  createConstraintDistancePointPoint(std::shared_ptr<PlaneGCSSolver_ScalarWrapper> theValue,
                                     std::shared_ptr<PlaneGCSSolver_PointWrapper> thePoint1,
                                     std::shared_ptr<PlaneGCSSolver_PointWrapper> thePoint2);
static ConstraintWrapperPtr
  createConstraintDistancePointLine(std::shared_ptr<PlaneGCSSolver_ScalarWrapper> theValue,
                                    std::shared_ptr<PlaneGCSSolver_PointWrapper>  thePoint,
                                    std::shared_ptr<PlaneGCSSolver_EdgeWrapper> theEntity);
static ConstraintWrapperPtr
  createConstraintHVDistance(const SketchSolver_ConstraintType& theType,
                             std::shared_ptr<PlaneGCSSolver_ScalarWrapper> theValue,
                             std::shared_ptr<PlaneGCSSolver_PointWrapper> thePoint1,
                             std::shared_ptr<PlaneGCSSolver_PointWrapper> thePoint2);
static ConstraintWrapperPtr
  createConstraintRadius(std::shared_ptr<PlaneGCSSolver_ScalarWrapper> theValue,
                         std::shared_ptr<PlaneGCSSolver_EdgeWrapper> theEntity);
static ConstraintWrapperPtr
  createConstraintAngle(ConstraintPtr theConstraint,
                        std::shared_ptr<PlaneGCSSolver_ScalarWrapper> theValue,
                        std::shared_ptr<PlaneGCSSolver_EdgeWrapper> theEntity1,
                        std::shared_ptr<PlaneGCSSolver_EdgeWrapper> theEntity2);
static ConstraintWrapperPtr
  createConstraintHorizVert(const SketchSolver_ConstraintType& theType,
                            std::shared_ptr<PlaneGCSSolver_EdgeWrapper> theEntity);
static ConstraintWrapperPtr
  createConstraintParallel(std::shared_ptr<PlaneGCSSolver_EdgeWrapper> theEntity1,
                           std::shared_ptr<PlaneGCSSolver_EdgeWrapper> theEntity2);
static ConstraintWrapperPtr
  createConstraintPerpendicular(std::shared_ptr<PlaneGCSSolver_EdgeWrapper> theEntity1,
                                std::shared_ptr<PlaneGCSSolver_EdgeWrapper> theEntity2);
static ConstraintWrapperPtr
  createConstraintEqual(const SketchSolver_ConstraintType& theType,
                        std::shared_ptr<PlaneGCSSolver_EdgeWrapper> theEntity1,
                        std::shared_ptr<PlaneGCSSolver_EdgeWrapper> theEntity2,
                        std::shared_ptr<PlaneGCSSolver_ScalarWrapper> theIntermed);
static ConstraintWrapperPtr
  createConstraintMiddlePoint(std::shared_ptr<PlaneGCSSolver_PointWrapper> thePoint,
                              std::shared_ptr<PlaneGCSSolver_EdgeWrapper>  theEntity,
                              std::shared_ptr<PlaneGCSSolver_PointWrapper> theAuxParameters);
static ConstraintWrapperPtr
  createConstraintAngleBetweenCurves(std::shared_ptr<PlaneGCSSolver_ScalarWrapper> theValue,
                                     std::shared_ptr<PlaneGCSSolver_PointWrapper> thePoint,
                                     std::shared_ptr<PlaneGCSSolver_EdgeWrapper> theEntity1,
                                     std::shared_ptr<PlaneGCSSolver_EdgeWrapper> theEntity2);

static GCS::SET_pD scalarParameters(const ScalarWrapperPtr& theScalar);
static GCS::SET_pD scalarArrayParameters(const EntityWrapperPtr& theArray);
static GCS::SET_pD pointParameters(const PointWrapperPtr& thePoint);
static GCS::SET_pD pointArrayParameters(const EntityWrapperPtr& theArray);
static GCS::SET_pD lineParameters(const EdgeWrapperPtr& theLine);
static GCS::SET_pD circleParameters(const EdgeWrapperPtr& theCircle);
static GCS::SET_pD arcParameters(const EdgeWrapperPtr& theArc);
static GCS::SET_pD ellipseParameters(const EdgeWrapperPtr& theEllipse);
static GCS::SET_pD ellipticArcParameters(const EdgeWrapperPtr& theEllipticArc);
static GCS::SET_pD bsplineParameters(const EdgeWrapperPtr& theEdge);




SolverConstraintPtr PlaneGCSSolver_Tools::createConstraint(ConstraintPtr theConstraint)
{
  if (theConstraint->getKind() == SketchPlugin_ConstraintCoincidence::ID() ||
      theConstraint->getKind() == SketchPlugin_ConstraintCoincidenceInternal::ID()) {
    return SolverConstraintPtr(new SketchSolver_ConstraintCoincidence(theConstraint));
  } else if (theConstraint->getKind() == SketchPlugin_ConstraintCollinear::ID()) {
    return SolverConstraintPtr(new SketchSolver_ConstraintCollinear(theConstraint));
  } else if (theConstraint->getKind() == SketchPlugin_ConstraintDistance::ID() ||
             theConstraint->getKind() == SketchPlugin_ConstraintDistanceHorizontal::ID() ||
             theConstraint->getKind() == SketchPlugin_ConstraintDistanceVertical::ID()) {
    return SolverConstraintPtr(new SketchSolver_ConstraintDistance(theConstraint));
  } else if (theConstraint->getKind() == SketchPlugin_ConstraintEqual::ID()) {
    return SolverConstraintPtr(new SketchSolver_ConstraintEqual(theConstraint));
  } else if (theConstraint->getKind() == SketchPlugin_ConstraintLength::ID()) {
    return SolverConstraintPtr(new SketchSolver_ConstraintLength(theConstraint));
  } else if (theConstraint->getKind() == SketchPlugin_ConstraintMiddle::ID()) {
    return SolverConstraintPtr(new SketchSolver_ConstraintMiddle(theConstraint));
  } else if (theConstraint->getKind() == SketchPlugin_ConstraintMirror::ID()) {
    return SolverConstraintPtr(new SketchSolver_ConstraintMirror(theConstraint));
  } else if (theConstraint->getKind() == SketchPlugin_ConstraintTangent::ID()) {
    return SolverConstraintPtr(new SketchSolver_ConstraintTangent(theConstraint));
  } else if (theConstraint->getKind() == SketchPlugin_ConstraintRigid::ID()) {
    return SolverConstraintPtr(new SketchSolver_ConstraintFixed(theConstraint));
  } else if (theConstraint->getKind() == SketchPlugin_MultiTranslation::ID()) {
    return SolverConstraintPtr(new SketchSolver_ConstraintMultiTranslation(theConstraint));
  } else if (theConstraint->getKind() == SketchPlugin_MultiRotation::ID()) {
    return SolverConstraintPtr(new SketchSolver_ConstraintMultiRotation(theConstraint));
  } else if (theConstraint->getKind() == SketchPlugin_ConstraintAngle::ID()) {
    return SolverConstraintPtr(new SketchSolver_ConstraintAngle(theConstraint));
  } else if (theConstraint->getKind() == SketchPlugin_ConstraintPerpendicular::ID()) {
    return SolverConstraintPtr(new SketchSolver_ConstraintPerpendicular(theConstraint));
  } else if (theConstraint->getKind() == SketchPlugin_Offset::ID()) {
    return SolverConstraintPtr(new SketchSolver_ConstraintOffset(theConstraint));
  }
  // All other types of constraints
  return SolverConstraintPtr(new SketchSolver_Constraint(theConstraint));
}

std::shared_ptr<SketchSolver_ConstraintMovement> PlaneGCSSolver_Tools::createMovementConstraint(
    FeaturePtr theMovedFeature)
{
  return std::shared_ptr<SketchSolver_ConstraintMovement>(
      new SketchSolver_ConstraintMovement(theMovedFeature));
}

std::shared_ptr<SketchSolver_ConstraintMovement> PlaneGCSSolver_Tools::createMovementConstraint(
    AttributePtr theMovedAttribute)
{
  return std::shared_ptr<SketchSolver_ConstraintMovement>(
      new SketchSolver_ConstraintMovement(theMovedAttribute));
}

std::shared_ptr<SketchSolver_ConstraintMovement> PlaneGCSSolver_Tools::createMovementConstraint(
    const std::pair<AttributePtr, int>& theMovedPointInArray)
{
  return std::shared_ptr<SketchSolver_ConstraintMovement>(
      new SketchSolver_ConstraintMovement(theMovedPointInArray.first, theMovedPointInArray.second));
}



ConstraintWrapperPtr PlaneGCSSolver_Tools::createConstraint(
    ConstraintPtr theConstraint,
    const SketchSolver_ConstraintType& theType,
    const EntityWrapperPtr& theValue,
    const EntityWrapperPtr& thePoint1,
    const EntityWrapperPtr& thePoint2,
    const EntityWrapperPtr& theEntity1,
    const EntityWrapperPtr& theEntity2)
{
  ConstraintWrapperPtr aResult;
  ScalarWrapperPtr anIntermediate;

  std::shared_ptr<PlaneGCSSolver_PointWrapper> aPoint1 = GCS_POINT_WRAPPER(thePoint1);
  std::shared_ptr<PlaneGCSSolver_PointWrapper> aPoint2 = GCS_POINT_WRAPPER(thePoint2);
  std::shared_ptr<PlaneGCSSolver_EdgeWrapper> anEntity1 = GCS_EDGE_WRAPPER(theEntity1);

  switch (theType) {
  case CONSTRAINT_PT_PT_COINCIDENT:
    aResult = createConstraintCoincidence(aPoint1, aPoint2);
    break;
  case CONSTRAINT_PT_ON_CURVE:
    aResult = anEntity1 ?
        createConstraintPointOnEntity(theType, aPoint1, anEntity1, GCS_SCALAR_WRAPPER(theValue)) :
        createConstraintPointsCollinear(aPoint1, aPoint2, GCS_POINT_WRAPPER(theEntity1));
    break;
  case CONSTRAINT_MIDDLE_POINT:
    aResult = createConstraintMiddlePoint(aPoint1, GCS_EDGE_WRAPPER(theEntity1), aPoint2);
    break;
  case CONSTRAINT_PT_PT_DISTANCE:
    aResult = createConstraintDistancePointPoint(GCS_SCALAR_WRAPPER(theValue), aPoint1, aPoint2);
    break;
  case CONSTRAINT_PT_LINE_DISTANCE:
    aResult = createConstraintDistancePointLine(GCS_SCALAR_WRAPPER(theValue),
                                                aPoint1,
                                                anEntity1);
    break;
  case CONSTRAINT_HORIZONTAL_DISTANCE:
  case CONSTRAINT_VERTICAL_DISTANCE:
    aResult = createConstraintHVDistance(theType, GCS_SCALAR_WRAPPER(theValue), aPoint1, aPoint2);
    break;
  case CONSTRAINT_RADIUS:
    aResult = createConstraintRadius(GCS_SCALAR_WRAPPER(theValue), anEntity1);
    break;
  case CONSTRAINT_ANGLE:
    aResult = createConstraintAngle(theConstraint,
                  GCS_SCALAR_WRAPPER(theValue),
                  anEntity1, GCS_EDGE_WRAPPER(theEntity2));
    break;
  case CONSTRAINT_FIXED:
    break;
  case CONSTRAINT_HORIZONTAL:
  case CONSTRAINT_VERTICAL:
    aResult = createConstraintHorizVert(theType, anEntity1);
    break;
  case CONSTRAINT_PARALLEL:
    aResult = createConstraintParallel(anEntity1, GCS_EDGE_WRAPPER(theEntity2));
    break;
  case CONSTRAINT_PERPENDICULAR:
    aResult = createConstraintPerpendicular(anEntity1, GCS_EDGE_WRAPPER(theEntity2));
    break;
  case CONSTRAINT_PERPENDICULAR_CURVES:
    aResult = createConstraintAngleBetweenCurves(GCS_SCALAR_WRAPPER(theValue),
                  aPoint1, anEntity1, GCS_EDGE_WRAPPER(theEntity2));
    break;
  case CONSTRAINT_EQUAL_LINES:
  case CONSTRAINT_EQUAL_ELLIPSES:
    anIntermediate = GCS_SCALAR_WRAPPER(theValue); // parameter is used to store length of lines
  case CONSTRAINT_EQUAL_LINE_ARC:
  case CONSTRAINT_EQUAL_RADIUS:
    aResult = createConstraintEqual(theType,
                                    anEntity1,
                                    GCS_EDGE_WRAPPER(theEntity2),
                                    anIntermediate);
    break;
  default:
    break;
  }

  return aResult;
}

std::shared_ptr<GeomAPI_Pnt2d> PlaneGCSSolver_Tools::point(EntityWrapperPtr theEntity)
{
  if (theEntity->type() != ENTITY_POINT)
    return std::shared_ptr<GeomAPI_Pnt2d>();

  std::shared_ptr<PlaneGCSSolver_PointWrapper> aPointWrapper =
      std::dynamic_pointer_cast<PlaneGCSSolver_PointWrapper>(theEntity);
  const GCSPointPtr& aPoint = aPointWrapper->point();
  return std::shared_ptr<GeomAPI_Pnt2d>(new GeomAPI_Pnt2d(*aPoint->x, *aPoint->y));
}

std::shared_ptr<GeomAPI_Lin2d> PlaneGCSSolver_Tools::line(EntityWrapperPtr theEntity)
{
  if (theEntity->type() != ENTITY_LINE)
    return std::shared_ptr<GeomAPI_Lin2d>();

  std::shared_ptr<PlaneGCSSolver_EdgeWrapper> anEntity =
      std::dynamic_pointer_cast<PlaneGCSSolver_EdgeWrapper>(theEntity);
  std::shared_ptr<GCS::Line> aLine = std::dynamic_pointer_cast<GCS::Line>(anEntity->entity());
  return std::shared_ptr<GeomAPI_Lin2d>(
      new GeomAPI_Lin2d(*(aLine->p1.x), *(aLine->p1.y), *(aLine->p2.x), *(aLine->p2.y)));
}

std::shared_ptr<GeomAPI_Lin2d> PlaneGCSSolver_Tools::line(FeaturePtr theFeature)
{
  if (theFeature->getKind() != SketchPlugin_Line::ID())
    return std::shared_ptr<GeomAPI_Lin2d>();

  AttributePoint2DPtr aStart = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      theFeature->attribute(SketchPlugin_Line::START_ID()));
  AttributePoint2DPtr aEnd = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      theFeature->attribute(SketchPlugin_Line::END_ID()));

  return std::shared_ptr<GeomAPI_Lin2d>(new GeomAPI_Lin2d(aStart->pnt(), aEnd->pnt()));
}

std::shared_ptr<GeomAPI_Circ2d> PlaneGCSSolver_Tools::circle(EntityWrapperPtr theEntity)
{
  if (theEntity->type() != ENTITY_CIRCLE && theEntity->type() != ENTITY_ARC)
    return std::shared_ptr<GeomAPI_Circ2d>();

  std::shared_ptr<PlaneGCSSolver_EdgeWrapper> anEntity =
    std::dynamic_pointer_cast<PlaneGCSSolver_EdgeWrapper>(theEntity);
  std::shared_ptr<GCS::Circle> aCirc = std::dynamic_pointer_cast<GCS::Circle>(anEntity->entity());
  return std::shared_ptr<GeomAPI_Circ2d>(
    new GeomAPI_Circ2d(*(aCirc->center.x), *(aCirc->center.y), *(aCirc->rad)));
}

std::shared_ptr<GeomAPI_Ellipse2d> PlaneGCSSolver_Tools::ellipse(EntityWrapperPtr theEntity)
{
  if (theEntity->type() != ENTITY_ELLIPSE && theEntity->type() != ENTITY_ELLIPTIC_ARC)
    return std::shared_ptr<GeomAPI_Ellipse2d>();

  std::shared_ptr<PlaneGCSSolver_EdgeWrapper> anEntity =
    std::dynamic_pointer_cast<PlaneGCSSolver_EdgeWrapper>(theEntity);
  std::shared_ptr<GCS::Ellipse> anEllipse =
      std::dynamic_pointer_cast<GCS::Ellipse>(anEntity->entity());

  std::shared_ptr<GeomAPI_Pnt2d> aCenter(
      new GeomAPI_Pnt2d(*(anEllipse->center.x), *(anEllipse->center.y)));
  std::shared_ptr<GeomAPI_Dir2d> anAxis(new GeomAPI_Dir2d(
      *(anEllipse->focus1.x) - *(anEllipse->center.x),
      *(anEllipse->focus1.y) - *(anEllipse->center.y)));

  return std::shared_ptr<GeomAPI_Ellipse2d>(
      new GeomAPI_Ellipse2d(aCenter, anAxis, anEllipse->getRadMaj(), *anEllipse->radmin));
}

std::shared_ptr<GeomAPI_BSpline2d> PlaneGCSSolver_Tools::bspline(EntityWrapperPtr theEntity)
{
  if (theEntity->type() != ENTITY_BSPLINE)
    return std::shared_ptr<GeomAPI_BSpline2d>();

  std::shared_ptr<PlaneGCSSolver_EdgeWrapper> anEntity =
      std::dynamic_pointer_cast<PlaneGCSSolver_EdgeWrapper>(theEntity);
  std::shared_ptr<GCS::BSpline> aSpline =
      std::dynamic_pointer_cast<GCS::BSpline>(anEntity->entity());

  std::list<GeomPnt2dPtr> aPoles;
  for (GCS::VEC_P::iterator anIt = aSpline->poles.begin(); anIt != aSpline->poles.end(); ++anIt)
    aPoles.push_back(GeomPnt2dPtr(new GeomAPI_Pnt2d(*anIt->x, *anIt->y)));

  std::list<double> aWeights;
  for (GCS::VEC_pD::iterator anIt = aSpline->weights.begin();
       anIt != aSpline->weights.end(); ++anIt)
    aWeights.push_back(**anIt);

  std::list<double> aKnots;
  for (GCS::VEC_pD::iterator anIt = aSpline->knots.begin(); anIt != aSpline->knots.end(); ++anIt)
    aKnots.push_back(**anIt);

  std::list<int> aMultiplicities;
  aMultiplicities.assign(aSpline->mult.begin(), aSpline->mult.end());

  return std::shared_ptr<GeomAPI_BSpline2d>(
         new GeomAPI_BSpline2d(aSpline->degree, aPoles, aWeights,
                               aKnots, aMultiplicities, aSpline->periodic));
}

void PlaneGCSSolver_Tools::recalculateArcParameters(EntityWrapperPtr theArc)
{
  std::shared_ptr<PlaneGCSSolver_EdgeWrapper> anEdge =
      std::dynamic_pointer_cast<PlaneGCSSolver_EdgeWrapper>(theArc);
  if (!anEdge)
    return;

  if (anEdge->type() == ENTITY_ARC) {
    std::shared_ptr<GCS::Arc> anArc = std::dynamic_pointer_cast<GCS::Arc>(anEdge->entity());

    GCS::Point aCenter = anArc->center;
    GCS::Point aStartPnt = anArc->start;
    GCS::Point aEndPnt = anArc->end;

    *anArc->rad = distance(aCenter, aStartPnt);

    static GeomDir2dPtr OX(new GeomAPI_Dir2d(1.0, 0.0));

    GeomDir2dPtr aDir(new GeomAPI_Dir2d(*aStartPnt.x - *aCenter.x, *aStartPnt.y - *aCenter.y));
    *anArc->startAngle = OX->angle(aDir);

    aDir.reset(new GeomAPI_Dir2d(*aEndPnt.x - *aCenter.x, *aEndPnt.y - *aCenter.y));
    *anArc->endAngle = OX->angle(aDir);
  }
  else if (anEdge->type() == ENTITY_ELLIPTIC_ARC) {
    std::shared_ptr<GCS::ArcOfEllipse> aEllArc =
        std::dynamic_pointer_cast<GCS::ArcOfEllipse>(anEdge->entity());

    GeomPnt2dPtr aCenter(new GeomAPI_Pnt2d(*aEllArc->center.x, *aEllArc->center.y));
    GeomPnt2dPtr aStartPnt(new GeomAPI_Pnt2d(*aEllArc->start.x, *aEllArc->start.y));
    GeomPnt2dPtr aEndPnt(new GeomAPI_Pnt2d(*aEllArc->end.x, *aEllArc->end.y));

    GeomDir2dPtr anAxis(new GeomAPI_Dir2d(*aEllArc->focus1.x - aCenter->x(),
                                          *aEllArc->focus1.y - aCenter->y()));
    GeomAPI_Ellipse2d anEllipse(aCenter, anAxis, aEllArc->getRadMaj(), *aEllArc->radmin);
    anEllipse.parameter(aStartPnt, 1.e-4, *aEllArc->startAngle);
    anEllipse.parameter(aEndPnt, 1.e-4, *aEllArc->endAngle);
  }
}



GCS::SET_pD PlaneGCSSolver_Tools::parameters(const EntityWrapperPtr& theEntity)
{
  switch (theEntity->type()) {
  case ENTITY_SCALAR:
  case ENTITY_ANGLE:
    return scalarParameters(GCS_SCALAR_WRAPPER(theEntity));
  case ENTITY_SCALAR_ARRAY:
    return scalarArrayParameters(theEntity);
  case ENTITY_POINT:
    return pointParameters(GCS_POINT_WRAPPER(theEntity));
  case ENTITY_POINT_ARRAY:
    return pointArrayParameters(theEntity);
  case ENTITY_LINE:
    return lineParameters(GCS_EDGE_WRAPPER(theEntity));
  case ENTITY_CIRCLE:
    return circleParameters(GCS_EDGE_WRAPPER(theEntity));
  case ENTITY_ARC:
    return arcParameters(GCS_EDGE_WRAPPER(theEntity));
  case ENTITY_ELLIPSE:
    return ellipseParameters(GCS_EDGE_WRAPPER(theEntity));
  case ENTITY_ELLIPTIC_ARC:
    return ellipticArcParameters(GCS_EDGE_WRAPPER(theEntity));
  case ENTITY_BSPLINE:
    return bsplineParameters(GCS_EDGE_WRAPPER(theEntity));
  default: break;
  }
  return GCS::SET_pD();
}

bool PlaneGCSSolver_Tools::isAttributeApplicable(const std::string& theAttrName,
                                                 const std::string& theOwnerName)
{
  if (theOwnerName == SketchPlugin_Arc::ID()) {
    return theAttrName == SketchPlugin_Arc::CENTER_ID() ||
           theAttrName == SketchPlugin_Arc::START_ID() ||
           theAttrName == SketchPlugin_Arc::END_ID() ||
           theAttrName == SketchPlugin_Arc::REVERSED_ID();
  }
  else if (theOwnerName == SketchPlugin_Circle::ID()) {
    return theAttrName == SketchPlugin_Circle::CENTER_ID() ||
           theAttrName == SketchPlugin_Circle::RADIUS_ID();
  }
  else if (theOwnerName == SketchPlugin_Line::ID()) {
    return theAttrName == SketchPlugin_Line::START_ID() ||
           theAttrName == SketchPlugin_Line::END_ID();
  }
  else if (theOwnerName == SketchPlugin_Ellipse::ID()) {
    return theAttrName == SketchPlugin_Ellipse::CENTER_ID() ||
           theAttrName == SketchPlugin_Ellipse::FIRST_FOCUS_ID() ||
           theAttrName == SketchPlugin_Ellipse::SECOND_FOCUS_ID() ||
           theAttrName == SketchPlugin_Ellipse::MAJOR_AXIS_START_ID() ||
           theAttrName == SketchPlugin_Ellipse::MAJOR_AXIS_END_ID() ||
           theAttrName == SketchPlugin_Ellipse::MINOR_AXIS_START_ID() ||
           theAttrName == SketchPlugin_Ellipse::MINOR_AXIS_END_ID() ||
           theAttrName == SketchPlugin_Ellipse::MAJOR_RADIUS_ID() ||
           theAttrName == SketchPlugin_Ellipse::MINOR_RADIUS_ID();
  }
  else if (theOwnerName == SketchPlugin_EllipticArc::ID()) {
    return theAttrName == SketchPlugin_EllipticArc::CENTER_ID() ||
           theAttrName == SketchPlugin_EllipticArc::FIRST_FOCUS_ID() ||
           theAttrName == SketchPlugin_EllipticArc::SECOND_FOCUS_ID() ||
           theAttrName == SketchPlugin_EllipticArc::MAJOR_AXIS_START_ID() ||
           theAttrName == SketchPlugin_EllipticArc::MAJOR_AXIS_END_ID() ||
           theAttrName == SketchPlugin_EllipticArc::MINOR_AXIS_START_ID() ||
           theAttrName == SketchPlugin_EllipticArc::MINOR_AXIS_END_ID() ||
           theAttrName == SketchPlugin_EllipticArc::MAJOR_RADIUS_ID() ||
           theAttrName == SketchPlugin_EllipticArc::MINOR_RADIUS_ID() ||
           theAttrName == SketchPlugin_EllipticArc::START_POINT_ID() ||
           theAttrName == SketchPlugin_EllipticArc::END_POINT_ID() ||
           theAttrName == SketchPlugin_EllipticArc::REVERSED_ID();
  }
  else if (theOwnerName == SketchPlugin_BSpline::ID()) {
    return theAttrName == SketchPlugin_BSpline::POLES_ID() ||
           theAttrName == SketchPlugin_BSpline::WEIGHTS_ID() ||
           theAttrName == SketchPlugin_BSpline::KNOTS_ID() ||
           theAttrName == SketchPlugin_BSpline::MULTS_ID() ||
           theAttrName == SketchPlugin_BSpline::DEGREE_ID() ||
           theAttrName == SketchPlugin_BSpline::START_ID() ||
           theAttrName == SketchPlugin_BSpline::END_ID();
  }
  else if (theOwnerName == SketchPlugin_BSplinePeriodic::ID()) {
    return theAttrName == SketchPlugin_BSplinePeriodic::POLES_ID() ||
           theAttrName == SketchPlugin_BSplinePeriodic::WEIGHTS_ID() ||
           theAttrName == SketchPlugin_BSplinePeriodic::KNOTS_ID() ||
           theAttrName == SketchPlugin_BSplinePeriodic::MULTS_ID() ||
           theAttrName == SketchPlugin_BSplinePeriodic::DEGREE_ID();
  }

  // suppose that all remaining features are points
  return theAttrName == SketchPlugin_Point::COORD_ID();
}

/// \brief Update value
bool PlaneGCSSolver_Tools::updateValue(const double& theSource, double& theDest,
                                       const double theTolerance)
{
  bool isUpdated = fabs(theSource - theDest) > theTolerance;
  if (isUpdated)
    theDest = theSource;
  return isUpdated;
}


double PlaneGCSSolver_Tools::distance(const GCS::Point& thePnt1, const GCS::Point& thePnt2)
{
  double x = *thePnt1.x - *thePnt2.x;
  double y = *thePnt1.y - *thePnt2.y;
  return sqrt(x*x + y*y);
}




// ================   AttributeArray methods   ==========================
PlaneGCSSolver_Tools::AttributeArray::AttributeArray(AttributePtr theAttribute)
{
  myDouble = std::dynamic_pointer_cast<ModelAPI_AttributeDoubleArray>(theAttribute);
  myInteger = std::dynamic_pointer_cast<ModelAPI_AttributeIntArray>(theAttribute);
}

bool PlaneGCSSolver_Tools::AttributeArray::isInitialized() const
{
  return (myDouble && myDouble->isInitialized()) || (myInteger && myInteger->isInitialized());
}

int PlaneGCSSolver_Tools::AttributeArray::size() const
{
  return myDouble.get() ? myDouble->size() : myInteger->size();
}

double PlaneGCSSolver_Tools::AttributeArray::value(const int theIndex) const
{
  return myDouble.get() ? myDouble->value(theIndex) : myInteger->value(theIndex);
}






// ================   Auxiliary functions   ==========================
ConstraintWrapperPtr createConstraintCoincidence(
    std::shared_ptr<PlaneGCSSolver_PointWrapper> thePoint1,
    std::shared_ptr<PlaneGCSSolver_PointWrapper> thePoint2)
{
  GCSPointPtr aPoint1 = thePoint1->point();
  GCSPointPtr aPoint2 = thePoint2->point();

  // Create equality constraint for corresponding attributes of the points
  std::list<GCSConstraintPtr> aConstrList;
  aConstrList.push_back(
      GCSConstraintPtr(new GCS::ConstraintEqual(aPoint1->x, aPoint2->x)));
  aConstrList.push_back(
      GCSConstraintPtr(new GCS::ConstraintEqual(aPoint1->y, aPoint2->y)));

  return ConstraintWrapperPtr(new PlaneGCSSolver_ConstraintWrapper(
      aConstrList, CONSTRAINT_PT_PT_COINCIDENT));
}

ConstraintWrapperPtr createConstraintPointOnEntity(
    const SketchSolver_ConstraintType& theType,
    std::shared_ptr<PlaneGCSSolver_PointWrapper> thePoint,
    std::shared_ptr<PlaneGCSSolver_EdgeWrapper> theEntity,
    std::shared_ptr<PlaneGCSSolver_ScalarWrapper> theValue)
{
  GCSConstraintPtr aNewConstr;

  switch (theEntity->type()) {
  case ENTITY_LINE: {
    std::shared_ptr<GCS::Line> aLine = std::dynamic_pointer_cast<GCS::Line>(theEntity->entity());
    aNewConstr = GCSConstraintPtr(new GCS::ConstraintPointOnLine(*(thePoint->point()), *aLine));
    break;
    }
  case ENTITY_ARC:
  case ENTITY_CIRCLE: {
    std::shared_ptr<GCS::Circle> aCirc =
      std::dynamic_pointer_cast<GCS::Circle>(theEntity->entity());
    aNewConstr = GCSConstraintPtr(
        new GCS::ConstraintP2PDistance(*(thePoint->point()), aCirc->center, aCirc->rad));
    break;
    }
  case ENTITY_ELLIPSE:
  case ENTITY_ELLIPTIC_ARC: {
    std::shared_ptr<GCS::Ellipse> anEllipse =
        std::dynamic_pointer_cast<GCS::Ellipse>(theEntity->entity());
    aNewConstr = GCSConstraintPtr(
        new GCS::ConstraintPointOnEllipse(*(thePoint->point()), *anEllipse));
    break;
    }
  case ENTITY_BSPLINE: {
    std::list<GCSConstraintPtr> aConstraints;
    aConstraints.push_back(GCSConstraintPtr(new GCS::ConstraintCurveValue(
        *thePoint->point(), thePoint->point()->x, *theEntity->entity(), theValue->scalar())));
    aConstraints.push_back(GCSConstraintPtr(new GCS::ConstraintCurveValue(
        *thePoint->point(), thePoint->point()->y, *theEntity->entity(), theValue->scalar())));
    return ConstraintWrapperPtr(new PlaneGCSSolver_ConstraintWrapper(aConstraints, theType));
  }
  default:
    return ConstraintWrapperPtr();
  }

  return ConstraintWrapperPtr(new PlaneGCSSolver_ConstraintWrapper(aNewConstr, theType));
}

ConstraintWrapperPtr createConstraintPointsCollinear(
    std::shared_ptr<PlaneGCSSolver_PointWrapper> thePoint1,
    std::shared_ptr<PlaneGCSSolver_PointWrapper> thePoint2,
    std::shared_ptr<PlaneGCSSolver_PointWrapper> thePoint3)
{
  GCSConstraintPtr aNewConstr(new GCS::ConstraintPointOnLine(
      *(thePoint1->point()), *(thePoint2->point()), *(thePoint3->point())));
  return ConstraintWrapperPtr(
      new PlaneGCSSolver_ConstraintWrapper(aNewConstr, CONSTRAINT_PT_ON_CURVE));
}

template <typename ARCTYPE>
void createConstraintMiddlePointOnArc(ARCTYPE theArc,
                                      GCSPointPtr thePoint,
                                      std::shared_ptr<PlaneGCSSolver_PointWrapper> theAuxParameters,
                                      std::list<GCSConstraintPtr>& theConstraints)
{
  double* u = theAuxParameters->point()->x;
  double* diff = theAuxParameters->point()->y;
  *u = (*theArc->startAngle + *theArc->endAngle) * 0.5;
  *diff = (*theArc->endAngle - *theArc->startAngle) * 0.5;

  theConstraints.push_back(GCSConstraintPtr(
      new GCS::ConstraintCurveValue(*thePoint, thePoint->x, *theArc, u)));
  theConstraints.push_back(GCSConstraintPtr(
      new GCS::ConstraintCurveValue(*thePoint, thePoint->y, *theArc, u)));
  theConstraints.push_back(GCSConstraintPtr(
      new GCS::ConstraintDifference(theArc->startAngle, u, diff)));
  theConstraints.push_back(GCSConstraintPtr(
      new GCS::ConstraintDifference(u, theArc->endAngle, diff)));
}

ConstraintWrapperPtr createConstraintMiddlePoint(
    std::shared_ptr<PlaneGCSSolver_PointWrapper> thePoint,
    std::shared_ptr<PlaneGCSSolver_EdgeWrapper> theEntity,
    std::shared_ptr<PlaneGCSSolver_PointWrapper> theAuxParameters)
{
  std::list<GCSConstraintPtr> aConstrList;

  GCSPointPtr aPoint = thePoint->point();
  std::shared_ptr<GCS::Line> aLine = std::dynamic_pointer_cast<GCS::Line>(theEntity->entity());
  if (aLine) {
    aConstrList.push_back(GCSConstraintPtr(new GCS::ConstraintPointOnLine(*aPoint, *aLine)));
    aConstrList.push_back(
        GCSConstraintPtr(new GCS::ConstraintPointOnPerpBisector(*aPoint, aLine->p1, aLine->p2)));
  }
  else {
    std::shared_ptr<GCS::Arc> anArc = std::dynamic_pointer_cast<GCS::Arc>(theEntity->entity());
    if (anArc)
      createConstraintMiddlePointOnArc(anArc, aPoint, theAuxParameters, aConstrList);
    else {
      std::shared_ptr<GCS::ArcOfEllipse> aEllArc =
          std::dynamic_pointer_cast<GCS::ArcOfEllipse>(theEntity->entity());
      if (aEllArc)
        createConstraintMiddlePointOnArc(aEllArc, aPoint, theAuxParameters, aConstrList);
    }
  }

  return aConstrList.empty() ? ConstraintWrapperPtr() : ConstraintWrapperPtr(
      new PlaneGCSSolver_ConstraintWrapper(aConstrList, CONSTRAINT_MIDDLE_POINT));
}


ConstraintWrapperPtr createConstraintDistancePointPoint(
    std::shared_ptr<PlaneGCSSolver_ScalarWrapper> theValue,
    std::shared_ptr<PlaneGCSSolver_PointWrapper>  thePoint1,
    std::shared_ptr<PlaneGCSSolver_PointWrapper>  thePoint2)
{
  GCSConstraintPtr aNewConstr(new GCS::ConstraintP2PDistance(
      *(thePoint1->point()), *(thePoint2->point()), theValue->scalar()));
  std::shared_ptr<PlaneGCSSolver_ConstraintWrapper> aResult(
      new PlaneGCSSolver_ConstraintWrapper(aNewConstr, CONSTRAINT_PT_PT_DISTANCE));
  aResult->setValueParameter(theValue);
  return aResult;
}

ConstraintWrapperPtr createConstraintDistancePointLine(
    std::shared_ptr<PlaneGCSSolver_ScalarWrapper> theValue,
    std::shared_ptr<PlaneGCSSolver_PointWrapper>  thePoint,
    std::shared_ptr<PlaneGCSSolver_EdgeWrapper> theEntity)
{
  std::shared_ptr<GCS::Line> aLine = std::dynamic_pointer_cast<GCS::Line>(theEntity->entity());
  GCSConstraintPtr aNewConstr(new GCS::ConstraintP2LDistance(
      *(thePoint->point()), *(aLine), theValue->scalar()));
  std::shared_ptr<PlaneGCSSolver_ConstraintWrapper> aResult(
      new PlaneGCSSolver_ConstraintWrapper(aNewConstr, CONSTRAINT_PT_LINE_DISTANCE));
  aResult->setValueParameter(theValue);
  return aResult;
}

ConstraintWrapperPtr createConstraintHVDistance(
    const SketchSolver_ConstraintType& theType,
    std::shared_ptr<PlaneGCSSolver_ScalarWrapper> theValue,
    std::shared_ptr<PlaneGCSSolver_PointWrapper> thePoint1,
    std::shared_ptr<PlaneGCSSolver_PointWrapper> thePoint2)
{
  GCSPointPtr aPoint1 = thePoint1->point();
  GCSPointPtr aPoint2 = thePoint2->point();

  double *aParam1, *aParam2;
  if (theType == CONSTRAINT_HORIZONTAL_DISTANCE) {
    aParam1 = aPoint1->x;
    aParam2 = aPoint2->x;
  } else if (theType == CONSTRAINT_VERTICAL_DISTANCE) {
    aParam1 = aPoint1->y;
    aParam2 = aPoint2->y;
  }

  GCSConstraintPtr aNewConstr(new GCS::ConstraintDifference(aParam1, aParam2, theValue->scalar()));

  std::shared_ptr<PlaneGCSSolver_ConstraintWrapper> aResult(
      new PlaneGCSSolver_ConstraintWrapper(aNewConstr, theType));
  aResult->setValueParameter(theValue);
  return aResult;
}

ConstraintWrapperPtr createConstraintRadius(
    std::shared_ptr<PlaneGCSSolver_ScalarWrapper> theValue,
    std::shared_ptr<PlaneGCSSolver_EdgeWrapper> theEntity)
{
  std::shared_ptr<GCS::Circle> aCircle =
    std::dynamic_pointer_cast<GCS::Circle>(theEntity->entity());
  GCSConstraintPtr aNewConstr(new GCS::ConstraintEqual(aCircle->rad, theValue->scalar()));
  std::shared_ptr<PlaneGCSSolver_ConstraintWrapper> aResult(
      new PlaneGCSSolver_ConstraintWrapper(aNewConstr, CONSTRAINT_RADIUS));
  aResult->setValueParameter(theValue);
  return aResult;
}

ConstraintWrapperPtr createConstraintAngle(
    ConstraintPtr theConstraint,
    std::shared_ptr<PlaneGCSSolver_ScalarWrapper> theValue,
    std::shared_ptr<PlaneGCSSolver_EdgeWrapper> theEntity1,
    std::shared_ptr<PlaneGCSSolver_EdgeWrapper> theEntity2)
{
  std::shared_ptr<GCS::Line> aLine1 = std::dynamic_pointer_cast<GCS::Line>(theEntity1->entity());
  std::shared_ptr<GCS::Line> aLine2 = std::dynamic_pointer_cast<GCS::Line>(theEntity2->entity());

  GCSConstraintPtr aNewConstr(new GCS::ConstraintL2LAngle(
      aLine1->p1, aLine1->p2, aLine2->p1, aLine2->p2, theValue->scalar()));

  std::shared_ptr<PlaneGCSSolver_ConstraintWrapper> aResult(
      new PlaneGCSSolver_ConstraintWrapper(aNewConstr, CONSTRAINT_ANGLE));
  aResult->setValueParameter(theValue);
  return aResult;
}

ConstraintWrapperPtr createConstraintHorizVert(
    const SketchSolver_ConstraintType& theType,
    std::shared_ptr<PlaneGCSSolver_EdgeWrapper> theEntity)
{
  std::shared_ptr<GCS::Line> aLine = std::dynamic_pointer_cast<GCS::Line>(theEntity->entity());
  GCSConstraintPtr aNewConstr;
  if (theType == CONSTRAINT_HORIZONTAL)
    aNewConstr = GCSConstraintPtr(new GCS::ConstraintEqual(aLine->p1.y, aLine->p2.y));
  else
    aNewConstr = GCSConstraintPtr(new GCS::ConstraintEqual(aLine->p1.x, aLine->p2.x));

  return ConstraintWrapperPtr(new PlaneGCSSolver_ConstraintWrapper(aNewConstr, theType));
}

ConstraintWrapperPtr createConstraintParallel(
    std::shared_ptr<PlaneGCSSolver_EdgeWrapper> theEntity1,
    std::shared_ptr<PlaneGCSSolver_EdgeWrapper> theEntity2)
{
  std::shared_ptr<GCS::Line> aLine1 = std::dynamic_pointer_cast<GCS::Line>(theEntity1->entity());
  std::shared_ptr<GCS::Line> aLine2 = std::dynamic_pointer_cast<GCS::Line>(theEntity2->entity());
  GCSConstraintPtr aNewConstr(new GCS::ConstraintParallel(*(aLine1), *(aLine2)));

  return ConstraintWrapperPtr(
      new PlaneGCSSolver_ConstraintWrapper(aNewConstr, CONSTRAINT_PARALLEL));
}

ConstraintWrapperPtr createConstraintPerpendicular(
    std::shared_ptr<PlaneGCSSolver_EdgeWrapper> theEntity1,
    std::shared_ptr<PlaneGCSSolver_EdgeWrapper> theEntity2)
{
  std::shared_ptr<GCS::Line> aLine1 = std::dynamic_pointer_cast<GCS::Line>(theEntity1->entity());
  std::shared_ptr<GCS::Line> aLine2 = std::dynamic_pointer_cast<GCS::Line>(theEntity2->entity());

  std::shared_ptr<GCS::Circle> aCirc1 =
      std::dynamic_pointer_cast<GCS::Circle>(theEntity1->entity());
  std::shared_ptr<GCS::Circle> aCirc2 =
      std::dynamic_pointer_cast<GCS::Circle>(theEntity2->entity());

  GCSConstraintPtr aNewConstr;
  if (aLine1 && aLine2)
    aNewConstr.reset(new GCS::ConstraintPerpendicular(*(aLine1), *(aLine2)));
  else {
    if (aLine1 && aCirc2)
      aCirc1 = aCirc2;
    else if (aLine2 && aCirc1)
      aLine1 = aLine2;

    aNewConstr.reset(new GCS::ConstraintPointOnLine(aCirc1->center, *aLine1));
  }

  return ConstraintWrapperPtr(
      new PlaneGCSSolver_ConstraintWrapper(aNewConstr, CONSTRAINT_PERPENDICULAR));
}

ConstraintWrapperPtr createConstraintAngleBetweenCurves(
    std::shared_ptr<PlaneGCSSolver_ScalarWrapper> theValue,
    std::shared_ptr<PlaneGCSSolver_PointWrapper> thePoint,
    std::shared_ptr<PlaneGCSSolver_EdgeWrapper> theEntity1,
    std::shared_ptr<PlaneGCSSolver_EdgeWrapper> theEntity2)
{
  GCSConstraintPtr aNewConstr(new GCS::ConstraintAngleViaPoint(
      *theEntity1->entity(), *theEntity2->entity(), *thePoint->point(), theValue->scalar()));
  return ConstraintWrapperPtr(
      new PlaneGCSSolver_ConstraintWrapper(aNewConstr, CONSTRAINT_PERPENDICULAR_CURVES));
}

ConstraintWrapperPtr createConstraintEqual(
    const SketchSolver_ConstraintType& theType,
    std::shared_ptr<PlaneGCSSolver_EdgeWrapper> theEntity1,
    std::shared_ptr<PlaneGCSSolver_EdgeWrapper> theEntity2,
    std::shared_ptr<PlaneGCSSolver_ScalarWrapper> theIntermed)
{
  if (theType == CONSTRAINT_EQUAL_LINE_ARC)
    return ConstraintWrapperPtr(); // line-arc equivalence is not supported yet

  std::list<GCSConstraintPtr> aConstrList;
  if (theType == CONSTRAINT_EQUAL_LINES) {
    std::shared_ptr<GCS::Line> aLine1 = std::dynamic_pointer_cast<GCS::Line>(theEntity1->entity());
    std::shared_ptr<GCS::Line> aLine2 = std::dynamic_pointer_cast<GCS::Line>(theEntity2->entity());

    aConstrList.push_back(GCSConstraintPtr(
        new GCS::ConstraintP2PDistance(aLine1->p1, aLine1->p2, theIntermed->scalar())));
    aConstrList.push_back(GCSConstraintPtr(
        new GCS::ConstraintP2PDistance(aLine2->p1, aLine2->p2, theIntermed->scalar())));
    // update value of intermediate parameter
    theIntermed->setValue(PlaneGCSSolver_Tools::distance(aLine1->p1, aLine1->p2));
  }
  else if (theType == CONSTRAINT_EQUAL_ELLIPSES) {
    std::shared_ptr<GCS::Ellipse> anEllipse1 =
        std::dynamic_pointer_cast<GCS::Ellipse>(theEntity1->entity());
    std::shared_ptr<GCS::Ellipse> anEllipse2 =
        std::dynamic_pointer_cast<GCS::Ellipse>(theEntity2->entity());

    aConstrList.push_back(GCSConstraintPtr(
        new GCS::ConstraintEqual(anEllipse1->radmin, anEllipse2->radmin)));
    aConstrList.push_back(GCSConstraintPtr(new GCS::ConstraintP2PDistance(
        anEllipse1->center, anEllipse1->focus1, theIntermed->scalar())));
    aConstrList.push_back(GCSConstraintPtr(new GCS::ConstraintP2PDistance(
        anEllipse2->center, anEllipse2->focus1, theIntermed->scalar())));
    // update value of intermediate parameter
    theIntermed->setValue(PlaneGCSSolver_Tools::distance(anEllipse1->center, anEllipse1->focus1));
  }
  else {
    std::shared_ptr<GCS::Circle> aCirc1 =
        std::dynamic_pointer_cast<GCS::Circle>(theEntity1->entity());
    std::shared_ptr<GCS::Circle> aCirc2 =
        std::dynamic_pointer_cast<GCS::Circle>(theEntity2->entity());
    aConstrList.push_back(GCSConstraintPtr(new GCS::ConstraintEqual(aCirc1->rad, aCirc2->rad)));
  }

  std::shared_ptr<PlaneGCSSolver_ConstraintWrapper> aResult(
      new PlaneGCSSolver_ConstraintWrapper(aConstrList, theType));
  if (theIntermed)
    aResult->setValueParameter(theIntermed);
  return aResult;
}

GCS::SET_pD scalarParameters(const ScalarWrapperPtr& theScalar)
{
  GCS::SET_pD aParams;
  aParams.insert(theScalar->scalar());
  return aParams;
}

GCS::SET_pD scalarArrayParameters(const EntityWrapperPtr& theArray)
{
  ScalarArrayWrapperPtr anArray =
      std::dynamic_pointer_cast<PlaneGCSSolver_ScalarArrayWrapper>(theArray);
  return GCS::SET_pD(anArray->array().begin(), anArray->array().end());
}

GCS::SET_pD pointParameters(const PointWrapperPtr& thePoint)
{
  GCS::SET_pD aParams;
  aParams.insert(thePoint->point()->x);
  aParams.insert(thePoint->point()->y);
  return aParams;
}

GCS::SET_pD pointArrayParameters(const EntityWrapperPtr& theArray)
{
  GCS::SET_pD aParams;
  PointArrayWrapperPtr aPoints =
      std::dynamic_pointer_cast<PlaneGCSSolver_PointArrayWrapper>(theArray);
  for (std::vector<PointWrapperPtr>::const_iterator anIt = aPoints->array().begin();
       anIt != aPoints->array().end(); ++anIt) {
    GCS::SET_pD aPointParams = PlaneGCSSolver_Tools::parameters(*anIt);
    aParams.insert(aPointParams.begin(), aPointParams.end());
  }
  return aParams;
}

GCS::SET_pD lineParameters(const EdgeWrapperPtr& theLine)
{
  GCS::SET_pD aParams;
  std::shared_ptr<GCS::Line> aLine = std::dynamic_pointer_cast<GCS::Line>(theLine->entity());
  aParams.insert(aLine->p1.x);
  aParams.insert(aLine->p1.y);
  aParams.insert(aLine->p2.x);
  aParams.insert(aLine->p2.y);
  return aParams;
}

GCS::SET_pD circleParameters(const EdgeWrapperPtr& theCircle)
{
  GCS::SET_pD aParams;
  std::shared_ptr<GCS::Circle> aCirc = std::dynamic_pointer_cast<GCS::Circle>(theCircle->entity());
  aParams.insert(aCirc->center.x);
  aParams.insert(aCirc->center.y);
  aParams.insert(aCirc->rad);
  return aParams;
}

GCS::SET_pD arcParameters(const EdgeWrapperPtr& theArc)
{
  GCS::SET_pD aParams = circleParameters(theArc);
  std::shared_ptr<GCS::Arc> anArc = std::dynamic_pointer_cast<GCS::Arc>(theArc->entity());
  aParams.insert(anArc->start.x);
  aParams.insert(anArc->start.y);
  aParams.insert(anArc->end.x);
  aParams.insert(anArc->end.y);
  aParams.insert(anArc->startAngle);
  aParams.insert(anArc->endAngle);
  return aParams;
}

GCS::SET_pD ellipseParameters(const EdgeWrapperPtr& theEllipse)
{
  GCS::SET_pD aParams;
  std::shared_ptr<GCS::Ellipse> anEllipse =
      std::dynamic_pointer_cast<GCS::Ellipse>(theEllipse->entity());
  aParams.insert(anEllipse->center.x);
  aParams.insert(anEllipse->center.y);
  aParams.insert(anEllipse->focus1.x);
  aParams.insert(anEllipse->focus1.y);
  aParams.insert(anEllipse->radmin);
  return aParams;
}

GCS::SET_pD ellipticArcParameters(const EdgeWrapperPtr& theEllipticArc)
{
  GCS::SET_pD aParams = ellipseParameters(theEllipticArc);
  std::shared_ptr<GCS::ArcOfEllipse> anArc =
      std::dynamic_pointer_cast<GCS::ArcOfEllipse>(theEllipticArc->entity());
  aParams.insert(anArc->start.x);
  aParams.insert(anArc->start.y);
  aParams.insert(anArc->end.x);
  aParams.insert(anArc->end.y);
  aParams.insert(anArc->startAngle);
  aParams.insert(anArc->endAngle);
  return aParams;
}

GCS::SET_pD bsplineParameters(const EdgeWrapperPtr& theEdge)
{
  GCS::SET_pD aParams;

  std::shared_ptr<GCS::BSpline> aBSpline =
    std::dynamic_pointer_cast<GCS::BSpline>(theEdge->entity());

  for (GCS::VEC_P::iterator it = aBSpline->poles.begin(); it != aBSpline->poles.end(); ++it) {
    aParams.insert(it->x);
    aParams.insert(it->y);
  }
  for (GCS::VEC_pD::iterator it = aBSpline->weights.begin(); it != aBSpline->weights.end(); ++it)
    aParams.insert(*it);

  return aParams;
}
