// Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

#include <GeomAlgoAPI_STEPImportXCAF.h>

#include <BRep_Builder.hxx>

#include <Interface_EntityIterator.hxx>
#include <Interface_Graph.hxx>
#include <Interface_InterfaceModel.hxx>

#include <Quantity_Color.hxx>

#include <StepRepr_DescriptiveRepresentationItem.hxx>
#include <StepRepr_ProductDefinitionShape.hxx>
#include <StepRepr_PropertyDefinitionRepresentation.hxx>
#include <StepRepr_Representation.hxx>

#include <TDataStd_Name.hxx>
#include <TDF_ChildIDIterator.hxx>
#include <TDocStd_Document.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <Transfer_TransientProcess.hxx>
#include <TransferBRep.hxx>

#include <XCAFApp_Application.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_Location.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XSControl_TransferReader.hxx>
#include <XSControl_WorkSession.hxx>

#include <Locale_Convert.h>

//=============================================================================
TopoDS_Shape getShape(const Handle(Standard_Transient) &theEnti,
                      const Handle(Transfer_TransientProcess) &theTP)
{
  TopoDS_Shape aResult;
  Handle(Transfer_Binder) aBinder = theTP->Find(theEnti);

  if (aBinder.IsNull()) {
    return aResult;
  }

  aResult = TransferBRep::ShapeResult(aBinder);

  return aResult;
}

//=============================================================================
std::shared_ptr<GeomAPI_Shape> readAttributes(STEPCAFControl_Reader &theReader,
                               std::shared_ptr<ModelAPI_ResultBody> theResultBody,
                               const bool  theIsMaterials,
                               std::map< std::wstring,std::list<std::wstring>> &theMaterialShape,
                               std::string& theError)
{
  // dummy XCAF Application to handle the STEP XCAF Document
  Handle(XCAFApp_Application) dummy_app = XCAFApp_Application::GetApplication();
  // XCAF Document to contain the STEP/IGES file itself
  Handle(TDocStd_Document) adoc;

  dummy_app->NewDocument( TCollection_ExtendedString("MDTV-CAF"), adoc);
  // transfer STEP/IGES into the document, and get the main label
  theReader.Transfer(adoc);
  TDF_Label mainLabel = adoc->Main();
  Handle_XCAFDoc_ShapeTool shapeTool = XCAFDoc_DocumentTool::ShapeTool(mainLabel);
  Handle_XCAFDoc_ColorTool colorTool = XCAFDoc_DocumentTool::ColorTool(mainLabel);
  Handle(XCAFDoc_MaterialTool) materialTool = XCAFDoc_DocumentTool::MaterialTool(mainLabel);
  // traverse the labels recursively to set attributes on shapes
  setShapeAttributes(shapeTool, colorTool, materialTool, mainLabel,
                     TopLoc_Location(),theResultBody,theMaterialShape,false);

  std::shared_ptr<GeomAPI_Shape> ageom =  setgeom(shapeTool,mainLabel,theError);

  STEPControl_Reader aReader = theReader.ChangeReader();

  // BEGIN: reading materials of sub-shapes from file
  if (theIsMaterials) {
    TopTools_IndexedMapOfShape anIndices;
    TopExp::MapShapes(ageom->impl<TopoDS_Shape>(), anIndices);

    Handle(Interface_InterfaceModel) Model = aReader.WS()->Model();
    Handle(XSControl_TransferReader) TR = aReader.WS()->TransferReader();
    if (!TR.IsNull()) {
      Handle(Transfer_TransientProcess) TP = TR->TransientProcess();

      Standard_Integer nb = Model->NbEntities();

      for (Standard_Integer ie = 1; ie <= nb; ie++) {
        Handle(Standard_Transient) enti = Model->Value(ie);

        // Store materials.
        storeMaterial(theResultBody,enti, anIndices, TP, mainLabel,theMaterialShape);
      }
    }
  }
  if (adoc->CanClose() == CDM_CCS_OK)
    adoc->Close();
  return ageom;
}

