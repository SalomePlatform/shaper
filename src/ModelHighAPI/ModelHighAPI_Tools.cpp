// Name   : ModelHighAPI_Tools.cpp
// Purpose: 
//
// History:
// 07/06/16 - Sergey POKHODENKO - Creation of the file

//--------------------------------------------------------------------------------------
#include "ModelHighAPI_Tools.h"
//--------------------------------------------------------------------------------------
#include <GeomAPI_Dir.h>
#include <GeomAPI_Pnt.h>
#include <GeomAPI_Pnt2d.h>
//--------------------------------------------------------------------------------------
#include <GeomDataAPI_Dir.h>
#include <GeomDataAPI_Point.h>
#include <GeomDataAPI_Point2D.h>
//--------------------------------------------------------------------------------------
#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_AttributeDocRef.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeIntArray.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeRefAttrList.h>
#include <ModelAPI_AttributeReference.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeString.h>
//--------------------------------------------------------------------------------------
#include "ModelHighAPI_Double.h"
#include "ModelHighAPI_Integer.h"
#include "ModelHighAPI_RefAttr.h"
#include "ModelHighAPI_Selection.h"

//--------------------------------------------------------------------------------------
void fillAttribute(const std::shared_ptr<GeomAPI_Pnt2d> & theValue,
                   const std::shared_ptr<GeomDataAPI_Point2D> & theAttribute)
{
  theAttribute->setValue(theValue);
}

void fillAttribute(const std::shared_ptr<GeomDataAPI_Point2D> & theAttribute,
                   double theX, double theY)
{
  theAttribute->setValue(theX, theY);
}

//--------------------------------------------------------------------------------------
void fillAttribute(const std::shared_ptr<GeomAPI_Dir> & theValue,
                   const std::shared_ptr<GeomDataAPI_Dir> & theAttribute)
{
  theAttribute->setValue(theValue);
}

//--------------------------------------------------------------------------------------
void fillAttribute(const std::shared_ptr<GeomAPI_Pnt> & theValue,
                   const std::shared_ptr<GeomDataAPI_Point> & theAttribute)
{
  theAttribute->setValue(theValue);
}

//--------------------------------------------------------------------------------------
void fillAttribute(bool theValue,
                   const std::shared_ptr<ModelAPI_AttributeBoolean> & theAttribute)
{
  theAttribute->setValue(theValue);
}

//--------------------------------------------------------------------------------------
void fillAttribute(const ModelHighAPI_Double & theValue,
                   const std::shared_ptr<ModelAPI_AttributeDouble> & theAttribute)
{
  theValue.fillAttribute(theAttribute);
}
void fillAttribute(double theValue,
                   const std::shared_ptr<ModelAPI_AttributeDouble> & theAttribute)
{
  theAttribute->setValue(theValue);
}

//--------------------------------------------------------------------------------------
void fillAttribute(const ModelHighAPI_Integer & theValue,
                   const std::shared_ptr<ModelAPI_AttributeInteger> & theAttribute)
{
  theValue.fillAttribute(theAttribute);
}

//--------------------------------------------------------------------------------------
void fillAttribute(const ModelHighAPI_RefAttr & theValue,
                   const std::shared_ptr<ModelAPI_AttributeRefAttr> & theAttribute)
{
  theValue.fillAttribute(theAttribute);
}

//--------------------------------------------------------------------------------------
void fillAttribute(const std::list<ModelHighAPI_RefAttr> & theValue,
                   const std::shared_ptr<ModelAPI_AttributeRefAttrList> & theAttribute)
{
  theAttribute->clear();
  for (auto it = theValue.begin(); it != theValue.end(); ++it)
    it->appendToList(theAttribute);
}

//--------------------------------------------------------------------------------------
void fillAttribute(const std::shared_ptr<ModelAPI_Object> & theValue,
                   const std::shared_ptr<ModelAPI_AttributeReference> & theAttribute)
{
  theAttribute->setValue(theValue);
}

//--------------------------------------------------------------------------------------
void fillAttribute(const std::list<std::shared_ptr<ModelAPI_Object> > & theValue,
                   const std::shared_ptr<ModelAPI_AttributeRefList> & theAttribute)
{
  theAttribute->clear();
  for (auto it = theValue.begin(); it != theValue.end(); ++it)
    theAttribute->append(*it);
}

//--------------------------------------------------------------------------------------
void fillAttribute(const ModelHighAPI_Selection & theValue,
                   const std::shared_ptr<ModelAPI_AttributeSelection> & theAttribute)
{
  theValue.fillAttribute(theAttribute);
}

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
void fillAttribute(const char * theValue,
                   const std::shared_ptr<ModelAPI_AttributeString> & theAttribute)
{
  theAttribute->setValue(theValue);
}

//--------------------------------------------------------------------------------------
