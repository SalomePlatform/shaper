/*
 * GeomAlgoAPI_Tools.h
 *
 *  Created on: May 18, 2015
 *      Author: spo
 */

#ifndef GEOMALGOAPI_TOOLS_H_
#define GEOMALGOAPI_TOOLS_H_

#include <GeomAlgoAPI.h>

#include <string>

namespace GeomAlgoAPI_Tools {

class GEOMALGOAPI_EXPORT Localizer
{
public:
  Localizer();
  ~Localizer();
private:
  std::string myCurLocale;
};

} // GeomAlgoAPI_Tools

#endif /* GEOMALGOAPI_TOOLS_H_ */