//=============================================================================
std::shared_ptr<GeomAPI_Shape> setgeom(const Handle(XCAFDoc_ShapeTool) &theShapeTool,
                                       const TDF_Label& /*theLabel*/,
                                       std::string& theError)
{
  BRep_Builder aB;
  TopoDS_Compound aCompound;
  aB.MakeCompound(aCompound);

  TDF_LabelSequence aFrshapes;
  theShapeTool->GetShapes(aFrshapes);

  std::shared_ptr<GeomAPI_Shape> aGeomShape(new GeomAPI_Shape);

  if (aFrshapes.Length() == 0) {
      aGeomShape->setImpl(new TopoDS_Shape());
      return aGeomShape;
  } else if (aFrshapes.Length() == 1) {
    TopoDS_Shape shape = theShapeTool->GetShape(aFrshapes.Value(1));
    aGeomShape->setImpl(new TopoDS_Shape(shape));
    return aGeomShape;
  } else {
    for (Standard_Integer i=1; i<aFrshapes.Length(); i++) {
      TopoDS_Shape aS = theShapeTool->GetShape(aFrshapes.Value(i));
      TDF_Label aLabel = theShapeTool->FindShape(aS, Standard_False);
      if ( (!aLabel.IsNull()) && (theShapeTool->IsShape(aLabel)) ) {
        if (theShapeTool->IsFree(aLabel) ) {
          if (aS.IsNull()) {
            continue;
          } else {
            if (!theShapeTool->IsReference(aLabel) ){
              for(TDF_ChildIterator anIt(aLabel); anIt.More(); anIt.Next()) {
                aB.Add(aCompound, theShapeTool->GetShape(anIt.Value()) );
              }
            } else {
              aB.Add(aCompound, aS);
            }
          }
        }
      }
    }

    TopoDS_Shape aShape = aCompound;
    // Check if any BRep entity has been read, there must be at least a vertex
    if (!TopExp_Explorer( aShape, TopAbs_VERTEX ).More()) {
      theError = "No geometrical data in the imported file.";
      aGeomShape->setImpl(new TopoDS_Shape());
      return aGeomShape;
    }

    aGeomShape->setImpl(new TopoDS_Shape(aShape));
    return aGeomShape;
  }
}
//=============================================================================
void setShapeAttributes(const Handle(XCAFDoc_ShapeTool) &theShapeTool,
                        const Handle(XCAFDoc_ColorTool) &theColorTool,
                        const Handle(XCAFDoc_MaterialTool) &theMaterialTool,
                        const TDF_Label &theLabel,
                        const TopLoc_Location &theLoc,
                        std::shared_ptr<ModelAPI_ResultBody> theResultBody,
                        std::map< std::wstring,std::list<std::wstring>> &theMaterialShape,
                        bool theIsRef)
{
  std::wstring aShapeName;
  Handle(TDataStd_Name) aN;

  if (theLabel.FindAttribute(TDataStd_Name::GetID(), aN)) {
    TCollection_ExtendedString aName = aN->Get();

    aShapeName =  Locale::Convert::toWString(TCollection_AsciiString(aName).ToCString()) ;
  }

  TopLoc_Location aPartLoc = theLoc;
  Handle(XCAFDoc_Location) al;
  if (theLabel.FindAttribute(XCAFDoc_Location::GetID(), al)) {
    if (theIsRef)
      aPartLoc = aPartLoc * al->Get();
    else
      aPartLoc = al->Get();
  }

  TDF_Label aRef;
  if (theShapeTool->IsReference(theLabel) && theShapeTool->GetReferredShape(theLabel, aRef)) {

    setShapeAttributes( theShapeTool, theColorTool, theMaterialTool, aRef,
                        aPartLoc,theResultBody,theMaterialShape,true);
  }

  if (theShapeTool->IsSimpleShape(theLabel) && (theIsRef || theShapeTool->IsFree(theLabel))) {

    TopoDS_Shape aShape = theShapeTool->GetShape(theLabel);

    std::shared_ptr<GeomAPI_Shape> aShapeGeom(new GeomAPI_Shape);
    if (!theLoc.IsIdentity()) {
        aShape.Move(theLoc);
    }
    aShapeGeom->setImpl(new TopoDS_Shape(aShape));
    aShapeName = theResultBody->addShapeName(aShapeGeom, aShapeName);


    aShape.Location(theIsRef ? theLoc : aPartLoc);
    int aDim =
      (aShape.ShapeType() == TopAbs_VERTEX) ?
        0 :
        (aShape.ShapeType() == TopAbs_EDGE || aShape.ShapeType() == TopAbs_WIRE) ?
        1 :
        (aShape.ShapeType() == TopAbs_FACE ||
         aShape.ShapeType() == TopAbs_SHELL) ? 2 :3;

    Handle(TCollection_HAsciiString) aMatName;
    Handle(TCollection_HAsciiString) aMatDescription;
    Standard_Real aMatDensity;
    Handle(TCollection_HAsciiString) aMatDensName;
    Handle(TCollection_HAsciiString) aMatDensValType;

    if (theMaterialTool->GetMaterial(theLabel, aMatName, aMatDescription, aMatDensity,
                                 aMatDensName, aMatDensValType)) {
      std::wstring aNameMaterial = Locale::Convert::toWString(aMatName->ToCString());

      theMaterialShape[aNameMaterial].push_back(aShapeName);
    }

    Quantity_Color aCol;
    if (theColorTool->GetColor(theLabel, XCAFDoc_ColorGen, aCol)) {
      double r = aCol.Red(), g = aCol.Green(), b = aCol.Blue();
      std::vector<int> ColRGB = {int(r*255),int(g*255),int(b*255)};
      theResultBody->addShapeColor(aShapeName, ColRGB);
    } else if (theColorTool->GetColor(theLabel, XCAFDoc_ColorSurf, aCol)) {
      double r = aCol.Red(), g = aCol.Green(), b = aCol.Blue();
      std::vector<int> aColRGB = {int(r*255),int(g*255),int(b*255)};
      theResultBody->addShapeColor(aShapeName, aColRGB);
    } else if (theColorTool->GetColor(theLabel, XCAFDoc_ColorCurv, aCol)) {
      double aR = aCol.Red(), aG = aCol.Green(), aB = aCol.Blue();
      std::vector<int> aColRGB = {int(aR*255),int(aG*255),int(aB*255)};
      theResultBody->addShapeColor(aShapeName, aColRGB);
    }
    // check explicit coloring of boundary entities
    if (aDim == 3) {
      TopExp_Explorer aXp2(aShape, TopAbs_FACE);
      while (aXp2.More()) {
        if (theColorTool->GetColor(aXp2.Current(), XCAFDoc_ColorGen, aCol) ||
          theColorTool->GetColor(aXp2.Current(), XCAFDoc_ColorSurf, aCol) ||
          theColorTool->GetColor(aXp2.Current(), XCAFDoc_ColorCurv, aCol)) {
          double aR = aCol.Red(), aG = aCol.Green(), aB = aCol.Blue();
          TopoDS_Face aFace = TopoDS::Face(aXp2.Current());
          std::vector<int> aColRGB = {int(aR*255),int(aG*255),int(aB*255)};
          std::wstringstream aNameFace;
          TopoDS_Shape aShapeface = aXp2.Current();
          if (!theLoc.IsIdentity()){
                  aShapeface.Move(theLoc);
          }
          aShapeGeom->setImpl(new TopoDS_Shape(aShapeface));
          theResultBody->addShapeColor(
          theResultBody->addShapeName(aShapeGeom , aNameFace.str()), aColRGB);
        }
        aXp2.Next();
      }
    }
  } else {
    if (!theShapeTool->IsReference(theLabel) ){
      TopoDS_Shape aShape = theShapeTool->GetShape(theLabel);

      std::shared_ptr<GeomAPI_Shape> aShapeGeom(new GeomAPI_Shape);
      if (!theLoc.IsIdentity()){
          aShape.Move(theLoc);
      }
      aShapeGeom->setImpl(new TopoDS_Shape(aShape));
      aShapeName = theResultBody->addShapeName(aShapeGeom, aShapeName);
    }
    for(TDF_ChildIterator anIt(theLabel); anIt.More(); anIt.Next()) {

      setShapeAttributes( theShapeTool, theColorTool, theMaterialTool,
                         anIt.Value(), aPartLoc,theResultBody,theMaterialShape, theIsRef);
    }
  }
}

