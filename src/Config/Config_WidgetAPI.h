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

class CONFIG_EXPORT Config_WidgetAPI
{
 public:
  Config_WidgetAPI(std::string theRawXml);
  virtual ~Config_WidgetAPI();

  bool toNextWidget();
  bool toChildWidget();
  bool toParentWidget();

  std::string widgetType() const;
  bool isContainerWidget() const;
  bool isPagedWidget() const;

  std::string widgetId() const;
  std::string widgetIcon() const;
  std::string widgetLabel() const;
  std::string widgetTooltip() const;

  std::string getProperty(const char* thePropName) const;

 private:
  xmlDocPtr myDoc;
  xmlNodePtr myCurrentNode;

};

#endif /* CONFIG_WIDGETAPI_H_ */
