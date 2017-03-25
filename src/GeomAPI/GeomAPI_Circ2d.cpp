// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAPI_Circ2d.cpp
// Created:     29 May 2014
// Author:      Artem ZHIDKOV

#include <GeomAPI_Circ2d.h>
#include <GeomAPI_Ax3.h>
#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_Dir2d.h>
#include <GeomAPI_Shape.h>

#include <BRep_Tool.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Ax2d.hxx>
#include <GccAna_Circ2d3Tan.hxx>
#include <GccAna_Circ2dTanCen.hxx>
#include <GccEnt.hxx>
#include <GccEnt_QualifiedCirc.hxx>
#include <GccEnt_QualifiedLin.hxx>
#include <GeomLib_Tool.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2dAPI_ProjectPointOnCurve.hxx>
#include <Geom_Plane.hxx>
#include <IntAna2d_AnaIntersection.hxx>
#include <Precision.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>

#include <vector>

#define MY_CIRC2D implPtr<gp_Circ2d>()

typedef std::shared_ptr<Geom2dAdaptor_Curve>  CurveAdaptorPtr;

class CircleBuilder
{
public:
  CircleBuilder(const std::shared_ptr<GeomAPI_Ax3>& theBasePlane)
    : myPlane(new Geom_Plane(theBasePlane->impl<gp_Ax3>()))
  {}

  void addCenter(const std::shared_ptr<GeomAPI_Pnt2d>& theCenter)
  { myCenter = theCenter; }

  void addPassingEntity(const std::shared_ptr<GeomAPI_Interface>& theEntity)
  {
    std::shared_ptr<GeomAPI_Pnt2d> aPoint = std::dynamic_pointer_cast<GeomAPI_Pnt2d>(theEntity);
    if (aPoint)
      addPassingPoint(aPoint);
    else {
      std::shared_ptr<GeomAPI_Shape> aShape = std::dynamic_pointer_cast<GeomAPI_Shape>(theEntity);
      if (aShape)
        addTangentCurve(aShape);
    }
  }

  void addTangentCurve(const std::shared_ptr<GeomAPI_Shape>& theEdge)
  {
    if (!theEdge->isEdge())
      return;

    const TopoDS_Edge& anEdge = TopoDS::Edge(theEdge->impl<TopoDS_Shape>());

    double aFirst, aLast;
    TopLoc_Location aLoc;
    CurveAdaptorPtr aCurve(new Geom2dAdaptor_Curve(
        BRep_Tool::CurveOnSurface(anEdge, myPlane, aLoc, aFirst, aLast)));

    myTangentShapes.push_back(aCurve);
  }

  void addPassingPoint(const std::shared_ptr<GeomAPI_Pnt2d>& thePoint)
  {
    myPassingPoints.push_back(thePoint->impl<gp_Pnt2d>());
  }

  gp_Circ2d* circle()
  {
    gp_Circ2d* aResult = 0;
    if (myCenter) {
      if (myPassingPoints.size() == 1)
        aResult = circleByCenterAndPassingPoint();
      else if (myTangentShapes.size() == 1)
        aResult = circleByCenterAndTangent();
    } else {
      switch (myPassingPoints.size()) {
      case 0:
        aResult = circleByThreeTangentCurves();
        break;
      case 1:
        aResult = circleByPointAndTwoTangentCurves();
        break;
      case 2:
        aResult = circleByTwoPointsAndTangentCurve();
        break;
      case 3:
        aResult = circleByThreePassingPoints();
        break;
      default:
        break;
      }
    }
    return aResult;
  }

private:
  gp_Circ2d* circleByCenterAndPassingPoint()
  {
    const gp_Pnt2d& aCenter = myCenter->impl<gp_Pnt2d>();
    GccAna_Circ2dTanCen aBuilder(myPassingPoints[0], aCenter);
    if (aBuilder.NbSolutions() > 0)
      return new gp_Circ2d(aBuilder.ThisSolution(1));
    return 0;
  }

  gp_Circ2d* circleByCenterAndTangent()
  {
    const gp_Pnt2d& aCenter = myCenter->impl<gp_Pnt2d>();
    CurveAdaptorPtr aCurve = myTangentShapes[0];

    std::shared_ptr<GccAna_Circ2dTanCen> aCircleBuilder;
    if (aCurve->GetType() == GeomAbs_Line) {
      aCircleBuilder = std::shared_ptr<GccAna_Circ2dTanCen>(
          new GccAna_Circ2dTanCen(aCurve->Line(), aCenter));
    } else if (aCurve->GetType() == GeomAbs_Circle) {
      aCircleBuilder = std::shared_ptr<GccAna_Circ2dTanCen>(new GccAna_Circ2dTanCen(
          GccEnt::Unqualified(aCurve->Circle()), aCenter, Precision::Confusion()));
    }

    return getProperCircle(aCircleBuilder);
  }

