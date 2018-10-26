// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#include "GeomAlgoAPI_Intersection.h"

#include <GeomAlgoAPI_DFLoader.h>

#include <BOPAlgo_PaveFiller.hxx>
#include <BOPAlgo_Section.hxx>

//==================================================================================================
GeomAlgoAPI_Intersection::GeomAlgoAPI_Intersection(const ListOfShape& theObjects)
  : myFiller(0)
{
  build(theObjects);
}

GeomAlgoAPI_Intersection::~GeomAlgoAPI_Intersection() {
  if (myFiller)
    delete (BOPAlgo_PaveFiller*)myFiller;
}
//==================================================================================================
void GeomAlgoAPI_Intersection::build(const ListOfShape& theObjects)
{
  if (theObjects.empty()) {
    return;
  }

  // Creating partition operation.
  BOPAlgo_Section* anOperation = new BOPAlgo_Section;
  this->setImpl(anOperation);
  this->setBuilderType(OCCT_BOPAlgo_Builder);

  // Getting objects.
  TopTools_ListOfShape anObjects;
  for (ListOfShape::const_iterator
    anObjectsIt = theObjects.begin(); anObjectsIt != theObjects.end(); anObjectsIt++) {
    const TopoDS_Shape& aShape = (*anObjectsIt)->impl<TopoDS_Shape>();
    if(!aShape.IsNull()) {
      anObjects.Append(aShape);
    }
  }

  BOPAlgo_PaveFiller* aDSFiller = new BOPAlgo_PaveFiller;
  myFiller = aDSFiller;
  aDSFiller->SetArguments(anObjects);

  aDSFiller->SetRunParallel(false);
  aDSFiller->SetNonDestructive(false);
  aDSFiller->SetGlue(BOPAlgo_GlueOff);

  // optimization for the issue #2399
  BOPAlgo_SectionAttribute theSecAttr(Standard_True,
                                      Standard_True,
                                      Standard_True);
  aDSFiller->SetSectionAttribute(theSecAttr);

  aDSFiller->Perform();
  if (aDSFiller->HasErrors()) {
    return;
  }

  anOperation->SetArguments(anObjects);
  anOperation->SetRunParallel(false);
  anOperation->SetCheckInverted(true);

  anOperation->PerformWithFiller(*aDSFiller); // it references a filler fields, so keep the filler
  if(anOperation->HasErrors()) {
    return;
  }
  TopoDS_Shape aResult = anOperation->Shape();

  if(aResult.ShapeType() == TopAbs_COMPOUND) {
    aResult = GeomAlgoAPI_DFLoader::refineResult(aResult);
  }

  std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape());
  aShape->setImpl(new TopoDS_Shape(aResult));
  this->setShape(aShape);
  this->setDone(true);
}
