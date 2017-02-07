// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    PlaneGCSSolver_Builder.cpp
// Created: 14 Dec 2015
// Author:  Artem ZHIDKOV

#include <PlaneGCSSolver_Builder.h>
#include <PlaneGCSSolver_Storage.h>
#include <PlaneGCSSolver_EntityWrapper.h>
#include <PlaneGCSSolver_PointWrapper.h>
#include <PlaneGCSSolver_ScalarWrapper.h>
#include <PlaneGCSSolver_ConstraintWrapper.h>

#include <SketchSolver_Manager.h>

#include <GeomDataAPI_Point2D.h>
#include <SketchPlugin_Line.h>
#include <SketchPlugin_ConstraintAngle.h>

#include <cmath>


#define GCS_ENTITY_WRAPPER(x) std::dynamic_pointer_cast<PlaneGCSSolver_EntityWrapper>(x)
#define GCS_POINT_WRAPPER(x)  std::dynamic_pointer_cast<PlaneGCSSolver_PointWrapper>(x)
#define GCS_SCALAR_WRAPPER(x) std::dynamic_pointer_cast<PlaneGCSSolver_ScalarWrapper>(x)



static ConstraintWrapperPtr
  createConstraintCoincidence(std::shared_ptr<PlaneGCSSolver_PointWrapper> thePoint1,
                              std::shared_ptr<PlaneGCSSolver_PointWrapper> thePoint2);
static ConstraintWrapperPtr
  createConstraintPointOnEntity(const SketchSolver_ConstraintType& theType,
                                std::shared_ptr<PlaneGCSSolver_PointWrapper> thePoint,
                                std::shared_ptr<PlaneGCSSolver_EntityWrapper> theEntity);
static ConstraintWrapperPtr
  createConstraintDistancePointPoint(std::shared_ptr<PlaneGCSSolver_ScalarWrapper> theValue,
                                     std::shared_ptr<PlaneGCSSolver_PointWrapper> thePoint1,
                                     std::shared_ptr<PlaneGCSSolver_PointWrapper> thePoint2);
static ConstraintWrapperPtr
  createConstraintDistancePointLine(std::shared_ptr<PlaneGCSSolver_ScalarWrapper> theValue,
                                    std::shared_ptr<PlaneGCSSolver_PointWrapper>  thePoint,
                                    std::shared_ptr<PlaneGCSSolver_EntityWrapper> theEntity);
static ConstraintWrapperPtr
  createConstraintRadius(std::shared_ptr<PlaneGCSSolver_ScalarWrapper> theValue,
                         std::shared_ptr<PlaneGCSSolver_EntityWrapper> theEntity);
static ConstraintWrapperPtr
  createConstraintAngle(ConstraintPtr theConstraint,
                        std::shared_ptr<PlaneGCSSolver_ScalarWrapper> theValue,
                        std::shared_ptr<PlaneGCSSolver_EntityWrapper> theEntity1,
                        std::shared_ptr<PlaneGCSSolver_EntityWrapper> theEntity2);
static ConstraintWrapperPtr
  createConstraintHorizVert(const SketchSolver_ConstraintType& theType,
                            std::shared_ptr<PlaneGCSSolver_EntityWrapper> theEntity);
static ConstraintWrapperPtr
  createConstraintParallel(std::shared_ptr<PlaneGCSSolver_EntityWrapper> theEntity1,
                           std::shared_ptr<PlaneGCSSolver_EntityWrapper> theEntity2);
static ConstraintWrapperPtr
  createConstraintPerpendicular(std::shared_ptr<PlaneGCSSolver_EntityWrapper> theEntity1,
                                std::shared_ptr<PlaneGCSSolver_EntityWrapper> theEntity2);
static ConstraintWrapperPtr
  createConstraintEqual(const SketchSolver_ConstraintType& theType,
                        std::shared_ptr<PlaneGCSSolver_EntityWrapper> theEntity1,
                        std::shared_ptr<PlaneGCSSolver_EntityWrapper> theEntity2,
                        std::shared_ptr<PlaneGCSSolver_ScalarWrapper> theIntermed);
