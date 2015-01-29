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
  /*!
   * \brief Retrieves all the necessary info from the validator node.
   * Sends ValidatorLoaded event
   */
  void processValidator(xmlNodePtr theNode);
  /*!
   * \brief Retrieves all the necessary info from the SelectionFilter node.
   * Sends SelectionFilterLoaded event
   */
  void processSelectionFilter(xmlNodePtr theNode);

 protected:
  std::string myCurrentFeature; ///< Name of currently processed feature
  std::string myDocumentPath; ///< Path to the xml document
  xmlDocPtr myXmlDoc; ///< Root of the xml document
};

#endif /* CONFIG_XMLREADER_H_ */
