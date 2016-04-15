// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

/*
 * Config_XMLReader.h
 *
 *  Created on: Mar 14, 2014
 *      Author: sbh
 */

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
  CONFIG_EXPORT Config_XMLReader(const std::string& theXmlFile);
  CONFIG_EXPORT virtual ~Config_XMLReader();
  /*!
   * Returns value of PLUGINS_CONFIG_FILE env variable, a path to the plugins.xml file
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
   * Returns xmlNodePtr to the root of reader's document
   * or NULL if not found
   */
  CONFIG_EXPORT xmlNodePtr findRoot();

 protected:
  /*!
   * \brief Allows to customize reader's behavior for a node. Virtual.
   * The default implementation process "source", "validator" and
   * "selection_filter" nodes.
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
  /// A map to store all parent's attributes.
  /// The key has from "Node_Name:Node_Attribute"
  std::map<std::string, std::string> myCachedAttributes;
};

#endif /* CONFIG_XMLREADER_H_ */
