// Copyright (C) 2020-2021  CEA/DEN, EDF R&D
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

#include <GeomAlgoAPI_Defeaturing.h>
#include <GeomAlgoAPI_DFLoader.h>

#include <BRepAlgoAPI_Defeaturing.hxx>

GeomAlgoAPI_Defeaturing::GeomAlgoAPI_Defeaturing(const GeomShapePtr& theBaseSolid,
                                                 const ListOfShape&  theFacesToRemove)
{
  build(theBaseSolid, theFacesToRemove);
}

void GeomAlgoAPI_Defeaturing::build(const GeomShapePtr& theBaseSolid,
                                    const ListOfShape&  theFacesToRemove)
{
  if (!theBaseSolid || theFacesToRemove.empty())
    return;

  BRepAlgoAPI_Defeaturing* aDefeaturing = new BRepAlgoAPI_Defeaturing;
  aDefeaturing->SetShape(theBaseSolid->impl<TopoDS_Shape>());
  aDefeaturing->SetRunParallel(Standard_True);

  // collect faces to remove
  TopTools_ListOfShape aFaces;
  for (ListOfShape::const_iterator anIt = theFacesToRemove.begin();
       anIt != theFacesToRemove.end(); ++anIt)
    aDefeaturing->AddFaceToRemove((*anIt)->impl<TopoDS_Shape>());

  setImpl(aDefeaturing);
  setBuilderType(OCCT_BRepBuilderAPI_MakeShape);

  // build and get result
  aDefeaturing->Build();
  if (!aDefeaturing->IsDone() || aDefeaturing->HasErrors() || aDefeaturing->HasWarnings()) {
    std::ostringstream errors;
    aDefeaturing->DumpErrors(errors);
    aDefeaturing->DumpWarnings(errors);
    myError = errors.str();
    // the error string may end by '\n', remove it for correct translation
    size_t aNbToRemove = 0;
    for (std::string::reverse_iterator it = myError.rbegin();
         it != myError.rend() && *it == '\n'; ++it)
      ++aNbToRemove;
    if (aNbToRemove > 0)
      myError = myError.substr(0, myError.size() - aNbToRemove);
    return;
  }

  TopoDS_Shape aResult = GeomAlgoAPI_DFLoader::refineResult(aDefeaturing->Shape());

  std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape());
  aShape->setImpl(new TopoDS_Shape(aResult));
  setShape(aShape);
  setDone(true);
}
