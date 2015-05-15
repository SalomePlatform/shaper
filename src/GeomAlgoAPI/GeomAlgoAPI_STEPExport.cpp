// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#include <GeomAlgoAPI_STEPExport.h>

// OOCT includes
#include <IFSelect_ReturnStatus.hxx>
#include <STEPControl_Writer.hxx>
#include <Interface_Static.hxx>

namespace STEPExport {

bool Export(const TCollection_AsciiString& theFileName,
            const TCollection_AsciiString& theFormatName,
            const TopoDS_Shape& theShape,
            TCollection_AsciiString& theError,
            const TDF_Label& theShapeLabel)
{
  #ifdef _DEBUG
  std::cout << "Export STEP into file " << theFileName.ToCString() << std::endl;
  #endif

  try
  {
//    // Set "C" numeric locale to save numbers correctly
//    Kernel_Utils::Localizer loc;

    IFSelect_ReturnStatus status ;
    //VRV: OCC 4.0 migration
    STEPControl_Writer aWriter;
    Interface_Static::SetCVal("xstep.cascade.unit","M");
    Interface_Static::SetCVal("write.step.unit", "M");
    Interface_Static::SetIVal("write.step.nonmanifold", 1);
    status = aWriter.Transfer( theShape, STEPControl_AsIs );
    //VRV: OCC 4.0 migration
    if( status == IFSelect_RetDone )
      status = aWriter.Write( theFileName.ToCString() );

    // Return previous locale
    if( status == IFSelect_RetDone )
      return true;
  }
  catch (Standard_Failure)
  {
    theError = "Exception catched in STEPExport";
  }
  return false;
}

} // namespace STEPExport
