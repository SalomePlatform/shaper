// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#ifndef CONFIG_WIDGETREADER_H_
#define CONFIG_WIDGETREADER_H_

#include <Config_def.h>
#include <Config_XMLReader.h>

#include <map>
#include <string>

/*!
 * \class Config_WidgetReader
 * \ingroup Config
 * \brief Class that dumps xml definitions of widgets for
 * further processing in the WidgetFactory
 */
class Config_WidgetReader : public Config_XMLReader
{
 public:
  /*!
   * Constructor
   * \param theXmlFile - full path to the xml file which will be processed by the reader
   */
  CONFIG_EXPORT Config_WidgetReader(const std::string& theXmlFile);
  CONFIG_EXPORT virtual ~Config_WidgetReader();

  /// Extract feature's widget configuration from local cache, stored on node processing
  CONFIG_EXPORT std::string featureWidgetCfg(const std::string& theFeatureName);
  /// Extract feature's description from local cache, stored on node processing
  CONFIG_EXPORT std::string featureDescription(const std::string& theFeatureName);

 protected:
  /// Overloaded method. Defines how to process each node
  void processNode(xmlNodePtr theNode);
  /// Overloaded method. Defines if the given node should be parsed recursively
  bool processChildren(xmlNodePtr theNode);
  /// Extracts xml definition of the given node and it's children
  std::string dumpNode(xmlNodePtr theNode);
  /// Replace all "source" nodes with their content (used before dumping nodes)
  void resolveSourceNodes(xmlNodePtr theNode);

 private:
  std::map<std::string, std::string> myWidgetCache;
  std::map<std::string, std::string> myDescriptionCache;

};

#endif /* CONFIG_WIDGETREADER_H_ */
