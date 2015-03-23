// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

/*
 * Config_ValidatorReader.h
 *
 *  Created on: Mar 20, 2015
 *      Author: sbh
 */

#ifndef CONFIG_VALIDATORREADER_H_
#define CONFIG_VALIDATORREADER_H_

#include <Config_def.h>
#include <Config_XMLReader.h>

#include <cstdarg>
#include <string>

/*!
 * \class Config_ValidatorReader
 * \ingroup Config
 * \brief Base class for all libxml readers. Provides high-level API
 * for all xml operations.
*/
class Config_ValidatorReader : public Config_XMLReader
{
 public:
  /*!
   * Constructor
   * \param theXmlFile - full path to the xml file which will be processed by the reader
   */
  CONFIG_EXPORT Config_ValidatorReader(const std::string& theXmlFile);
  CONFIG_EXPORT virtual ~Config_ValidatorReader();

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
   * \brief Retrieves all the necessary info from the validator node.
   * Sends ValidatorLoaded event
   */
  void processValidator(xmlNodePtr theNode);
  /*!
   * \brief Retrieves all the necessary info from the SelectionFilter node.
   * Sends SelectionFilterLoaded event
   */
  void processSelectionFilter(xmlNodePtr theNode);
};

#endif /* CONFIG_VALIDATORREADER_H_ */
