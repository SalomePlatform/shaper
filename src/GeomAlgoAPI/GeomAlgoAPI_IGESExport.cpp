// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    GEOMALGOAPI_IGESExport.cpp
// Created: Dec 24, 2014
// Author:  Sergey BELASH

#include <GeomAlgoAPI_IGESExport.h>

#include "GeomAlgoAPI_Tools.h"

// OOCT includes
#include <IGESControl_Controller.hxx>
#include <IGESControl_Writer.hxx>
#include <Interface_Static.hxx>

#include <TopoDS_Shape.hxx>
#include <TopoDS_Iterator.hxx>

//=============================================================================
/*!
 *
 */
//=============================================================================
/*!
 *  KindOfBRep
 *  \return 0 if theShape contains only simple entities (wires, edges and vertices),
 *          1 if theShape contains only complex entities (shells, solids and compsolids)
 *          2 if theShape contains only indifferent entities (faces)
 *         -1 if theShape contains both simple and complex entities (and in this case it
 *            cannot be saved without any loss neither in BRepMode == 0 nor in BRepMode == 1)
 */
//=============================================================================
int KindOfBRep (const TopoDS_Shape& theShape)
{
  int aKind = 2;

  switch (theShape.ShapeType())
  {
  case TopAbs_COMPOUND:
    {
      bool isSimple = false;
      bool isComplex = false;
      TopoDS_Iterator anIt (theShape, Standard_True, Standard_True);
      for (; anIt.More(); anIt.Next()) {
        TopoDS_Shape aS = anIt.Value();
        int aKindSub = KindOfBRep(aS);
        if (aKindSub == 0)
          isSimple = true;
        else if (aKindSub == 1)
          isComplex = true;
        else if (aKindSub == -1) {
          return -1; // heterogeneous
        }
      }
      if (isSimple && isComplex)
        aKind = -1; // heterogeneous
      else if (isSimple)
        aKind = 0;
      else if (isComplex)
        aKind = 1;
    }
    break;
  case TopAbs_COMPSOLID:
  case TopAbs_SOLID:
  case TopAbs_SHELL:
    aKind = 1;
    break;
  case TopAbs_WIRE:
  case TopAbs_EDGE:
  case TopAbs_VERTEX:
    aKind = 0;
    break;
  default:
    aKind = 2;
  }

  return aKind;
}

//=============================================================================
bool IGESExport(const std::string& theFileName,
                const std::string& theFormatName,
                const TopoDS_Shape& theShape,
                std::string& theError)
{
  // theFormatName expected "IGES-5.1", "IGES-5.3"...
  TCollection_AsciiString aFormatName(theFormatName.c_str());
  TCollection_AsciiString aVersion = aFormatName.Token("-", 2);
  #ifdef _DEBUG
  if (!aVersion.IsEqual("5.1") || !aVersion.IsEqual("5.3"))
    std::cout << "Warning: unrecognized version " << aVersion.ToCString()
              << ". Default version: 5.1." << std::endl;
  #endif
  // define, whether to write only faces (5.1 IGES format)
  // or shells and solids also (5.3 IGES format)
  int aBrepMode = 0;
  if( aVersion.IsEqual( "5.3" ) )
    aBrepMode = 1;

  #ifdef _DEBUG
  std::cout << "Export IGES into file " << theFileName.ToCString() << std::endl;
  #endif

  // Mantis issue 0021350: check being exported shape, as some stand-alone
  // entities (edges, wires and vertices) cannot be saved in BRepMode
  if( aBrepMode == 1 ) {
    int aKind = KindOfBRep( theShape );
    if( aKind == -1 ) {
      theError = "EXPORT_IGES_HETEROGENEOUS_COMPOUND";
      return false;
    } else if( aKind == 2 )
      aBrepMode = 1;
    else
      aBrepMode = aKind;
  }

  // Set "C" numeric locale to save numbers correctly
  GeomAlgoAPI_Tools::Localizer loc;

  // initialize writer
  IGESControl_Controller::Init();
  IGESControl_Writer ICW( "M", aBrepMode ); // export explicitly in meters
  Interface_Static::SetCVal( "xstep.cascade.unit", "M" );

  // 09.03.2010 skl for bug 0020726
  // change default value "Average" to "Max"
  Interface_Static::SetCVal( "write.precision.mode", "Max" );

  // perform shape writing
  if( ICW.AddShape( theShape ) ) {
    ICW.ComputeModel();
    return ICW.Write(theFileName.c_str());
  }
  return false;
}
