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

#include <ModelAPI_Folder.h>

#include <memory>
//--------------------------------------------------------------------------------------
class ModelAPI_AttributeReference;
class ModelAPI_Document;
class ModelHighAPI_Reference;
//--------------------------------------------------------------------------------------
/**\class ModelHighAPI_Folder
 * \ingroup CPPHighAPI
 * \brief Class for filling ModelAPI_Folder
 */
class ModelHighAPI_Folder : public ModelHighAPI_Interface
{
public:
  /// Constructor for a folder
  MODELHIGHAPI_EXPORT
  explicit ModelHighAPI_Folder(const std::shared_ptr<ModelAPI_Folder>& theFolder);
  /// Destructor
  MODELHIGHAPI_EXPORT virtual ~ModelHighAPI_Folder();

  static std::string ID() { return ModelAPI_Folder::ID(); }
  virtual std::string getID() { return ID(); }

  /// First feature reference
  std::shared_ptr<ModelAPI_AttributeReference> firstFeature() const
  { return myFirstFeature; }

  /// Last feature reference
  std::shared_ptr<ModelAPI_AttributeReference> lastFeature() const
  { return myLastFeature; }

  /// Shortcut for data()->setName()
  MODELHIGHAPI_EXPORT
  void setName(const std::string& theName);

  /// Shortcut for data()->name()
  MODELHIGHAPI_EXPORT
  std::string name() const;

  /// Dump wrapped feature
  MODELHIGHAPI_EXPORT virtual void dump(ModelHighAPI_Dumper& theDumper) const;

protected:
  bool initialize();

private:
  std::shared_ptr<ModelAPI_Folder> myFolder;

  std::shared_ptr<ModelAPI_AttributeReference> myFirstFeature;
  std::shared_ptr<ModelAPI_AttributeReference> myLastFeature;
};

//--------------------------------------------------------------------------------------
/**\ingroup CPPHighAPI
 * \brief Create empty Folder feature
 */
MODELHIGHAPI_EXPORT
std::shared_ptr<ModelHighAPI_Folder> addFolder(const std::shared_ptr<ModelAPI_Document>& theDoc);

/**\ingroup CPPHighAPI
 * \brief Create Folder feature
 */
MODELHIGHAPI_EXPORT
std::shared_ptr<ModelHighAPI_Folder> addFolder(const std::shared_ptr<ModelAPI_Document>& theDoc,
                                               const ModelHighAPI_Reference& theFirstFeature,
                                               const ModelHighAPI_Reference& theLastFeature);
//--------------------------------------------------------------------------------------
#endif /* SRC_MODELHIGHAPI_MODELHIGHAPI_FOLDER_H_ */
