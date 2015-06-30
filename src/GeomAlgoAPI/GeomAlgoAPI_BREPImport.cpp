// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    GeomAlgoAPI_BREPImport.cpp
// Created: May 14, 2015
// Author:  Sergey POKHODENKO

#include <GeomAlgoAPI_BREPImport.h>

#include <BRepTools.hxx>
#include <BRep_Builder.hxx>

//=============================================================================
/*!
 *
 */
//=============================================================================
TopoDS_Shape BREPImport(const std::string& theFileName,
                        const std::string&,
                        std::string& theError)
{
  #ifdef _DEBUG
  std::cout << "Import BREP from file " << theFileName << std::endl;
  #endif
  TopoDS_Shape aShape;
  BRep_Builder aBuilder;
  BRepTools::Read(aShape, theFileName.c_str(), aBuilder);
  if (aShape.IsNull()) {
    theError = "BREP Import failed";
  }
  return aShape;
}
