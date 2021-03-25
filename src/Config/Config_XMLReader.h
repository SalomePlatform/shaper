// Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

#ifndef CONFIG_XMLREADER_H_
#define CONFIG_XMLREADER_H_

#include <Config_def.h>

#include <cstdarg>
#include <string>
#include <map>

//>> Forward declaration of xmlNodePtr.
typedef struct _xmlNode xmlNode;
typedef xmlNode *xmlNodePtr;
struct _xmlNode;
//<<

//>> Forward declaration of xmlDocPtr.
typedef struct _xmlDoc xmlDoc;
typedef xmlDoc *xmlDocPtr;
struct _xmlDoc;
//<<

/*!
 * \class Config_XMLReader
 * \ingroup Config
 * \brief Base class for all libxml readers. Provides high-level API
 * for all xml operations.
*/
class Config_XMLReader
{
 public:
  /*!
   * Constructor
   * \param theXmlFile - full path to the xml file which will be processed by the reader
   */
  CONFIG_EXPORT Config_XMLReader(const std::string& theXmlFile, bool isXMLContent = false);
  CONFIG_EXPORT virtual ~Config_XMLReader();
  /*!
   * Returns a path to resource files (created from ROOT_DIR environment variable)
   * \return string value
   */
  CONFIG_EXPORT static std::string resourcesConfigFile();
  /*!
   * Returns a path to the plugins.xml file (created from ROOT_DIR environment variable)
   * \return string value
   */
  CONFIG_EXPORT static std::string pluginConfigFile();
  /*!
   * Read all nodes in associated xml file,
   * recursively if processChildren(xmlNode) is true for the xmlNode.
   * For each read node the processNode will be called.
   */
  CONFIG_EXPORT void readAll();
  /*!
   * Returns xmlNodePtr to the root of reader's document or NULL if not found.
   * If the path to the document to read is empty, uses myDocumentPath.
   */
  CONFIG_EXPORT xmlNodePtr findRoot(const std::string theDocumentPath = "");

  CONFIG_EXPORT const char* encoding() const;

  /// Checks all possible paths to configuration file given
  /// Uses theFindIndex if several solutions can be found (this is the number of solution to find)
  CONFIG_EXPORT static std::string
    findConfigFile(const std::string theFileName, const int theFindIndex = 0);

 protected:
  /*!
   * \brief Allows to customize reader's behavior for a node. Virtual.
   * The default implementation process "source" and "validator" nodes.
   */
  virtual void processNode(xmlNodePtr aNode);

  /*!
   * This method gives an ability to finalize processing of a node,
   * when reader is about to leave the node (node and all it's children are processed)
   */
  virtual void cleanup(xmlNodePtr aNode);
  /*!
   * \brief Defines which nodes should be processed recursively. Virtual.
   * The default impl is to read all nodes.
   */
  virtual bool processChildren(xmlNodePtr aNode);
  /*!
   * Calls processNode() for each child (for some - recursively)
   * of the given node.
   * \sa ReadAll()
   */
  void readRecursively(xmlNodePtr theParent);
  /*!
   * \brief void* -> xmlNodePtr
   */
  xmlNodePtr node(void* theNode);
  /// Gets xml node name
  std::string getNodeName(xmlNodePtr theNode);
  /// Stores an attribute in internal map for later use.
  /// Key is "Node_Name:Node_Attribute" and value is getProperty(theNodeAttribute)
  void storeAttribute(xmlNodePtr theNode, const char* theNodeAttribute, bool doClean = false);
  /// Restores an attribute from internal map.
  std::string restoreAttribute(xmlNodePtr theNode, const char* theNodeAttribute);
  /// Restores an attribute from internal map.
  std::string restoreAttribute(const char* theNodeName, const char* theNodeAttribute);
  /// Cleanups attribute from cache
  bool cleanupAttribute(xmlNodePtr theNode, const char* theNodeAttribute);
  /// Cleanups attribute from cache
  bool cleanupAttribute(const char* theNodeName, const char* theNodeAttribute);

 protected:
  std::string myDocumentPath; ///< Path to the xml document
  xmlDocPtr myXmlDoc; ///< Root of the xml document
  std::string myRootFileName; ///< name of the root file
  /// A map to store all parent's attributes.
  /// The key has from "Node_Name:Node_Attribute"
  std::map<std::string, std::string> myCachedAttributes;

  bool isFromMemory;
};

#endif /* CONFIG_XMLREADER_H_ */
