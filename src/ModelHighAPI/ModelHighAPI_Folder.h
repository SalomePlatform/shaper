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

#ifndef SRC_MODELHIGHAPI_MODELHIGHAPI_FOLDER_H_
#define SRC_MODELHIGHAPI_MODELHIGHAPI_FOLDER_H_

//--------------------------------------------------------------------------------------
#include <ModelHighAPI_Interface.h>

#include <memory>
//--------------------------------------------------------------------------------------
class ModelAPI_Document;
class ModelAPI_Folder;
class ModelHighAPI_Selection;
//--------------------------------------------------------------------------------------
/**\class ModelHighAPI_Folder
 * \ingroup CPPHighAPI
 * \brief Class for filling ModelAPI_Folder
 */
class ModelHighAPI_Folder
{
public:
  /// Constructor for a folder
  MODELHIGHAPI_EXPORT
  explicit ModelHighAPI_Folder(const std::shared_ptr<ModelAPI_Folder> & theFolder);
  /// Destructor
  MODELHIGHAPI_EXPORT virtual ~ModelHighAPI_Folder();
};

//--------------------------------------------------------------------------------------
/**\ingroup CPPHighAPI
 * \brief Create Folder feature
 */
MODELHIGHAPI_EXPORT
std::shared_ptr<ModelHighAPI_Folder> addFolder(const std::shared_ptr<ModelAPI_Document>& theDoc);
//--------------------------------------------------------------------------------------
#endif /* SRC_MODELHIGHAPI_MODELHIGHAPI_FOLDER_H_ */
