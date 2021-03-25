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
  CONFIG_EXPORT Config_ValidatorReader(const std::string& theXmlFile, bool isXMLContent = false);
  CONFIG_EXPORT virtual ~Config_ValidatorReader();


  /// Set feature ID for cases when XML for validators is parsed from memory
  CONFIG_EXPORT void setFeatureId(const std::string& theId) { myExtFeatureId = theId; }

  // for correct SWIG wrapping
  using Config_XMLReader::readAll;

 protected:
  /*!
   * \brief Allows to customize reader's behavior for a node. Virtual.
   * The default implementation process "source" and "validator" nodes.
   */
  virtual void processNode(xmlNodePtr aNode);
  /*!
   * \brief Defines which nodes should be processed recursively. Virtual.
   * The default impl is to read all nodes.
   */
  virtual bool processChildren(xmlNodePtr aNode);

  /*!
   * Cleans the cached information about parent feature or attribute (widget)
   */
  virtual void cleanup(xmlNodePtr theNode);

  /*!
   * \brief Retrieves all the necessary info from the validator node.
   * Sends ValidatorLoaded event
   */
  void processValidator(xmlNodePtr theNode);

 private:
  std::string myCurrentWidget;
  std::string myExtFeatureId;
};

#endif /* CONFIG_VALIDATORREADER_H_ */
