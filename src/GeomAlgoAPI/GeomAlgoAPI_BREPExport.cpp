// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    GEOMALGOAPI_BREPExport.cpp
// Created: May 14, 2015
// Author:  Sergey POKHODENKO

#include <GeomAlgoAPI_BREPExport.h>

#include "GeomAlgoAPI_Tools.h"

#include <TopoDS_Shape.hxx>

#include <BRepTools.hxx>
#include <BRep_Builder.hxx>

//=============================================================================
/*!
 *
 */
//=============================================================================
bool BREPExport(const std::string& theFileName,
                const std::string&,
                const std::shared_ptr<GeomAPI_Shape>& theShape,
                std::string& theError)
{
  #ifdef _DEBUG
  std::cout << "Export BREP into file " << theFileName << std::endl;
  #endif

  if (!theShape.get()) {
    theError = "BREP Export failed: An invalid argument";
    return false;
  }

  // Set "C" numeric locale to save numbers correctly
  GeomAlgoAPI_Tools::Localizer loc;

  if (!BRepTools::Write(theShape->impl<TopoDS_Shape>(), theFileName.c_str())) {
    theError = "BREP Export failed";
    return false;
  }
  return true;
}
