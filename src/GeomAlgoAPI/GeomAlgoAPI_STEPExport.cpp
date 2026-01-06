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

#include <GeomAlgoAPI_STEPExport.h>

#include "GeomAlgoAPI_Tools.h"

#include <GeomAPI_Shape.h>
#include <ModelAPI_ResultBody.h>

#include <TDocStd_Document.hxx>
#include <TDataStd_Name.hxx>
#include <STEPCAFControl_Writer.hxx>
#include <StepData_StepModel.hxx>
#include <UnitsMethods.hxx>
#include <Interface_Static.hxx>
#include <Quantity_Color.hxx>
#include <XSControl_WorkSession.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_ColorTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <TopExp_Explorer.hxx>

#include <Basics_OCCTVersion.hxx>

bool STEPExport(const std::string& theFileName,
                const std::list<std::shared_ptr<GeomAPI_Shape> >& theShapes,
                const std::list<std::shared_ptr<ModelAPI_Result> >& theResults,
                std::string& theError)
{
  theError = "";
  // prepare XCAF document to store the whole data structure there
  Handle(TDocStd_Document) aDoc = new TDocStd_Document("BinXCAF");

  GeomAlgoAPI_Tools::AttributeExport_Tools::GeomAlgoAPI_Attributes anAttrs(aDoc->Main());

  std::list<std::shared_ptr<GeomAPI_Shape> >::const_iterator aShape = theShapes.cbegin();
  std::list<std::shared_ptr<ModelAPI_Result> >::const_iterator aResult = theResults.cbegin();
  for (; aShape != theShapes.cend(); aShape++, aResult++) {
    TDF_Label aNullLab;
    if (aResult->get() && !(*aShape)->isSame((*aResult)->shape()))
    { // simple sub-shape
      GeomAlgoAPI_Tools::AttributeExport_Tools::getAttributes(*aResult, anAttrs);
      GeomAlgoAPI_Tools::AttributeExport_Tools::exportShape(*aShape, anAttrs, GeomShapePtr(), aNullLab);
    }
    else if (aResult->get()) { // whole result selection
      GeomAlgoAPI_Tools::AttributeExport_Tools::putResult(*aResult, GeomShapePtr(), aNullLab, anAttrs);
    }
    else { // feature selection => export simple shape
      GeomAlgoAPI_Tools::AttributeExport_Tools::exportShape(*aShape, anAttrs, GeomShapePtr(), aNullLab);
    }
  }
  // store the XCAF document to STEP file
  try {
    GeomAlgoAPI_Tools::Localizer aLocalizer; // Set "C" numeric locale to save numbers correctly

#if OCC_VERSION_LARGE < 0x07070000
    STEPCAFControl_Writer aWriter;
    Interface_Static::SetCVal("xstep.cascade.unit", "M");
    Interface_Static::SetIVal("write.step.nonmanifold", 0); // 1 don't allow to export assemly tree
    Interface_Static::SetCVal("write.step.unit", "M");
#elif OCC_VERSION_LARGE < 0x07080000
    STEPCAFControl_Writer aWriterTmp;
    Interface_Static::SetCVal("xstep.cascade.unit", "M");
    Interface_Static::SetIVal("write.step.nonmanifold", 0); // 1 don't allow to export assemly tree
    Interface_Static::SetCVal("write.step.unit", "M");
    STEPCAFControl_Writer aWriter;
#else
    STEPCAFControl_Writer aWriter;
    Interface_Static::SetCVal("xstep.cascade.unit", "M");
    Interface_Static::SetIVal("write.step.nonmanifold", 0); // 1 don't allow to export assemly tree
    Interface_Static::SetCVal("write.step.unit", "M");
    Handle(StepData_StepModel) aModel = aWriter.ChangeWriter().Model();
    aModel->InternalParameters.InitFromStatic();
    Standard_Integer aWriteUnitInt = Interface_Static::IVal("write.step.unit");
    Standard_Real aWriteUnitReal = UnitsMethods::GetLengthFactorValue(aWriteUnitInt);
    aModel->SetWriteLengthUnit(aWriteUnitReal);
#endif

    auto aStatus = aWriter.Transfer(aDoc, STEPControl_AsIs);
    if (aStatus == IFSelect_RetDone)
      aStatus = aWriter.Write(theFileName.c_str());
    if (aStatus != IFSelect_RetDone)
      theError = "STEP Export failed";
  }
  catch (Standard_Failure&) {
    theError = "Exception caught in STEPExport";
  }
  return theError.empty();
}

