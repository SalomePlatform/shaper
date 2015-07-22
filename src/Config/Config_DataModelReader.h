// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

/*
 * Config_DataModelReader.h
 *
 *  Created on: Jul 21, 2015
 *      Author: vsv
 */

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
   * \param theXmlFile - full path to the xml file which will be processed by the reader
   */
  CONFIG_EXPORT Config_DataModelReader();
  CONFIG_EXPORT virtual ~Config_DataModelReader();

  /// Returns name of type of tree items in root
  CONFIG_EXPORT std::string rootType() const { return myRootTypes; }

  /// Returns number of folders under root 
  CONFIG_EXPORT int rootFoldersNumber() const { return myRootFolderNames.size(); }

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

 protected:
  /// Overloaded method. Defines how to process each node
  virtual void processNode(xmlNodePtr theNode);

private:
  std::vector<std::string> myRootFolderNames;
  std::vector<std::string> myRootFolderTypes;
  std::vector<std::string> myRootFolderIcons;

  std::string myRootTypes;
};


#endif