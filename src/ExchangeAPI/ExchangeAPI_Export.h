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


/// \class ExchangeAPI_Export
/// \ingroup CPPHighAPI
/// \brief Interface for Export feature.
class ExchangeAPI_Export: public ModelHighAPI_Interface
{
public:
  /// Constructor without values.
  EXCHANGEAPI_EXPORT
  explicit ExchangeAPI_Export(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values for XAO export.
  EXCHANGEAPI_EXPORT
  explicit ExchangeAPI_Export(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                              const std::string & theFilePath,
                              const std::string & theAuthor = std::string(),
                              const std::string & theGeometryName = std::string());

  /// Constructor with values for export in other formats than XAO.
  EXCHANGEAPI_EXPORT
  explicit ExchangeAPI_Export(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                              const std::string & theFilePath,
                              const std::list<ModelHighAPI_Selection> & theSelectionList,
                              const std::string & theFileFormat = std::string());

  /// Destructor.
  EXCHANGEAPI_EXPORT
  virtual ~ExchangeAPI_Export();

  INTERFACE_7(ExchangePlugin_ExportFeature::ID(),
             exportType, ExchangePlugin_ExportFeature::EXPORT_TYPE_ID(),
             ModelAPI_AttributeString, /** ExportType */,
             filePath, ExchangePlugin_ExportFeature::FILE_PATH_ID(),
             ModelAPI_AttributeString, /** file path */,
             xaoFilePath, ExchangePlugin_ExportFeature::XAO_FILE_PATH_ID(),
             ModelAPI_AttributeString, /** xao_file_path */,
             fileFormat, ExchangePlugin_ExportFeature::FILE_FORMAT_ID(),
             ModelAPI_AttributeString, /** file format */,
             selectionList, ExchangePlugin_ExportFeature::SELECTION_LIST_ID(),
             ModelAPI_AttributeString, /** selection list */,
             xaoAuthor, ExchangePlugin_ExportFeature::XAO_AUTHOR_ID(),
             ModelAPI_AttributeString, /** xao author */,
             xaoGeometryName, ExchangePlugin_ExportFeature::XAO_GEOMETRY_NAME_ID(),
             ModelAPI_AttributeString, /** xao geometry name */)

  /// Dump wrapped feature
  EXCHANGEAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

/// Pointer on Export object
typedef std::shared_ptr<ExchangeAPI_Export> ExportPtr;

/**\ingroup CPPHighAPI
 * \brief Export to file
 */
EXCHANGEAPI_EXPORT
ExportPtr exportToFile(const std::shared_ptr<ModelAPI_Document> & thePart,
                  const std::string & theFilePath,
                  const std::list<ModelHighAPI_Selection> & theSelectionList,
                  const std::string & theFileFormat = std::string());

/**\ingroup CPPHighAPI
 * \brief Export XAO
 */
EXCHANGEAPI_EXPORT
ExportPtr exportToXAO(const std::shared_ptr<ModelAPI_Document> & thePart,
                 const std::string & theFilePath,
                 const std::string & theAuthor = std::string(),
                 const std::string & theGeometryName = std::string());

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
#endif /* SRC_EXCHANGEAPI_EXCHANGEAPI_EXPORT_H_ */
