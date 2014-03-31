/*
 * Config_XMLReader.h
 *
 *  Created on: Mar 14, 2014
 *      Author: sbh
 */

#ifndef CONFIG_XMLREADER_H_
#define CONFIG_XMLREADER_H_

#include "Config.h"
#include "Config_FeatureMessage.h"

#include <string>
#include <cstdarg>

//Forward declaration for xmlNodePtr.
typedef struct _xmlNode xmlNode;
typedef xmlNode *xmlNodePtr;
struct _xmlNode;

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

private:
  std::string m_DocumentPath;
};

#endif /* CONFIG_XMLREADER_H_ */
