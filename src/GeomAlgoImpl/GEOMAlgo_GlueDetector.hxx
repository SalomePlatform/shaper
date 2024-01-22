// Copyright (C) 2007-2024  CEA, EDF, OPEN CASCADE
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

// File:        GEOMAlgo_GlueDetector.hxx
// Author:      Peter KURNEV

#ifndef _GEOMAlgo_GlueDetector_HeaderFile
#define _GEOMAlgo_GlueDetector_HeaderFile

#include <Standard.hxx>
#include <Standard_Macro.hxx>

#include <TopAbs_ShapeEnum.hxx>

#include <TopTools_ListOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>

#include <GEOMAlgo_GluerAlgo.hxx>
#include <GEOMAlgo_Algo.hxx>
#include <GEOMAlgo_PassKeyShape.hxx>


//=======================================================================
//function : GEOMAlgo_GlueDetector
//purpose  :
//=======================================================================
class GEOMAlgo_GlueDetector  : public GEOMAlgo_GluerAlgo,
                               public GEOMAlgo_Algo
{
public:
  GEOMALGOIMPL_EXPORT GEOMAlgo_GlueDetector();

  GEOMALGOIMPL_EXPORT virtual ~GEOMAlgo_GlueDetector();

  GEOMALGOIMPL_EXPORT virtual void Perform() ;

  GEOMALGOIMPL_EXPORT const TopTools_IndexedDataMapOfShapeListOfShape& StickedShapes();

protected:
  GEOMALGOIMPL_EXPORT void DetectVertices();
  GEOMALGOIMPL_EXPORT void DetectEdges();
  GEOMALGOIMPL_EXPORT void DetectFaces();

  GEOMALGOIMPL_EXPORT void DetectShapes(const TopAbs_ShapeEnum aType);

  GEOMALGOIMPL_EXPORT void EdgePassKey(const TopoDS_Edge& aE,
                                       GEOMAlgo_PassKeyShape& aPK);

  GEOMALGOIMPL_EXPORT void FacePassKey(const TopoDS_Face& aF,
                                       GEOMAlgo_PassKeyShape& aPK);

  GEOMALGOIMPL_EXPORT void CheckDetected();
  //
  GEOMALGOIMPL_EXPORT void CheckDetected(const TopTools_ListOfShape& aLVSD,
                                         const TopTools_IndexedDataMapOfShapeListOfShape& aMVE,
                                         const TopTools_IndexedDataMapOfShapeListOfShape& aMEV);


 protected:
  TopTools_IndexedDataMapOfShapeListOfShape myStickedShapes;
};

#endif
