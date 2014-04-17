/*
 * Config_XMLReader.h
 *
 *  Created on: Mar 14, 2014
 *      Author: sbh
 */

#ifndef CONFIG_XMLREADER_H_
#define CONFIG_XMLREADER_H_

#include <Config.h>

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

/*
 * Base class for all libxml readers. Provides high-level API
 * for all xml operations.
 *
 */
class Config_XMLReader
{
public:
  CONFIG_EXPORT Config_XMLReader(const std::string& theXmlFile);
  CONFIG_EXPORT virtual ~Config_XMLReader();

  CONFIG_EXPORT void readAll();

protected:
  virtual void processNode(xmlNodePtr aNode);
  virtual bool processChildren(xmlNodePtr aNode);

  xmlNodePtr findRoot();
  void readRecursively(xmlNodePtr theParent);

  xmlNodePtr node(void* theNode);
  std::string getProperty(xmlNodePtr theNode, const char* property);

protected:
  std::string myDocumentPath;
  xmlDocPtr   myXmlDoc;
};

#endif /* CONFIG_XMLREADER_H_ */
