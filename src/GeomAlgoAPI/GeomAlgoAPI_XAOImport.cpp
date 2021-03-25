// Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

#include <GeomAlgoAPI_XAOImport.h>

#include <TopoDS_Shape.hxx>

#include <XAO_XaoExporter.hxx>
#include <XAO_BrepGeometry.hxx>

//=============================================================================
/*!
 *
 */
//=============================================================================
std::shared_ptr<GeomAPI_Shape> XAOImport(const std::string& theFileName,
                                         std::string& theError,
                                         XAO::Xao* theXao)
{
  #ifdef _DEBUG
  std::cout << "Import XAO from file " << theFileName << std::endl;
  #endif
  if (theFileName.empty() || !theXao) {
    theError = "An invalid argument.";
    return std::shared_ptr<GeomAPI_Shape>();
  }

  TopoDS_Shape aShape;
  try {
    if (XAO::XaoExporter::readFromFile(theFileName, theXao)) {
      XAO::Geometry* aGeometry = theXao->getGeometry();
      XAO::Format aFormat = aGeometry->getFormat();
      if (aFormat == XAO::BREP) {
        if (XAO::BrepGeometry* aBrepGeometry = dynamic_cast<XAO::BrepGeometry*>(aGeometry))
          aShape = aBrepGeometry->getTopoDS_Shape();
      } else {
        theError = "Unsupported XAO geometry format:" + XAO::XaoUtils::shapeFormatToString(aFormat);
        aShape.Nullify();
      }
    } else {
      theError = "XAO object was not read successful";
      aShape.Nullify();
    }
  } catch (XAO::XAO_Exception& e) {
    theError = e.what();
    aShape.Nullify();
  }

  std::shared_ptr<GeomAPI_Shape> aGeomShape(new GeomAPI_Shape);
  aGeomShape->setImpl(new TopoDS_Shape(aShape));
  return aGeomShape;
}
