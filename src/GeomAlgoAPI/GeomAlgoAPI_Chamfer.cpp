// Copyright (C) 2017-2020  CEA/DEN, EDF R&D
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

#include "GeomAlgoAPI_Chamfer.h"
#include "GeomAlgoAPI_DFLoader.h"

#include <BRep_Tool.hxx>
#include <BRepFilletAPI_MakeChamfer.hxx>
#include <BRepTools.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>

//=================================================================================================
GeomAlgoAPI_Chamfer::GeomAlgoAPI_Chamfer(const GeomShapePtr& theBaseSolid,
                                         const ListOfShape&  theChamferShapes,
                                         const std::map<GeomShapePtr, GeomShapePtr> theMapEdgeFace,
                                         const bool performDistances,
                                         const double theVal1,
                                         const double theVal2)
{
  build(theBaseSolid, theChamferShapes, theMapEdgeFace, performDistances, theVal1, theVal2);
}

//=================================================================================================
void GeomAlgoAPI_Chamfer::build(const GeomShapePtr& theBaseSolid,
                                const ListOfShape&  theChamferShapes,
                                const std::map<GeomShapePtr, GeomShapePtr> theMapEdgeFace,
                                const bool performDistances,
                                const double theVal1,
                                const double theVal2)
{
  TopoDS_Shape aShapeBase = theBaseSolid->impl<TopoDS_Shape>();
  TopTools_IndexedDataMapOfShapeListOfShape M;
  TopExp::MapShapesAndAncestors(aShapeBase, TopAbs_EDGE, TopAbs_FACE, M);

  // create chamfer builder
  BRepFilletAPI_MakeChamfer* aChamferBuilder =
      new BRepFilletAPI_MakeChamfer(aShapeBase);
  setImpl(aChamferBuilder);
  setBuilderType(OCCT_BRepBuilderAPI_MakeShape);

  for (ListOfShape::const_iterator anIt = theChamferShapes.begin();
     anIt != theChamferShapes.end(); ++anIt) {
    if ((*anIt)->isEdge()) {
      TopoDS_Edge E = (*anIt)->impl<TopoDS_Edge>();
      if (theMapEdgeFace.find(*anIt) != theMapEdgeFace.end()) {
        //TopoDS_Face F = (theMapEdgeFace[*anIt])->impl<TopoDS_Face>();
        TopoDS_Face F = (theMapEdgeFace.at(*anIt))->impl<TopoDS_Face>();
        if (!BRepTools::IsReallyClosed(E,F) && !BRep_Tool::Degenerated(E) &&
              M.FindFromKey(E).Extent() == 2) {
          if (performDistances) {
              aChamferBuilder->Add(theVal1, theVal2, E, F);
            } else {
              aChamferBuilder->AddDA(theVal1, theVal2 * M_PI / 180., E, F);
            }
          }
      } else {
        const TopTools_ListOfShape& aFacesList = M.FindFromKey(E);
        TopoDS_Face F = TopoDS::Face(aFacesList.First());
        if (performDistances) {
          aChamferBuilder->Add(theVal1, theVal2, E, F);
        } else {
          aChamferBuilder->AddDA(theVal1, theVal2 * M_PI / 180., E, F);
        }
      }
    }
  }

  // build and get result
  aChamferBuilder->Build();
  if (!aChamferBuilder->IsDone())
    return;
  TopoDS_Shape aResult = GeomAlgoAPI_DFLoader::refineResult(aChamferBuilder->Shape());

  std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape());
  aShape->setImpl(new TopoDS_Shape(aResult));
  setShape(aShape);
  setDone(true);
}
