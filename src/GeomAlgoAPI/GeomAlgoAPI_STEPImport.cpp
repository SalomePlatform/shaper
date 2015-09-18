// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    GEOMALGOAPI_STEPImport.cpp
// Created: Dec 24, 2014
// Author:  Sergey BELASH

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
#include <TopoDS_Shape.hxx>


#include <TColStd_SequenceOfAsciiString.hxx>

#include <Standard_Failure.hxx>
#include <Standard_ErrorHandler.hxx> // CAREFUL ! position of this file is critic : see Lucien PIGNOLONI / OCC
std::shared_ptr<GeomAPI_Shape> STEPImport(const std::string& theFileName,
                                          const std::string& theFormatName,
                                          std::string& theError)
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

    IFSelect_ReturnStatus status = aReader.ReadFile(theFileName.c_str());

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
            std::shared_ptr<GeomAPI_Shape> aGeomShape(new GeomAPI_Shape);
            aGeomShape->setImpl(new TopoDS_Shape(aResShape));
            return aGeomShape;
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
        std::shared_ptr<GeomAPI_Shape> aGeomShape(new GeomAPI_Shape);
        aGeomShape->setImpl(new TopoDS_Shape());
        return aGeomShape;
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
  std::shared_ptr<GeomAPI_Shape> aGeomShape(new GeomAPI_Shape);
  aGeomShape->setImpl(new TopoDS_Shape(aResShape));
  return aGeomShape;
}
