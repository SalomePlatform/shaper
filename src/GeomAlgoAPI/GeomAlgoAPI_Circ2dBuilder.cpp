// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

#include <GeomAlgoAPI_Circ2dBuilder.h>
#include <GeomAPI_Ax3.h>
#include <GeomAPI_Circ2d.h>
#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_Dir2d.h>
#include <GeomAPI_Shape.h>

#include <BRep_Tool.hxx>
#include <ElCLib.hxx>
#include <GccAna_Circ2d2TanOn.hxx>
#include <GccAna_Circ2d2TanRad.hxx>
#include <GccAna_Circ2d3Tan.hxx>
#include <GccAna_Circ2dTanCen.hxx>
#include <GccEnt.hxx>
#include <GccEnt_QualifiedCirc.hxx>
#include <GccEnt_QualifiedLin.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom_Plane.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>

#include <cmath>

typedef std::shared_ptr<gp_Circ2d> Circ2dPtr;
typedef std::shared_ptr<Geom2dAdaptor_Curve> CurveAdaptorPtr;
typedef std::vector< std::shared_ptr<GccEnt_QualifiedCirc> > VectorOfGccCirc;
typedef std::vector< std::shared_ptr<GccEnt_QualifiedLin> >  VectorOfGccLine;


// Provide different mechanisms to create circle:
// * by passing points
// * by tangent edges
// * by transversal line
// * with specified radius
// * etc.
class CircleBuilder
{
public:
  CircleBuilder(const std::shared_ptr<GeomAPI_Ax3>& theBasePlane)
    : myPlane(new Geom_Plane(theBasePlane->impl<gp_Ax3>())),
      myRadius(0.0)
  {}

  void setRadius(const double theRadius)
  { myRadius = theRadius; }

  void setCenter(const std::shared_ptr<GeomAPI_Pnt2d>& theCenter)
  { myCenter = theCenter; }

  void setTangentCurves(const std::vector< std::shared_ptr<GeomAPI_Shape> >& theEdges)
  {
    std::vector< std::shared_ptr<GeomAPI_Shape> >::const_iterator anEdgeIt;
    for (anEdgeIt = theEdges.begin(); anEdgeIt != theEdges.end(); ++anEdgeIt) {
      const TopoDS_Edge& anEdge = TopoDS::Edge((*anEdgeIt)->impl<TopoDS_Shape>());

      double aFirst, aLast;
      TopLoc_Location aLoc;
      Handle(Geom2d_Curve) aCurve = BRep_Tool::CurveOnSurface(anEdge, myPlane, aLoc, aFirst, aLast);
      CurveAdaptorPtr aCurveAdaptor(new Geom2dAdaptor_Curve(aCurve, aFirst, aLast));

      // sort curves (circles should become first)
      if (aCurveAdaptor->GetType() == GeomAbs_Circle)
        myTangentShapes.insert(myTangentShapes.begin(), aCurveAdaptor);
      else
        myTangentShapes.push_back(aCurveAdaptor);
    }
  }

  void setTransversalLine(const std::shared_ptr<GeomAPI_Shape>& theLine)
  {
    if (!theLine)
      return;

    const TopoDS_Edge& anEdge = TopoDS::Edge(theLine->impl<TopoDS_Shape>());

    double aFirst, aLast;
    TopLoc_Location aLoc;
    Handle(Geom2d_Curve) aCurve = BRep_Tool::CurveOnSurface(anEdge, myPlane, aLoc, aFirst, aLast);
    myTransversalLine = CurveAdaptorPtr(new Geom2dAdaptor_Curve(aCurve, aFirst, aLast));
  }

  void setPassingPoints(const std::vector< std::shared_ptr<GeomAPI_Pnt2d> >& thePoints)
  {
    std::vector< std::shared_ptr<GeomAPI_Pnt2d> >::const_iterator aPIt;
    for (aPIt = thePoints.begin(); aPIt != thePoints.end(); ++aPIt)
      myPassingPoints.push_back((*aPIt)->impl<gp_Pnt2d>());
  }

  void setClosestPoint(const std::shared_ptr<GeomAPI_Pnt2d>& thePoint)
  { myClosestPoint = thePoint; }


