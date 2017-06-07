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

#ifndef ATTRIBUTE_MESSAGE_H
#define ATTRIBUTE_MESSAGE_H

#include <Config_def.h>
#include <Config_FeatureMessage.h>
#include <Events_Message.h>

#include <string>
#include <list>

/*!
 * \class Config_AttributeMessage
 * \brief Class to pass an attribute's (widget's) entry info extracted from xml file.
 * \code
 * <widget id="tool_object" concealment="true" obligatory="0"/>
 * \endcode
 */
class Config_AttributeMessage : public Events_Message
{
  std::string myAttributeId; ///< Attribute's unique id
  std::string myFeatureId; ///< Attribute's feature's unique id
  bool myIsObligatory; ///< Required to be set by user, else it's feature is invalid.
  bool myIsConcealment; ///< If true, conceals features used as input
  ///< a list of pairs, if the attribute is placed inside paged containers: (case, switch)
  std::list<std::pair<std::string, std::string> > myCases;

public:
  /// Same event as Config_FeatureMessage::MODEL_EVENT()
  inline static const char* MODEL_EVENT()
  {
    return Config_FeatureMessage::MODEL_EVENT();
  }

  /// Constructor
  CONFIG_EXPORT Config_AttributeMessage(const Events_ID theId, const void* theParent = 0);
  /// Destructor
  CONFIG_EXPORT virtual ~Config_AttributeMessage();

  //Auto-generated getters/setters
  /// Returns attribute's unique id
  CONFIG_EXPORT const std::string& attributeId() const;
  /// Returns attribute's feature's unique id
  CONFIG_EXPORT const std::string& featureId() const;
  /// Returns true if attribute is obligatory:
  /// Required to be set by user, else it's feature is invalid.
  CONFIG_EXPORT bool isObligatory() const;
  /// Returns true if attribute should conceal input features
  CONFIG_EXPORT bool isConcealment() const;
  /// Returns container of ids of pair of a case and switches
  CONFIG_EXPORT const std::list<std::pair<std::string, std::string> >& getCases() const;
  /// Sets ids of pair of a case and switches
  CONFIG_EXPORT void setCases(const std::list<std::pair<std::string, std::string> >& theCases);

  /// Set attribute's unique id
  CONFIG_EXPORT void setAttributeId(const std::string& theId);
  /// Set attribute's feature's unique id
  CONFIG_EXPORT void setFeatureId(const std::string& id);
  /// Set attribute's concealment state
  CONFIG_EXPORT void setConcealment(bool isConcealment);
  /// Set attribute's obligatory state
  CONFIG_EXPORT void setObligatory(bool isObligatory);
};

#endif // ATTRIBUTE_MESSAGE_H
