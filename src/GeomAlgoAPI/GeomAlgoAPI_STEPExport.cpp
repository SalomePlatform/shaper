// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

#include <GeomAlgoAPI_STEPExport.h>

#include "GeomAlgoAPI_Tools.h"

#include <TopoDS_Shape.hxx>

// OOCT includes
#include <IFSelect_ReturnStatus.hxx>
#include <STEPControl_Writer.hxx>
#include <Interface_Static.hxx>

bool STEPExport(const std::string& theFileName,
                const std::string& theFormatName,
                const std::shared_ptr<GeomAPI_Shape>& theShape,
                std::string& theError)
{
  #ifdef _DEBUG
  std::cout << "Export STEP into file " << theFileName << std::endl;
  #endif

  if (!theShape.get()) {
    theError = "STEP Export failed: An invalid argument";
    return false;
  }

  try
  {
    // Set "C" numeric locale to save numbers correctly
    GeomAlgoAPI_Tools::Localizer loc;

    IFSelect_ReturnStatus status ;
    //VRV: OCC 4.0 migration
    STEPControl_Writer aWriter;
    Interface_Static::SetCVal("xstep.cascade.unit","M");
    Interface_Static::SetCVal("write.step.unit", "M");
    Interface_Static::SetIVal("write.step.nonmanifold", 1);
    status = aWriter.Transfer(theShape->impl<TopoDS_Shape>(), STEPControl_AsIs);
    //VRV: OCC 4.0 migration
    if( status == IFSelect_RetDone )
      status = aWriter.Write(theFileName.c_str());

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
