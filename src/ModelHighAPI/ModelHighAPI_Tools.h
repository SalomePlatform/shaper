// Name   : ModelHighAPI_Tools.h
// Purpose: 
//
// History:
// 07/06/16 - Sergey POKHODENKO - Creation of the file

#ifndef SRC_MODELHIGHAPI_MODELHIGHAPI_TOOLS_H_
#define SRC_MODELHIGHAPI_MODELHIGHAPI_TOOLS_H_

//--------------------------------------------------------------------------------------
#include "ModelHighAPI.h"

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
class ModelAPI_AttributeDocRef;
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
class ModelAPI_Object;
//--------------------------------------------------------------------------------------
class ModelHighAPI_Double;
class ModelHighAPI_Integer;
class ModelHighAPI_RefAttr;
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
void fillAttribute(const ModelHighAPI_RefAttr & theValue,
                   const std::shared_ptr<ModelAPI_AttributeRefAttr> & theAttribute);

MODELHIGHAPI_EXPORT
void fillAttribute(const std::list<ModelHighAPI_RefAttr> & theValue,
                   const std::shared_ptr<ModelAPI_AttributeRefAttrList> & theAttribute);

MODELHIGHAPI_EXPORT
void fillAttribute(const std::shared_ptr<ModelAPI_Object> & theValue,
                   const std::shared_ptr<ModelAPI_AttributeReference> & theAttribute);

MODELHIGHAPI_EXPORT
void fillAttribute(const std::list<std::shared_ptr<ModelAPI_Object> > & theValue,
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

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
#endif /* SRC_MODELHIGHAPI_MODELHIGHAPI_TOOLS_H_ */ 