static ConstraintWrapperPtr
  createConstraintTangent(const SketchSolver_ConstraintType& theType,
                          std::shared_ptr<PlaneGCSSolver_EntityWrapper> theEntity1,
                          std::shared_ptr<PlaneGCSSolver_EntityWrapper> theEntity2);
static ConstraintWrapperPtr
  createConstraintCollinear(ConstraintPtr theConstraint,
                           std::shared_ptr<PlaneGCSSolver_EntityWrapper> theEntity1,
                           std::shared_ptr<PlaneGCSSolver_EntityWrapper> theEntity2);
static ConstraintWrapperPtr
  createConstraintMiddlePoint(std::shared_ptr<PlaneGCSSolver_PointWrapper> thePoint,
                              std::shared_ptr<PlaneGCSSolver_EntityWrapper> theEntity);



// Initialization of pointer to builder
static BuilderPtr gBuilder = PlaneGCSSolver_Builder::getInstance();

BuilderPtr PlaneGCSSolver_Builder::getInstance()
{
  static BuilderPtr mySelf;
  if (!mySelf) {
    mySelf = BuilderPtr(new PlaneGCSSolver_Builder);
    SketchSolver_Manager::instance()->setBuilder(mySelf);
  }
  return mySelf;
}

StoragePtr PlaneGCSSolver_Builder::createStorage(const SolverPtr& theSolver) const
{
  return StoragePtr(new PlaneGCSSolver_Storage(theSolver));
}

SolverPtr PlaneGCSSolver_Builder::createSolver() const
{
  return SolverPtr(new PlaneGCSSolver_Solver);
}


ConstraintWrapperPtr PlaneGCSSolver_Builder::createConstraint(
    ConstraintPtr theConstraint,
    const SketchSolver_ConstraintType& theType,
    const EntityWrapperPtr& theValue,
    const EntityWrapperPtr& thePoint1,
    const EntityWrapperPtr& thePoint2,
    const EntityWrapperPtr& theEntity1,
    const EntityWrapperPtr& theEntity2) const
{
  ConstraintWrapperPtr aResult;
  ScalarWrapperPtr anIntermediate;

  std::shared_ptr<PlaneGCSSolver_PointWrapper> aPoint1 = GCS_POINT_WRAPPER(thePoint1);
  std::shared_ptr<PlaneGCSSolver_PointWrapper> aPoint2 = GCS_POINT_WRAPPER(thePoint2);

  switch (theType) {
  case CONSTRAINT_PT_PT_COINCIDENT:
    aResult = createConstraintCoincidence(aPoint1, aPoint2);
    break;
  case CONSTRAINT_PT_ON_LINE:
  case CONSTRAINT_PT_ON_CIRCLE:
    aResult = createConstraintPointOnEntity(theType, aPoint1, GCS_ENTITY_WRAPPER(theEntity1));
    break;
  case CONSTRAINT_MIDDLE_POINT:
    aResult = createConstraintMiddlePoint(aPoint1, GCS_ENTITY_WRAPPER(theEntity1));
    break;
  case CONSTRAINT_PT_PT_DISTANCE:
    aResult = createConstraintDistancePointPoint(GCS_SCALAR_WRAPPER(theValue), aPoint1, aPoint2);
    break;
  case CONSTRAINT_PT_LINE_DISTANCE:
    aResult = createConstraintDistancePointLine(GCS_SCALAR_WRAPPER(theValue),
                                                aPoint1,
                                                GCS_ENTITY_WRAPPER(theEntity1));
    break;
  case CONSTRAINT_RADIUS:
    aResult = createConstraintRadius(GCS_SCALAR_WRAPPER(theValue),
                                     GCS_ENTITY_WRAPPER(theEntity1));
    break;
  case CONSTRAINT_ANGLE:
    aResult = createConstraintAngle(theConstraint,
                  GCS_SCALAR_WRAPPER(theValue),
                  GCS_ENTITY_WRAPPER(theEntity1), GCS_ENTITY_WRAPPER(theEntity2));
    break;
  case CONSTRAINT_FIXED:
    break;
  case CONSTRAINT_HORIZONTAL:
  case CONSTRAINT_VERTICAL:
    aResult = createConstraintHorizVert(theType, GCS_ENTITY_WRAPPER(theEntity1));
    break;
  case CONSTRAINT_PARALLEL:
    aResult = createConstraintParallel(GCS_ENTITY_WRAPPER(theEntity1),
                                       GCS_ENTITY_WRAPPER(theEntity2));
    break;
  case CONSTRAINT_PERPENDICULAR:
    aResult = createConstraintPerpendicular(GCS_ENTITY_WRAPPER(theEntity1),
                                            GCS_ENTITY_WRAPPER(theEntity2));
    break;
  case CONSTRAINT_EQUAL_LINES:
    anIntermediate = GCS_SCALAR_WRAPPER(theValue); // parameter is used to store length of lines
  case CONSTRAINT_EQUAL_LINE_ARC:
  case CONSTRAINT_EQUAL_RADIUS:
    aResult = createConstraintEqual(theType,
                                    GCS_ENTITY_WRAPPER(theEntity1),
                                    GCS_ENTITY_WRAPPER(theEntity2),
                                    anIntermediate);
    break;
  case CONSTRAINT_TANGENT_ARC_LINE:
  case CONSTRAINT_TANGENT_CIRCLE_LINE:
  case CONSTRAINT_TANGENT_ARC_ARC:
    aResult = createConstraintTangent(theType,
                                      GCS_ENTITY_WRAPPER(theEntity1),
                                      GCS_ENTITY_WRAPPER(theEntity2));
    break;
  case CONSTRAINT_COLLINEAR:
    aResult = createConstraintCollinear(theConstraint,
                  GCS_ENTITY_WRAPPER(theEntity1), GCS_ENTITY_WRAPPER(theEntity2));
    break;
  case CONSTRAINT_MULTI_TRANSLATION:
  case CONSTRAINT_MULTI_ROTATION:
  case CONSTRAINT_SYMMETRIC:
  default:
    break;
  }

  return aResult;
}

