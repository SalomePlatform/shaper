// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        SketcherPrs_SensitivePoint.h
// Created:     24 March 2015
// Author:      Vitaly SMETANNIKOV

#ifndef SketcherPrs_SensitivePoint_H
#define SketcherPrs_SensitivePoint_H

#include <Select3D_SensitiveEntity.hxx>
#include <Graphic3d_ArrayOfPoints.hxx>
#include <Select3D_Pnt2d.hxx>
#include <Standard_DefineHandle.hxx>


DEFINE_STANDARD_HANDLE(SketcherPrs_SensitivePoint, Select3D_SensitiveEntity)

/**
* \ingroup GUI
* This sensitive point is done in order to optimize process of the sinzitive area update
* It referes on point array directly and this lets to avoid procedure of the point synchronisation
*/
class SketcherPrs_SensitivePoint : public Select3D_SensitiveEntity
{
public: 
  //! Constructs a sensitive point object defined by the
  //! owner OwnerId and the point Point.
  Standard_EXPORT SketcherPrs_SensitivePoint(const Handle(SelectBasics_EntityOwner)& OwnerId, 
    const Handle(Graphic3d_ArrayOfPoints)& thePntArray, int theId);
  
  //! Converts the stored 3D point into a 2D point according
  //! to <aProjector> ; this method is called by the selection Manager.
  Standard_EXPORT   void Project (const Handle(Select3D_Projector)& aProjector) ;
  
  //! stores in <aresult> the 2D sensitive box which represents
  //! the point area in the selection process.
  Standard_EXPORT   void Areas (SelectBasics_ListOfBox2d& aresult) ;
  
  Standard_EXPORT   Handle(Select3D_SensitiveEntity) GetConnected (const TopLoc_Location& aLocation) ;
  
  //! Checks whether the sensitive entity matches the picking
  //! detection area (close to the picking line).
  //! For details please refer to base class declaration.
  Standard_EXPORT   Standard_Boolean Matches (const SelectBasics_PickArgs& thePickArgs, Standard_Real& theMatchDMin, Standard_Real& theMatchDepth) ;
  
  Standard_EXPORT   Standard_Boolean Matches (const Standard_Real XMin, const Standard_Real YMin, const Standard_Real XMax, const Standard_Real YMax, const Standard_Real aTol) ;
  
  Standard_EXPORT virtual   Standard_Boolean Matches (const TColgp_Array1OfPnt2d& Polyline, const Bnd_Box2d& aBox, const Standard_Real aTol) ;
  
  Standard_EXPORT   Standard_Real ComputeDepth (const gp_Lin& EyeLine)  const;
  
  //! Returns the point used at the time of construction.
  Standard_EXPORT   gp_Pnt Point()  const;

  Standard_EXPORT virtual   void Dump (Standard_OStream& S, const Standard_Boolean FullDump = Standard_True)  const;

  DEFINE_STANDARD_RTTI(SketcherPrs_SensitivePoint)

private: 
  int myId;
  Handle(Graphic3d_ArrayOfPoints) myPntArray;
  Select3D_Pnt2d myprojpt;
};


#endif