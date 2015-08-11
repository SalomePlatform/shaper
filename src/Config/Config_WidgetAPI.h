// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

/*
 * Config_WidgetAPI.h
 *
 *  Created on: Apr 1, 2014
 *      Author: sbh
 */

#ifndef CONFIG_WIDGETAPI_H_
#define CONFIG_WIDGETAPI_H_

#include <Config_def.h>

#include <cstdarg>
#include <list>
#include <string>

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
 * \class Config_WidgetAPI
 * \ingroup Config
 * \brief Provides low-level API for WidgetFactory for reading xml definitions of widgets
 */
class CONFIG_EXPORT Config_WidgetAPI
{
 public:
  virtual ~Config_WidgetAPI();

  //! Returns name of widget's node (attribute)
  std::string widgetType() const;
  //! Returns true if widget has container type, which means it able to contain other widgets
  bool isGroupBoxWidget() const;
  //! Returns true if widget has page type;
  //! Page is container widget with combo box control to switch between pages
  bool isPagedWidget() const;

  //! Returns id of current widget
  std::string widgetId() const;
  //! Returns icon of current widget
  std::string widgetIcon() const;
  //! Returns text for label of current widget
  std::string widgetLabel() const;
  //! Returns text for tooltip of current widget
  std::string widgetTooltip() const;
  //! Returns a custom property of current widget
  std::string getProperty(const char* thePropName) const;

  /*! Returns a list of attributes.
   *  If theRole is 0 then returns all attributes.
   *  If theRole is "main" then returns widgetId().
   */
  std::list<std::string> getAttributes(const std::string& theRole = std::string()) const;
  //! Returns a custom property of attribute
  std::string getAttributeProperty(const std::string& theAttribute,
                                   const std::string& thePropName) const;

  /*! Checks if the XML representation of widget has given attribute,
   *  if yes - returns it's bool value, if no, or if the value can not
   *  be converted to bool - returns theDefault.
   *  \param theAttributeName attribute to check
   *  \param theDefault default value on bad data
   *  \return the boolean result
   */
  bool getBooleanAttribute(const char* theAttributeName, bool theDefault) const;

 protected:
  /// These fields are accessible for ModuleBase_WidgetFactory only
  Config_WidgetAPI(const std::string& theRawXml);
  //! Pass to the next (sibling) node of widget's xml definition. If impossible, returns false
  bool toNextWidget();
  //! Pass into the child node of widget's xml definition. If impossible, returns false
  bool toChildWidget();
  //! Pass into the parent node of widget's xml definition. If impossible, returns false
  bool toParentWidget();

  std::list<xmlNodePtr> attributes() const;

 private:
  xmlDocPtr myDoc; //!< Pointer to the root of widget's xml definition
  xmlNodePtr myCurrentNode; //!< Pointer to the current node in the widget's xml definition

  friend class ModuleBase_WidgetFactory;
};

#endif /* CONFIG_WIDGETAPI_H_ */