//=============================================================================
void storeMaterial( std::shared_ptr<ModelAPI_ResultBody>    theResultBody,
                    const Handle(Standard_Transient)        &theEnti,
                    const TopTools_IndexedMapOfShape        &theIndices,
                    const Handle(Transfer_TransientProcess) &theTP,
                    const TDF_Label                         &/*theShapeLabel*/,
                    std::map< std::wstring, std::list<std::wstring>> &theMaterialShape )
{
  // Treat Product Definition Shape only.
  Handle(StepRepr_ProductDefinitionShape) aPDS =
    Handle(StepRepr_ProductDefinitionShape)::DownCast(theEnti);
  Handle(StepBasic_ProductDefinition)     aProdDef;

  if (!aPDS.IsNull()) {
    // Product Definition Shape ==> Product Definition
    aProdDef = aPDS->Definition().ProductDefinition();
  }

  if (!aProdDef.IsNull()) {
    // Product Definition ==> Property Definition
    const Interface_Graph    &aGraph = theTP->Graph();
    Interface_EntityIterator  aSubs  = aGraph.Sharings(aProdDef);
    TopoDS_Shape              aShape;

    for(aSubs.Start(); aSubs.More(); aSubs.Next()) {
      Handle(StepRepr_PropertyDefinition) aPropD =
        Handle(StepRepr_PropertyDefinition)::DownCast(aSubs.Value());

      if (!aPropD.IsNull()) {
        // Property Definition ==> Representation.
        Interface_EntityIterator aSubs1 = aGraph.Sharings(aPropD);

        for(aSubs1.Start(); aSubs1.More(); aSubs1.Next()) {
          Handle(StepRepr_PropertyDefinitionRepresentation) aPDR =
            Handle(StepRepr_PropertyDefinitionRepresentation)::
            DownCast(aSubs1.Value());

          if (!aPDR.IsNull()) {
            // Property Definition ==> Material Name.
            Handle(StepRepr_Representation) aRepr = aPDR->UsedRepresentation();

            if (!aRepr.IsNull()) {
              Standard_Integer anIr;

              for(anIr = 1; anIr <= aRepr->NbItems(); anIr++) {
                Handle(StepRepr_RepresentationItem) aRI = aRepr->ItemsValue(anIr);
                Handle(StepRepr_DescriptiveRepresentationItem) aDRI =
                  Handle(StepRepr_DescriptiveRepresentationItem)::DownCast(aRI);

                if (!aDRI.IsNull()) {
                  // Get shape from Product Definition
                  Handle(TCollection_HAsciiString) aMatName = aDRI->Name();
                  if (!aMatName.IsNull()) {
                    TCollection_ExtendedString
                      aMatNameExt (aMatName->ToCString());

                    if (aShape.IsNull()) {
                      //Get the shape.
                      aShape = getShape(aProdDef, theTP);
                      if (aShape.IsNull()) {
                        return;
                      }
                    }

                    // as PRODUCT can be included in the main shape
                    // several times, we look here for all iclusions.
                    Standard_Integer anISub, aNbSubs = theIndices.Extent();

                    for (anISub = 1; anISub <= aNbSubs; anISub++) {
                      TopoDS_Shape aSub = theIndices.FindKey(anISub);

                      if (aSub.IsPartner(aShape)) {
                        std::shared_ptr<GeomAPI_Shape> aShapeGeom(new GeomAPI_Shape);
                        aShapeGeom->setImpl(new TopoDS_Shape(aSub));
                        std::wstring aNom = theResultBody->findShapeName(aShapeGeom);
                        std::wstring aMName= Locale::Convert::toWString(aMatName->ToCString());
                        theMaterialShape[aMName].push_back(aNom);

                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
}