ConstraintWrapperPtr PlaneGCSSolver_Builder::createConstraint(
    ConstraintPtr theConstraint,
    const SketchSolver_ConstraintType& theType,
    const EntityWrapperPtr& theValue,
    const bool theFullValue,
    const EntityWrapperPtr& thePoint1,
    const EntityWrapperPtr& thePoint2,
    const std::list<EntityWrapperPtr>& theTrsfEnt) const
{
  if (theType != CONSTRAINT_MULTI_ROTATION && theType != CONSTRAINT_MULTI_TRANSLATION)
    return ConstraintWrapperPtr();

  std::list<EntityWrapperPtr> aConstrAttrList = theTrsfEnt;
  if (thePoint2)
    aConstrAttrList.push_front(thePoint2);
  aConstrAttrList.push_front(thePoint1);

  ScalarWrapperPtr aValue = GCS_SCALAR_WRAPPER(theValue);

  std::shared_ptr<PlaneGCSSolver_ConstraintWrapper> aResult(
      new PlaneGCSSolver_ConstraintWrapper(std::list<GCSConstraintPtr>(), theType));
  aResult->setEntities(aConstrAttrList);
  if (aValue)
    aResult->setValueParameter(aValue);
  aResult->setIsFullValue(theFullValue);
  return aResult;
}

std::shared_ptr<GeomAPI_Pnt2d> PlaneGCSSolver_Builder::point(EntityWrapperPtr theEntity) const
{
  if (theEntity->type() != ENTITY_POINT)
    return std::shared_ptr<GeomAPI_Pnt2d>();

  std::shared_ptr<PlaneGCSSolver_PointWrapper> aPointWrapper =
      std::dynamic_pointer_cast<PlaneGCSSolver_PointWrapper>(theEntity);
  const GCSPointPtr& aPoint = aPointWrapper->point();
  return std::shared_ptr<GeomAPI_Pnt2d>(new GeomAPI_Pnt2d(*aPoint->x, *aPoint->y));
}

