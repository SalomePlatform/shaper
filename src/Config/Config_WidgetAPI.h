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
 * \brief Provides low-level API for WidgetFactory for reading xml definitions of widgets
 */
class CONFIG_EXPORT Config_WidgetAPI
{
 public:
  virtual ~Config_WidgetAPI();

  std::string widgetType() const;
  bool isContainerWidget() const;
  bool isPagedWidget() const;

  std::string widgetId() const;
  std::string widgetIcon() const;
  std::string widgetLabel() const;
  std::string widgetTooltip() const;

  std::string getProperty(const char* thePropName) const;

  /// Checks if the XML representation of widget has given attribute,
  /// if yes - returns it's bool value, if no, or if the value can not
  /// be converted to bool - returns theDefault.
  /// \param theAttributeName attribute to check
  /// \param theDefault default value on bad data
  /// \return the boolean result
  bool getBooleanAttribute(const char* theAttributeName, bool theDefault) const;

  bool isComputedDefault() const;

 protected:
  /// These fields are accessible for ModuleBase_WidgetFactory only
  Config_WidgetAPI(std::string theRawXml);
  bool toNextWidget();
  bool toChildWidget();
  bool toParentWidget();

 private:
  xmlDocPtr myDoc;
  xmlNodePtr myCurrentNode;

  friend class ModuleBase_WidgetFactory;
};

#endif /* CONFIG_WIDGETAPI_H_ */
