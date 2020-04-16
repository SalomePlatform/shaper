// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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