std::shared_ptr<GeomAPI_Lin2d> PlaneGCSSolver_Builder::line(EntityWrapperPtr theEntity) const
{
  if (theEntity->type() != ENTITY_LINE)
    return std::shared_ptr<GeomAPI_Lin2d>();

  std::shared_ptr<PlaneGCSSolver_EntityWrapper> anEntity =
      std::dynamic_pointer_cast<PlaneGCSSolver_EntityWrapper>(theEntity);
  std::shared_ptr<GCS::Line> aLine = std::dynamic_pointer_cast<GCS::Line>(anEntity->entity());
  return std::shared_ptr<GeomAPI_Lin2d>(
      new GeomAPI_Lin2d(*(aLine->p1.x), *(aLine->p1.y), *(aLine->p2.x), *(aLine->p2.y)));
}

std::shared_ptr<GeomAPI_Lin2d> PlaneGCSSolver_Builder::line(FeaturePtr theFeature) const
{
  if (theFeature->getKind() != SketchPlugin_Line::ID())
    return std::shared_ptr<GeomAPI_Lin2d>();

  AttributePoint2DPtr aStart = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      theFeature->attribute(SketchPlugin_Line::START_ID()));
  AttributePoint2DPtr aEnd = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      theFeature->attribute(SketchPlugin_Line::END_ID()));

  return std::shared_ptr<GeomAPI_Lin2d>(new GeomAPI_Lin2d(aStart->pnt(), aEnd->pnt()));
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
    std::shared_ptr<PlaneGCSSolver_EntityWrapper> theEntity)
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
  default:
    return ConstraintWrapperPtr();
  }

  return ConstraintWrapperPtr(new PlaneGCSSolver_ConstraintWrapper(aNewConstr, theType));
}

ConstraintWrapperPtr createConstraintMiddlePoint(
    std::shared_ptr<PlaneGCSSolver_PointWrapper> thePoint,
    std::shared_ptr<PlaneGCSSolver_EntityWrapper> theEntity)
{
  GCSPointPtr aPoint = thePoint->point();
  std::shared_ptr<GCS::Line> aLine = std::dynamic_pointer_cast<GCS::Line>(theEntity->entity());
  if (!aLine)
    return ConstraintWrapperPtr();

  std::list<GCSConstraintPtr> aConstrList;
  aConstrList.push_back(
      GCSConstraintPtr(new GCS::ConstraintPointOnPerpBisector(*aPoint, aLine->p1, aLine->p2)));
  aConstrList.push_back(GCSConstraintPtr(new GCS::ConstraintPointOnLine(*aPoint, *aLine)));

  return ConstraintWrapperPtr(
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
    std::shared_ptr<PlaneGCSSolver_EntityWrapper> theEntity)
{
  std::shared_ptr<GCS::Line> aLine = std::dynamic_pointer_cast<GCS::Line>(theEntity->entity());
  GCSConstraintPtr aNewConstr(new GCS::ConstraintP2LDistance(
      *(thePoint->point()), *(aLine), theValue->scalar()));
  std::shared_ptr<PlaneGCSSolver_ConstraintWrapper> aResult(
      new PlaneGCSSolver_ConstraintWrapper(aNewConstr, CONSTRAINT_PT_LINE_DISTANCE));
  aResult->setValueParameter(theValue);
  return aResult;
}

ConstraintWrapperPtr createConstraintRadius(
    std::shared_ptr<PlaneGCSSolver_ScalarWrapper> theValue,
    std::shared_ptr<PlaneGCSSolver_EntityWrapper> theEntity)
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
    std::shared_ptr<PlaneGCSSolver_EntityWrapper> theEntity1,
    std::shared_ptr<PlaneGCSSolver_EntityWrapper> theEntity2)
{
  std::shared_ptr<GCS::Line> aLine1 = std::dynamic_pointer_cast<GCS::Line>(theEntity1->entity());
  bool isLine1Rev = theConstraint->boolean(
      SketchPlugin_ConstraintAngle::ANGLE_REVERSED_FIRST_LINE_ID())->value();
  GCS::Point aLine1Pt1 = isLine1Rev ? aLine1->p2 : aLine1->p1;
  GCS::Point aLine1Pt2 = isLine1Rev ? aLine1->p1 : aLine1->p2;

  std::shared_ptr<GCS::Line> aLine2 = std::dynamic_pointer_cast<GCS::Line>(theEntity2->entity());
  bool isLine2Rev = theConstraint->boolean(
      SketchPlugin_ConstraintAngle::ANGLE_REVERSED_SECOND_LINE_ID())->value();
  GCS::Point aLine2Pt1 = isLine2Rev ? aLine2->p2 : aLine2->p1;
  GCS::Point aLine2Pt2 = isLine2Rev ? aLine2->p1 : aLine2->p2;

  GCSConstraintPtr aNewConstr(new GCS::ConstraintL2LAngle(
      aLine1Pt1, aLine1Pt2, aLine2Pt1, aLine2Pt2, theValue->scalar()));

  std::shared_ptr<PlaneGCSSolver_ConstraintWrapper> aResult(
      new PlaneGCSSolver_ConstraintWrapper(aNewConstr, CONSTRAINT_ANGLE));
  aResult->setValueParameter(theValue);
  return aResult;
}

