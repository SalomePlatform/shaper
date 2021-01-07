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

#ifndef GEOMALGOAPI_STEPIMPORTXCAF_H_
#define GEOMALGOAPI_STEPIMPORTXCAF_H_

#include <GeomAlgoAPI.h>

#include <XCAFDoc_ColorTool.hxx>
#include <XCAFDoc_MaterialTool.hxx>

#include <STEPCAFControl_Reader.hxx>

#include <ModelAPI_ResultBody.h>
#include <TopTools_IndexedMapOfShape.hxx>

 /// read Attributs of step file
 GEOMALGOAPI_EXPORT
 std::shared_ptr<GeomAPI_Shape>  readAttributes(STEPCAFControl_Reader &theReader,
                                 std::shared_ptr<ModelAPI_ResultBody> theResultBody,
                                 const bool  theMaterials,
                                 std::map< std::wstring,std::list<std::wstring>> &theMaterialShape,
                                 std::string& theError);
/// read attributs for  label
 GEOMALGOAPI_EXPORT
 void setShapeAttributes(const Handle(XCAFDoc_ShapeTool) &theShapeTool,
                         const Handle(XCAFDoc_ColorTool) &theColorTool,
                         const Handle(XCAFDoc_MaterialTool) &TheMaterialTool,
                         const TDF_Label &theLabel,
                         const TopLoc_Location &theLoc,
                         std::shared_ptr<ModelAPI_ResultBody> theResultBody,
                         std::map< std::wstring, std::list<std::wstring>> &theMaterialShape,
                         bool theIsRef);

// read geometry
GEOMALGOAPI_EXPORT
std::shared_ptr<GeomAPI_Shape> setgeom(const Handle(XCAFDoc_ShapeTool) &shapeTool,
                                       const TDF_Label &theLabel,
                                       std::string& theError);

// store Materiel for theShapeLabel in the map theMaterialShape
GEOMALGOAPI_EXPORT
void storeMaterial(std::shared_ptr<ModelAPI_ResultBody> theResultBody,
                   const Handle(Standard_Transient) &theEnti,
                   const TopTools_IndexedMapOfShape &theIndices,
                   const Handle(Transfer_TransientProcess) &theTP,
                   const TDF_Label &theShapeLabel,
                   std::map< std::wstring, std::list<std::wstring>> &theMaterialShape);

#endif /* GEOMALGOAPI_STEPIMPORTXCAF_H_ */
