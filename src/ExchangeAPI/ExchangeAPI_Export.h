// Name   : ExchangeAPI_Export.h
// Purpose: 
//
// History:
// 07/06/16 - Sergey POKHODENKO - Creation of the file

#ifndef SRC_EXCHANGEAPI_EXCHANGEAPI_EXPORT_H_
#define SRC_EXCHANGEAPI_EXCHANGEAPI_EXPORT_H_

//--------------------------------------------------------------------------------------
#include "ExchangeAPI.h"

#include <list>
#include <string>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>

#include <ExchangePlugin_ExportFeature.h>
//--------------------------------------------------------------------------------------
class ModelHighAPI_Selection;
//--------------------------------------------------------------------------------------
/**\ingroup CPPHighAPI
 * \brief Export to file
 */
EXCHANGEAPI_EXPORT
void exportToFile(const std::shared_ptr<ModelAPI_Document> & thePart,
                  const std::string & theFilePath,
                  const std::list<ModelHighAPI_Selection> & theSelectionList,
                  const std::string & theFileFormat = std::string());

/**\ingroup CPPHighAPI
 * \brief Export XAO
 */
EXCHANGEAPI_EXPORT
void exportToXAO(const std::shared_ptr<ModelAPI_Document> & thePart,
                 const std::string & theFilePath,
                 const std::string & theAuthor = std::string(),
                 const std::string & theGeometryName = std::string());

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
#endif /* SRC_EXCHANGEAPI_EXCHANGEAPI_EXPORT_H_ */