ConstraintWrapperPtr createConstraintHorizVert(
    const SketchSolver_ConstraintType& theType,
    std::shared_ptr<PlaneGCSSolver_EntityWrapper> theEntity)
{
  std::shared_ptr<GCS::Line> aLine = std::dynamic_pointer_cast<GCS::Line>(theEntity->entity());
  GCSConstraintPtr aNewConstr;
  if (theType == CONSTRAINT_HORIZONTAL)
    aNewConstr = GCSConstraintPtr(new GCS::ConstraintEqual(aLine->p1.y, aLine->p2.y));
  else
    aNewConstr = GCSConstraintPtr(new GCS::ConstraintEqual(aLine->p1.x, aLine->p2.x));

  return ConstraintWrapperPtr(new PlaneGCSSolver_ConstraintWrapper(aNewConstr, theType));
}

ConstraintWrapperPtr createConstraintCollinear(
    ConstraintPtr theConstraint,
    std::shared_ptr<PlaneGCSSolver_EntityWrapper> theEntity1,
    std::shared_ptr<PlaneGCSSolver_EntityWrapper> theEntity2)
{
  std::shared_ptr<GCS::Line> aLine1 = std::dynamic_pointer_cast<GCS::Line>(theEntity1->entity());
  std::shared_ptr<GCS::Line> aLine2 = std::dynamic_pointer_cast<GCS::Line>(theEntity2->entity());

  // create two point-on-line constraints
  std::list<GCSConstraintPtr> aConstrList;
  aConstrList.push_back( GCSConstraintPtr(new GCS::ConstraintPointOnLine(aLine2->p1, *aLine1)) );
  aConstrList.push_back( GCSConstraintPtr(new GCS::ConstraintPointOnLine(aLine2->p2, *aLine1)) );

  return ConstraintWrapperPtr(
      new PlaneGCSSolver_ConstraintWrapper(aConstrList, CONSTRAINT_COLLINEAR));
}

ConstraintWrapperPtr createConstraintParallel(
    std::shared_ptr<PlaneGCSSolver_EntityWrapper> theEntity1,
    std::shared_ptr<PlaneGCSSolver_EntityWrapper> theEntity2)
{
  std::shared_ptr<GCS::Line> aLine1 = std::dynamic_pointer_cast<GCS::Line>(theEntity1->entity());
  std::shared_ptr<GCS::Line> aLine2 = std::dynamic_pointer_cast<GCS::Line>(theEntity2->entity());
  GCSConstraintPtr aNewConstr(new GCS::ConstraintParallel(*(aLine1), *(aLine2)));

  return ConstraintWrapperPtr(
      new PlaneGCSSolver_ConstraintWrapper(aNewConstr, CONSTRAINT_PARALLEL));
}

