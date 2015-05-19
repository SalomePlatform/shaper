/*
 * ExchangePlugin_Tools.h
 *
 *  Created on: May 15, 2015
 *      Author: spo
 */

#ifndef EXCHANGEPLUGIN_TOOLS_H_
#define EXCHANGEPLUGIN_TOOLS_H_

#include <ExchangePlugin.h>

#include <list>
#include <string>

/**\class ExchangePlugin_Tools
 * \ingroup Plugins
 * \brief Internal tools for the plugin.
 */
class EXCHANGEPLUGIN_EXPORT ExchangePlugin_Tools {
public:
  /// Splits theString using theDelimiter.
  static std::list<std::string> split(const std::string& theString,
                                      char theDelimiter);

};

#endif /* EXCHANGEPLUGIN_TOOLS_H_ */
