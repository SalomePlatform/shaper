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

#include <ModuleBase_Preferences.h>

#include <CollectionPlugin_Field.h>

#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeIntArray.h>
#include <ModelAPI_AttributeStringArray.h>

#include <SUIT_ResourceMgr.h>

#include <AIS_ColorScale.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_PointAspect.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopExp_Explorer.hxx>
#include <Prs3d_Root.hxx>


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

  SUIT_ResourceMgr* aResMgr = ModuleBase_Preferences::resourceMgr();
  QColor aQColor = aResMgr->colorValue("Viewer", "scalar_bar_text_color", Qt::black);

  myLabelColor = Quantity_Color(aQColor.redF(), aQColor.greenF(), aQColor.blueF(),
    Quantity_TOC_RGB);

  SetMaterial(Graphic3d_NOM_PLASTIC);
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
  for (int k = 1; k < aRows; k++) { // Do not use default values
    for (int j = 0; j < aCols; j++) {
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
  SUIT_ResourceMgr* aResMgr = ModuleBase_Preferences::resourceMgr();
  ModelAPI_AttributeTables::ValueType aType = dataType();
  DataPtr aData = myFeature->data();
  switch (aType) {
  case ModelAPI_AttributeTables::DOUBLE:
  case ModelAPI_AttributeTables::INTEGER:
  {
    double aMin, aMax;
    QList<double> aShapeData = range(aMin, aMax);
    int aNbIntertvals = aResMgr->integerValue("Viewer", "scalar_bar_nb_intervals", 20);

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
  break;
  case ModelAPI_AttributeTables::BOOLEAN:
  {
    QList<double> aShapeData = booleanValues();

    AttributeSelectionListPtr aSelList = aData->selectionList(CollectionPlugin_Field::SELECTED_ID());
    for (int i = 0; i < aSelList->size(); i++) {
      AttributeSelectionPtr aSelection = aSelList->value(i);
      GeomShapePtr aShapePtr = aSelection->value();
      TopoDS_Shape aShape = aShapePtr->impl<TopoDS_Shape>();
      double aValue = aShapeData.at(i);
      Quantity_Color aColor;
      if (AIS_ColorScale::FindColor(aValue, 0., 1., 2, aColor))
        SetCustomColor(aShape, aColor);
    }
  }
  break;
  case ModelAPI_AttributeTables::STRING:
  {
    QStringList aValues = strings();
    AttributeSelectionListPtr aSelList = aData->selectionList(CollectionPlugin_Field::SELECTED_ID());
    Handle(Graphic3d_Group) aGroup = Prs3d_Root::NewGroup(thePrs);
    for (int i = 0; i < aSelList->size(); i++) {
      AttributeSelectionPtr aSelection = aSelList->value(i);
      GeomShapePtr aShapePtr = aSelection->value();
      TopoDS_Shape aShape = aShapePtr->impl<TopoDS_Shape>();
      gp_Pnt aCenter;
      if (computeMassCenter(aShape, aCenter)) {
        Graphic3d_Vertex aVertex(aCenter.X(), aCenter.Y(), aCenter.Z());

        Handle(Graphic3d_AspectText3d) anAspectText3d = new Graphic3d_AspectText3d();
        anAspectText3d->SetStyle(Aspect_TOST_ANNOTATION);
        anAspectText3d->SetColor(myLabelColor);
        aGroup->SetPrimitivesAspect(anAspectText3d);

        int aT = aResMgr->integerValue("Viewer", "scalar_bar_text_height", 14);
        QString aString = aValues.at(i);
        aGroup->Text(aString.toUtf8().constData(), aVertex, aT);
      }
    }
  }
  break;
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
  for (int k = 1; k < aRows; k++) { // Do not use default values
    for (int j = 0; j < aCols; j++) {
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

QStringList PartSet_FieldStepPrs::strings() const
{
  DataPtr aData = myFeature->data();
  int aStep = myStep->id();
  AttributeTablesPtr aTablesAttr = aData->tables(CollectionPlugin_Field::VALUES_ID());
  int aRows = aTablesAttr->rows();
  int aCols = aTablesAttr->columns();
  QStringList aFieldStepData;
  for (int k = 1; k < aRows; k++) { // Do not use default values
    for (int j = 0; j < aCols; j++) {
      ModelAPI_AttributeTables::Value aVal = aTablesAttr->value(k, j, aStep);
      aFieldStepData << aVal.myStr.c_str();
    }
  }
  QStringList aShapeData;
  for (int aRow = 0; aRow < aRows - 1; aRow++) {
    QStringList aRowStrings;
    int aBaseIndex = aRow * aCols;
    for (int aCol = 0; aCol < aCols; aCol++) {
      aRowStrings << aFieldStepData.at(aCol + aBaseIndex);
    }
    aRowStrings.join('\n');
    aShapeData << aRowStrings;
  }
  return aShapeData;
}

bool PartSet_FieldStepPrs::computeMassCenter(const TopoDS_Shape& theShape, gp_Pnt& theCenter)
{
  theCenter.SetCoord(0, 0, 0);
  int aNbPoints = 0;

  if (theShape.ShapeType() == TopAbs_EDGE) {
    double f, l;
    Handle(Geom_Curve) curve = BRep_Tool::Curve(TopoDS::Edge(theShape), f, l);
    if (!curve.IsNull()) {
      theCenter = curve->Value(0.5 * (f + l));
      aNbPoints = 1;
    }
  }
  else if (theShape.ShapeType() == TopAbs_FACE) {
    const TopoDS_Face& F = TopoDS::Face(theShape);
    BRepAdaptor_Surface surface(F);

    TopLoc_Location L;
    Handle(Poly_Triangulation) triangulation = BRep_Tool::Triangulation(F, L);
    if (!triangulation.IsNull() && triangulation->HasUVNodes()) {
      gp_XY C(0, 0);
      double A = 0;
      const TColgp_Array1OfPnt2d& uvArray = triangulation->UVNodes();
      const Poly_Array1OfTriangle&  trias = triangulation->Triangles();
      int n1, n2, n3;
      for (int iT = trias.Lower(); iT <= trias.Upper(); ++iT) {
        trias(iT).Get(n1, n2, n3);
        const gp_Pnt2d& uv1 = uvArray(n1);
        const gp_Pnt2d& uv2 = uvArray(n2);
        const gp_Pnt2d& uv3 = uvArray(n3);
        double a = 0.5 * sqrt((uv1.X() - uv3.X()) * (uv2.Y() - uv1.Y()) -
          (uv1.X() - uv2.X()) * (uv3.Y() - uv1.Y()));
        C += (uv1.XY() + uv2.XY() + uv3.XY()) / 3. * a;
        A += a;
      }
      if (A > std::numeric_limits<double>::min()) {
        C /= A;
        theCenter = surface.Value(C.X(), C.Y());
        aNbPoints = 1;
      }
    }
    if (aNbPoints == 0) {
      theCenter = surface.Value(0.5 * (surface.FirstUParameter() + surface.LastUParameter()),
        0.5 * (surface.FirstVParameter() + surface.LastVParameter()));
    }
    aNbPoints = 1;
  }

  if (aNbPoints == 0) {
    TopExp_Explorer anExp;
    for (anExp.Init(theShape, TopAbs_VERTEX); anExp.More(); anExp.Next()) {
      TopoDS_Vertex aVertex = TopoDS::Vertex(anExp.Current());
      if (!aVertex.IsNull()) {
        gp_Pnt aPnt = BRep_Tool::Pnt(aVertex);
        theCenter.ChangeCoord() += aPnt.XYZ();
        aNbPoints++;
      }
    }
  }

  if (aNbPoints > 0)
    theCenter.ChangeCoord() /= (double)aNbPoints;

  return aNbPoints;
}
