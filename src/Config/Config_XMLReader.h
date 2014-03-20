/*
 * Config_XMLReader.h
 *
 *  Created on: Mar 14, 2014
 *      Author: sbh
 */

#ifndef CONFIG_XMLREADER_H_
#define CONFIG_XMLREADER_H_

#include "Config.h"
#include "Config_Message.h"

#include <string>
#include <map>

class CONFIG_EXPORT Config_XMLReader {
public:
  Config_XMLReader(const std::string& theXmlFile);
  virtual ~Config_XMLReader();

  std::string documentPath() const;
  void setDocumentPath(std::string documentName);

  void readAll();

protected:
  //! Performs the real import of the given xml file, return false if file is not found
  //! or generates an algo error if file content is bad
  //! \param theFile name of the imported XML file
  //! \returns true if file exists and not corrupted
  bool import();
  bool importWorkbench(void*);
  void fillFeature(void *theRoot, Config_FeatureMessage& outFeatureMessage);
  std::string getProperty(void *theRoot, const char* name);

private:
  std::string m_DocumentPath;
};

#endif /* CONFIG_XMLREADER_H_ */
