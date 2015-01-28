// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

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

/*!
 * Checks is the given node is attribute (widget) node.
 */
CONFIG_EXPORT bool isWidgetNode(xmlNodePtr theNode);

/*!
 * Every xml node has child. Even if there is no explicit
 * child nodes libxml gives the "Text node" as child.
 *
 * This method checks if real child nodes exist in the
 * given node.
 */
CONFIG_EXPORT bool hasChild(xmlNodePtr theNode);

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
