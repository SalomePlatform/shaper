// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// Name   : ExchangeAPI_Import.h
// Purpose: 
//
// History:
// 07/06/16 - Sergey POKHODENKO - Creation of the file

#ifndef SRC_EXCHANGEAPI_EXCHANGEAPI_IMPORT_H_
#define SRC_EXCHANGEAPI_EXCHANGEAPI_IMPORT_H_

//--------------------------------------------------------------------------------------
#include "ExchangeAPI.h"

#include <string>

#include <ExchangePlugin_ImportFeature.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>
//--------------------------------------------------------------------------------------
/**\class ExchangeAPI_Import
 * \ingroup CPPHighAPI
 * \brief Interface for Import feature
 */
class ExchangeAPI_Import : public ModelHighAPI_Interface
{
public:
  /// Constructor without values
  EXCHANGEAPI_EXPORT
  explicit ExchangeAPI_Import(const std::shared_ptr<ModelAPI_Feature> & theFeature);
  /// Constructor with values
  EXCHANGEAPI_EXPORT
  ExchangeAPI_Import(const std::shared_ptr<ModelAPI_Feature> & theFeature,
                     const std::string & theFilePath);
  /// Destructor
  EXCHANGEAPI_EXPORT
  virtual ~ExchangeAPI_Import();

  INTERFACE_1(ExchangePlugin_ImportFeature::ID(),
              filePath, ExchangePlugin_ImportFeature::FILE_PATH_ID(), 
              ModelAPI_AttributeString, /** File path */
  )

  /// Set point values
  EXCHANGEAPI_EXPORT
  void setFilePath(const std::string & theFilePath);

  /// Dump wrapped feature
  EXCHANGEAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

//! Pointer on Import object
typedef std::shared_ptr<ExchangeAPI_Import> ImportPtr;

/**\ingroup CPPHighAPI
 * \brief Create Import feature
 */
EXCHANGEAPI_EXPORT
ImportPtr addImport(const std::shared_ptr<ModelAPI_Document> & thePart,
                    const std::string & theFilePath);

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
#endif /* SRC_EXCHANGEAPI_EXCHANGEAPI_IMPORT_H_ */
