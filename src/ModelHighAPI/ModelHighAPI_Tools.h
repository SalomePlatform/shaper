// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#ifndef SRC_MODELHIGHAPI_MODELHIGHAPI_TOOLS_H_
#define SRC_MODELHIGHAPI_MODELHIGHAPI_TOOLS_H_

//--------------------------------------------------------------------------------------
#include "ModelHighAPI.h"
#include <ModelAPI_AttributeTables.h>

#include <GeomAPI_Shape.h>

#include <list>
#include <memory>
#include <string>
//--------------------------------------------------------------------------------------
class GeomAPI_Dir;
class GeomAPI_Pnt;
class GeomAPI_Pnt2d;
//--------------------------------------------------------------------------------------
class GeomDataAPI_Dir;
class GeomDataAPI_Point;
class GeomDataAPI_Point2D;
//--------------------------------------------------------------------------------------
class ModelAPI_AttributeBoolean;
class ModelAPI_AttributeDouble;
class ModelAPI_AttributeIntArray;
class ModelAPI_AttributeInteger;
class ModelAPI_AttributeRefAttr;
class ModelAPI_AttributeRefAttrList;
class ModelAPI_AttributeReference;
class ModelAPI_AttributeRefList;
class ModelAPI_AttributeSelection;
class ModelAPI_AttributeSelectionList;
class ModelAPI_AttributeString;
class ModelAPI_AttributeStringArray;
class ModelAPI_Object;
//--------------------------------------------------------------------------------------
class ModelHighAPI_Double;
class ModelHighAPI_Integer;
class ModelHighAPI_RefAttr;
class ModelHighAPI_Reference;
class ModelHighAPI_Selection;
//--------------------------------------------------------------------------------------
MODELHIGHAPI_EXPORT
void fillAttribute(const std::shared_ptr<GeomAPI_Pnt2d> & theValue,
                   const std::shared_ptr<GeomDataAPI_Point2D> & theAttribute);

MODELHIGHAPI_EXPORT
void fillAttribute(const std::shared_ptr<GeomDataAPI_Point2D> & theAttribute,
                   double theX, double theY);

MODELHIGHAPI_EXPORT
void fillAttribute(const std::shared_ptr<GeomAPI_Dir> & theValue,
                   const std::shared_ptr<GeomDataAPI_Dir> & theAttribute);

MODELHIGHAPI_EXPORT
void fillAttribute(const std::shared_ptr<GeomAPI_Pnt> & theValue,
                   const std::shared_ptr<GeomDataAPI_Point> & theAttribute);

//--------------------------------------------------------------------------------------
MODELHIGHAPI_EXPORT
void fillAttribute(bool theValue,
                   const std::shared_ptr<ModelAPI_AttributeBoolean> & theAttribute);

MODELHIGHAPI_EXPORT
void fillAttribute(const ModelHighAPI_Double & theValue,
                   const std::shared_ptr<ModelAPI_AttributeDouble> & theAttribute);

MODELHIGHAPI_EXPORT
void fillAttribute(double theValue,
                   const std::shared_ptr<ModelAPI_AttributeDouble> & theAttribute);

MODELHIGHAPI_EXPORT
void fillAttribute(const ModelHighAPI_Integer & theValue,
                   const std::shared_ptr<ModelAPI_AttributeInteger> & theAttribute);

MODELHIGHAPI_EXPORT
void fillAttribute(int theValue,
                   const std::shared_ptr<ModelAPI_AttributeInteger> & theAttribute);

MODELHIGHAPI_EXPORT
void fillAttribute(const ModelHighAPI_RefAttr & theValue,
                   const std::shared_ptr<ModelAPI_AttributeRefAttr> & theAttribute);

MODELHIGHAPI_EXPORT
void fillAttribute(const std::list<ModelHighAPI_RefAttr> & theValue,
                   const std::shared_ptr<ModelAPI_AttributeRefAttrList> & theAttribute);

MODELHIGHAPI_EXPORT
void fillAttribute(const ModelHighAPI_Reference & theValue,
                   const std::shared_ptr<ModelAPI_AttributeReference> & theAttribute);

MODELHIGHAPI_EXPORT
void fillAttribute(const std::list<ModelHighAPI_Reference> & theValue,
                   const std::shared_ptr<ModelAPI_AttributeRefList> & theAttribute);

MODELHIGHAPI_EXPORT
void fillAttribute(const std::shared_ptr<ModelAPI_Object> & theValue,
                   const std::shared_ptr<ModelAPI_AttributeReference> & theAttribute);

MODELHIGHAPI_EXPORT
void fillAttribute(const std::list<std::shared_ptr<ModelAPI_Object> > & theValue,
                   const std::shared_ptr<ModelAPI_AttributeRefList> & theAttribute);

MODELHIGHAPI_EXPORT
void fillAttribute(const std::list<ModelHighAPI_Selection> & theValue,
                   const std::shared_ptr<ModelAPI_AttributeRefList> & theAttribute);

MODELHIGHAPI_EXPORT
void fillAttribute(const ModelHighAPI_Selection & theValue,
                   const std::shared_ptr<ModelAPI_AttributeSelection> & theAttribute);

MODELHIGHAPI_EXPORT
void fillAttribute(const std::list<ModelHighAPI_Selection> & theValue,
                   const std::shared_ptr<ModelAPI_AttributeSelectionList> & theAttribute);

MODELHIGHAPI_EXPORT
void fillAttribute(const std::string & theValue,
                   const std::shared_ptr<ModelAPI_AttributeString> & theAttribute);
MODELHIGHAPI_EXPORT
void fillAttribute(const char * theValue,
                   const std::shared_ptr<ModelAPI_AttributeString> & theAttribute);

MODELHIGHAPI_EXPORT
void fillAttribute(const std::list<std::string> & theValue,
                   const std::shared_ptr<ModelAPI_AttributeStringArray> & theAttribute);

MODELHIGHAPI_EXPORT
void fillAttribute(const std::list<ModelHighAPI_Integer> & theValue,
                   const std::shared_ptr<ModelAPI_AttributeIntArray> & theAttribute);

MODELHIGHAPI_EXPORT
GeomAPI_Shape::ShapeType shapeTypeByStr(std::string theShapeTypeStr);

MODELHIGHAPI_EXPORT
GeomAPI_Shape::ShapeType getShapeType(const ModelHighAPI_Selection& theSelection);

MODELHIGHAPI_EXPORT
ModelAPI_AttributeTables::ValueType valueTypeByStr(const std::string& theValueTypeStr);

MODELHIGHAPI_EXPORT
std::string strByValueType(const ModelAPI_AttributeTables::ValueType theType);

/// Performs the high level API dump, then closes all and executes the script:
/// model must be recreated fully, with all attributes
/// \returns true if check is well done
MODELHIGHAPI_EXPORT
bool checkPythonDump();

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
#endif /* SRC_MODELHIGHAPI_MODELHIGHAPI_TOOLS_H_ */