ConstraintWrapperPtr createConstraintPerpendicular(
    std::shared_ptr<PlaneGCSSolver_EntityWrapper> theEntity1,
    std::shared_ptr<PlaneGCSSolver_EntityWrapper> theEntity2)
{
  std::shared_ptr<GCS::Line> aLine1 = std::dynamic_pointer_cast<GCS::Line>(theEntity1->entity());
  std::shared_ptr<GCS::Line> aLine2 = std::dynamic_pointer_cast<GCS::Line>(theEntity2->entity());
  GCSConstraintPtr aNewConstr(new GCS::ConstraintPerpendicular(*(aLine1), *(aLine2)));

  return ConstraintWrapperPtr(
      new PlaneGCSSolver_ConstraintWrapper(aNewConstr, CONSTRAINT_PERPENDICULAR));
}

ConstraintWrapperPtr createConstraintEqual(
    const SketchSolver_ConstraintType& theType,
    std::shared_ptr<PlaneGCSSolver_EntityWrapper> theEntity1,
    std::shared_ptr<PlaneGCSSolver_EntityWrapper> theEntity2,
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
    double x = *aLine1->p1.x - *aLine1->p2.x;
    double y = *aLine1->p1.y - *aLine1->p2.y;
    double aLen = sqrt(x*x + y*y);
    theIntermed->setValue(aLen);
  } else {
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

ConstraintWrapperPtr createConstraintTangent(
    const SketchSolver_ConstraintType& theType,
    std::shared_ptr<PlaneGCSSolver_EntityWrapper> theEntity1,
    std::shared_ptr<PlaneGCSSolver_EntityWrapper> theEntity2)
{
  GCSConstraintPtr aNewConstr;
  if (theType == CONSTRAINT_TANGENT_ARC_LINE || theType == CONSTRAINT_TANGENT_CIRCLE_LINE) {
    std::shared_ptr<GCS::Circle> aCirc =
      std::dynamic_pointer_cast<GCS::Circle>(theEntity1->entity());
    std::shared_ptr<GCS::Line> aLine =
      std::dynamic_pointer_cast<GCS::Line>(theEntity2->entity());

    aNewConstr =
      GCSConstraintPtr(new GCS::ConstraintP2LDistance(aCirc->center, *aLine, aCirc->rad));
  } else {
    std::shared_ptr<GCS::Circle> aCirc1 =
      std::dynamic_pointer_cast<GCS::Circle>(theEntity1->entity());
    std::shared_ptr<GCS::Circle> aCirc2 =
      std::dynamic_pointer_cast<GCS::Circle>(theEntity2->entity());

    double aDX = *(aCirc1->center.x) - *(aCirc2->center.x);
    double aDY = *(aCirc1->center.y) - *(aCirc2->center.y);
    double aDist = sqrt(aDX * aDX + aDY * aDY);
    aNewConstr = GCSConstraintPtr(new GCS::ConstraintTangentCircumf(aCirc1->center, aCirc2->center,
        aCirc1->rad, aCirc2->rad, (aDist < *(aCirc1->rad) || aDist < *(aCirc2->rad))));
  }

  return ConstraintWrapperPtr(new PlaneGCSSolver_ConstraintWrapper(aNewConstr, theType));
}

bool PlaneGCSSolver_Builder::isArcArcTangencyInternal(
    EntityWrapperPtr theArc1, EntityWrapperPtr theArc2) const
{
  std::shared_ptr<GCS::Circle> aCirc1 = std::dynamic_pointer_cast<GCS::Circle>(
      GCS_ENTITY_WRAPPER(theArc1)->entity());
  std::shared_ptr<GCS::Circle> aCirc2 = std::dynamic_pointer_cast<GCS::Circle>(
      GCS_ENTITY_WRAPPER(theArc2)->entity());

  if (!aCirc1 || !aCirc2)
    return false;

  double aDX = *(aCirc1->center.x) - *(aCirc2->center.x);
  double aDY = *(aCirc1->center.y) - *(aCirc2->center.y);
  double aDist = sqrt(aDX * aDX + aDY * aDY);

  return (aDist < *(aCirc1->rad) || aDist < *(aCirc2->rad));
}
