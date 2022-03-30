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

#include <GeomAlgoAPI_STEPImport.h>
#include <GeomAlgoAPI_STEPImportXCAF.h>

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

#include <STEPCAFControl_Reader.hxx>
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

#include <Standard_ErrorHandler.hxx> // CAREFUL ! position of this file is critic : see Lucien PIGNOLONI / OCC
#include <OSD_Exception.hxx>

//=================================================================================================
bool readUnits(STEPControl_Reader& aReader,
               const bool theScalInterUnits,
               std::string& theError)
{
  // Regard or not the model units
  if (!theScalInterUnits) {
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
        return false;
      }
      // TODO (for other units than mm, cm, m or inch)
      //else if (aLenUnits == "")
      //  Interface_Static::SetCVal("xstep.cascade.unit", "???");
    }
  }
  else {
    //cout<<"need re-scale a model"<<endl;
    // set UnitFlag to 'meter'
    Interface_Static::SetCVal("xstep.cascade.unit", "M");
  }
  return true;
}


//==================================================================================================
std::shared_ptr<GeomAPI_Shape> STEPImport(const std::string& theFileName,
                                          const std::string& /*theFormatName*/,
                                          const bool theScalInterUnits,
                                          std::string& theError)
{

  TopoDS_Shape aResShape;

  // Set "C" numeric locale to save numbers correctly
  // Kernel_Utils::Localizer loc;

  STEPControl_Reader aReader;

  //VSR: 16/09/09: Convert to METERS
  Interface_Static::SetCVal("xstep.cascade.unit", "M");
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
      if( !readUnits(aReader, theScalInterUnits, theError)) {
        std::shared_ptr<GeomAPI_Shape> aGeomShape(new GeomAPI_Shape);
        aGeomShape->setImpl(new TopoDS_Shape());
        return aGeomShape;
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
      if (!TopExp_Explorer(aResShape, TopAbs_VERTEX).More())
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
  catch (Standard_Failure const& anException) {
    theError = anException.GetMessageString();
    aResShape.Nullify();
  }
  // Return previous locale
  std::shared_ptr<GeomAPI_Shape> aGeomShape(new GeomAPI_Shape);
  aGeomShape->setImpl(new TopoDS_Shape(aResShape));
  return aGeomShape;
}

//==================================================================================================
GeomShapePtr STEPImportAttributs(const std::string& theFileName,
                                 std::shared_ptr<ModelAPI_ResultBody> theResultBody,
                                 const bool theScalInterUnits,
                                 const bool theMaterials,
                                 const bool /*theColor*/,
                                 std::map< std::wstring,
                                 std::list<std::wstring>>& theMaterialShape,
                                 std::string& theError)
{

  try {
    STEPControl_Reader aReader;
    std::shared_ptr<GeomAPI_Shape> aGeomShape(new GeomAPI_Shape);

    Interface_Static::SetCVal("xstep.cascade.unit", "M");
    Interface_Static::SetIVal("read.step.ideas", 1);
    Interface_Static::SetIVal("read.step.nonmanifold", 1);

    try {
      OCC_CATCH_SIGNALS;

      IFSelect_ReturnStatus status = aReader.ReadFile(theFileName.c_str());

      if (status == IFSelect_RetDone) {
        // Regard or not the model units
        if( !readUnits(aReader, theScalInterUnits, theError)) {
          aGeomShape->setImpl(new TopoDS_Shape());
          return aGeomShape;
        }
      }
    } catch (Standard_Failure const& anException) {
      theError = anException.GetMessageString();
      aGeomShape->setImpl(new TopoDS_Shape());
      return aGeomShape;
    }

    STEPCAFControl_Reader aCafreader;
    aCafreader.SetColorMode(true);
    aCafreader.SetNameMode(true);
    aCafreader.SetMatMode(true);

    if (aCafreader.ReadFile(theFileName.c_str()) != IFSelect_RetDone) {
      theError = "Wrong format of the imported file. Can't import file.";
      aGeomShape->setImpl(new TopoDS_Shape());
      return aGeomShape;
    }

    return readAttributes(aCafreader,
                          theResultBody,
                          theMaterials,
                          theMaterialShape,
                          theError);
  } catch (OSD_Exception&) {
    //Try to load STEP file without colors...
    return STEPImport(theFileName, "", theScalInterUnits, theError);
  }
}