  gp_Circ2d* getProperCircle(const std::shared_ptr<GccAna_Circ2dTanCen>& theBuilder) const
  {
    if (!theBuilder)
      return 0;

    CurveAdaptorPtr aCurve = myTangentShapes[0];

    gp_Circ2d* aResult = 0;
    int aNbSol = theBuilder->NbSolutions();
    double aParSol, aPonTgCurve;
    gp_Pnt2d aTgPnt;
    for (int i = 1; i <= aNbSol && aCurve; ++i) {
      theBuilder->Tangency1(i, aParSol, aPonTgCurve, aTgPnt);
      if (aPonTgCurve >= aCurve->FirstParameter() && aPonTgCurve <= aCurve->LastParameter()) {
        aResult = new gp_Circ2d(theBuilder->ThisSolution(i));
        break;
      }
    }
    // unable to build circle passing through the tangent curve,
    // so get a circle passing any tangent point
    if (!aResult && aNbSol > 0)
      aResult =  new gp_Circ2d(theBuilder->ThisSolution(1));
    return aResult;
  }


  gp_Circ2d* circleByThreeTangentCurves()
  {
    std::shared_ptr<GccEnt_QualifiedCirc> aTgCirc[3];
    std::shared_ptr<GccEnt_QualifiedLin>  aTgLine[3];
    int aNbTgCirc = 0;
    int aNbTgLine = 0;

    std::vector<CurveAdaptorPtr>::iterator anIt = myTangentShapes.begin();
    for (; anIt != myTangentShapes.end(); ++anIt) {
      switch ((*anIt)->GetType()) {
      case GeomAbs_Line:
        aTgLine[aNbTgLine++] = std::shared_ptr<GccEnt_QualifiedLin>(
            new GccEnt_QualifiedLin((*anIt)->Line(), GccEnt_unqualified));
        break;
      case GeomAbs_Circle:
        aTgCirc[aNbTgCirc++] = std::shared_ptr<GccEnt_QualifiedCirc>(
            new GccEnt_QualifiedCirc((*anIt)->Circle(), GccEnt_unqualified));
        break;
      default:
        break;
      }
    }
    if (aNbTgCirc + aNbTgLine != 3)
      return 0;

    std::shared_ptr<GccAna_Circ2d3Tan> aCircleBuilder;
    switch (aNbTgLine) {
    case 0:
      aCircleBuilder = std::shared_ptr<GccAna_Circ2d3Tan>(new GccAna_Circ2d3Tan(
          *aTgCirc[0], *aTgCirc[1], *aTgCirc[2], Precision::Confusion()));
      break;
    case 1:
      aCircleBuilder = std::shared_ptr<GccAna_Circ2d3Tan>(new GccAna_Circ2d3Tan(
          *aTgCirc[0], *aTgCirc[1], *aTgLine[0], Precision::Confusion()));
      break;
    case 2:
      aCircleBuilder = std::shared_ptr<GccAna_Circ2d3Tan>(new GccAna_Circ2d3Tan(
          *aTgCirc[0], *aTgLine[0], *aTgLine[1], Precision::Confusion()));
      break;
    case 3:
      aCircleBuilder = std::shared_ptr<GccAna_Circ2d3Tan>(new GccAna_Circ2d3Tan(
          *aTgLine[0], *aTgLine[1], *aTgLine[0], Precision::Confusion()));
      break;
    default:
      break;
    }

    return getProperCircle(aCircleBuilder);
  }

