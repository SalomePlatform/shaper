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

#include <GeomAlgoAPI_STEPExport.h>

#include "GeomAlgoAPI_Tools.h"

#include <GeomAPI_Shape.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_AttributeIntArray.h>

#include <TDocStd_Document.hxx>
#include <TDataStd_Name.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFDoc_ColorTool.hxx>
#include <STEPCAFControl_Writer.hxx>
#include <Interface_Static.hxx>
#include <Quantity_Color.hxx>

// a structure to manage step document exported attributes
struct GeomAlgoAPI_STEPAttributes {
  bool myHasColor; ///< true if color is defined
  Quantity_Color myColor;
  std::wstring myName;
  Handle(XCAFDoc_ShapeTool) myShapeTool;
  Handle(XCAFDoc_ColorTool) myColorTool;

  GeomAlgoAPI_STEPAttributes(const TDF_Label& theMain) : myHasColor(false)
  {
    myShapeTool = XCAFDoc_DocumentTool::ShapeTool(theMain);
    myShapeTool->SetAutoNaming(Standard_False);
    myColorTool = XCAFDoc_DocumentTool::ColorTool(theMain);
  }
};

static TDF_Label exportShape(const GeomShapePtr theShape, GeomAlgoAPI_STEPAttributes& theAttrs,
  const GeomShapePtr theFatherShape, TDF_Label& theFaterID) {
  TopoDS_Shape aShape = theShape->impl<TopoDS_Shape>();
  TDF_Label aShapeLab;
  if (!theFaterID.IsNull()) { // make sub-component of father's assembly
    gp_Trsf aFatherTrsf;
    if (theFatherShape.get())
      aFatherTrsf = theFatherShape->implPtr<TopoDS_Shape>()->Location().Transformation();
    TopLoc_Location aLocation = aFatherTrsf.Inverted() * aShape.Location();
    static const TopLoc_Location anEmptyLoc;
    aShape.Location(anEmptyLoc);
    aShapeLab = theAttrs.myShapeTool->AddShape(aShape, Standard_False);
    TDF_Label aRetLabel = theAttrs.myShapeTool->AddComponent(theFaterID, aShapeLab, aLocation);
    if (!aRetLabel.IsNull())
      aRetLabel.ForgetAttribute(TDataStd_Name::GetID());
  }
  else { // make a single shape
    aShapeLab = theAttrs.myShapeTool->AddShape(aShape, Standard_False);
    TDF_Label aRefShapeLab;
    if (theAttrs.myShapeTool->GetReferredShape(aShapeLab, aRefShapeLab))
      aShapeLab = aRefShapeLab;
  }
  TDataStd_Name::Set(aShapeLab, TCollection_ExtendedString(theAttrs.myName.c_str()));
  if (theAttrs.myHasColor) {
    TDF_Label aColorLab = theAttrs.myColorTool->AddColor(theAttrs.myColor);
    theAttrs.myColorTool->SetColor(aShapeLab, aColorLab, XCAFDoc_ColorGen);
  }
  return aShapeLab;
}

/// Returns the attributes of the result: names and color
static void getAttributes(const ResultPtr& theResult, GeomAlgoAPI_STEPAttributes& theAttrs)
{
  theAttrs.myName = theResult->data()->name();
  AttributeIntArrayPtr aColorAttr = theResult->data()->intArray(ModelAPI_Result::COLOR_ID());
  if (aColorAttr.get() && aColorAttr->size() > 2) {
    theAttrs.myHasColor = true;
    theAttrs.myColor.SetValues(aColorAttr->value(0) / 255., aColorAttr->value(1) / 255.,
                               aColorAttr->value(2) / 255., Quantity_TOC_RGB);
  }
  else
    theAttrs.myHasColor = false;
}

/// Performs the whole result export: as an assembly or a single one, but with results attributes.
static void putResult(const ResultPtr& theResult, const GeomShapePtr theFatherShape,
  TDF_Label& theFaterID, GeomAlgoAPI_STEPAttributes& theAttrs)
{
  GeomShapePtr aShape = theResult->shape();
  if (!aShape.get() || aShape->isNull())
    return;
  ResultBodyPtr aBody = std::dynamic_pointer_cast<ModelAPI_ResultBody>(theResult);
  if (aBody.get() && aBody->numberOfSubs()) { // make an assembly
    getAttributes(theResult, theAttrs);
    TDF_Label aBodyID = exportShape(aShape, theAttrs, theFatherShape, theFaterID);
    int aNumSubs = aBody->numberOfSubs();
    for (int a = 0; a < aNumSubs; a++) {
      ResultBodyPtr aSub = aBody->subResult(a);
      if (!aSub->isDisabled())
        putResult(aSub, aShape, aBodyID, theAttrs);
    }
  }
  else { // a simple shape-body
    getAttributes(theResult, theAttrs);
    exportShape(aShape, theAttrs, theFatherShape, theFaterID);
  }
}

bool STEPExport(const std::string& theFileName,
                const std::list<std::shared_ptr<GeomAPI_Shape> >& theShapes,
                const std::list<std::shared_ptr<ModelAPI_Result> >& theResults,
                std::string& theError)
{
  theError = "";
  // prepare XCAF document to store the whole data structure there
  Handle(TDocStd_Document) aDoc = new TDocStd_Document("BinXCAF");

  GeomAlgoAPI_STEPAttributes anAttrs(aDoc->Main());

  std::list<std::shared_ptr<GeomAPI_Shape> >::const_iterator aShape = theShapes.cbegin();
  std::list<std::shared_ptr<ModelAPI_Result> >::const_iterator aResult = theResults.cbegin();
  for (; aShape != theShapes.cend(); aShape++, aResult++) {
    TDF_Label aNullLab;
    if (aResult->get() && !(*aShape)->isSame((*aResult)->shape()))
    { // simple sub-shape
      getAttributes(*aResult, anAttrs);
      exportShape(*aShape, anAttrs, GeomShapePtr(), aNullLab);
    }
    else { // whole result selection
      putResult(*aResult, GeomShapePtr(), aNullLab, anAttrs);
    }
  }
  // store the XCAF document to STEP file
  try {
    GeomAlgoAPI_Tools::Localizer aLocalizer; // Set "C" numeric locale to save numbers correctly
    STEPCAFControl_Writer aWriter;
    Interface_Static::SetCVal("xstep.cascade.unit", "M");
    Interface_Static::SetIVal("write.step.nonmanifold", 0); // 1 don't allow to export assemly tree
    Interface_Static::SetCVal("write.step.unit", "M");

    auto aStatus = aWriter.Transfer(aDoc, STEPControl_AsIs);
    if (aStatus == IFSelect_RetDone)
      aStatus = aWriter.Write(theFileName.c_str());
    if (aStatus != IFSelect_RetDone)
      theError = "STEP Export failed";
  }
  catch (Standard_Failure exception) {
    theError = "Exception catched in STEPExport";
  }
  return theError.empty();
}
