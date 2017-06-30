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
