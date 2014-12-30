// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#include <GeomAlgoAPI_STEPImport.h>

#include <TDF_ChildIDIterator.hxx>
#include <TDF_Label.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_Comment.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_NamedShape.hxx>

#include <IFSelect_ReturnStatus.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Graph.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Static.hxx>
#include <STEPControl_Reader.hxx>
#include <StepBasic_Product.hxx>
#include <StepBasic_ProductDefinition.hxx>
#include <StepBasic_ProductDefinitionFormation.hxx>
#include <StepGeom_GeometricRepresentationItem.hxx>
#include <StepShape_TopologicalRepresentationItem.hxx>
#include <StepRepr_DescriptiveRepresentationItem.hxx>
#include <StepRepr_ProductDefinitionShape.hxx>
#include <StepRepr_PropertyDefinitionRepresentation.hxx>
#include <StepRepr_Representation.hxx>
#include <TransferBRep.hxx>
#include <Transfer_Binder.hxx>
#include <Transfer_TransientProcess.hxx>
#include <XSControl_TransferReader.hxx>
#include <XSControl_WorkSession.hxx>

#include <BRep_Builder.hxx>

#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Iterator.hxx>

#include <TColStd_SequenceOfAsciiString.hxx>

#include <Standard_Failure.hxx>
#include <Standard_ErrorHandler.hxx> // CAREFUL ! position of this file is critic : see Lucien PIGNOLONI / OCC

