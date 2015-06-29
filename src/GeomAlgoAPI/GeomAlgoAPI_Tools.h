// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    GeomAlgoAPI_Tools.h
// Created: May 18, 2015
// Author:  Sergey POKHODENKO

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

class File_Tools {
public:
  /**
   * Returns an extension of theFileName
   */
  static std::string extension(const std::string& theFileName);
  /**
   * Returns a name of theFileName
   */
  static std::string name(const std::string& theFileName);
};

} // GeomAlgoAPI_Tools

#endif /* GEOMALGOAPI_TOOLS_H_ */