bool STEPExport(const std::string& theFileName,
  const std::shared_ptr<GeomAPI_Shape>& theShape,
  std::map<std::shared_ptr<GeomAPI_Shape>, std::vector<int>>& theColoredShapes,
  std::string& theError)
{
  Handle(TDocStd_Document) aDoc = new TDocStd_Document("MDTV-CAF");
#ifdef _DEBUG
  std::cout << "Export STEP into file " << theFileName << std::endl;
#endif
  GeomAlgoAPI_Tools::Localizer loc;

  Handle(XSControl_WorkSession) aWS = new XSControl_WorkSession();
  STEPCAFControl_Writer aWriter(aWS, false);
  aWriter.SetColorMode(true);
  aWriter.SetNameMode(true);

  IFSelect_ReturnStatus aStatus;
  //VRV: OCC 4.0 migration
  Interface_Static::SetCVal("xstep.cascade.unit", "M");
  Interface_Static::SetCVal("write.step.unit", "M");
  Interface_Static::SetIVal("write.step.nonmanifold", 1);

  TopoDS_Shape aShape = theShape->impl<TopoDS_Shape>();
  Handle(XCAFDoc_ShapeTool) aShapeTool = XCAFDoc_DocumentTool::ShapeTool(aDoc->Main());
  Handle(XCAFDoc_ColorTool) aColorTool = XCAFDoc_DocumentTool::ColorTool(aDoc->Main());

  TDF_Label aShapeLabel = aShapeTool->AddShape(aShape, false);
  TDF_Label aRefShapeLab;
  if (aShapeTool->GetReferredShape(aShapeLabel, aRefShapeLab))
    aShapeLabel = aRefShapeLab;

  if (theColoredShapes.find(theShape) != theColoredShapes.end())
  {
    std::vector<int> aColor = theColoredShapes.find(theShape)->second;
    Quantity_Color aQColor(aColor[0] / 255.,
      aColor[1] / 255.,
      aColor[2] / 255.,
      Quantity_TOC_RGB);
    aColorTool->SetColor(aShapeLabel, aQColor, XCAFDoc_ColorGen);
    theColoredShapes.erase(theShape);
  }

  for (std::map<GeomShapePtr, std::vector<int>>::iterator anIter(theColoredShapes.begin());
    anIter != theColoredShapes.end(); ++anIter)
  {
    TopoDS_Shape aSubShape = (anIter->first)->impl<TopoDS_Shape>();
    if (aSubShape.IsNull())
      continue;
    bool sub = theShape->isSubShape(anIter->first);
    Quantity_Color aColor(anIter->second.at(0) / 255.,
      anIter->second.at(1) / 255.,
      anIter->second.at(2) / 255.,
      Quantity_TOC_RGB);
    TDF_Label aSubShapeLabel;
    aShapeTool->Search(aShape, aSubShapeLabel);
    if (!aSubShapeLabel.IsNull())
    {
      aColorTool->SetColor(aSubShapeLabel, aColor, XCAFDoc_ColorSurf);
    }
  }
  /*for (TopExp_Explorer aFaceExp(aShape, TopAbs_FACE); aFaceExp.More(); aFaceExp.Next())
  {
    TDF_Label aSubShapeLabel;
    std::shared_ptr<GeomAPI_Shape> aFace(new GeomAPI_Shape);
    aFace->setImpl(new TopoDS_Shape(aFaceExp.Value()));
    if (theColoredShapes.find(theShape) == theColoredShapes.end())
      continue;
    aShapeTool->Search(aFaceExp.Value(), aSubShapeLabel);
    if (!aSubShapeLabel.IsNull())
    {
      std::vector<int> aColor = theColoredShapes.find(theShape)->second;
      Quantity_Color aQColor(aColor[0] / 255.,
                             aColor[1] / 255.,
                             aColor[2] / 255.,
                             Quantity_TOC_RGB);
      aColorTool->SetColor(aSubShapeLabel, aQColor, XCAFDoc_ColorSurf);
    }
  }*/
  /*for (std::map<GeomShapePtr, std::vector<int>>::const_iterator anIter(theColoredShapes.cbegin());
       anIter != theColoredShapes.cend(); ++anIter)
  {
    TopoDS_Shape aSubShape = (anIter->first)->impl<TopoDS_Shape>();
    if (aSubShape.IsNull())
      continue;
    bool sub = theShape->isSubShape(anIter->first);
    Quantity_Color aColor(anIter->second.at(0) / 255.,
                          anIter->second.at(1) / 255.,
                          anIter->second.at(2) / 255.,
                          Quantity_TOC_RGB);
    //TDF_Label aColorLabel = aColorTool->AddColor(aColor);
    TDF_Label aSubShapeLabel = aShape.IsEqual(aSubShape) ?
      aShapeLabel : aShapeTool->AddSubShape(aShapeLabel, aSubShape);
    aColorTool->SetColor(aSubShapeLabel, aColor,
                         aShape.IsEqual(aSubShape) ? XCAFDoc_ColorGen : XCAFDoc_ColorSurf);
  }*/

  /////////////////
  TDF_LabelSequence aLabels;
  aColorTool->GetColors(aLabels);
  Standard_Integer i, nbc = aLabels.Length();
  for (i = 1; i <= nbc; ++i)
  {
    Quantity_Color aColorQ;
    TDF_Label aLabel = aLabels.Value(i);
    aColorTool->GetColor(aLabel, aColorQ);
    int a = 0;
  }
  TDF_LabelSequence aShapesL;
  aShapeTool->GetFreeShapes(aShapesL);
  nbc = aShapesL.Length();
  for (i = 1; i <= nbc; ++i)
  {
    Quantity_Color aColorR;
    TDF_LabelSequence aSubShapesL;
    TopoDS_Shape aShape2;
    aShapeTool->GetShape(aShapesL.Value(i), aShape2);
    TDF_Label aShapeLabel = aShapeTool->FindShape(aShape2);
    aColorTool->GetColor(aShapeLabel, XCAFDoc_ColorGen, aColorR);
    aShapeTool->GetSubShapes(aShapesL.Value(i), aSubShapesL);
    for (TDF_LabelSequence::Iterator aLabIt(aSubShapesL); aLabIt.More(); aLabIt.Next())
    {
      const TDF_Label& aSubShapeLab = aLabIt.Value();
      TopoDS_Shape aSubShape = aShapeTool->GetShape(aSubShapeLab);
      aColorTool->GetColor(aSubShapeLab, XCAFDoc_ColorSurf, aColorR);
    }
  }
  /////////////////////
  bool aaaaaa = aWriter.GetColorMode();
  if (!aWriter.Transfer(aDoc, STEPControl_AsIs))
  {
    theError = "Error: Failed transfer to file.";
    return false;
  }
  aStatus = aWriter.Write(theFileName.c_str());
  if (aStatus != IFSelect_RetDone)
  {

    theError = "Error: Failed write to file.";
    return false;
  }
  return true;
}
