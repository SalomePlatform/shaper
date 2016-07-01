// Copyright (C) 2014-2016 CEA/DEN, EDF R&D

// Name   : ConstructionAPI_Axis.h
// Purpose: 
//
// History:
// 15/06/16 - Sergey POKHODENKO - Creation of the file
// 24/06/16 - Clarisse GENRAULT (CEA) - Modification of the file

#ifndef SRC_CONSTRUCTIONAPI_CONSTRUCTIONAPI_AXIS_H_
#define SRC_CONSTRUCTIONAPI_CONSTRUCTIONAPI_AXIS_H_

//--------------------------------------------------------------------------------------
#include "ConstructionAPI.h"

#include <ConstructionPlugin_Axis.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>
//--------------------------------------------------------------------------------------
class ModelHighAPI_Double;
class ModelHighAPI_Selection;
//--------------------------------------------------------------------------------------
/**\class ConstructionAPI_Axis
 * \ingroup CPPHighAPI
 * \brief Interface for Axis feature
 */
class ConstructionAPI_Axis : public ModelHighAPI_Interface
{
public:
  /// Constructor without values
  CONSTRUCTIONAPI_EXPORT
  explicit ConstructionAPI_Axis(const std::shared_ptr<ModelAPI_Feature> & theFeature);
  /// Constructor with values
  CONSTRUCTIONAPI_EXPORT
  ConstructionAPI_Axis(const std::shared_ptr<ModelAPI_Feature> & theFeature,
                       const ModelHighAPI_Selection & thePoint1,
                       const ModelHighAPI_Selection & thePoint2);
  /// Constructor with values
  CONSTRUCTIONAPI_EXPORT
  ConstructionAPI_Axis(const std::shared_ptr<ModelAPI_Feature> & theFeature,
                       const ModelHighAPI_Selection & theCylindricalFace);
  /// Constructor with values
  CONSTRUCTIONAPI_EXPORT
  ConstructionAPI_Axis(const std::shared_ptr<ModelAPI_Feature> & theFeature,
                       const ModelHighAPI_Selection & thePoint,
                       const ModelHighAPI_Double & theX,
                       const ModelHighAPI_Double & theY,
                       const ModelHighAPI_Double & theZ);
  /// Constructor with values
  CONSTRUCTIONAPI_EXPORT
  ConstructionAPI_Axis(const std::shared_ptr<ModelAPI_Feature> & theFeature,
                       const ModelHighAPI_Double & theDX,
                       const ModelHighAPI_Double & theDY,
                       const ModelHighAPI_Double & theDZ);
  /// Destructor
  CONSTRUCTIONAPI_EXPORT
  virtual ~ConstructionAPI_Axis();

  INTERFACE_10(ConstructionPlugin_Axis::ID(),
              creationMethod, ConstructionPlugin_Axis::METHOD(), ModelAPI_AttributeString, /** Creation method */,
              firstPoint, ConstructionPlugin_Axis::POINT_FIRST(), ModelAPI_AttributeSelection, /** First point */,
              secondPoint, ConstructionPlugin_Axis::POINT_SECOND(), ModelAPI_AttributeSelection, /** Second point */,
              cylindricalFace, ConstructionPlugin_Axis::CYLINDRICAL_FACE(), ModelAPI_AttributeSelection, /** Cylindrical face */,
              xDirection, ConstructionPlugin_Axis::X_DIRECTION(), ModelAPI_AttributeDouble, /** X direction */,
              yDirection, ConstructionPlugin_Axis::Y_DIRECTION(), ModelAPI_AttributeDouble, /** Y direction */,
              zDirection, ConstructionPlugin_Axis::Z_DIRECTION(), ModelAPI_AttributeDouble, /** Z direction */,
              xDimension, ConstructionPlugin_Axis::DX(), ModelAPI_AttributeDouble, /** X dimension */,
              yDimension, ConstructionPlugin_Axis::DY(), ModelAPI_AttributeDouble, /** Y dimension */,
              zDimension, ConstructionPlugin_Axis::DZ(), ModelAPI_AttributeDouble, /** Z dimension */
  )

  /// Set points
  CONSTRUCTIONAPI_EXPORT
  void setPoints(const ModelHighAPI_Selection & thePoint1,
                 const ModelHighAPI_Selection & thePoint2);

  /// Set cylindrical face
  CONSTRUCTIONAPI_EXPORT
  void setCylindricalFace(const ModelHighAPI_Selection & theCylindricalFace);

  /// Set direction
  CONSTRUCTIONAPI_EXPORT
  void setPointAndDirection(const ModelHighAPI_Selection & thePoint,
                            const ModelHighAPI_Double & theX,
                            const ModelHighAPI_Double & theY,
                            const ModelHighAPI_Double & theZ);
  
  /// Set dimensions
  CONSTRUCTIONAPI_EXPORT
  void setDimensions(const ModelHighAPI_Double & theDX,
                     const ModelHighAPI_Double & theDY,
                     const ModelHighAPI_Double & theDZ);
};

//! Pointer on Axis object
typedef std::shared_ptr<ConstructionAPI_Axis> AxisPtr;

/**\ingroup CPPHighAPI
 * \brief Create Axis feature
 */
CONSTRUCTIONAPI_EXPORT
AxisPtr addAxis(const std::shared_ptr<ModelAPI_Document> & thePart,
                const ModelHighAPI_Selection & thePoint1,
                const ModelHighAPI_Selection & thePoint2);

/**\ingroup CPPHighAPI
 * \brief Create Axis feature
 */
CONSTRUCTIONAPI_EXPORT
AxisPtr addAxis(const std::shared_ptr<ModelAPI_Document> & thePart,
                const ModelHighAPI_Selection & theCylindricalFace);

/**\ingroup CPPHighAPI
 * \brief Create Axis feature
 */
CONSTRUCTIONAPI_EXPORT
AxisPtr addAxis(const std::shared_ptr<ModelAPI_Document> & thePart,
                const ModelHighAPI_Selection & thePoint,
                const ModelHighAPI_Double & theX,
                const ModelHighAPI_Double & theY,
                const ModelHighAPI_Double & theZ);

/**\ingroup CPPHighAPI
 * \brief Create Axis feature
 */
CONSTRUCTIONAPI_EXPORT
AxisPtr addAxis(const std::shared_ptr<ModelAPI_Document> & thePart,
                const ModelHighAPI_Double & theDX,
                const ModelHighAPI_Double & theDY,
                const ModelHighAPI_Double & theDZ);

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
#endif /* SRC_CONSTRUCTIONAPI_CONSTRUCTIONAPI_AXIS_H_ */
