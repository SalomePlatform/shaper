// Name   : ConstructionAPI_Point.h
// Purpose: 
//
// History:
// 29/03/16 - Sergey POKHODENKO - Creation of the file

#ifndef SRC_CONSTRUCTIONAPI_CONSTRUCTIONAPI_POINT_H_
#define SRC_CONSTRUCTIONAPI_CONSTRUCTIONAPI_POINT_H_

//--------------------------------------------------------------------------------------
#include "ConstructionAPI.h"

#include <ModelHighAPI_Interface.h>
//--------------------------------------------------------------------------------------
class ModelAPI_AttributeDouble;
class ModelHighAPI_Double;
//--------------------------------------------------------------------------------------
/*
 *
 */
class ConstructionAPI_Point : public ModelHighAPI_Interface
{
public:
  ConstructionAPI_Point(const std::shared_ptr<ModelAPI_Feature> & theFeature);
  ConstructionAPI_Point(const std::shared_ptr<ModelAPI_Feature> & theFeature,
                        const ModelHighAPI_Double & theX,
                        const ModelHighAPI_Double & theY,
                        const ModelHighAPI_Double & theZ);
  virtual ~ConstructionAPI_Point();

  void setPoint(const ModelHighAPI_Double & theX,
                const ModelHighAPI_Double & theY,
                const ModelHighAPI_Double & theZ);

  std::shared_ptr<ModelAPI_AttributeDouble> x() const;
  std::shared_ptr<ModelAPI_AttributeDouble> y() const;
  std::shared_ptr<ModelAPI_AttributeDouble> z() const;

protected:
  std::shared_ptr<ModelAPI_AttributeDouble> myX, myY, myZ;

  bool initialize();
};

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
#endif /* SRC_CONSTRUCTIONAPI_CONSTRUCTIONAPI_POINT_H_ */ 
