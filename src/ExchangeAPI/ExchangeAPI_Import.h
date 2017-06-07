// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

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
