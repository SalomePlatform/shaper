// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    GeomAlgoAPI_Tools.h
// Created: May 18, 2015
// Author:  Sergey POKHODENKO

#ifndef GEOMALGOAPI_TOOLS_H_
#define GEOMALGOAPI_TOOLS_H_

#include <GeomAlgoAPI.h>

#include <string>

namespace GeomAlgoAPI_Tools {

/** \class Localizer
 *  \ingroup DataAlgo
 *  \brief Localizer tool.
 */
class Localizer
{
public:
  GEOMALGOAPI_EXPORT Localizer();
  GEOMALGOAPI_EXPORT ~Localizer();
private:
  std::string myCurLocale;
};

/** \class File_Tools
 *  \ingroup DataAlgo
 *  \brief File tool.
 */
class File_Tools {
public:
  /**
   * Returns an extension of theFileName
   */
  GEOMALGOAPI_EXPORT static std::string extension(const std::string& theFileName);
  /**
   * Returns a name of theFileName
   */
  GEOMALGOAPI_EXPORT static std::string name(const std::string& theFileName);
};

} // GeomAlgoAPI_Tools

#endif /* GEOMALGOAPI_TOOLS_H_ */
