// Copyright (C) 2007-2022  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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
// File:        GEOMAlgo_GluerAlgo.hxx
// Created:
// Author:      Peter KURNEV
//              <pkv@irinox>
//
#ifndef _GEOMAlgo_GluerAlgo_HeaderFile
#define _GEOMAlgo_GluerAlgo_HeaderFile

#include <GeomAlgoImpl.h>

#include <Standard_Macro.hxx>
#include <TopoDS_Shape.hxx>
#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
#include <IntTools_Context.hxx>
#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>


//=======================================================================
//class    : GEOMAlgo_GluerAlgo
//purpose  :
//=======================================================================
class GEOMAlgo_GluerAlgo {
public:

  GEOMALGOIMPL_EXPORT
    GEOMAlgo_GluerAlgo();

  GEOMALGOIMPL_EXPORT
    virtual ~GEOMAlgo_GluerAlgo();

  GEOMALGOIMPL_EXPORT
    virtual void SetArgument(const TopoDS_Shape& theShape) ;

  GEOMALGOIMPL_EXPORT
    const TopoDS_Shape& Argument() const;

  GEOMALGOIMPL_EXPORT
    void SetTolerance(const Standard_Real aT) ;

  GEOMALGOIMPL_EXPORT
    Standard_Real Tolerance() const;

  GEOMALGOIMPL_EXPORT
    void SetCheckGeometry(const Standard_Boolean aFlag) ;

  GEOMALGOIMPL_EXPORT
    Standard_Boolean CheckGeometry() const;

  GEOMALGOIMPL_EXPORT
    virtual  void Perform() ;

  GEOMALGOIMPL_EXPORT
    virtual  void Clear() ;

  GEOMALGOIMPL_EXPORT
    void SetContext(const Handle(IntTools_Context)&) ;

  GEOMALGOIMPL_EXPORT
    const Handle(IntTools_Context)& Context() ;

  GEOMALGOIMPL_EXPORT
    const TopTools_DataMapOfShapeListOfShape& Images() const;

  GEOMALGOIMPL_EXPORT
    const TopTools_DataMapOfShapeShape& Origins() const;

protected:
  TopoDS_Shape myArgument;
  Standard_Real myTolerance;
  Standard_Boolean myCheckGeometry;
  Handle(IntTools_Context) myContext;
  TopTools_DataMapOfShapeListOfShape myImages;
  TopTools_DataMapOfShapeShape myOrigins;
};

#endif
