// Copyright (C) 2014-2026  CEA, EDF
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

#include "GeomAlgoAPI_Tools.h"
#include "GeomAlgoAPI_MakeShape.h"

#include <TopExp_Explorer.hxx>
#include <Poly_Triangulation.hxx>
#include <BRep_Tool.hxx>
#include <TopoDS.hxx>
#include <BRepMesh_IncrementalMesh.hxx>

#include <clocale>

#include <TCollection_AsciiString.hxx>
#include <OSD_Path.hxx>

#include <GeomAlgoAPI_ShapeTools.h>
#include <GeomAPI_Shape.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_AttributeIntArray.h>

#include <TDataStd_Name.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFDoc_ColorTool.hxx>
#include <Quantity_Color.hxx>

using namespace GeomAlgoAPI_Tools;

namespace {

  // This function computes the triangulation of the given shape if it is not already tessellated.
  static void computeTriangulation(const TopoDS_Shape& theShape)
  {
    Standard_Boolean isTessellate = Standard_False;
    TopLoc_Location aLoc;
    for (TopExp_Explorer anExp(theShape, TopAbs_FACE); anExp.More() && !isTessellate; anExp.Next()) {
      Handle(Poly_Triangulation) aTria = BRep_Tool::Triangulation(TopoDS::Face(anExp.Value()), aLoc);
      isTessellate = aTria.IsNull();
    }

    if (isTessellate) {
      BRepMesh_IncrementalMesh aMesher(theShape, 0.1);
      Standard_ProgramError_Raise_if(!aMesher.IsDone(), "Meshing failed");
    }
  }
}

Localizer::Localizer()
{
  myCurLocale = std::setlocale(LC_NUMERIC, 0);
  std::setlocale(LC_NUMERIC, "C");
}

Localizer::~Localizer()
{
  std::setlocale(LC_NUMERIC, myCurLocale.c_str());
}


std::string File_Tools::extension(const std::string& theFileName)
{
  // retrieve the file and plugin library names
  TCollection_AsciiString aFileName(theFileName.c_str());
  OSD_Path aPath(aFileName);
  TCollection_AsciiString anExtension = aPath.Extension();
  if (anExtension.Length() < 2)
    return "";
  // TCollection_AsciiString are numbered from 1
  anExtension = anExtension.SubString(2, anExtension.Length());
  anExtension.UpperCase();
  return anExtension.ToCString();
}

std::string File_Tools::name(const std::string& theFileName)
{
  // retrieve the file and plugin library names
  TCollection_AsciiString aFileName(theFileName.c_str());
  OSD_Path aPath(aFileName);
  return aPath.Name().ToCString();
}

std::string File_Tools::path(const std::string& theFileName)
{
  OSD_Path aPath (theFileName.c_str());
  Standard_Integer aTrekLen =
    int(theFileName.size()) - aPath.Extension().Length() - aPath.Name().Length();
  return theFileName.substr(0, aTrekLen);
}

bool AlgoError::isAlgorithmFailed(const GeomMakeShapePtr& theAlgorithm,
                                  const std::string& theFeature,
                                  std::string& theError)
{
  theError.clear();
  if (!theAlgorithm->isDone()) {
    theError = "Error: " + (theFeature.empty() ? "The" : theFeature) + " algorithm failed.";
    std::string anAlgoError = theAlgorithm->getError();
    if (!anAlgoError.empty())
      theError += " " + anAlgoError;
    return true;
  }
  if (!theAlgorithm->shape() || theAlgorithm->shape()->isNull()) {
    theError = "Error: Resulting shape";
    if (!theFeature.empty())
      theError += " of " + theFeature;
    theError += " is Null.";
    return true;
  }
  if (!theAlgorithm->isValid()) {
    theError = "Error: Resulting shape";
    if (!theFeature.empty())
      theError += " of " + theFeature;
    theError += " is not valid.";
    return true;
  }
  return false;
}


TDF_Label GeomAlgoAPI_Tools::AttributeExport_Tools::exportShape(const GeomShapePtr theShape, GeomAlgoAPI_Attributes& theAttrs, const GeomShapePtr theFatherShape, TDF_Label& theFaterID)
{
  Handle(XCAFDoc_ShapeTool) aShapeTool = XCAFDoc_DocumentTool::ShapeTool(theAttrs.myMain);
  aShapeTool->SetAutoNaming(Standard_False);
  Handle(XCAFDoc_ColorTool) aColorTool = XCAFDoc_DocumentTool::ColorTool(theAttrs.myMain);

  TopoDS_Shape aShape = theShape->impl<TopoDS_Shape>();
  TDF_Label aShapeLab;
  if (!theFaterID.IsNull()) { // make sub-component of father's assembly
    gp_Trsf aFatherTrsf;
    if (theFatherShape.get())
      aFatherTrsf = theFatherShape->implPtr<TopoDS_Shape>()->Location().Transformation();
    TopLoc_Location aLocation = aFatherTrsf.Inverted() * aShape.Location();
    static const TopLoc_Location anEmptyLoc;
    aShape.Location(anEmptyLoc);
    aShapeLab = aShapeTool->AddShape(aShape, Standard_False);
    TDF_Label aRetLabel = aShapeTool->AddComponent(theFaterID, aShapeLab, aLocation);
    if (!aRetLabel.IsNull())
      aRetLabel.ForgetAttribute(TDataStd_Name::GetID());
  }
  else { // make a single shape
    aShapeLab = aShapeTool->AddShape(aShape, Standard_False);
    TDF_Label aRefShapeLab;
    if (aShapeTool->GetReferredShape(aShapeLab, aRefShapeLab))
      aShapeLab = aRefShapeLab;
  }
  TDataStd_Name::Set(aShapeLab, TCollection_ExtendedString(theAttrs.myName.c_str()));
  if (theAttrs.myHasColor) {
    TDF_Label aColorLab = aColorTool->AddColor(theAttrs.myColor);
    aColorTool->SetColor(aShapeLab, aColorLab, XCAFDoc_ColorGen);
  }
  return aShapeLab;
}

void GeomAlgoAPI_Tools::AttributeExport_Tools::getAttributes(const ResultPtr& theResult, GeomAlgoAPI_Attributes& theAttrs)
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

void GeomAlgoAPI_Tools::AttributeExport_Tools::putResult(const ResultPtr& theResult, const GeomShapePtr theFatherShape, TDF_Label& theFaterID, GeomAlgoAPI_Attributes& theAttrs)
{
  GeomShapePtr aShape = theResult->shape();
  computeTriangulation(aShape->impl<TopoDS_Shape>());

  if (!aShape.get() || aShape->isNull())
    return;
  ResultBodyPtr aBody = std::dynamic_pointer_cast<ModelAPI_ResultBody>(theResult);
  if (aBody.get() && aBody->numberOfSubs()) { // make an assembly
    AttributeExport_Tools::getAttributes(theResult, theAttrs);
    TDF_Label aBodyID = AttributeExport_Tools::exportShape(aShape, theAttrs, theFatherShape, theFaterID);
    int aNumSubs = aBody->numberOfSubs();
    for (int a = 0; a < aNumSubs; a++) {
      ResultBodyPtr aSub = aBody->subResult(a);
      if (!aSub->isDisabled())
        AttributeExport_Tools::putResult(aSub, aShape, aBodyID, theAttrs);
    }
  }
  else { // a simple shape-body
    AttributeExport_Tools::getAttributes(theResult, theAttrs);
    AttributeExport_Tools::exportShape(aShape, theAttrs, theFatherShape, theFaterID);
  }
}
