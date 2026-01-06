// Copyright (C) 2014-2026  CEA, EDF
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

#include <GeomAlgoAPI_glTFExport.h>

#include "GeomAlgoAPI_Tools.h"

#include <ModelAPI_ResultBody.h>

#include <TColStd_IndexedDataMapOfStringString.hxx>
#include <RWGltf_CafWriter.hxx>
#include <Message_ProgressRange.hxx>
#include <TDocStd_Document.hxx>
#include <TDataStd_Name.hxx>

#include <TopoDS_Shape.hxx>

bool GLTFExport(const std::string& theFileName,
  const std::list<std::shared_ptr<GeomAPI_Shape> >& theShapes,
  const std::list<std::shared_ptr<ModelAPI_Result> >& theResults,
  bool theIsBin,
  std::string& theError)
{
  theError = "";
  // prepare XCAF document to store the whole data structure there
  Handle(TDocStd_Document) aDoc = new TDocStd_Document("BinXCAF");

  GeomAlgoAPI_Tools::AttributeExport_Tools::GeomAlgoAPI_Attributes anAttrs(aDoc->Main());

  std::list<std::shared_ptr<GeomAPI_Shape> >::const_iterator aShape = theShapes.cbegin();
  std::list<std::shared_ptr<ModelAPI_Result> >::const_iterator aResult = theResults.cbegin();
  for (; aShape != theShapes.cend(); aShape++, aResult++) {
    TDF_Label aNullLab;
    if (aResult->get() && !(*aShape)->isSame((*aResult)->shape())) { // simple sub-shape
      GeomAlgoAPI_Tools::AttributeExport_Tools::getAttributes(*aResult, anAttrs);
      GeomAlgoAPI_Tools::AttributeExport_Tools::exportShape(*aShape, anAttrs, GeomShapePtr(), aNullLab);
    } else if (aResult->get()) { // whole result selection
      GeomAlgoAPI_Tools::AttributeExport_Tools::putResult(*aResult, GeomShapePtr(), aNullLab, anAttrs);
    }
    else { // feature selection => export simple shape
      GeomAlgoAPI_Tools::AttributeExport_Tools::exportShape(*aShape, anAttrs, GeomShapePtr(), aNullLab);
    }
  }

  TColStd_IndexedDataMapOfStringString aMetadata;
  RWGltf_CafWriter aWriter(theFileName.c_str(), theIsBin);
  aWriter.SetTransformationFormat(RWGltf_WriterTrsfFormat_Compact);
  aWriter.ChangeCoordinateSystemConverter().SetInputLengthUnit(0.001);
  aWriter.ChangeCoordinateSystemConverter().SetInputCoordinateSystem(RWMesh_CoordinateSystem_Zup);
  Standard_Boolean ret = aWriter.Perform(aDoc, aMetadata, Message_ProgressRange());
  if (!ret) {
    theError = "Failed to export to " + theFileName;
  }
  return theError.empty();
}
