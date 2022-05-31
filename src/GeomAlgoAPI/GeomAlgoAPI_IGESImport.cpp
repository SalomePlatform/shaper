// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

#include <GeomAlgoAPI_IGESImport.h>

#include <TopoDS_Shape.hxx>

// OOCT includes
#include <IGESControl_Reader.hxx>
#include <IGESData_IGESModel.hxx>

//=============================================================================
/*!
 *
 */
//=============================================================================
std::shared_ptr<GeomAPI_Shape> IGESImport(const std::string& theFileName,
                                          const std::string&,
                                          std::string& theError)
{
  #ifdef _DEBUG
  std::cout << "Import IGES from file " << theFileName << std::endl;
  #endif
  TopoDS_Shape aShape;
  IGESControl_Reader aReader;
  try {
    IFSelect_ReturnStatus status = aReader.ReadFile( theFileName.c_str() );

    if (status == IFSelect_RetDone) {
      #ifdef _DEBUG
      std::cout << "ImportIGES : all Geometry Transfer" << std::endl;
      #endif
      aReader.ClearShapes();
      aReader.TransferRoots();

      #ifdef _DEBUG
      std::cout << "ImportIGES : count of shapes produced = " << aReader.NbShapes() << std::endl;
      #endif
      aShape = aReader.OneShape();
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
      aShape.Nullify();
    }
  }
  catch( Standard_Failure const& anException) {
    theError = anException.GetMessageString();
    aShape.Nullify();
  }

  std::shared_ptr<GeomAPI_Shape> aGeomShape(new GeomAPI_Shape);
  aGeomShape->setImpl(new TopoDS_Shape(aShape));
  return aGeomShape;
}
