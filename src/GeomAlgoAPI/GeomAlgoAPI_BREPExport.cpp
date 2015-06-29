// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    GEOMALGOAPI_BREPExport.cpp
// Created: May 14, 2015
// Author:  Sergey POKHODENKO

#include <GeomAlgoAPI_BREPExport.h>

#include "GeomAlgoAPI_Tools.h"

#include <BRepTools.hxx>
#include <BRep_Builder.hxx>

//=============================================================================
/*!
 *
 */
//=============================================================================
namespace BREPExport {

bool Export(const std::string& theFileName,
            const std::string&,
            const TopoDS_Shape& theShape,
            std::string& theError)
{
  #ifdef _DEBUG
  std::cout << "Export BREP into file " << theFileName << std::endl;
  #endif

  // Set "C" numeric locale to save numbers correctly
  GeomAlgoAPI_Tools::Localizer loc;

  if (!BRepTools::Write(theShape, theFileName.c_str())) {
    theError = "BREP Export failed";
    return false;
  }
  return true;
}

} // namespace BREPExport
