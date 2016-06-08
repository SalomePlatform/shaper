// Name   : SketchAPI_Sketch.h
// Purpose: 
//
// History:
// 07/06/16 - Sergey POKHODENKO - Creation of the file

#ifndef SRC_SKETCHAPI_SKETCHAPI_SKETCH_H_
#define SRC_SKETCHAPI_SKETCHAPI_SKETCH_H_

//--------------------------------------------------------------------------------------
#include "SketchAPI.h"

#include <SketchPlugin_Sketch.h>
#include <SketchPlugin_SketchEntity.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>
//--------------------------------------------------------------------------------------
class ModelAPI_CompositeFeature;
class ModelHighAPI_Selection;
class SketchAPI_Line;
//--------------------------------------------------------------------------------------
/**\class SketchAPI_Sketch
 * \ingroup CPPHighAPI
 * \brief Interface for Sketch feature
 */
class SketchAPI_Sketch : public ModelHighAPI_Interface
{
public:
  /// Constructor without values
  SKETCHAPI_EXPORT
  explicit SketchAPI_Sketch(const std::shared_ptr<ModelAPI_Feature> & theFeature);
  /// Constructor with values
  SKETCHAPI_EXPORT
  SketchAPI_Sketch(const std::shared_ptr<ModelAPI_Feature> & theFeature,
                   const std::shared_ptr<GeomAPI_Ax3> & thePlane);
  /// Constructor with values
  SKETCHAPI_EXPORT
  SketchAPI_Sketch(const std::shared_ptr<ModelAPI_Feature> & theFeature,
                   const ModelHighAPI_Selection & theExternal);
  /// Destructor
  SKETCHAPI_EXPORT
  virtual ~SketchAPI_Sketch();

  INTERFACE_7(SketchPlugin_Sketch::ID(),
              origin, SketchPlugin_Sketch::ORIGIN_ID(), GeomDataAPI_Point, /** Origin point */,
              dirX, SketchPlugin_Sketch::DIRX_ID(), GeomDataAPI_Dir, /** Direction of X */,
              normal, SketchPlugin_Sketch::NORM_ID(), GeomDataAPI_Dir, /** Normal */,
              features, SketchPlugin_Sketch::FEATURES_ID(), ModelAPI_AttributeRefList, /** Features */,
              external, SketchPlugin_SketchEntity::EXTERNAL_ID(), ModelAPI_AttributeSelection, /** External */,
              solverError, SketchPlugin_Sketch::SOLVER_ERROR(), ModelAPI_AttributeString, /** Solver error */,
              solverDOF, SketchPlugin_Sketch::SOLVER_DOF(), ModelAPI_AttributeString, /** Solver DOF */
  )

  /// Set plane
  SKETCHAPI_EXPORT
  void setPlane(const std::shared_ptr<GeomAPI_Ax3> & thePlane);

  /// Set external
  SKETCHAPI_EXPORT
  void setExternal(const ModelHighAPI_Selection & theExternal);

  // TODO(spo): addPoint

  /// Add line
  SKETCHAPI_EXPORT
  std::shared_ptr<SketchAPI_Line> addLine(
      double theX1, double theY1, double theX2, double theY2);
  /// Add line
  SKETCHAPI_EXPORT
  std::shared_ptr<SketchAPI_Line> addLine(
      const std::shared_ptr<GeomAPI_Pnt2d> & theStartPoint,
      const std::shared_ptr<GeomAPI_Pnt2d> & theEndPoint);
  /// Add line
  SKETCHAPI_EXPORT
  std::shared_ptr<SketchAPI_Line> addLine(const ModelHighAPI_Selection & theExternal);
  /// Add line
  SKETCHAPI_EXPORT
  std::shared_ptr<SketchAPI_Line> addLine(const std::string & theExternalName);

  // TODO(spo): addCircle
  // TODO(spo): addArc

  // TODO(spo): set* (constraints)

  // TODO(spo): setValue
  // TODO(spo): setText

protected:
  std::shared_ptr<ModelAPI_CompositeFeature> compositeFeature() const;

};

//! Pointer on Sketch object
typedef std::shared_ptr<SketchAPI_Sketch> SketchPtr;

/**\ingroup CPPHighAPI
 * \brief Create Sketch feature
 */
SKETCHAPI_EXPORT
SketchPtr addSketch(const std::shared_ptr<ModelAPI_Document> & thePart,
                    const std::shared_ptr<GeomAPI_Ax3> & thePlane);

/**\ingroup CPPHighAPI
 * \brief Create Sketch feature
 */
SKETCHAPI_EXPORT
SketchPtr addSketch(const std::shared_ptr<ModelAPI_Document> & thePart,
                    const ModelHighAPI_Selection & theExternal);

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
#endif /* SRC_SKETCHAPI_SKETCHAPI_SKETCH_H_ */
