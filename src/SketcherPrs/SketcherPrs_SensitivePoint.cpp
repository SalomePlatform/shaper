// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        SketcherPrs_SensitivePoint.cpp
// Created:     24 March 2015
// Author:      Vitaly SMETANNIKOV


#include "SketcherPrs_SensitivePoint.h"

#include <Select3D_Projector.hxx>
#include <Bnd_Box2d.hxx>
#include <CSLib_Class2d.hxx>
#include <ElCLib.hxx>
#include <TopLoc_Location.hxx>
#include <SelectBasics_ListOfBox2d.hxx>

IMPLEMENT_STANDARD_HANDLE(SketcherPrs_SensitivePoint, Select3D_SensitiveEntity);
IMPLEMENT_STANDARD_RTTIEXT(SketcherPrs_SensitivePoint, Select3D_SensitiveEntity);

SketcherPrs_SensitivePoint::SketcherPrs_SensitivePoint(const Handle(SelectBasics_EntityOwner)& anOwner,
                                                        const Handle(Graphic3d_ArrayOfPoints)& thePntArray, 
                                                        int theId)
:Select3D_SensitiveEntity(anOwner), myId(theId), myPntArray(thePntArray)
{
  SetSensitivityFactor(4.);
}

void SketcherPrs_SensitivePoint::Project (const Handle(Select3D_Projector)& aProj)
{
  gp_Pnt2d aPoint2d;
  gp_Pnt aPnt = Point();
  if(!HasLocation())
    aProj->Project(aPnt, aPoint2d);
  else
  {
    gp_Pnt aP(aPnt.X(), aPnt.Y(), aPnt.Z());
    aProj->Project(aP.Transformed(Location().Transformation()), aPoint2d);
  }
  myprojpt = aPoint2d;
}

void SketcherPrs_SensitivePoint::Areas(SelectBasics_ListOfBox2d& boxes)
{
  Bnd_Box2d abox;
  abox.Set(myprojpt);
  boxes.Append(abox);
}

Standard_Boolean SketcherPrs_SensitivePoint::Matches(const SelectBasics_PickArgs& thePickArgs,
                                                      Standard_Real& theMatchDMin,
                                                      Standard_Real& theMatchDepth)
{
  // check coordinate matching
  Standard_Real aDist = gp_Pnt2d (thePickArgs.X(), thePickArgs.Y()).Distance (myprojpt);
  if (aDist > thePickArgs.Tolerance() * SensitivityFactor())
  {
    return Standard_False;
  }

  Standard_Real aDepth = ComputeDepth (thePickArgs.PickLine());
  if (thePickArgs.IsClipped (aDepth))
  {
    return Standard_False;
  }

  theMatchDMin = aDist;
  theMatchDepth = aDepth;
  return Standard_True;
}

Standard_Boolean SketcherPrs_SensitivePoint::Matches(const Standard_Real XMin,
                                                     const Standard_Real YMin,
                                                     const Standard_Real XMax,
                                                     const Standard_Real YMax,
                                                     const Standard_Real aTol)
{
  Bnd_Box2d B;
  B.Update(Min(XMin,XMax),Min(YMin,YMax),Max(XMin,XMax),Max(YMin,YMax));
  B.Enlarge(aTol);
  return !B.IsOut(myprojpt);
}

Standard_Boolean SketcherPrs_SensitivePoint::Matches(const TColgp_Array1OfPnt2d& aPoly,
                                                     const Bnd_Box2d& aBox,
                                                     const Standard_Real aTol)
{
  Standard_Real Umin,Vmin,Umax,Vmax;
  aBox.Get(Umin,Vmin,Umax,Vmax);
  CSLib_Class2d aClassifier2d(aPoly,aTol,aTol,Umin,Vmin,Umax,Vmax);

  Standard_Integer RES = aClassifier2d.SiDans(myprojpt);
  if(RES==1) return Standard_True;

  return Standard_False;
}

gp_Pnt SketcherPrs_SensitivePoint::Point() const
{
  return myPntArray->Vertice(myId);
}

Handle(Select3D_SensitiveEntity) SketcherPrs_SensitivePoint::GetConnected(const TopLoc_Location& aLoc)
{
  Handle(SketcherPrs_SensitivePoint) NiouEnt = new SketcherPrs_SensitivePoint(myOwnerId,myPntArray,myId);
  if(HasLocation()) NiouEnt->SetLocation(Location());
  NiouEnt->UpdateLocation(aLoc);
  return NiouEnt;
}

void SketcherPrs_SensitivePoint::Dump(Standard_OStream& S,const Standard_Boolean /*FullDump*/) const
{
  gp_Pnt aPnt = Point();
  S<<"\tSensitivePoint 3D :";
  if(HasLocation())
    S<<"\t\tExisting Location"<<endl;

  S<<"\t\t P3d [ "<<aPnt.X()<<" , "<<aPnt.Y()<<" , "<<aPnt.Z()<<" ]"<<endl;
  S<<"\t\t P2d [ "<<myprojpt.x<<" , "<<myprojpt.y<<" ]"<<endl;
}

Standard_Real SketcherPrs_SensitivePoint::ComputeDepth(const gp_Lin& EyeLine) const
{
  return ElCLib::Parameter(EyeLine, Point());
}
