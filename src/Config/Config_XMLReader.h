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
class CONFIG_EXPORT Config_XMLReader
{
public:
  Config_XMLReader(const std::string& theXmlFile);
  virtual ~Config_XMLReader();

  void readAll();

protected:
  virtual void processNode(xmlNodePtr aNode);
  virtual bool processChildren(xmlNodePtr aNode);

  xmlNodePtr findRoot();
  void readRecursively(xmlNodePtr theParent);

  xmlNodePtr node(void* theNode);
  std::string getProperty(xmlNodePtr theNode, const char* property);
  /*
   * Please note that this function should be called with NULL last argument. 
   * In example: isNode(aNode, "type1", ["type2", ...], NULL);
   * ", NULL" is required to use unlimited number of arguments.
   * TODO(sbh): find a way to simplify calling this method.
   */
  bool isNode(xmlNodePtr theNode, const char* name, ...);

protected:
  std::string myDocumentPath;
  xmlDocPtr   myXmlDoc;
};

#endif /* CONFIG_XMLREADER_H_ */
