// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    GeomAlgoAPI_Tools.cpp
// Created: May 18, 2015
// Author:  Sergey POKHODENKO

#include "GeomAlgoAPI_Tools.h"

#include <clocale>

#include <TCollection_AsciiString.hxx>
#include <OSD_Path.hxx>

using namespace GeomAlgoAPI_Tools;

Localizer::Localizer()
{
  myCurLocale = std::setlocale(LC_NUMERIC, 0);
  std::setlocale(LC_NUMERIC, "C");
}

Localizer::~Localizer()
{
  std::setlocale(LC_NUMERIC, myCurLocale.c_str());
}


std::string File_Tools::extension(const std::string& theFileName)
{
  // retrieve the file and plugin library names
  TCollection_AsciiString aFileName(theFileName.c_str());
  OSD_Path aPath(aFileName);
  TCollection_AsciiString anExtension = aPath.Extension();
  // TCollection_AsciiString are numbered from 1
  anExtension = anExtension.SubString(2, anExtension.Length());
  anExtension.UpperCase();
  return anExtension.ToCString();
}

std::string File_Tools::name(const std::string& theFileName)
{
  // retrieve the file and plugin library names
  TCollection_AsciiString aFileName(theFileName.c_str());
  OSD_Path aPath(aFileName);
  return aPath.Name().ToCString();
}