  gp_Circ2d* circleByPointAndTwoTangentCurves()
  {
    const gp_Pnt2d& aPoint = myPassingPoints[0];
    CurveAdaptorPtr aCurve1 = myTangentShapes[0];
    CurveAdaptorPtr aCurve2 = myTangentShapes[1];
    if (!aCurve1 || !aCurve2)
      return 0;

    std::shared_ptr<GccAna_Circ2d3Tan> aCircleBuilder;
    if (aCurve1->GetType() == GeomAbs_Line) {
      if (aCurve2->GetType() == GeomAbs_Line) {
        aCircleBuilder = std::shared_ptr<GccAna_Circ2d3Tan>(
            new GccAna_Circ2d3Tan(GccEnt::Unqualified(aCurve1->Line()),
                                  GccEnt::Unqualified(aCurve2->Line()),
                                  aPoint, Precision::Confusion()));
      } else if (aCurve2->GetType() == GeomAbs_Circle) {
        aCircleBuilder = std::shared_ptr<GccAna_Circ2d3Tan>(
            new GccAna_Circ2d3Tan(GccEnt::Unqualified(aCurve2->Circle()),
                                  GccEnt::Unqualified(aCurve1->Line()),
                                  aPoint, Precision::Confusion()));
      }
    } else if (aCurve1->GetType() == GeomAbs_Circle) {
      if (aCurve2->GetType() == GeomAbs_Line) {
        aCircleBuilder = std::shared_ptr<GccAna_Circ2d3Tan>(
            new GccAna_Circ2d3Tan(GccEnt::Unqualified(aCurve1->Circle()),
                                  GccEnt::Unqualified(aCurve2->Line()),
                                  aPoint, Precision::Confusion()));
      } else if (aCurve2->GetType() == GeomAbs_Circle) {
        aCircleBuilder = std::shared_ptr<GccAna_Circ2d3Tan>(
            new GccAna_Circ2d3Tan(GccEnt::Unqualified(aCurve2->Circle()),
                                  GccEnt::Unqualified(aCurve1->Circle()),
                                  aPoint, Precision::Confusion()));
      }
    }

    return getProperCircle(aCircleBuilder);
  }

  gp_Circ2d* circleByTwoPointsAndTangentCurve()
  {
    const gp_Pnt2d& aPoint1 = myPassingPoints[0];
    const gp_Pnt2d& aPoint2 = myPassingPoints[1];
    CurveAdaptorPtr aCurve = myTangentShapes[0];
    if (!aCurve)
      return 0;

    std::shared_ptr<GccAna_Circ2d3Tan> aCircleBuilder;
    if (aCurve->GetType() == GeomAbs_Line) {
      aCircleBuilder = std::shared_ptr<GccAna_Circ2d3Tan>(new GccAna_Circ2d3Tan(
        GccEnt::Unqualified(aCurve->Line()), aPoint1, aPoint2, Precision::Confusion()));
    } else if (aCurve->GetType() == GeomAbs_Circle) {
      aCircleBuilder = std::shared_ptr<GccAna_Circ2d3Tan>(new GccAna_Circ2d3Tan(
          GccEnt::Unqualified(aCurve->Circle()), aPoint1, aPoint2, Precision::Confusion()));
    }

    return getProperCircle(aCircleBuilder);
  }

  gp_Circ2d* circleByThreePassingPoints()
  {
    GccAna_Circ2d3Tan aCircleBuilder(myPassingPoints[0],
                                     myPassingPoints[1],
                                     myPassingPoints[2],
                                     Precision::Confusion());
    if (aCircleBuilder.NbSolutions() > 0)
      return new gp_Circ2d(aCircleBuilder.ThisSolution(1));
    return 0;
  }


  gp_Circ2d* getProperCircle(const std::shared_ptr<GccAna_Circ2d3Tan>& theBuilder) const
  {
    if (!theBuilder)
      return 0;

    gp_Circ2d* aResult = 0;
    int aNbSol = theBuilder->NbSolutions();
    double aParSol, aPonTgCurve;
    gp_Pnt2d aTgPnt;
    for (int i = 1; i <= aNbSol; ++i) {
      bool isApplicable = false;
      if (myTangentShapes.size() >= 1) {
        theBuilder->Tangency1(i, aParSol, aPonTgCurve, aTgPnt);
        isApplicable = aPonTgCurve >= myTangentShapes[0]->FirstParameter() &&
                       aPonTgCurve <= myTangentShapes[0]->LastParameter();
      }
      if (myTangentShapes.size() >= 2 && isApplicable) {
        theBuilder->Tangency2(i, aParSol, aPonTgCurve, aTgPnt);
        isApplicable = aPonTgCurve >= myTangentShapes[1]->FirstParameter() &&
                       aPonTgCurve <= myTangentShapes[1]->LastParameter();
      }
      if (myTangentShapes.size() >= 3 && isApplicable) {
        theBuilder->Tangency3(i, aParSol, aPonTgCurve, aTgPnt);
        isApplicable = aPonTgCurve >= myTangentShapes[2]->FirstParameter() &&
                       aPonTgCurve <= myTangentShapes[2]->LastParameter();
      }

      if (isApplicable) {
        aResult = new gp_Circ2d(theBuilder->ThisSolution(i));
        break;
      }
    }
    // unable to build circle passing through the tangent curve => get any tangent point
    if (!aResult && aNbSol > 0)
      aResult =  new gp_Circ2d(theBuilder->ThisSolution(1));
    return aResult;
  }

private:
  Handle(Geom_Plane) myPlane;
  std::shared_ptr<GeomAPI_Pnt2d> myCenter;
  std::vector<gp_Pnt2d> myPassingPoints;
  std::vector<CurveAdaptorPtr> myTangentShapes;
};

