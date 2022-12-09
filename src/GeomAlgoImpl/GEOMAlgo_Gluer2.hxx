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

// File:        GEOMAlgo_Gluer2.hxx
// Author:      Peter KURNEV

#ifndef _GEOMAlgo_Gluer2_HeaderFile
#define _GEOMAlgo_Gluer2_HeaderFile

#include <GeomAlgoImpl.h>

#include <Standard.hxx>
#include <Standard_Macro.hxx>
#include <Standard_Boolean.hxx>

#include <TopAbs_ShapeEnum.hxx>

#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>

#include <GEOMAlgo_GluerAlgo.hxx>
#include <GEOMAlgo_BuilderShape.hxx>
#include <GEOMAlgo_GlueDetector.hxx>
#include <GEOMAlgo_CoupleOfShapes.hxx>
#include <GEOMAlgo_ListOfCoupleOfShapes.hxx>

//=======================================================================
//class : GEOMAlgo_Gluer2
//purpose  :
//=======================================================================
class GEOMAlgo_Gluer2  : public GEOMAlgo_GluerAlgo,
                         public GEOMAlgo_BuilderShape {
public:

  GEOMALGOIMPL_EXPORT GEOMAlgo_Gluer2();

  GEOMALGOIMPL_EXPORT virtual ~GEOMAlgo_Gluer2();

  GEOMALGOIMPL_EXPORT void SetShapesToGlue(const TopTools_DataMapOfShapeListOfShape& aM);

  GEOMALGOIMPL_EXPORT const TopTools_DataMapOfShapeListOfShape& ShapesToGlue() const;

  GEOMALGOIMPL_EXPORT void SetKeepNonSolids(const Standard_Boolean theFlag);

  GEOMALGOIMPL_EXPORT Standard_Boolean KeepNonSolids() const;

  GEOMALGOIMPL_EXPORT virtual void Clear();

  GEOMALGOIMPL_EXPORT virtual void Perform();

  GEOMALGOIMPL_EXPORT virtual void CheckData();

  GEOMALGOIMPL_EXPORT void Detect();

  GEOMALGOIMPL_EXPORT const TopTools_DataMapOfShapeListOfShape& ShapesDetected() const;

  GEOMALGOIMPL_EXPORT const TopTools_DataMapOfShapeListOfShape& ImagesToWork() const;

  GEOMALGOIMPL_EXPORT virtual const TopTools_ListOfShape& Generated(const TopoDS_Shape& theS);

  GEOMALGOIMPL_EXPORT virtual const TopTools_ListOfShape& Modified(const TopoDS_Shape& theS);

  GEOMALGOIMPL_EXPORT virtual Standard_Boolean IsDeleted(const TopoDS_Shape& theS);


  GEOMALGOIMPL_EXPORT static void MakeVertex(const TopTools_ListOfShape& theLV,
                                             TopoDS_Vertex& theV);

  GEOMALGOIMPL_EXPORT static void MapBRepShapes(const TopoDS_Shape& theS,
                                                TopTools_MapOfShape& theM);

  GEOMALGOIMPL_EXPORT static void MapShapes(const TopoDS_Shape& theS,
                                            TopTools_MapOfShape& theM);

//modified by NIZNHY-PKV Tue Mar 13 12:23:20 2012f
  GEOMALGOIMPL_EXPORT const TopTools_IndexedDataMapOfShapeListOfShape& StickedShapes();
//modified by NIZNHY-PKV Tue Mar 13 12:23:26 2012t
//------------------------------------------------
protected:
  GEOMALGOIMPL_EXPORT void PerformShapesToWork() ;

  GEOMALGOIMPL_EXPORT void FillVertices();
  GEOMALGOIMPL_EXPORT void FillEdges();
  GEOMALGOIMPL_EXPORT void FillWires();
  GEOMALGOIMPL_EXPORT void FillFaces();
  GEOMALGOIMPL_EXPORT void FillShells();
  GEOMALGOIMPL_EXPORT void FillSolids();
  GEOMALGOIMPL_EXPORT void FillCompSolids();
  GEOMALGOIMPL_EXPORT void FillCompounds();

  GEOMALGOIMPL_EXPORT void BuildResult();

  GEOMALGOIMPL_EXPORT void FillBRepShapes(const TopAbs_ShapeEnum theType);
  GEOMALGOIMPL_EXPORT void FillContainers(const TopAbs_ShapeEnum theType);
  GEOMALGOIMPL_EXPORT void FillCompound(const TopoDS_Shape& theC);

  GEOMALGOIMPL_EXPORT virtual void PrepareHistory();

  GEOMALGOIMPL_EXPORT Standard_Boolean HasImage(const TopoDS_Shape& theC);

  GEOMALGOIMPL_EXPORT void MakeBRepShapes(const TopoDS_Shape& theS,
                                          TopoDS_Shape& theSnew);

  GEOMALGOIMPL_EXPORT void MakeEdge(const TopoDS_Edge& theE,
                                    TopoDS_Edge& theEnew);

  GEOMALGOIMPL_EXPORT void MakeFace(const TopoDS_Face& theF,
                                    TopoDS_Face& theFnew);

  GEOMALGOIMPL_EXPORT void TreatPair(const GEOMAlgo_CoupleOfShapes& theCS,
                                     GEOMAlgo_ListOfCoupleOfShapes& theLCS);

protected:
  TopTools_DataMapOfShapeListOfShape myShapesToGlue;
  TopTools_DataMapOfShapeListOfShape myImagesDetected;
  TopTools_DataMapOfShapeShape       myOriginsDetected;
  TopTools_DataMapOfShapeListOfShape myImagesToWork;
  TopTools_DataMapOfShapeShape       myOriginsToWork;
  Standard_Boolean                   myKeepNonSolids;
  GEOMAlgo_GlueDetector              myDetector;
};

#endif