namespace STEPImport {
//=============================================================================
/*!
 *  GetShape()
 */
//=============================================================================

static TopoDS_Shape GetShape(const Handle(Standard_Transient)        &theEnti,
                             const Handle(Transfer_TransientProcess) &theTP)
{
  TopoDS_Shape            aResult;
  Handle(Transfer_Binder) aBinder = theTP->Find(theEnti);

  if (aBinder.IsNull()) {
    return aResult;
  }

  aResult = TransferBRep::ShapeResult(aBinder);

  return aResult;
}

//=============================================================================
/*!
 *  GetLabel()
 */
//=============================================================================

static TDF_Label GetLabel(const Handle(Standard_Transient) &theEnti,
                          const TDF_Label                  &theShapeLabel,
                          const TopoDS_Shape               &aShape)
{
  TDF_Label aResult;

  if (theEnti->IsKind
            (STANDARD_TYPE(StepGeom_GeometricRepresentationItem))) {
    // check all named shapes using iterator
    TDF_ChildIDIterator anIt
      (theShapeLabel, TDataStd_Name::GetID(), Standard_True);

    for (; anIt.More(); anIt.Next()) {
      Handle(TDataStd_Name) nameAttr =
        Handle(TDataStd_Name)::DownCast(anIt.Value());

      if (nameAttr.IsNull()) {
        continue;
      }

      TDF_Label aLab = nameAttr->Label();
      Handle(TNaming_NamedShape) shAttr; 

      if (aLab.FindAttribute(TNaming_NamedShape::GetID(), shAttr) &&
          shAttr->Get().IsEqual(aShape)) {
        aResult = aLab;
      }
    }
  }

  // create label and set shape
  if (aResult.IsNull()) {
    TDF_TagSource aTag;

    aResult = aTag.NewChild(theShapeLabel);

    TNaming_Builder tnBuild (aResult);

    tnBuild.Generated(aShape);
  }

  return aResult;
}

//=============================================================================
/*!
 *  StoreName()
 */
//=============================================================================

static void StoreName(const Handle(Standard_Transient)        &theEnti,
                      const TopTools_IndexedMapOfShape        &theIndices,
                      const Handle(Transfer_TransientProcess) &theTP,
                      const TDF_Label                         &theShapeLabel)
{
  Handle(TCollection_HAsciiString) aName;

  if (theEnti->IsKind(STANDARD_TYPE(StepShape_TopologicalRepresentationItem)) ||
      theEnti->IsKind(STANDARD_TYPE(StepGeom_GeometricRepresentationItem))) {
    aName = Handle(StepRepr_RepresentationItem)::DownCast(theEnti)->Name();
  } else {
    Handle(StepBasic_ProductDefinition) PD =
      Handle(StepBasic_ProductDefinition)::DownCast(theEnti);

    if (PD.IsNull() == Standard_False) {
      Handle(StepBasic_Product) Prod = PD->Formation()->OfProduct();
      aName = Prod->Name();
    }
  }

  bool isValidName = false;

  if (aName.IsNull() == Standard_False) {
    isValidName = true;

    if (aName->UsefullLength() < 1) {
      isValidName = false;
    } else if (aName->UsefullLength() == 4 &&
               toupper (aName->Value(1)) == 'N' &&
               toupper (aName->Value(2)) == 'O' &&
               toupper (aName->Value(3)) == 'N' &&
               toupper (aName->Value(4)) == 'E') {
      // skip 'N0NE' name
      isValidName = false;
    } else {
      // special check to pass names like "Open CASCADE STEP translator 6.3 1"
      TCollection_AsciiString aSkipName ("Open CASCADE STEP translator");

      if (aName->Length() >= aSkipName.Length()) {
        if (aName->String().SubString
                            (1, aSkipName.Length()).IsEqual(aSkipName)) {
          isValidName = false;
        }
      }
    }
  }

  if (isValidName) {
    TCollection_ExtendedString aNameExt (aName->ToCString());

    // find target shape
    TopoDS_Shape S = GetShape(theEnti, theTP);

    if (S.IsNull()) {
      return;
    }

    // as PRODUCT can be included in the main shape
    // several times, we look here for all iclusions.
    Standard_Integer isub, nbSubs = theIndices.Extent();

    for (isub = 1; isub <= nbSubs; isub++) {
      TopoDS_Shape aSub = theIndices.FindKey(isub);

      if (aSub.IsPartner(S)) {
        TDF_Label L = GetLabel(theEnti, theShapeLabel, aSub);

        // set a name
        TDataStd_Name::Set(L, aNameExt);
      }
    }
  }
}

//=============================================================================
/*!
 *  StoreMaterial()
 */
//=============================================================================

static void StoreMaterial
                    (const Handle(Standard_Transient)        &theEnti,
                     const TopTools_IndexedMapOfShape        &theIndices,
                     const Handle(Transfer_TransientProcess) &theTP,
                     const TDF_Label                         &theShapeLabel)
{
  // Treat Product Definition Shape only.
  Handle(StepRepr_ProductDefinitionShape) aPDS =
      Handle(StepRepr_ProductDefinitionShape)::DownCast(theEnti);
  Handle(StepBasic_ProductDefinition)     aProdDef;

  if(aPDS.IsNull() == Standard_False) {
    // Product Definition Shape ==> Product Definition
    aProdDef = aPDS->Definition().ProductDefinition();
  }

  if (aProdDef.IsNull() == Standard_False) {
    // Product Definition ==> Property Definition
    const Interface_Graph    &aGraph = theTP->Graph();
    Interface_EntityIterator  aSubs  = aGraph.Sharings(aProdDef);
    TopoDS_Shape              aShape;

    for(aSubs.Start(); aSubs.More(); aSubs.Next()) {
      Handle(StepRepr_PropertyDefinition) aPropD =
        Handle(StepRepr_PropertyDefinition)::DownCast(aSubs.Value());

      if(aPropD.IsNull() == Standard_False) {
        // Property Definition ==> Representation.
        Interface_EntityIterator aSubs1 = aGraph.Sharings(aPropD);

        for(aSubs1.Start(); aSubs1.More(); aSubs1.Next()) {
          Handle(StepRepr_PropertyDefinitionRepresentation) aPDR =
            Handle(StepRepr_PropertyDefinitionRepresentation)::
              DownCast(aSubs1.Value());

          if(aPDR.IsNull() == Standard_False) {
            // Property Definition ==> Material Name.
            Handle(StepRepr_Representation) aRepr = aPDR->UsedRepresentation();

            if(aRepr.IsNull() == Standard_False) {
              Standard_Integer ir;

              for(ir = 1; ir <= aRepr->NbItems(); ir++) {
                Handle(StepRepr_RepresentationItem) aRI = aRepr->ItemsValue(ir);
                Handle(StepRepr_DescriptiveRepresentationItem) aDRI =
                  Handle(StepRepr_DescriptiveRepresentationItem)::DownCast(aRI);

                if(aDRI.IsNull() == Standard_False) {
                  // Get shape from Product Definition
                  Handle(TCollection_HAsciiString) aMatName = aDRI->Name();

                  if(aMatName.IsNull() == Standard_False) {
                    TCollection_ExtendedString
                                 aMatNameExt (aMatName->ToCString());

                    if (aShape.IsNull()) {
                      // Get the shape.
                      aShape = GetShape(aProdDef, theTP);

                      if (aShape.IsNull()) {
                        return;
                      }
                    }

                    // as PRODUCT can be included in the main shape
                    // several times, we look here for all iclusions.
                    Standard_Integer isub, nbSubs = theIndices.Extent();

                    for (isub = 1; isub <= nbSubs; isub++) {
                      TopoDS_Shape aSub = theIndices.FindKey(isub);

                      if (aSub.IsPartner(aShape)) {
                        TDF_Label aLabel =
                          GetLabel(aProdDef, theShapeLabel, aSub);

                        // set a name
                        TDataStd_Comment::Set(aLabel, aMatNameExt);
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

//=============================================================================
/*!
 *  Import()
 */
//=============================================================================

Handle(TCollection_HAsciiString) GetValue (const TCollection_AsciiString& theFileName,
                                           const TCollection_AsciiString& theParameterName,
                                           TCollection_AsciiString&       theError)
{
  Handle(TCollection_HAsciiString) aValue;

  if (theParameterName != "LEN_UNITS") {
    theError = theParameterName + " parameter reading is not supported by STEP plugin";
    return aValue;
  }

  // Set "C" numeric locale to save numbers correctly
  // Kernel_Utils::Localizer loc;

  STEPControl_Reader aReader;

  Interface_Static::SetCVal("xstep.cascade.unit","M");
  Interface_Static::SetIVal("read.step.ideas", 1);
  Interface_Static::SetIVal("read.step.nonmanifold", 1);

  try {
#if OCC_VERSION_LARGE > 0x06010000
    OCC_CATCH_SIGNALS;
#endif
    IFSelect_ReturnStatus status = aReader.ReadFile(theFileName.ToCString());
    if (status == IFSelect_RetDone) {
      TColStd_SequenceOfAsciiString anUnitLengthNames;
      TColStd_SequenceOfAsciiString anUnitAngleNames;
      TColStd_SequenceOfAsciiString anUnitSolidAngleNames;
      aReader.FileUnits(anUnitLengthNames, anUnitAngleNames, anUnitSolidAngleNames);
      if (anUnitLengthNames.Length() > 0) {
        aValue = new TCollection_HAsciiString( anUnitLengthNames.First() );
        /*
        TCollection_AsciiString aLenUnits = anUnitLengthNames.First();
        if (aLenUnits == "millimetre")
          aValue = new TCollection_HAsciiString ("MM");
        else if (aLenUnits == "centimetre")
          aValue = new TCollection_HAsciiString ("CM");
        else if (aLenUnits == "metre")
          aValue = new TCollection_HAsciiString ("M");
        else if (aLenUnits == "INCH")
          aValue = new TCollection_HAsciiString ("INCH");
        // TODO (for other units than mm, cm, m or inch)
        //else if (aLenUnits == "")
        //  aValue = new TCollection_HAsciiString ("");

        // tmp begin
        //std::cout << "$$$ --- " << anUnitLengthNames.First();
        //for (int ii = 2; ii <= anUnitLengthNames.Length(); ii++)
        //  std::cout << ", " << anUnitLengthNames.Value(ii);
        //std::cout << std::endl;
        // tmp end
        */
      }
    }
    else {
      theError = theFileName + " reading failed";
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    theError = aFail->GetMessageString();
  }

  return aValue;
}

TopoDS_Shape Import (const TCollection_AsciiString& theFileName,
                     const TCollection_AsciiString& theFormatName,
                     TCollection_AsciiString&       theError,
                     const TDF_Label&               theShapeLabel)
{
  TopoDS_Shape aResShape;

  // Set "C" numeric locale to save numbers correctly
  // Kernel_Utils::Localizer loc;

  STEPControl_Reader aReader;

  //VSR: 16/09/09: Convert to METERS
  Interface_Static::SetCVal("xstep.cascade.unit","M");
  Interface_Static::SetIVal("read.step.ideas", 1);
  Interface_Static::SetIVal("read.step.nonmanifold", 1);

  BRep_Builder B;
  TopoDS_Compound compound;
  B.MakeCompound(compound);

  try {
    OCC_CATCH_SIGNALS;

    IFSelect_ReturnStatus status = aReader.ReadFile(theFileName.ToCString());

    if (status == IFSelect_RetDone) {

      // Regard or not the model units
      if (theFormatName == "STEP_SCALE") {
        // set UnitFlag to units from file
        TColStd_SequenceOfAsciiString anUnitLengthNames;
        TColStd_SequenceOfAsciiString anUnitAngleNames;
        TColStd_SequenceOfAsciiString anUnitSolidAngleNames;
        aReader.FileUnits(anUnitLengthNames, anUnitAngleNames, anUnitSolidAngleNames);
        if (anUnitLengthNames.Length() > 0) {
          TCollection_AsciiString aLenUnits = anUnitLengthNames.First();
          if (aLenUnits == "millimetre")
            Interface_Static::SetCVal("xstep.cascade.unit", "MM");
          else if (aLenUnits == "centimetre")
            Interface_Static::SetCVal("xstep.cascade.unit", "CM");
          else if (aLenUnits == "metre" || aLenUnits.IsEmpty())
            Interface_Static::SetCVal("xstep.cascade.unit", "M");
          else if (aLenUnits == "INCH")
            Interface_Static::SetCVal("xstep.cascade.unit", "INCH");
          else {
            theError = "The file contains not supported units.";
            return aResShape;
          }
          // TODO (for other units than mm, cm, m or inch)
          //else if (aLenUnits == "")
          //  Interface_Static::SetCVal("xstep.cascade.unit", "???");
        }
      }
      else {
        //cout<<"need re-scale a model"<<endl;
        // set UnitFlag to 'meter'
        Interface_Static::SetCVal("xstep.cascade.unit","M");
      }

      Standard_Boolean failsonly = Standard_False;
      aReader.PrintCheckLoad(failsonly, IFSelect_ItemsByEntity);

      /* Root transfers */
      Standard_Integer nbr = aReader.NbRootsForTransfer();
      aReader.PrintCheckTransfer(failsonly, IFSelect_ItemsByEntity);

      for (Standard_Integer n = 1; n <= nbr; n++) {
        Standard_Boolean ok = aReader.TransferRoot(n);
        /* Collecting resulting entities */
        Standard_Integer nbs = aReader.NbShapes();
        if (!ok || nbs == 0)
        {
          // THROW_SALOME_CORBA_EXCEPTION("Exception catched in GEOM_Gen_i::ImportStep", SALOME::BAD_PARAM);
          continue; // skip empty root
        }
        /* For a single entity */
        else if (nbr == 1 && nbs == 1) {
          aResShape = aReader.Shape(1);
          // ATTENTION: this is a workaround for mantis issue 0020442 remark 0010776
          // It should be removed after patching OCCT for bug OCC22436
          // (fix for OCCT is expected in service pack next to OCCT6.3sp12)
          if (aResShape.ShapeType() == TopAbs_COMPOUND) {
            int nbSub1 = 0;
            TopoDS_Shape currShape;
            TopoDS_Iterator It (aResShape, Standard_True, Standard_True);
            for (; It.More(); It.Next()) {
              nbSub1++;
              currShape = It.Value();
            }
            if (nbSub1 == 1)
              aResShape = currShape;
          }
          // END workaround
          break;
        }

        for (Standard_Integer i = 1; i <= nbs; i++) {
          TopoDS_Shape aShape = aReader.Shape(i);
          if (aShape.IsNull()) {
            // THROW_SALOME_CORBA_EXCEPTION("Null shape in GEOM_Gen_i::ImportStep", SALOME::BAD_PARAM) ;
            //return aResShape;
            continue;
          }
          else {
            B.Add(compound, aShape);
          }
        }
      }
      if (aResShape.IsNull())
        aResShape = compound;

      // Check if any BRep entity has been read, there must be at least a vertex
      if ( !TopExp_Explorer( aResShape, TopAbs_VERTEX ).More() )
      {
        theError = "No geometrical data in the imported file.";
        return TopoDS_Shape();
      }
    } else {
      theError = "Wrong format of the imported file. Can't import file.";
      aResShape.Nullify();
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    theError = aFail->GetMessageString();
    aResShape.Nullify();
  }
  // Return previous locale
  return aResShape;
}

} // namespace STEPImport
