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
   */
  CONFIG_EXPORT Config_DataModelReader();
  CONFIG_EXPORT virtual ~Config_DataModelReader();

  // ROOT folders propertiues *****************
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

  /// Returns true if the folder can be shown without items
  /// \param theId id of the folder
  CONFIG_EXPORT bool rootShowEmpty(int theId) const { return myRootFolderShowEmpty[theId]; }



  // SUB folders propertiues ********************
  /// Returns name of type of tree items in sub document
  CONFIG_EXPORT std::string subType() const { return mySubTypes; }

  /// Returns number of folders under sub document 
  CONFIG_EXPORT int subFoldersNumber() const { return mySubFolderNames.size(); }

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
  std::vector<bool> myRootFolderShowEmpty;

  std::string myRootTypes;

  /// Sub document data
  std::vector<std::string> mySubFolderNames;
  std::vector<std::string> mySubFolderTypes;
  std::vector<std::string> mySubFolderIcons;
  std::vector<bool> mySubFolderShowEmpty;

  bool myIsResultLink;
  std::string mySubTypes;
};


#endif
