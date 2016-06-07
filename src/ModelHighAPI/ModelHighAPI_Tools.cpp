// Name   : ModelHighAPI_Tools.cpp
// Purpose: 
//
// History:
// 07/06/16 - Sergey POKHODENKO - Creation of the file

//--------------------------------------------------------------------------------------
#include "ModelHighAPI_Tools.h"

#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeString.h>

#include "ModelHighAPI_Selection.h"
//--------------------------------------------------------------------------------------
void fillAttribute(const std::list<ModelHighAPI_Selection> & theValue,
                   const std::shared_ptr<ModelAPI_AttributeSelectionList> & theAttribute)
{
  theAttribute->clear();
  for (auto it = theValue.begin(); it != theValue.end(); ++it)
    it->appendToList(theAttribute);
}

//--------------------------------------------------------------------------------------
void fillAttribute(const std::string & theValue,
                   const std::shared_ptr<ModelAPI_AttributeString> & theAttribute)
{
  theAttribute->setValue(theValue);
}
//--------------------------------------------------------------------------------------
