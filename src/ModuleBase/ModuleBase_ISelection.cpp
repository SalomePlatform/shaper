// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#include "ModuleBase_ISelection.h"

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
