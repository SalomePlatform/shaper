// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

#ifndef CONFIG_FEATUREREADER_H_
#define CONFIG_FEATUREREADER_H_

#include <Config_def.h>
#include <Config_XMLReader.h>
#include <memory>

#include <string>
#include <list>

class Config_FeatureMessage;

/*!
 * \class Config_FeatureReader
 * \ingroup Config
 * \brief Class to process feature's xml definition.
 */
class Config_FeatureReader : public Config_XMLReader
{
 public:
  /// Constructor
  Config_FeatureReader(const std::string& theXmlFile,
                       const std::string& theLibraryName,
                       const std::string& theDocSection = std::string(),
                       const char* theEventGenerated = 0);
  virtual ~Config_FeatureReader();
  /// Returns list of all features defined in reader's file
  std::list<std::string> features() const;

 protected:
  /// Overloaded method. Defines how to process each node
  virtual void processNode(xmlNodePtr aNode);

  /// Overloaded method. Clears attribute cache on exit from attribute's node
  virtual void cleanup(xmlNodePtr aNode);

  /// Overloaded method. Defines if the given node should be parsed recursively
  virtual bool processChildren(xmlNodePtr aNode);

  /// Fills feature message
  void fillFeature(xmlNodePtr theRoot,
                   const std::shared_ptr<Config_FeatureMessage>& outFeatureMessage);

 private:
  std::string myLibraryName;
  std::string myLibraryDocSection;

  std::list<std::string> myFeatures;
  /// event generated on feature data sending, by default it is Config_FeatureMessage::GUI_EVENT()
  const char* myEventGenerated;
  bool myIsProcessWidgets;
};

#endif /* CONFIG_FEATUREREADER_H_ */
