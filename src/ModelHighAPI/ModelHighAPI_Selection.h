// Name   : ModelHighAPI_Selection.h
// Purpose: 
//
// History:
// 06/06/16 - Sergey POKHODENKO - Creation of the file

#ifndef SRC_MODELHIGHAPI_MODELHIGHAPI_SELECTION_H_
#define SRC_MODELHIGHAPI_MODELHIGHAPI_SELECTION_H_

//--------------------------------------------------------------------------------------
#include <memory>
#include <string>
#include <utility>

#include <boost/variant.hpp>
//--------------------------------------------------------------------------------------
class GeomAPI_Shape;
class ModelAPI_AttributeSelection;
class ModelAPI_Result;
//--------------------------------------------------------------------------------------
typedef std::pair<std::shared_ptr<ModelAPI_Result>, std::shared_ptr<GeomAPI_Shape> > ResultSubShapePair;
typedef std::pair<std::string, std::string> TypeSubShapeNamePair;
//--------------------------------------------------------------------------------------
/**\class ModelHighAPI_Selection
 * \ingroup CPPHighAPI
 * \brief Class for filling ModelAPI_AttributeSelection
 */
class ModelHighAPI_Selection
{
public:
  /// Constructor for result and sub-shape
  ModelHighAPI_Selection(const std::shared_ptr<ModelAPI_Result>& theContext = std::shared_ptr<ModelAPI_Result>(),
                         const std::shared_ptr<GeomAPI_Shape>& theSubShape = std::shared_ptr<GeomAPI_Shape>());
  /// Constructor for sub-shape by the textual Name
  ModelHighAPI_Selection(const std::string& theType,
                         const std::string& theSubShapeName);
  /// Destructor
  virtual ~ModelHighAPI_Selection();

  /// Fill attribute values
  virtual void fillAttribute(std::shared_ptr<ModelAPI_AttributeSelection> & theAttribute) const;

private:
  boost::variant<ResultSubShapePair, TypeSubShapeNamePair> myValue;
};

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
#endif /* SRC_MODELHIGHAPI_MODELHIGHAPI_SELECTION_H_ */
