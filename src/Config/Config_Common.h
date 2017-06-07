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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

/*
 * Config_Common.h
 *
 *  Created on: Apr 17, 2014
 *      Author: sbh
 */

#ifndef CONFIG_COMMON_H_
#define CONFIG_COMMON_H_

#include "Config_def.h"

#include <string>
#include <list>
#include <stdarg.h>

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
 * Returns true if theNode is XML ELEMENT node (not a "text" node ie).
 */
CONFIG_EXPORT bool isElementNode(xmlNodePtr theNode);

/*!
 * Returns true if theNode is XML node with a given name.

 * Please note that this function should be called with NULL last argument.
 * In example: isNode(aNode, "type1", ["type2", ...], NULL);
 * ", NULL" is required to use unlimited number of arguments.
 * TODO(sbh): find a way to simplify calling this method.
 */
CONFIG_EXPORT bool isNode(xmlNodePtr theNode, const char* theNodeName, ...);

//#define isNode(p) _isNode(p, NULL)

/*!
 * Checks if the given node is attribute node.
 * Attribute node represents a widget, that is able to store/restore
 * values from the model. Actually it's every widget, displayed
 * in the XGUI_PropertyPanel, except paged containers (toolbox, switch/case).
 */
CONFIG_EXPORT bool isAttributeNode(xmlNodePtr theNode);

/*!
 * Checks if the given node is widget node.
 * Widget nodes are attribute node + paged containers nodes.
 */
CONFIG_EXPORT bool isWidgetNode(xmlNodePtr theNode);

CONFIG_EXPORT bool isCaseNode(xmlNodePtr theNode);

/*!
 * Every xml node has child. Even if there is no explicit
 * child nodes libxml gives the "Text node" as child.
 *
 * This method checks if real child nodes exist in the
 * given node.
 */
CONFIG_EXPORT bool hasChild(xmlNodePtr theNode);


/*!
 * Checks if the given node has a valid parent.
 */
CONFIG_EXPORT bool hasParent(xmlNodePtr theNode);

/*!
 * Checks if the given node has a valid parent with any of the given node names.
 */
CONFIG_EXPORT bool hasParent(xmlNodePtr theNode, const char* theNodeName, ...);

/*!
 * Checks if the given node has any valid parent in hierarchy with any of the given node names.
 */
CONFIG_EXPORT xmlNodePtr hasParentRecursive(xmlNodePtr theNode, const char* theNodeName, ...);


/*!
 * Returns named property for an id node as std::string and the parameters of the node.
 */
CONFIG_EXPORT bool getParametersInfo(xmlNodePtr theNode, std::string& outPropertyId,
                                     std::list<std::string>& outValidatorParameters);

/*!
 \brief Convert the given parameter to the platform-specific library name.

 The function appends platform-specific prefix (lib) and suffix (.dll/.so)
 to the library file name.
 For example, if \a str = "mylib", "libmylib.so" is returned for Linux and
 mylib.dll for Windows.

 \param str short library name
 \return full library name
 */
CONFIG_EXPORT std::string library(const std::string& theLibName);

/*!
 * Returns named property for a given node as std::string.
 */
CONFIG_EXPORT std::string getProperty(xmlNodePtr theNode, const char* thePropName);

/*!
 * Returns content of the node as std::string if it is exists.
 */
CONFIG_EXPORT std::string getContent(xmlNodePtr theNode);

/*!
 * Returns normalized (lower case) named property for a given node as std::string.
 */
std::string getNormalizedProperty(xmlNodePtr theNode, const char* thePropName);

/*!
 * Checks if the given XML node has the given attribute,
 * if yes - returns it's bool value, if no, or if the value can not
 * be converted to bool - returns theDefault value.
 * \param theAttributeName attribute to check
 * \param theDefault default value on bad data
 * \return the boolean result
 */
CONFIG_EXPORT bool getBooleanAttribute(xmlNodePtr theNode,
                                       const char* theAttributeName,
                                       bool theDefault);

/*!
 * Returns normalized (lower case) version of string.
 * Should be used for case insensitive string matching.
 */
CONFIG_EXPORT std::string normalize(const char* theString);
/*!
 * Returns normalized (lower case) version of string.
 * Should be used for case insensitive string matching.
 */
CONFIG_EXPORT std::string normalize(const std::string& theString);

#endif
