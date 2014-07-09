// File:        PartSet_Validators.cpp
// Created:     09 July 2014
// Author:      Vitaly SMETANNIKOV

#include "PartSet_Validators.h"

#include <list>


int shapesNb(const ModuleBase_ISelection* theSelection, TopAbs_ShapeEnum theType)
{
  std::list<ModuleBase_ViewerPrs> aList = theSelection->getSelected();
  std::list<ModuleBase_ViewerPrs>::iterator it;
  ModuleBase_ViewerPrs aPrs;
  int aCount = 0;
  for (it = aList.begin(); it != aList.end(); ++it) {
    aPrs = *it;
    const TopoDS_Shape& aShape = aPrs.shape();
    if (!aShape.IsNull()) {
      if (aShape.ShapeType() == theType)
        aCount++;
    }
  }
  return aCount;
}

bool PartSet_DistanceValidator::isValid(const ModuleBase_ISelection* theSelection) const
{
  int aCount = shapesNb(theSelection, TopAbs_VERTEX);
  return (aCount > 0) && (aCount < 3);
}

bool PartSet_LengthValidator::isValid(const ModuleBase_ISelection* theSelection) const
{
  int aCount = shapesNb(theSelection, TopAbs_EDGE);
  return (aCount == 1);
}