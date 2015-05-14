// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#include <GeomAlgoAPI_IGESImport.h>

// OOCT includes
#include <IGESControl_Reader.hxx>
#include <IGESData_IGESModel.hxx>

//=============================================================================
/*!
 *
 */
//=============================================================================
//extern "C" {
namespace IGESImport {
TopoDS_Shape Import(const TCollection_AsciiString& theFileName,
                    const TCollection_AsciiString&,
                    TCollection_AsciiString& theError, const TDF_Label&)
{
  #ifdef _DEBUG
  std::cout << "Import IGES from file " << theFileName << std::endl;
  #endif
  TopoDS_Shape aResShape;
  IGESControl_Reader aReader;
  try {
    IFSelect_ReturnStatus status = aReader.ReadFile( theFileName.ToCString() );

    if (status == IFSelect_RetDone) {
      #ifdef _DEBUG
      std::cout << "ImportIGES : all Geometry Transfer" << std::endl;
      #endif
      aReader.ClearShapes();
      aReader.TransferRoots();

      #ifdef _DEBUG
      std::cout << "ImportIGES : count of shapes produced = " << aReader.NbShapes() << std::endl;
      #endif
      aResShape = aReader.OneShape();
    }
    else {
      switch (status) {
        case IFSelect_RetVoid:
          theError = "Nothing created or No data to process";
          break;
        case IFSelect_RetError:
          theError = "Error in command or input data";
          break;
        case IFSelect_RetFail:
          theError = "Execution was run, but has failed";
          break;
        case IFSelect_RetStop:
          theError = "Execution has been stopped. Quite possible, an exception was raised";
          break;
        default:
          theError = "Wrong format of the imported file. Can't import file.";
          break;
      }
      aResShape.Nullify();
    }
  }
  catch( Standard_Failure ) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    theError = aFail->GetMessageString();
    aResShape.Nullify();
  }

  return aResShape;
}

}
//}
