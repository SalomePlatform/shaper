// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

#include "ModuleBase_ISelection.h"

#include "ModuleBase_ViewerPrs.h"
#include "ModelAPI_Feature.h"

#include <StdSelect_BRepOwner.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS.hxx>
#include <BRep_Tool.hxx>
#include <GeomAPI_Pnt.h>

//********************************************************************
void ModuleBase_ISelection::appendSelected(const QList<ModuleBase_ViewerPrsPtr> theValues,
                                           QList<ModuleBase_ViewerPrsPtr>& theValuesTo)
{
  // collect the objects from the viewer
  QObjectPtrList anExistedObjects;
  foreach(ModuleBase_ViewerPrsPtr aPrs, theValuesTo) {
    if (aPrs->owner() && aPrs->object())
      anExistedObjects.append(aPrs->object());
  }

  ObjectPtr anObject;
  foreach(ModuleBase_ViewerPrsPtr aPrs, theValues) {
    anObject = aPrs->object();
    if (anObject.get() != NULL && !anExistedObjects.contains(anObject)) {
      theValuesTo.append(std::shared_ptr<ModuleBase_ViewerPrs>(
               new ModuleBase_ViewerPrs(anObject, GeomShapePtr(), NULL)));
    }
  }
}

//********************************************************************
ResultPtr ModuleBase_ISelection::getResult(const ModuleBase_ViewerPrsPtr& thePrs)
{
  ResultPtr aResult;

  if (thePrs->object().get()) {
    ObjectPtr aObject = thePrs->object();
    aResult = std::dynamic_pointer_cast<ModelAPI_Result>(aObject);
  }
  else if (!thePrs->owner().IsNull()) {
    ObjectPtr anObject = getSelectableObject(thePrs->owner());
    aResult = std::dynamic_pointer_cast<ModelAPI_Result>(anObject);
  }

  return aResult;
}

//********************************************************************
GeomShapePtr ModuleBase_ISelection::getShape(const ModuleBase_ViewerPrsPtr& thePrs)
{
  GeomShapePtr aShape;

  const GeomShapePtr& aPrsShape = thePrs->shape();
  // if only result is selected, an empty shape is set to the model
  if (!aPrsShape.get() || aPrsShape->isNull()) {
  }
  else {
    aShape = aPrsShape;
    // If the result object is built on the same shape, the result shape here is empty one
    ResultPtr aResult = getResult(thePrs);
    if (aResult.get() && aShape->isEqual(aResult->shape()))
      aShape = GeomShapePtr();
  }
  return aShape;
}

//********************************************************************
QList<ModuleBase_ViewerPrsPtr> ModuleBase_ISelection::getViewerPrs(const QObjectPtrList& theObjects)
{
  QList<ModuleBase_ViewerPrsPtr> aSelectedPrs;
  QObjectPtrList::const_iterator anIt = theObjects.begin(), aLast = theObjects.end();
  for (; anIt != aLast; anIt++) {
    ObjectPtr anObject = *anIt;
    if (anObject.get() != NULL) {
      aSelectedPrs.append(std::shared_ptr<ModuleBase_ViewerPrs>(
               new ModuleBase_ViewerPrs(anObject, GeomShapePtr(), NULL)));
    }
  }
  return aSelectedPrs;
}

//********************************************************************
void ModuleBase_ISelection::filterSelectionOnEqualPoints
                                              (QList<ModuleBase_ViewerPrsPtr>& theSelected)
{
  QList<ModuleBase_ViewerPrsPtr> aCandidatesToRemove;
  QList<ModuleBase_ViewerPrsPtr>::const_iterator anIt = theSelected.begin(),
                                              aLast = theSelected.end();
  QList<ModuleBase_ViewerPrsPtr>::const_iterator aSubIt;

  std::set<std::shared_ptr<GeomAPI_Vertex> > aVerticesMap;
  for (; anIt != aLast; anIt++) {
    ModuleBase_ViewerPrsPtr aPrs = *anIt;
    std::shared_ptr<GeomAPI_Vertex> aGeomPrsVertex = getPresentationVertex(aPrs);
    if (aGeomPrsVertex.get()) {
      const TopoDS_Vertex& aPrsVertex = aGeomPrsVertex->impl<TopoDS_Vertex>();
      std::set<std::shared_ptr<GeomAPI_Vertex> >::const_iterator aVIt = aVerticesMap.begin(),
                                                                 aVLast = aVerticesMap.end();
      bool aFound = false;
      for (; aVIt != aVLast && !aFound; aVIt++) {
        std::shared_ptr<GeomAPI_Vertex> aGeomVertex = *aVIt;
        const TopoDS_Vertex& aVertex = aGeomVertex->impl<TopoDS_Vertex>();
        gp_Pnt aPoint1 = BRep_Tool::Pnt(aVertex);
        gp_Pnt aPoint2 = BRep_Tool::Pnt(aPrsVertex);

        std::shared_ptr<GeomAPI_Pnt> aPnt1 = std::shared_ptr<GeomAPI_Pnt>
                        (new GeomAPI_Pnt(aPoint1.X(), aPoint1.Y(), aPoint1.Z()));
        std::shared_ptr<GeomAPI_Pnt> aPnt2 = std::shared_ptr<GeomAPI_Pnt>
                        (new GeomAPI_Pnt(aPoint2.X(), aPoint2.Y(), aPoint2.Z()));
        aFound = aPnt1->isEqual(aPnt2);
      }
      if (aFound) {
        aCandidatesToRemove.append(aPrs);
        continue;
      }
      aVerticesMap.insert(aGeomPrsVertex);
    }
  }
  QList<ModuleBase_ViewerPrsPtr>::const_iterator aRemIt = aCandidatesToRemove.begin(),
                                              aRemLast = aCandidatesToRemove.end();
  for (; aRemIt != aRemLast; aRemIt++) {
    theSelected.removeAll(*aRemIt);
  }
}

std::shared_ptr<GeomAPI_Vertex> ModuleBase_ISelection::getPresentationVertex(
                                                         const ModuleBase_ViewerPrsPtr& thePrs)
{
  std::shared_ptr<GeomAPI_Vertex> aGeomVertex;
  Handle(StdSelect_BRepOwner) anOwner = Handle(StdSelect_BRepOwner)::DownCast(thePrs->owner());

  if (!anOwner.IsNull() && anOwner->HasShape()) {
    const TopoDS_Shape& aShape = anOwner->Shape();
    if (aShape.ShapeType() == TopAbs_VERTEX) {
      const TopoDS_Vertex& aVertex = TopoDS::Vertex(aShape);
      if (!aVertex.IsNull())  {
        gp_Pnt aPoint = BRep_Tool::Pnt(aVertex);
        aGeomVertex = std::shared_ptr<GeomAPI_Vertex>(new GeomAPI_Vertex(aPoint.X(), aPoint.Y(),
                                                                         aPoint.Z()));
      }
    }
  }
  return aGeomVertex;
}

