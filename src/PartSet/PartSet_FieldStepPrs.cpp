// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

#include "PartSet_FieldStepPrs.h"

#include <CollectionPlugin_Field.h>

#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeIntArray.h>
#include <ModelAPI_AttributeStringArray.h>

#include <AIS_ColorScale.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_PointAspect.hxx>


IMPLEMENT_STANDARD_RTTIEXT(PartSet_FieldStepPrs, ViewerData_AISShape);

#define POINT_SIZE 8


void emptyDeleter(ModelAPI_ResultField* theF)
{
  // Do nothing
}

PartSet_FieldStepPrs::PartSet_FieldStepPrs(FieldStepPtr theStep)
: ViewerData_AISShape(TopoDS_Shape()), myStep(theStep)
{
  ModelAPI_ResultField* aField = theStep->field();
  GeomShapePtr aShapePtr = aField->shape();
  TopoDS_Shape aShape = aShapePtr->impl<TopoDS_Shape>();
  Set(aShape);

  // Get parameters of the Field
  // Make shared_ptr which will not delete original pointer after exit
  std::shared_ptr<ModelAPI_ResultField> aFieldPtr(aField, emptyDeleter);
  myFeature = ModelAPI_Feature::feature(aFieldPtr);

  Handle(Prs3d_Drawer) aDrawer = Attributes();
  if (aDrawer->HasOwnPointAspect()) {
    aDrawer->PointAspect()->SetTypeOfMarker(Aspect_TOM_POINT);
    aDrawer->PointAspect()->SetScale(POINT_SIZE);
  }
  else
    aDrawer->SetPointAspect(
      new Prs3d_PointAspect(Aspect_TOM_POINT, Quantity_NOC_YELLOW, POINT_SIZE));
}


ModelAPI_AttributeTables::ValueType PartSet_FieldStepPrs::dataType() const
{
  DataPtr aData = myFeature->data();
  AttributeTablesPtr aTablesAttr = aData->tables(CollectionPlugin_Field::VALUES_ID());
  return aTablesAttr->type();
}

bool PartSet_FieldStepPrs::dataRange(double& theMin, double& theMax) const
{
  ModelAPI_AttributeTables::ValueType aType = dataType();
  if ((aType == ModelAPI_AttributeTables::DOUBLE) || (aType == ModelAPI_AttributeTables::INTEGER)) {
    range(theMin, theMax);
    return true;
  }
  return false;
}

QList<double> PartSet_FieldStepPrs::range(double& theMin, double& theMax) const
{
  ModelAPI_AttributeTables::ValueType aType = dataType();
  DataPtr aData = myFeature->data();
  int aStep = myStep->id();
  AttributeTablesPtr aTablesAttr = aData->tables(CollectionPlugin_Field::VALUES_ID());
  int aRows = aTablesAttr->rows();
  int aCols = aTablesAttr->columns();

  QList<double> aFieldStepData;
  for (int j = 0; j < aCols; j++) {
    for (int k = 1; k < aRows; k++) { // Do not use default values
      ModelAPI_AttributeTables::Value aVal = aTablesAttr->value(k, j, aStep);
      switch (aType) {
      case ModelAPI_AttributeTables::DOUBLE:
        aFieldStepData << aVal.myDouble;
        break;
      case ModelAPI_AttributeTables::INTEGER:
        aFieldStepData << aVal.myInt;
        break;
      }
    }
  }
  QList<double> aShapeData;
  double aRangeMin = aFieldStepData.first(), aRangeMax = aFieldStepData.last();
  for (int aRow = 0; aRow < aRows - 1; aRow++) {
    double aNorm = 0;
    int aBaseIndex = aRow * aCols;
    for (int aCol = 0; aCol < aCols; aCol++) {
      int anIndex = aCol + aBaseIndex;
      double aValue = aFieldStepData.at(anIndex);
      aNorm += aValue * aValue;
    }
    aNorm = pow(aNorm, 0.5);
    aRangeMin = Min(aRangeMin, aNorm);
    aRangeMax = Max(aRangeMax, aNorm);
    aShapeData << aNorm;
  }
  theMin = aRangeMin;
  theMax = aRangeMax;
  return aShapeData;
}


void PartSet_FieldStepPrs::Compute(const Handle(PrsMgr_PresentationManager3d)& thePrsMgr,
  const Handle(Prs3d_Presentation)& thePrs, const Standard_Integer theMode)
{
  ModelAPI_AttributeTables::ValueType aType = dataType();
  DataPtr aData = myFeature->data();
  if ((aType == ModelAPI_AttributeTables::DOUBLE) ||
    (aType == ModelAPI_AttributeTables::INTEGER)) {
    double aMin, aMax;
    QList<double> aShapeData = range(aMin, aMax);
    int aNbIntertvals = 20;

    AttributeSelectionListPtr aSelList = aData->selectionList(CollectionPlugin_Field::SELECTED_ID());
    for (int i = 0; i < aSelList->size(); i++) {
      AttributeSelectionPtr aSelection = aSelList->value(i);
      GeomShapePtr aShapePtr = aSelection->value();
      TopoDS_Shape aShape = aShapePtr->impl<TopoDS_Shape>();
      double aValue = aShapeData.at(i);
      Quantity_Color aColor;
      if (AIS_ColorScale::FindColor(aValue, aMin, aMax, aNbIntertvals, aColor))
        SetCustomColor(aShape, aColor);
    }
  }
  else if (aType == ModelAPI_AttributeTables::BOOLEAN) {
    QList<double> aShapeData = booleanValues();

    AttributeSelectionListPtr aSelList = aData->selectionList(CollectionPlugin_Field::SELECTED_ID());
    for (int i = 0; i < aSelList->size(); i++) {
      AttributeSelectionPtr aSelection = aSelList->value(i);
      GeomShapePtr aShapePtr = aSelection->value();
      TopoDS_Shape aShape = aShapePtr->impl<TopoDS_Shape>();
      double aValue = aShapeData.at(i);
      Quantity_Color aColor;
      if (AIS_ColorScale::FindColor(aValue, 0., 1. + Precision::Confusion(), 2, aColor))
        SetCustomColor(aShape, aColor);
    }
  }
  ViewerData_AISShape::Compute(thePrsMgr, thePrs, theMode);
}

QList<double> PartSet_FieldStepPrs::booleanValues() const
{
  DataPtr aData = myFeature->data();
  int aStep = myStep->id();
  AttributeTablesPtr aTablesAttr = aData->tables(CollectionPlugin_Field::VALUES_ID());
  int aRows = aTablesAttr->rows();
  int aCols = aTablesAttr->columns();
  QList<int> aFieldStepData;
  for (int j = 0; j < aCols; j++) {
    for (int k = 1; k < aRows; k++) { // Do not use default values
      ModelAPI_AttributeTables::Value aVal = aTablesAttr->value(k, j, aStep);
      aFieldStepData << (aVal.myBool ? 1 : 0);
    }
  }
  QList<double> aShapeData;
  double aRangeMin = aFieldStepData.first(), aRangeMax = aFieldStepData.last();
  for (int aRow = 0; aRow < aRows - 1; aRow++) {
    double aNorm = 0;
    int aBaseIndex = aRow * aCols;
    for (int aCol = 0; aCol < aCols; aCol++) {
      aNorm += aFieldStepData.at(aCol + aBaseIndex);
    }
    aNorm /= aCols;
    aShapeData << aNorm;
  }
  return aShapeData;
}