  Circ2dPtr circle()
  {
    Circ2dPtr aResult;
    if (myCenter) {
      if (myPassingPoints.size() == 1)
        aResult = circleByCenterAndPassingPoint();
      else if (myTangentShapes.size() == 1)
        aResult = circleByCenterAndTangent();
      else if (myRadius > 0.0)
        aResult = circleByCenterAndRadius();
    } else if (myRadius > 0.0) {
      if (myTangentShapes.size() == 2)
        aResult = circleByRadiusAndTwoTangentCurves();
    } else {
      switch (myPassingPoints.size()) {
      case 0:
        aResult = circleByThreeTangentCurves();
        break;
      case 1:
        aResult = circleByPointAndTwoTangentCurves();
        break;
      case 2: {
        if (myTransversalLine)
          aResult = circleByTwoPointsAndTransversalLine();
        else
          aResult = circleByTwoPointsAndTangentCurve();
        break;
      }
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
  Circ2dPtr circleByCenterAndRadius()
  {
    const gp_Pnt2d& aCenter = myCenter->impl<gp_Pnt2d>();
    return Circ2dPtr(new gp_Circ2d(gp_Ax2d(aCenter, gp::DX2d()), myRadius));
  }

  Circ2dPtr circleByCenterAndPassingPoint()
  {
    const gp_Pnt2d& aCenter = myCenter->impl<gp_Pnt2d>();
    if (aCenter.SquareDistance(myPassingPoints[0]) > Precision::SquareConfusion()) {
      GccAna_Circ2dTanCen aBuilder(myPassingPoints[0], aCenter);
      if (aBuilder.NbSolutions() > 0)
        return Circ2dPtr(new gp_Circ2d(aBuilder.ThisSolution(1)));
    }
    return Circ2dPtr();
  }

  Circ2dPtr circleByCenterAndTangent()
  {
    const gp_Pnt2d& aCenter = myCenter->impl<gp_Pnt2d>();
    CurveAdaptorPtr aCurve = myTangentShapes[0];

    std::shared_ptr<GccAna_Circ2dTanCen> aCircleBuilder;
    if (aCurve->GetType() == GeomAbs_Line &&
        aCurve->Line().Distance(aCenter) > Precision::Confusion()) {
      aCircleBuilder = std::shared_ptr<GccAna_Circ2dTanCen>(
          new GccAna_Circ2dTanCen(aCurve->Line(), aCenter));
    } else if (aCurve->GetType() == GeomAbs_Circle) {
      aCircleBuilder = std::shared_ptr<GccAna_Circ2dTanCen>(new GccAna_Circ2dTanCen(
          GccEnt::Unqualified(aCurve->Circle()), aCenter, Precision::Confusion()));
    }

    return getProperCircle(aCircleBuilder);
  }

  Circ2dPtr getProperCircle(const std::shared_ptr<GccAna_Circ2dTanCen>& theBuilder) const
  {
    if (!theBuilder)
      return Circ2dPtr();

    CurveAdaptorPtr aCurve = myTangentShapes[0];

    int aNbSol = theBuilder->NbSolutions();
    if (aNbSol == 0)
      return Circ2dPtr();

    int anAppropriateSolution = 1;
    double aMinDist = Precision::Infinite();

    double aParSol, aPonTgCurve;
    gp_Pnt2d aTgPnt;
    for (int i = 1; i <= aNbSol && aNbSol > 1 && aCurve; ++i) {
      theBuilder->Tangency1(i, aParSol, aPonTgCurve, aTgPnt);
      if (isParamOnCurve(aPonTgCurve, aCurve)) {
        double aDist = distanceToClosestPoint(theBuilder->ThisSolution(i));
        if (aDist < aMinDist) {
          anAppropriateSolution = i;
          aMinDist = aDist;
        }
      }
    }

    return Circ2dPtr(new gp_Circ2d(theBuilder->ThisSolution(anAppropriateSolution)));
  }

  double distanceToClosestPoint(const gp_Circ2d& theCirc) const
  {
    if (myClosestPoint) {
      double aDist = myClosestPoint->impl<gp_Pnt2d>().Distance(theCirc.Location());
      return fabs(aDist - theCirc.Radius());
    }
    return 0.0;
  }


  Circ2dPtr circleByThreeTangentCurves()
  {
    VectorOfGccCirc aTgCirc;
    VectorOfGccLine aTgLine;
    convertTangentCurvesToGccEnt(aTgCirc, aTgLine);

    std::shared_ptr<GccAna_Circ2d3Tan> aCircleBuilder;
    if (aTgCirc.size() + aTgLine.size() == 3) {
      switch (aTgLine.size()) {
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
            *aTgLine[0], *aTgLine[1], *aTgLine[2], Precision::Confusion()));
        break;
      default:
        break;
      }
    }

    return getProperCircle(aCircleBuilder);
  }

  Circ2dPtr circleByPointAndTwoTangentCurves()
  {
    const gp_Pnt2d& aPoint = myPassingPoints[0];

    VectorOfGccCirc aTgCirc;
    VectorOfGccLine aTgLine;
    convertTangentCurvesToGccEnt(aTgCirc, aTgLine);

    std::shared_ptr<GccAna_Circ2d3Tan> aCircleBuilder;
    if (aTgCirc.size() + aTgLine.size() == 2) {
      switch (aTgLine.size()) {
      case 0:
        aCircleBuilder = std::shared_ptr<GccAna_Circ2d3Tan>(new GccAna_Circ2d3Tan(
            *aTgCirc[0], *aTgCirc[1], aPoint, Precision::Confusion()));
        break;
      case 1:
        aCircleBuilder = std::shared_ptr<GccAna_Circ2d3Tan>(new GccAna_Circ2d3Tan(
            *aTgCirc[0], *aTgLine[0], aPoint, Precision::Confusion()));
        break;
      case 2:
        aCircleBuilder = std::shared_ptr<GccAna_Circ2d3Tan>(new GccAna_Circ2d3Tan(
            *aTgLine[0], *aTgLine[1], aPoint, Precision::Confusion()));
        break;
      default:
        break;
      }
    }

    return getProperCircle(aCircleBuilder);
  }

  Circ2dPtr circleByTwoPointsAndTangentCurve()
  {
    const gp_Pnt2d& aPoint1 = myPassingPoints[0];
    const gp_Pnt2d& aPoint2 = myPassingPoints[1];
    CurveAdaptorPtr aCurve = myTangentShapes[0];

    std::shared_ptr<GccAna_Circ2d3Tan> aCircleBuilder;
    if (aCurve) {
      if (aCurve->GetType() == GeomAbs_Line) {
        aCircleBuilder = std::shared_ptr<GccAna_Circ2d3Tan>(new GccAna_Circ2d3Tan(
            GccEnt::Unqualified(aCurve->Line()), aPoint1, aPoint2, Precision::Confusion()));
      }
      else if (aCurve->GetType() == GeomAbs_Circle) {
        aCircleBuilder = std::shared_ptr<GccAna_Circ2d3Tan>(new GccAna_Circ2d3Tan(
            GccEnt::Unqualified(aCurve->Circle()), aPoint1, aPoint2, Precision::Confusion()));
      }
    }

    return getProperCircle(aCircleBuilder);
  }

  Circ2dPtr circleByThreePassingPoints()
  {
    GccAna_Circ2d3Tan aCircleBuilder(myPassingPoints[0],
                                     myPassingPoints[1],
                                     myPassingPoints[2],
                                     Precision::Confusion());
    if (aCircleBuilder.NbSolutions() > 0)
      return Circ2dPtr(new gp_Circ2d(aCircleBuilder.ThisSolution(1)));
    return Circ2dPtr();
  }

  Circ2dPtr getProperCircle(const std::shared_ptr<GccAna_Circ2d3Tan>& theBuilder) const
  {
    if (!theBuilder)
      return Circ2dPtr();

    int aNbSol = theBuilder->NbSolutions();
    if (aNbSol == 0)
      return Circ2dPtr();

    int anAppropriateSolution = 1;
    double aMinDist = Precision::Infinite();

    double aParSol, aPonTgCurve;
    gp_Pnt2d aTgPnt;
    for (int i = 1; i <= aNbSol && aNbSol > 1; ++i) {
      bool isApplicable = false;
      if (myTangentShapes.size() >= 1) {
        theBuilder->Tangency1(i, aParSol, aPonTgCurve, aTgPnt);
        isApplicable = isParamOnCurve(aPonTgCurve, myTangentShapes[0]);
      }
      if (myTangentShapes.size() >= 2 && isApplicable) {
        theBuilder->Tangency2(i, aParSol, aPonTgCurve, aTgPnt);
        isApplicable = isParamOnCurve(aPonTgCurve, myTangentShapes[1]);
      }
      if (myTangentShapes.size() >= 3 && isApplicable) {
        theBuilder->Tangency3(i, aParSol, aPonTgCurve, aTgPnt);
        isApplicable = isParamOnCurve(aPonTgCurve, myTangentShapes[2]);
      }

      if (isApplicable) {
        double aDist = distanceToClosestPoint(theBuilder->ThisSolution(i));
        if (aDist < aMinDist) {
          anAppropriateSolution = i;
          aMinDist = aDist;
        }
      }
    }

    return Circ2dPtr(new gp_Circ2d(theBuilder->ThisSolution(anAppropriateSolution)));
  }


  Circ2dPtr circleByTwoPointsAndTransversalLine()
  {
    const gp_Pnt2d& aPoint1 = myPassingPoints[0];
    const gp_Pnt2d& aPoint2 = myPassingPoints[1];

    if (myTransversalLine && myTransversalLine->GetType() == GeomAbs_Line) {
      GccAna_Circ2d2TanOn aCircleBuilder(aPoint1, aPoint2, myTransversalLine->Line(),
                                         Precision::Confusion());
      if (aCircleBuilder.NbSolutions() > 0)
        return Circ2dPtr(new gp_Circ2d(aCircleBuilder.ThisSolution(1)));
    }

    return Circ2dPtr();
  }


  Circ2dPtr circleByRadiusAndTwoTangentCurves()
  {
    VectorOfGccCirc aTgCirc;
    VectorOfGccLine aTgLine;
    convertTangentCurvesToGccEnt(aTgCirc, aTgLine);

    std::shared_ptr<GccAna_Circ2d2TanRad> aCircleBuilder;
    if (aTgCirc.size() + aTgLine.size() == 2) {
      switch (aTgLine.size()) {
      case 0:
        aCircleBuilder = std::shared_ptr<GccAna_Circ2d2TanRad>(new GccAna_Circ2d2TanRad(
            *aTgCirc[0], *aTgCirc[1], myRadius, Precision::Confusion()));
        break;
      case 1:
        aCircleBuilder = std::shared_ptr<GccAna_Circ2d2TanRad>(new GccAna_Circ2d2TanRad(
            *aTgCirc[0], *aTgLine[0], myRadius, Precision::Confusion()));
        break;
      case 2:
        aCircleBuilder = std::shared_ptr<GccAna_Circ2d2TanRad>(new GccAna_Circ2d2TanRad(
            *aTgLine[0], *aTgLine[1], myRadius, Precision::Confusion()));
        break;
      default:
        break;
      }
    }

    return getProperCircle(aCircleBuilder);
  }

  Circ2dPtr getProperCircle(const std::shared_ptr<GccAna_Circ2d2TanRad>& theBuilder) const
  {
    if (!theBuilder)
      return Circ2dPtr();

    int aNbSol = theBuilder->NbSolutions();
    if (aNbSol == 0)
      return Circ2dPtr();

    int anAppropriateSolution = 1;
    double aMinDist = Precision::Infinite();

    double aParSol, aPonTgCurve;
    gp_Pnt2d aTgPnt;
    for (int i = 1; i <= aNbSol && aNbSol > 1; ++i) {
      bool isApplicable = false;
      if (myTangentShapes.size() >= 1) {
        theBuilder->Tangency1(i, aParSol, aPonTgCurve, aTgPnt);
        isApplicable = isParamInCurve(aPonTgCurve, myTangentShapes[0]);
      }
      if (myTangentShapes.size() >= 2 && isApplicable) {
        theBuilder->Tangency2(i, aParSol, aPonTgCurve, aTgPnt);
        isApplicable = isParamInCurve(aPonTgCurve, myTangentShapes[1]);
      }

      if (isApplicable) {
        double aDist = distanceToClosestPoint(theBuilder->ThisSolution(i));
        if (aDist < aMinDist) {
          anAppropriateSolution = i;
          aMinDist = aDist;
        }
      }
    }

    return Circ2dPtr(new gp_Circ2d(theBuilder->ThisSolution(anAppropriateSolution)));
  }


  void convertTangentCurvesToGccEnt(VectorOfGccCirc& theTangentCircles,
                                    VectorOfGccLine& theTangentLines)
  {
    theTangentCircles.reserve(3);
    theTangentLines.reserve(3);

    std::vector<CurveAdaptorPtr>::iterator anIt = myTangentShapes.begin();
    for (; anIt != myTangentShapes.end(); ++anIt) {
      switch ((*anIt)->GetType()) {
      case GeomAbs_Line:
        theTangentLines.push_back(
            std::shared_ptr<GccEnt_QualifiedLin>(
            new GccEnt_QualifiedLin((*anIt)->Line(), GccEnt_unqualified))
        );
        break;
      case GeomAbs_Circle:
        theTangentCircles.push_back(
            std::shared_ptr<GccEnt_QualifiedCirc>(
            new GccEnt_QualifiedCirc((*anIt)->Circle(), GccEnt_unqualified))
        );
        break;
      default:
        break;
      }
    }
  }


  static void adjustPeriod(double& theParameter, const CurveAdaptorPtr& theCurve)
  {
    if (theCurve->Curve()->IsPeriodic()) {
      theParameter = ElCLib::InPeriod(theParameter,
                                      theCurve->FirstParameter(),
                                      theCurve->FirstParameter() + theCurve->Period());
    }
  }

  // boundary parameters of curve are NOT applied
  static bool isParamInCurve(double& theParameter, const CurveAdaptorPtr& theCurve)
  {
    adjustPeriod(theParameter, theCurve);
    return theParameter > theCurve->FirstParameter() &&
           theParameter < theCurve->LastParameter();
  }

  // boundary parameters of curve are applied too
  static bool isParamOnCurve(double& theParameter, const CurveAdaptorPtr& theCurve)
  {
    adjustPeriod(theParameter, theCurve);
    return theParameter >= theCurve->FirstParameter() &&
           theParameter <= theCurve->LastParameter();
  }

private:
  Handle(Geom_Plane) myPlane;
  std::shared_ptr<GeomAPI_Pnt2d> myCenter;
  std::vector<gp_Pnt2d> myPassingPoints;
  std::vector<CurveAdaptorPtr> myTangentShapes;
  CurveAdaptorPtr myTransversalLine;
  double myRadius;
  std::shared_ptr<GeomAPI_Pnt2d> myClosestPoint;
};





GeomAlgoAPI_Circ2dBuilder::GeomAlgoAPI_Circ2dBuilder(const std::shared_ptr<GeomAPI_Ax3>& thePlane)
  : myPlane(thePlane),
    myRadius(0.)
{
}

void GeomAlgoAPI_Circ2dBuilder::setRadius(const double theRadius)
{
  myRadius = theRadius;
}

void GeomAlgoAPI_Circ2dBuilder::setCenter(const std::shared_ptr<GeomAPI_Pnt2d>& theCenter)
{
  myCenter = theCenter;
}

void GeomAlgoAPI_Circ2dBuilder::addTangentCurve(const std::shared_ptr<GeomAPI_Shape>& theEdge)
{
  if (theEdge->isEdge())
    myTangentShapes.push_back(theEdge);
}

void GeomAlgoAPI_Circ2dBuilder::setTransversalLine(const std::shared_ptr<GeomAPI_Shape>& theEdge)
{
  if (theEdge->isEdge())
    myTransversalLine = theEdge;
}

void GeomAlgoAPI_Circ2dBuilder::addPassingPoint(const std::shared_ptr<GeomAPI_Pnt2d>& thePoint)
{
  myPassingPoints.push_back(thePoint);
}

void GeomAlgoAPI_Circ2dBuilder::setClosestPoint(const std::shared_ptr<GeomAPI_Pnt2d>& thePoint)
{
  myClosestPoint = thePoint;
}

std::shared_ptr<GeomAPI_Circ2d> GeomAlgoAPI_Circ2dBuilder::circle(
    const std::shared_ptr<GeomAPI_Pnt2d>& theFirstPoint,
    const std::shared_ptr<GeomAPI_Pnt2d>& theSecondPoint,
    const std::shared_ptr<GeomAPI_Pnt2d>& theThirdPoint)
{
  std::shared_ptr<GeomAPI_Ax3> aPlane(new GeomAPI_Ax3);

  GeomAlgoAPI_Circ2dBuilder aBuilder(aPlane);
  aBuilder.addPassingPoint(theFirstPoint);
  aBuilder.addPassingPoint(theSecondPoint);
  aBuilder.addPassingPoint(theThirdPoint);
  return aBuilder.circle();
}

std::shared_ptr<GeomAPI_Circ2d> GeomAlgoAPI_Circ2dBuilder::circle()
{
  CircleBuilder aCircleBuilder(myPlane);
  aCircleBuilder.setCenter(myCenter);
  aCircleBuilder.setTangentCurves(myTangentShapes);
  aCircleBuilder.setTransversalLine(myTransversalLine);
  aCircleBuilder.setPassingPoints(myPassingPoints);
  aCircleBuilder.setClosestPoint(myClosestPoint);
  aCircleBuilder.setRadius(myRadius);
  Circ2dPtr aCirc2d = aCircleBuilder.circle();

  std::shared_ptr<GeomAPI_Circ2d> aCircle;
  if (aCirc2d) {
    const gp_Pnt2d& aCenter = aCirc2d->Location();
    const gp_Dir2d& aXAxis = aCirc2d->Position().XDirection();

    std::shared_ptr<GeomAPI_Pnt2d> aCircleCenter(new GeomAPI_Pnt2d(aCenter.X(), aCenter.Y()));
    std::shared_ptr<GeomAPI_Dir2d> aCircleDir(new GeomAPI_Dir2d(aXAxis.X(), aXAxis.Y()));

    aCircle = std::shared_ptr<GeomAPI_Circ2d>(
        new GeomAPI_Circ2d(aCircleCenter, aCircleDir, aCirc2d->Radius()));
  }
  return aCircle;
}