typedef std::shared_ptr<CircleBuilder> CircleBuilderPtr;


static gp_Circ2d* newCirc2d(const double theCenterX, const double theCenterY, const gp_Dir2d theDir,
                            const double theRadius)
{
  gp_Pnt2d aCenter(theCenterX, theCenterY);
  return new gp_Circ2d(gp_Ax2d(aCenter, theDir), theRadius);
}

static gp_Circ2d* newCirc2d(const double theCenterX, const double theCenterY,
                            const double thePointX, const double thePointY)
{
  gp_Pnt2d aCenter(theCenterX, theCenterY);
  gp_Pnt2d aPoint(thePointX, thePointY);

  double aRadius = aCenter.Distance(aPoint);

  if (aCenter.IsEqual(aPoint, Precision::Confusion()))
    return NULL;

  gp_Dir2d aDir(thePointX - theCenterX, thePointY - theCenterY);

  return newCirc2d(theCenterX, theCenterY, aDir, aRadius);
}

static gp_Circ2d* newCirc2d(const std::shared_ptr<GeomAPI_Pnt2d>& theFirstPoint,
                            const std::shared_ptr<GeomAPI_Pnt2d>& theSecondPoint,
                            const std::shared_ptr<GeomAPI_Pnt2d>& theThirdPoint)
{
  gp_XY aFirstPnt(theFirstPoint->x(), theFirstPoint->y());
  gp_XY aSecondPnt(theSecondPoint->x(), theSecondPoint->y());
  gp_XY aThirdPnt(theThirdPoint->x(), theThirdPoint->y());

  gp_XY aVec12 = aSecondPnt - aFirstPnt;
  gp_XY aVec23 = aThirdPnt - aSecondPnt;
  gp_XY aVec31 = aFirstPnt - aThirdPnt;

  // coefficients to calculate center
  double aCoeff1, aCoeff2, aCoeff3;

  // square of parallelogram
  double aSquare2 = aVec12.Crossed(aVec23);
  aSquare2 *= aSquare2 * 2.0;
  if (aSquare2 < 1.e-20) {
    // if two points are equal, build a circle on two different points as on diameter
    double aSqLen12 = aVec12.SquareModulus();
    double aSqLen23 = aVec23.SquareModulus();
    double aSqLen31 = aVec31.SquareModulus();
    if (aSqLen12 < Precision::SquareConfusion() &&
        aSqLen23 < Precision::SquareConfusion() &&
        aSqLen31 < Precision::SquareConfusion())
      return NULL;
    aCoeff1 = aCoeff2 = aCoeff3 = 1.0 / 3.0;
  }
  else {
    aCoeff1 = aVec23.Dot(aVec23) / aSquare2 * aVec12.Dot(aVec31.Reversed());
    aCoeff2 = aVec31.Dot(aVec31) / aSquare2 * aVec23.Dot(aVec12.Reversed());
    aCoeff3 = aVec12.Dot(aVec12) / aSquare2 * aVec31.Dot(aVec23.Reversed());
  }
  // center
  gp_XY aCenter = aFirstPnt * aCoeff1 + aSecondPnt * aCoeff2 + aThirdPnt * aCoeff3;
  // radius
  double aRadius = (aFirstPnt - aCenter).Modulus();

  gp_Dir2d aDir(aFirstPnt - aCenter);
  return newCirc2d(aCenter.X(), aCenter.Y(), aDir, aRadius);
}



GeomAPI_Circ2d::GeomAPI_Circ2d(const std::shared_ptr<GeomAPI_Pnt2d>& theCenter,
                               const std::shared_ptr<GeomAPI_Pnt2d>& theCirclePoint)
    : GeomAPI_Interface(
        newCirc2d(theCenter->x(), theCenter->y(), theCirclePoint->x(), theCirclePoint->y()))
{
}

GeomAPI_Circ2d::GeomAPI_Circ2d(const std::shared_ptr<GeomAPI_Pnt2d>& theCenter,
                               const std::shared_ptr<GeomAPI_Dir2d>& theDir, double theRadius)
    : GeomAPI_Interface(
        newCirc2d(theCenter->x(), theCenter->y(), theDir->impl<gp_Dir2d>(), theRadius))
{
}

