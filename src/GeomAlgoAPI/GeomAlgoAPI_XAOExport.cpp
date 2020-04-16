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

#include <GeomAlgoAPI_XAOExport.h>

#include "GeomAlgoAPI_Tools.h"

#include <TopoDS_Shape.hxx>

#include <XAO_XaoExporter.hxx>
#include <XAO_BrepGeometry.hxx>

//=============================================================================
bool SetShapeToXAO(const std::shared_ptr<GeomAPI_Shape>& theShape,
                   XAO::Xao* theXao,
                   std::string& theError)
{
  if (!theShape.get() || !theXao) {
    theError = "An invalid argument.";
    return false;
  }

  TopoDS_Shape aShape = theShape->impl<TopoDS_Shape>();
  try {
    XAO::BrepGeometry* aGeometry = new XAO::BrepGeometry;
    theXao->setGeometry(aGeometry);
    aGeometry->setTopoDS_Shape(aShape);
  } catch (XAO::XAO_Exception& e) {
    theError = e.what();
    return false;
  }
  return true;
}

//=============================================================================
/*!
 *
 */
//=============================================================================
bool XAOExport(const std::string& theFileName,
               XAO::Xao* theXao,
               std::string& theError)
{
  #ifdef _DEBUG
  std::cout << "Export XAO into file " << theFileName << std::endl;
  #endif

  if (theFileName.empty() || !theXao) {
    theError = "An invalid argument.";
    return false;
  }

  try {
    XAO::BrepGeometry* aGeometry = dynamic_cast<XAO::BrepGeometry*>(theXao->getGeometry());
    TopoDS_Shape aShape = aGeometry->getTopoDS_Shape();
    bool aWasFree = aShape.Free(); // make top level topology free, same as imported
    if (!aWasFree)
      aShape.Free(Standard_True);
    XAO::XaoExporter::saveToFile(theXao, theFileName, "");
    if (!aWasFree)
      aShape.Free(Standard_False);
  } catch (XAO::XAO_Exception& e) {
    theError = e.what();
    return false;
  }
  return true;
}
