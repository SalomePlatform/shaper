// Name   : PartSetAPI_Part.h
// Purpose: 
//
// History:
// 16/06/16 - Sergey POKHODENKO - Creation of the file

#ifndef SRC_PARTSETAPI_PARTSETAPI_PART_H_
#define SRC_PARTSETAPI_PARTSETAPI_PART_H_

//--------------------------------------------------------------------------------------
#include "PartSetAPI.h"

#include <PartSetPlugin_Part.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>
//--------------------------------------------------------------------------------------
class ModelAPI_Document;
//--------------------------------------------------------------------------------------
/**\class PartSetAPI_Part
 * \ingroup CPPHighAPI
 * \brief Interface for Part feature
 */
class PartSetAPI_Part : public ModelHighAPI_Interface
{
public:
  /// Constructor without values
  PARTSETAPI_EXPORT
  explicit PartSetAPI_Part(const std::shared_ptr<ModelAPI_Feature> & theFeature);
  /// Destructor
  PARTSETAPI_EXPORT
  virtual ~PartSetAPI_Part();

  INTERFACE_0(PartSetPlugin_Part::ID())

  /// Return document
  PARTSETAPI_EXPORT
  std::shared_ptr<ModelAPI_Document> document() const;
};

//! Pointer on Part object
typedef std::shared_ptr<PartSetAPI_Part> PartPtr;

/**\ingroup CPPHighAPI
 * \brief Create Part feature
 */
PARTSETAPI_EXPORT
PartPtr addPart(const std::shared_ptr<ModelAPI_Document> & thePartSet);

/**\ingroup CPPHighAPI
 * \brief Duplicate Part feature
 */
PARTSETAPI_EXPORT
PartPtr duplicatePart(const std::shared_ptr<ModelAPI_Document> & thePart);

/**\ingroup CPPHighAPI
 * \brief Remove Part feature
 */
PARTSETAPI_EXPORT
void removePart(const std::shared_ptr<ModelAPI_Document> & thePart);

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
#endif /* SRC_PARTSETAPI_PARTSETAPI_PART_H_ */
