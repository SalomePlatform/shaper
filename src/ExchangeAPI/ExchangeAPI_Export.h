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
/**\class ExchangeAPI_Export
 * \ingroup CPPHighAPI
 * \brief Interface for Export feature
 */
class ExchangeAPI_Export : public ModelHighAPI_Interface
{
public:
  /// Constructor without values
  EXCHANGEAPI_EXPORT
  explicit ExchangeAPI_Export(const std::shared_ptr<ModelAPI_Feature> & theFeature);
  /// Constructor with values
  EXCHANGEAPI_EXPORT
  ExchangeAPI_Export(const std::shared_ptr<ModelAPI_Feature> & theFeature,
                     const std::string & theFilePath,
                     const std::string & theFileFormat,
                     const std::list<ModelHighAPI_Selection> & theSelectionList);
  /// Destructor
  EXCHANGEAPI_EXPORT
  virtual ~ExchangeAPI_Export();

  INTERFACE_3(ExchangePlugin_ExportFeature::ID(),
              filePath, ExchangePlugin_ExportFeature::FILE_PATH_ID(), String, /** File path */,
              fileFormat, ExchangePlugin_ExportFeature::FILE_FORMAT_ID(), String, /** File format */,
              selectionList, ExchangePlugin_ExportFeature::SELECTION_LIST_ID(), SelectionList, /** Selection list */
  )

  /// Set file path (without execute)
  EXCHANGEAPI_EXPORT
  void setFilePath(const std::string & theFilePath);

  /// Set file format (without execute)
  EXCHANGEAPI_EXPORT
  void setFileFormat(const std::string & theFileFormat);


  /// Set selection list (without execute)
  EXCHANGEAPI_EXPORT
  void setSelectionList(const std::list<ModelHighAPI_Selection> & theSelectionList);
};

//! Pointer on Export object
typedef std::shared_ptr<ExchangeAPI_Export> ExportPtr;

/**\ingroup CPPHighAPI
 * \brief Create Export feature
 */
EXCHANGEAPI_EXPORT
ExportPtr exportToFile(const std::shared_ptr<ModelAPI_Document> & thePart,
                       const std::string & theFilePath,
                       const std::string & theFileFormat,
                       const std::list<ModelHighAPI_Selection> & theSelectionList);

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
#endif /* SRC_EXCHANGEAPI_EXCHANGEAPI_EXPORT_H_ */
