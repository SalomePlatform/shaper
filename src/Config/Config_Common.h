/*
 * Config_Common.h
 *
 *  Created on: Apr 17, 2014
 *      Author: sbh
 */

#include <libxml/parser.h>
#include <libxml/tree.h>

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
static bool isElementNode(xmlNodePtr theNode)
{
  return theNode->type == XML_ELEMENT_NODE;
}

/*
 * Returns true if theNode is XML node with a given name.

 * Please note that this function should be called with NULL last argument.
 * In example: isNode(aNode, "type1", ["type2", ...], NULL);
 * ", NULL" is required to use unlimited number of arguments.
 * TODO(sbh): find a way to simplify calling this method.
 */
static bool isNode(xmlNodePtr theNode, const char* theNodeName, ...)
{
  bool result = false;
  const xmlChar* aName = theNode->name;
  if (!aName || !isElementNode(theNode)) {
    return false;
  }
  if (!xmlStrcmp(aName, (const xmlChar *) theNodeName)) {
    return true;
  }
  va_list args; // define argument list variable
  va_start(args, theNodeName); // init list; point to last defined argument
  while(true) {
    char *anArg = va_arg (args, char*); // get next argument
    if (anArg == NULL)
      break;
    if (!xmlStrcmp(aName, (const xmlChar *) anArg)) {
      va_end(args); // cleanup the system stack
      return true;
    }
  }
  va_end(args); // cleanup the system stack
  return false;
}


/*
 * Every xml node has child. Even if there is no explicit
 * child nodes libxml gives the "Text node" as child.
 *
 * This method checks if real child nodes exist in the
 * given node.
 */
static bool hasChild(xmlNodePtr theNode)
{
  xmlNodePtr aNode = theNode->children;
  for(; aNode; aNode = aNode->next) {
    if (isElementNode(theNode)) {
      return true;
    }
  }
  return false;
}
