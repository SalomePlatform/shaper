/*
 * Config_Common.h
 *
 *  Created on: Apr 17, 2014
 *      Author: sbh
 */

#ifndef Config_Common_H_
#define Config_Common_H_

#include "Config_def.h"

#include <string>
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

/*
 * Returns true if theNode is XML ELEMENT node (not a "text" node ie).
 */
CONFIG_EXPORT bool isElementNode(xmlNodePtr theNode);

/*
 * Returns true if theNode is XML node with a given name.

 * Please note that this function should be called with NULL last argument.
 * In example: isNode(aNode, "type1", ["type2", ...], NULL);
 * ", NULL" is required to use unlimited number of arguments.
 * TODO(sbh): find a way to simplify calling this method.
 */
CONFIG_EXPORT bool isNode(xmlNodePtr theNode, const char* theNodeName, ...);

/*
 * Every xml node has child. Even if there is no explicit
 * child nodes libxml gives the "Text node" as child.
 *
 * This method checks if real child nodes exist in the
 * given node.
 */
CONFIG_EXPORT bool hasChild(xmlNodePtr theNode);

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

#endif
