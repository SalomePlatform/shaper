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
#include <ModelHighAPI_Macro.h>
//--------------------------------------------------------------------------------------
class ModelAPI_AttributeDouble;
class ModelAPI_Document;
class ModelHighAPI_Double;
//--------------------------------------------------------------------------------------
/**\class ConstructionAPI_Point
 * \ingroup CPPHighAPI
 * \brief Interface for Point feature
 */
class ConstructionAPI_Point : public ModelHighAPI_Interface
{
public:
  /// Constructor without values
  explicit ConstructionAPI_Point(const std::shared_ptr<ModelAPI_Feature> & theFeature);
  /// Constructor with values
  ConstructionAPI_Point(const std::shared_ptr<ModelAPI_Feature> & theFeature,
                        const ModelHighAPI_Double & theX,
                        const ModelHighAPI_Double & theY,
                        const ModelHighAPI_Double & theZ);
  /// Destructor
  virtual ~ConstructionAPI_Point();

  INTERFACE_3("Point",
              x, "x", Double, /** X attribute */,
              y, "y", Double, /** Y attribute */,
              z, "z", Double, /** Z attribute */)

  /// Set point values
  void setPoint(const ModelHighAPI_Double & theX,
                const ModelHighAPI_Double & theY,
                const ModelHighAPI_Double & theZ);
};

//! Pointer on Point object
typedef std::shared_ptr<ConstructionAPI_Point> PointPtr;

/**\ingroup CPPHighAPI
 * \brief Create Point feature
 */
PointPtr addPoint(const std::shared_ptr<ModelAPI_Document> & thePart,
                  const ModelHighAPI_Double & theX,
                  const ModelHighAPI_Double & theY,
                  const ModelHighAPI_Double & theZ);

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
#endif /* SRC_CONSTRUCTIONAPI_CONSTRUCTIONAPI_POINT_H_ */ 
