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
class ModelAPI_AttributeSelectionList;
class ModelAPI_AttributeString;
//--------------------------------------------------------------------------------------
class ModelHighAPI_Selection;
//--------------------------------------------------------------------------------------
MODELHIGHAPI_EXPORT
void fillAttribute(const std::list<ModelHighAPI_Selection> & theValue,
                   const std::shared_ptr<ModelAPI_AttributeSelectionList> & theAttribute);

MODELHIGHAPI_EXPORT
void fillAttribute(const std::string & theValue,
                   const std::shared_ptr<ModelAPI_AttributeString> & theAttribute);

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
#endif /* SRC_MODELHIGHAPI_MODELHIGHAPI_TOOLS_H_ */ 
