// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#include <GeomAlgoAPI_BREPExport.h>

#include <BRepTools.hxx>
#include <BRep_Builder.hxx>

//=============================================================================
/*!
 *
 */
//=============================================================================
//extern "C" {
namespace BREPExport {
bool Export(const TCollection_AsciiString& theFileName,
            const TCollection_AsciiString&,
            const TopoDS_Shape& theShape,
            TCollection_AsciiString& theError, const TDF_Label&)
{
  #ifdef _DEBUG
  std::cout << "Export BREP into file " << theFileName << std::endl;
  #endif
  if ( !BRepTools::Write( theShape, theFileName.ToCString() ) ) {
    theError = "BREP Export failed";
    return false;
  }
  return true;
}

}
//}
