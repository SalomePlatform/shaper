// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#include <ExchangePlugin_BREPImport.h>

#include <BRepTools.hxx>
#include <BRep_Builder.hxx>

//=============================================================================
/*!
 *
 */
//=============================================================================
//extern "C" {
namespace BREPImport {
TopoDS_Shape Import(const TCollection_AsciiString& theFileName,
                    const TCollection_AsciiString&,
                    TCollection_AsciiString& theError, const TDF_Label&)
{
  #ifdef _DEBUG
  std::cout << "Import BREP from file " << theFileName << std::endl;
  #endif
  TopoDS_Shape aShape;
  BRep_Builder aBuilder;
  BRepTools::Read(aShape, theFileName.ToCString(), aBuilder);
  if (aShape.IsNull()) {
    theError = "BREP Import failed";
  }
  return aShape;
}

}
//}
