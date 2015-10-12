// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#include "ModuleBase_ISelection.h"

#include <StdSelect_BRepOwner.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS.hxx>
#include <BRep_Tool.hxx>
#include <GeomAPI_Pnt.h>

//********************************************************************
void ModuleBase_ISelection::appendSelected(const QList<ModuleBase_ViewerPrs> theValues,
                                           QList<ModuleBase_ViewerPrs>& theValuesTo)
{
  // collect the objects from the viewer
  QObjectPtrList anExistedObjects;
  QList<ModuleBase_ViewerPrs>::const_iterator aPrsIt = theValuesTo.begin(),
                                              aPrsLast = theValuesTo.end();
  for (; aPrsIt != aPrsLast; aPrsIt++) {
    if ((*aPrsIt).owner() && (*aPrsIt).object())
      anExistedObjects.push_back((*aPrsIt).object());
  }


  QList<ModuleBase_ViewerPrs>::const_iterator anIt = theValues.begin(),
                                              aLast = theValues.end();
  for (; anIt != aLast; anIt++) {
    ObjectPtr anObject = (*anIt).object();
    if (anObject.get() != NULL && !anExistedObjects.contains(anObject)) {
      theValuesTo.append(ModuleBase_ViewerPrs(anObject, TopoDS_Shape(), NULL));
    }
  }

}

//********************************************************************
ResultPtr ModuleBase_ISelection::getResult(const ModuleBase_ViewerPrs& thePrs)
{
  ResultPtr aResult;

  if (!thePrs.owner().IsNull()) {
    ObjectPtr anObject = getSelectableObject(thePrs.owner());
    aResult = std::dynamic_pointer_cast<ModelAPI_Result>(anObject);
  }
  else {
    aResult = std::dynamic_pointer_cast<ModelAPI_Result>(thePrs.object());
  }

  return aResult;
}

//********************************************************************
GeomShapePtr ModuleBase_ISelection::getShape(const ModuleBase_ViewerPrs& thePrs)
{
  GeomShapePtr aShape;

  const TopoDS_Shape& aTDSShape = thePrs.shape();
  // if only result is selected, an empty shape is set to the model
  if (aTDSShape.IsNull()) {
  }
  else {
    aShape = GeomShapePtr(new GeomAPI_Shape());
    aShape->setImpl(new TopoDS_Shape(aTDSShape));
    // If the result object is built on the same shape, the result shape here is empty one
    ResultPtr aResult = getResult(thePrs);
    if (aResult.get() && aShape->isEqual(aResult->shape()))
      aShape = GeomShapePtr();
  }
  return aShape;
}

//********************************************************************
QList<ModuleBase_ViewerPrs> ModuleBase_ISelection::getViewerPrs(const QObjectPtrList& theObjects)
{
  QList<ModuleBase_ViewerPrs> aSelectedPrs;
  QObjectPtrList::const_iterator anIt = theObjects.begin(), aLast = theObjects.end();
  for (; anIt != aLast; anIt++) {
    ObjectPtr anObject = *anIt;
    if (anObject.get() != NULL) {
      aSelectedPrs.append(ModuleBase_ViewerPrs(anObject, TopoDS_Shape(), NULL));
    }
  }
  return aSelectedPrs;
}

//********************************************************************
void ModuleBase_ISelection::filterPreselectionOnEqualPoints
                                              (QList<ModuleBase_ViewerPrs>& theSelected)
{
  QList<ModuleBase_ViewerPrs> aCandidatesToRemove;
  QList<ModuleBase_ViewerPrs>::const_iterator anIt = theSelected.begin(),
                                              aLast = theSelected.end();
  QList<ModuleBase_ViewerPrs>::const_iterator aSubIt;
  for (; anIt != aLast; anIt++) {
    aSubIt = anIt;
    aSubIt++;
    for (; aSubIt != aLast; aSubIt++) {
      if (isEqualVertices(*anIt, *aSubIt)) {
        aCandidatesToRemove.append(*aSubIt);
        break;
      }
    }
  }
  QList<ModuleBase_ViewerPrs>::const_iterator aRemIt = aCandidatesToRemove.begin(),
                                              aRemLast = aCandidatesToRemove.end();
  for (; aRemIt != aRemLast; aRemIt++) {
    theSelected.removeAll(*aRemIt);
  }
}

bool ModuleBase_ISelection::isEqualVertices(const ModuleBase_ViewerPrs thePrs1,
                                            const ModuleBase_ViewerPrs thePrs2)
{
  bool isEqual = false;
  Handle(StdSelect_BRepOwner) anOwner1 = Handle(StdSelect_BRepOwner)::DownCast(thePrs1.owner());
  Handle(StdSelect_BRepOwner) anOwner2 = Handle(StdSelect_BRepOwner)::DownCast(thePrs2.owner());

  if (!anOwner1.IsNull() && anOwner1->HasShape() &&
      !anOwner2.IsNull() && anOwner2->HasShape()) {
    const TopoDS_Shape& aShape1 = anOwner1->Shape();
    const TopoDS_Shape& aShape2 = anOwner2->Shape();
    //TopAbs_ShapeEnum aShapeType = aShape.ShapeType();
    if (aShape1.ShapeType() == TopAbs_VERTEX &&
        aShape2.ShapeType() == TopAbs_VERTEX) {
      const TopoDS_Vertex& aVertex1 = TopoDS::Vertex(aShape1);
      const TopoDS_Vertex& aVertex2 = TopoDS::Vertex(aShape2);
      if (!aVertex1.IsNull() && !aVertex2.IsNull())  {
        gp_Pnt aPoint1 = BRep_Tool::Pnt(aVertex1);
        gp_Pnt aPoint2 = BRep_Tool::Pnt(aVertex2);

        std::shared_ptr<GeomAPI_Pnt> aPnt1 = std::shared_ptr<GeomAPI_Pnt>
                        (new GeomAPI_Pnt(aPoint1.X(), aPoint1.Y(), aPoint1.Z()));
        std::shared_ptr<GeomAPI_Pnt> aPnt2 = std::shared_ptr<GeomAPI_Pnt>
                        (new GeomAPI_Pnt(aPoint2.X(), aPoint2.Y(), aPoint2.Z()));
        isEqual = aPnt1->isEqual(aPnt2);
      }
    }
  }

  return isEqual;
}
