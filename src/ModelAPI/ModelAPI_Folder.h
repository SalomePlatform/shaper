// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#ifndef ModelAPI_Folder_H_
#define ModelAPI_Folder_H_

#include <ModelAPI.h>
#include <ModelAPI_Object.h>

/**\class ModelAPI_Folder
 * \ingroup DataModel
 * \brief Folder feature (groups the features).
 */
class ModelAPI_Folder : public ModelAPI_Object
{
public:
  MODELAPI_EXPORT ModelAPI_Folder();
  /// To virtually destroy the fields of successors
  MODELAPI_EXPORT virtual ~ModelAPI_Folder();

  /// Folder feature ID
  static const std::string& ID()
  {
    static const std::string MY_FOLDER_ID("Folder");
    return MY_FOLDER_ID;
  }

  /// Returns the group identifier of all features
  inline static std::string group()
  {
    static std::string MY_GROUP = "Folders";
    return MY_GROUP;
  }

  /// Returns the group identifier of this result
  virtual std::string groupName()
  {
    return group();
  }

  /// Attribute referring first feature in the folder
  static const std::string& FIRST_FEATURE_ID()
  {
    static const std::string MY_FIRST_FEATURE_ID("first_feature");
    return MY_FIRST_FEATURE_ID;
  }

  /// Attribute referring last feature in the folder
  static const std::string& LAST_FEATURE_ID()
  {
    static const std::string MY_LAST_FEATURE_ID("last_feature");
    return MY_LAST_FEATURE_ID;
  }

  /// Request for initialization of data model of the object: adding all attributes
  MODELAPI_EXPORT virtual void initAttributes();

  /// Computes or recomputes the results
  MODELAPI_EXPORT virtual void execute();

  /// Returns the feature is disabled or not.
  virtual bool isDisabled()
  { return false; }

  //
  // Helper methods, aliases for data()->method()
  // -----------------------------------------------------------------------------------------------
  /// Returns the name stored in the attribute
  inline std::wstring name()
  {
    return data()->name();
  }
  /// Returns the reference attribute by the identifier
  inline std::shared_ptr<ModelAPI_AttributeReference> reference(const std::string& theID)
  {
    return data()->reference(theID);
  }

  /// Returns the last visible feature in the folder, passing through invisible,
  /// that may appear as the last ones.
  MODELAPI_EXPORT std::shared_ptr<ModelAPI_Feature> lastVisibleFeature();

protected:
  /// This method is called just after creation of the object: it must initialize
  /// all fields, normally initialized in the constructor
  MODELAPI_EXPORT virtual void init();
};

//! Pointer on a folder object
typedef std::shared_ptr<ModelAPI_Folder> FolderPtr;

#endif
