// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

#ifndef EXCHANGEPLUGIN_IMPORTPART_H_
#define EXCHANGEPLUGIN_IMPORTPART_H_

#include <ExchangePlugin.h>
#include <ModelAPI_Feature.h>

/**
 * \class ExchangePlugin_ImportPart
 * \ingroup Plugins
 * \brief Feature for import the structure of Part into the current document.
 */
class ExchangePlugin_ImportPart : public ModelAPI_Feature
{
public:
  /// Feature kind
  inline static const std::string& ID()
  {
    static const std::string MY_IMPORT_ID("ImportPart");
    return MY_IMPORT_ID;
  }
  /// attribute name of file path
  inline static const std::string& FILE_PATH_ID()
  {
    static const std::string MY_FILE_PATH_ID("file_path");
    return MY_FILE_PATH_ID;
  }
  /// attribute name of target part
  inline static const std::string& TARGET_PART_ID()
  {
    static const std::string MY_TARGET_PART_ID("target_part");
    return MY_TARGET_PART_ID;
  }
  /// attribute name of list of target parts
  inline static const std::string& TARGET_PARTS_LIST_ID()
  {
    static const std::string MY_TARGET_PARTS_LIST_ID("target_parts_list");
    return MY_TARGET_PARTS_LIST_ID;
  }
  /// Default constructor
  ExchangePlugin_ImportPart();

  /// Returns the unique kind of a feature
  EXCHANGEPLUGIN_EXPORT virtual const std::string& getKind()
  {
    return ExchangePlugin_ImportPart::ID();
  }

  /// Request for initialization of data model of the feature: adding all attributes
  EXCHANGEPLUGIN_EXPORT virtual void initAttributes();

  /// Called on change of any argument-attribute of this object
  /// \param theID identifier of changed attribute
  EXCHANGEPLUGIN_EXPORT virtual void attributeChanged(const std::string& theID);

  /// Computes or recomputes the results
  EXCHANGEPLUGIN_EXPORT virtual void execute();

  /// Returns true if this feature is used as macro: creates other features and then removed.
  EXCHANGEPLUGIN_EXPORT virtual bool isMacro() const { return true; }

  /// Reimplemented from ModelAPI_Feature::isPreviewNeeded(). Returns false.
  EXCHANGEPLUGIN_EXPORT virtual bool isPreviewNeeded() const { return false; }
};

#endif /* EXCHANGEPLUGIN_IMPORTPART_H_ */
