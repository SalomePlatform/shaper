// Copyright (C) 2017-2019  CEA/DEN, EDF R&D
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

#include <GeomAlgoAPI_Fillet.h>
#include <GeomAlgoAPI_DFLoader.h>

#include <BRepFilletAPI_MakeFillet.hxx>

//=================================================================================================
GeomAlgoAPI_Fillet::GeomAlgoAPI_Fillet(const GeomShapePtr& theBaseSolid,
                                       const ListOfShape&  theFilletEdges,
                                       const double        theFilletRadius)
{
  build(theBaseSolid, theFilletEdges, theFilletRadius);
}

//=================================================================================================
GeomAlgoAPI_Fillet::GeomAlgoAPI_Fillet(const GeomShapePtr& theBaseSolid,
                                       const ListOfShape&  theFilletEdges,
                                       const double        theStartRadius,
                                       const double        theEndRadius)
{
  if (theEndRadius < 0.)
    return;
  build(theBaseSolid, theFilletEdges, theStartRadius, theEndRadius);
}

//=================================================================================================
void GeomAlgoAPI_Fillet::build(const GeomShapePtr& theBaseSolid,
                               const ListOfShape&  theFilletEdges,
                               const double        theStartRadius,
                               const double        theEndRadius)
{
  if (!theBaseSolid || theFilletEdges.empty() || theStartRadius < 0.)
    return;

  // create fillet builder
  BRepFilletAPI_MakeFillet* aFilletBuilder =
      new BRepFilletAPI_MakeFillet(theBaseSolid->impl<TopoDS_Shape>());
  setImpl(aFilletBuilder);
  setBuilderType(OCCT_BRepBuilderAPI_MakeShape);

  // assign filleting edges
  for (ListOfShape::const_iterator anIt = theFilletEdges.begin();
       anIt != theFilletEdges.end(); ++anIt) {
    if ((*anIt)->isEdge())
      aFilletBuilder->Add( (*anIt)->impl<TopoDS_Edge>() );
  }

  // assign fillet radii for each contour of filleting edges
  bool isFixedRadius = theEndRadius < 0.;
  int aNbContours = aFilletBuilder->NbContours();
  for (int ind = 1; ind <= aNbContours; ++ind) {
    if (isFixedRadius)
      aFilletBuilder->SetRadius(theStartRadius, ind, 1);
    else
      aFilletBuilder->SetRadius(theStartRadius, theEndRadius, ind, 1);
  }

  // build and get result
  aFilletBuilder->Build();
  if (!aFilletBuilder->IsDone())
    return;
  TopoDS_Shape aResult = GeomAlgoAPI_DFLoader::refineResult(aFilletBuilder->Shape());

  std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape());
  aShape->setImpl(new TopoDS_Shape(aResult));
  setShape(aShape);
  setDone(true);
}
