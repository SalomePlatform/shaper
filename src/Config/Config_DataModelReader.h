// Copyright (C) 2014-2023  CEA, EDF
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

#ifndef CONFIG_DATAMODELREADER_H_
#define CONFIG_DATAMODELREADER_H_

#include <Config_def.h>
#include <Config_XMLReader.h>

#include <vector>
#include <string>

/*!
 * \class Config_DataModelReader
 * \ingroup Config
 * \brief Class that reads data model definition XML for
 * further processing in the XGUI_DataModel
 */
class Config_DataModelReader : public Config_XMLReader
{
 public:
  /*!
   * Constructor
   */
  CONFIG_EXPORT Config_DataModelReader();
  CONFIG_EXPORT virtual ~Config_DataModelReader();

  // ROOT folders propertiues *****************
  /// Returns name of type of tree items in root
  CONFIG_EXPORT std::string rootType() const { return myRootTypes; }

  /// Returns number of folders under root
  CONFIG_EXPORT size_t rootFoldersNumber() const { return myRootFolderNames.size(); }

  /// Returns name of the folder by its Id
  /// \param theId id of the folder
  CONFIG_EXPORT std::string rootFolderName(int theId) const { return myRootFolderNames[theId]; }

  /// Returns data type in the folder by its Id
  /// \param theId id of the folder
  CONFIG_EXPORT std::string rootFolderType(int theId) const { return myRootFolderTypes[theId]; }

  /// Returns icon of a folder by its Id
  /// \param theId id of the folder
  CONFIG_EXPORT std::string rootFolderIcon(int theId) const { return myRootFolderIcons[theId]; }

  /// Returns id of a folder containing the given type
  /// \param theType type of objects in folder
  CONFIG_EXPORT int rootFolderId(std::string theType) const;

  /// Returns true if the folder can be shown without items
  /// \param theId id of the folder
  CONFIG_EXPORT bool rootShowEmpty(int theId) const { return myRootFolderShowEmpty[theId]; }

  /// Returns list of features attached to folder with name theFolderName in sub-document
  /// \param theFolderName a name of the folder
  CONFIG_EXPORT std::string rootFolderFeatures(const std::string& theFolderName) const;


  // SUB folders propertiues ********************
  /// Returns name of type of tree items in sub document
  CONFIG_EXPORT std::string subType() const { return mySubTypes; }

  /// Returns number of folders under sub document
  CONFIG_EXPORT size_t subFoldersNumber() const { return mySubFolderNames.size(); }

  /// Returns name of the folder by its Id
  /// \param theId id of the folder
  CONFIG_EXPORT std::string subFolderName(int theId) const { return mySubFolderNames[theId]; }

  /// Returns data type in the folder by its Id
  /// \param theId id of the folder
  CONFIG_EXPORT std::string subFolderType(int theId) const { return mySubFolderTypes[theId]; }

  /// Returns icon of a folder by its Id
  /// \param theId id of the folder
  CONFIG_EXPORT std::string subFolderIcon(int theId) const { return mySubFolderIcons[theId]; }

  /// Returns true if the folder can be shown without items
  /// \param theId id of the folder
  CONFIG_EXPORT bool subShowEmpty(int theId) const { return mySubFolderShowEmpty[theId]; }

  /// Returns id of a folder containing the given type
  /// \param theType type of objects in folder
  CONFIG_EXPORT int subFolderId(std::string theType) const;

  /// Returns list of features attached to folder with name theFolderName in sub-document
  /// \param theFolderName a name of the folder
  CONFIG_EXPORT std::string subFolderFeatures(const std::string& theFolderName) const;


  /// Returns true if the sub-document data tree has to be attached to Part Result node
  /// Otherwise it has to be connected to Part feature node
  CONFIG_EXPORT bool isAttachToResult() const { return myIsResultLink; }

protected:
  /// Overloaded method. Defines how to process each node
  virtual void processNode(xmlNodePtr theNode);

private:
  bool isRootReading;

  /// Root document data
  std::vector<std::string> myRootFolderNames;
  std::vector<std::string> myRootFolderTypes;
  std::vector<std::string> myRootFolderIcons;
  std::vector<std::string> myRootFeaturesList;
  std::vector<bool> myRootFolderShowEmpty;

  std::string myRootTypes;

  /// Sub document data
  std::vector<std::string> mySubFolderNames;
  std::vector<std::string> mySubFolderTypes;
  std::vector<std::string> mySubFolderIcons;
  std::vector<std::string> mySubFeaturesList;
  std::vector<bool> mySubFolderShowEmpty;

  bool myIsResultLink;
  std::string mySubTypes;
};


#endif
