// Name   : ConstructionAPI_Plane.h
// Purpose: 
//
// History:
// 27/05/16 - Sergey POKHODENKO - Creation of the file

#ifndef SRC_CONSTRUCTIONAPI_CONSTRUCTIONAPI_PLANE_H_
#define SRC_CONSTRUCTIONAPI_CONSTRUCTIONAPI_PLANE_H_

//--------------------------------------------------------------------------------------
#include "ConstructionAPI.h"

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>
//--------------------------------------------------------------------------------------
class ModelHighAPI_Double;
class ModelHighAPI_Selection;
//--------------------------------------------------------------------------------------
/**\class ConstructionAPI_Plane
 * \ingroup CPPHighAPI
 * \brief Interface for Plane feature
 */
class ConstructionAPI_Plane : public ModelHighAPI_Interface
{
public:
  /// Constructor without values
  CONSTRUCTIONAPI_EXPORT
  explicit ConstructionAPI_Plane(const std::shared_ptr<ModelAPI_Feature> & theFeature);
  /// Constructor with values
  CONSTRUCTIONAPI_EXPORT
  ConstructionAPI_Plane(const std::shared_ptr<ModelAPI_Feature> & theFeature,
                        const ModelHighAPI_Selection & theFace,
                        const ModelHighAPI_Double & theDistance);
  /// Constructor with values
  CONSTRUCTIONAPI_EXPORT
  ConstructionAPI_Plane(const std::shared_ptr<ModelAPI_Feature> & theFeature,
                        const ModelHighAPI_Double & theA,
                        const ModelHighAPI_Double & theB,
                        const ModelHighAPI_Double & theC,
                        const ModelHighAPI_Double & theD);
  /// Destructor
  CONSTRUCTIONAPI_EXPORT
  virtual ~ConstructionAPI_Plane();

  INTERFACE_7("Plane",
              creationMethod, "CreationMethod", String, /** Creation method */,
              face, "planeFace", Selection, /** Plane face */,
              distance, "distance", Double, /** Distance */,
              A, "A", Double, /** Parameter A for general equation */,
              B, "B", Double, /** Parameter B for general equation */,
              C, "C", Double, /** Parameter C for general equation */,
              D, "D", Double, /** Parameter D for general equation */
  )

  /// Set face and distance
  CONSTRUCTIONAPI_EXPORT
  void setFaceAndDistance(const ModelHighAPI_Selection & theFace,
                          const ModelHighAPI_Double & theDistance);

  /// Set GeneralEquation parameters of the feature
  CONSTRUCTIONAPI_EXPORT
  void setGeneralEquation(const ModelHighAPI_Double & theA,
                          const ModelHighAPI_Double & theB,
                          const ModelHighAPI_Double & theC,
                          const ModelHighAPI_Double & theD);
};

//! Pointer on Plane object
typedef std::shared_ptr<ConstructionAPI_Plane> PlanePtr;

/**\ingroup CPPHighAPI
 * \brief Create Plane feature
 */
CONSTRUCTIONAPI_EXPORT
PlanePtr addPlane(const std::shared_ptr<ModelAPI_Document> & thePart,
                  const ModelHighAPI_Selection & theFace,
                  const ModelHighAPI_Double & theDistance);

/**\ingroup CPPHighAPI
 * \brief Create Plane feature
 */
CONSTRUCTIONAPI_EXPORT
PlanePtr addPlane(const std::shared_ptr<ModelAPI_Document> & thePart,
                  const ModelHighAPI_Double & theA,
                  const ModelHighAPI_Double & theB,
                  const ModelHighAPI_Double & theC,
                  const ModelHighAPI_Double & theD);

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
#endif /* SRC_CONSTRUCTIONAPI_CONSTRUCTIONAPI_PLANE_H_ */