GeomAPI_Circ2d::GeomAPI_Circ2d(const std::shared_ptr<GeomAPI_Pnt2d>& theFirstPoint,
                               const std::shared_ptr<GeomAPI_Pnt2d>& theSecondPoint,
                               const std::shared_ptr<GeomAPI_Pnt2d>& theThirdPoint)
    : GeomAPI_Interface(newCirc2d(theFirstPoint, theSecondPoint, theThirdPoint))
{
}

GeomAPI_Circ2d::GeomAPI_Circ2d(const std::shared_ptr<GeomAPI_Pnt2d>& theCenter,
                               const std::shared_ptr<GeomAPI_Shape>& theTangent,
                               const std::shared_ptr<GeomAPI_Ax3>&   thePlane)
{
  CircleBuilderPtr aBuilder(new CircleBuilder(thePlane));
  aBuilder->addCenter(theCenter);
  aBuilder->addTangentCurve(theTangent);
  setImpl(aBuilder->circle());
}

GeomAPI_Circ2d::GeomAPI_Circ2d(const std::shared_ptr<GeomAPI_Interface>& theEntity1,
                               const std::shared_ptr<GeomAPI_Interface>& theEntity2,
                               const std::shared_ptr<GeomAPI_Interface>& theEntity3,
                               const std::shared_ptr<GeomAPI_Ax3>&       thePlane)
{
  CircleBuilderPtr aBuilder(new CircleBuilder(thePlane));
  aBuilder->addPassingEntity(theEntity1);
  aBuilder->addPassingEntity(theEntity2);
  aBuilder->addPassingEntity(theEntity3);
  setImpl(aBuilder->circle());
}


const std::shared_ptr<GeomAPI_Pnt2d> GeomAPI_Circ2d::project(
    const std::shared_ptr<GeomAPI_Pnt2d>& thePoint) const
{
  std::shared_ptr<GeomAPI_Pnt2d> aResult;
  if (!MY_CIRC2D)
    return aResult;

  const gp_Pnt2d& aCenter = MY_CIRC2D->Location();
  const gp_Pnt2d& aPoint = thePoint->impl<gp_Pnt2d>();

  double aDist = aCenter.Distance(aPoint);
  if (aDist < Precision::Confusion())
    return aResult;

  if (Abs(aDist - MY_CIRC2D->Radius()) < Precision::Confusion()) {
    // Point on the circle
    aResult = std::shared_ptr<GeomAPI_Pnt2d>(
        new GeomAPI_Pnt2d(thePoint->x(), thePoint->y()));
  } else {
    gp_Dir2d aDir(aPoint.XY() - aCenter.XY());
    gp_XY aNewPoint = aCenter.XY() + aDir.XY() * MY_CIRC2D->Radius();
    aResult = std::shared_ptr<GeomAPI_Pnt2d>(
        new GeomAPI_Pnt2d(aNewPoint.X(), aNewPoint.Y()));
  }

  return aResult;
}

const std::shared_ptr<GeomAPI_Pnt2d> GeomAPI_Circ2d::center() const
{
  if (!MY_CIRC2D)
    return std::shared_ptr<GeomAPI_Pnt2d>();
  const gp_Pnt2d& aCenter = MY_CIRC2D->Location();
  return std::shared_ptr<GeomAPI_Pnt2d>(new GeomAPI_Pnt2d(aCenter.X(), aCenter.Y()));
}

double GeomAPI_Circ2d::radius() const
{
  if (!MY_CIRC2D)
    return 0.0;
  return MY_CIRC2D->Radius();
}

//=================================================================================================
const bool GeomAPI_Circ2d::parameter(const std::shared_ptr<GeomAPI_Pnt2d> thePoint,
                                   const double theTolerance,
                                   double& theParameter) const
{
  Handle(Geom2d_Circle) aCurve = new Geom2d_Circle(*MY_CIRC2D);
  return GeomLib_Tool::Parameter(aCurve, thePoint->impl<gp_Pnt2d>(),
                                 theTolerance, theParameter) == Standard_True;
}

//=================================================================================================
void GeomAPI_Circ2d::D0(const double theU, std::shared_ptr<GeomAPI_Pnt2d>& thePoint)
{
  Handle(Geom2d_Circle) aCurve = new Geom2d_Circle(*MY_CIRC2D);
  gp_Pnt2d aPnt;
  aCurve->D0(theU, aPnt);
  thePoint.reset(new GeomAPI_Pnt2d(aPnt.X(), aPnt.Y()));
}

