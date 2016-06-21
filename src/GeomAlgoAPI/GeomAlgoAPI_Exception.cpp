// Copyright (C) 2014-2016 CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_Exception.cpp
// Created:     14 April 2016
// Author:      Clarisse Genrault (CEA)

#include <GeomAlgoAPI_Exception.h>

//=================================================================================================
GeomAlgoAPI_Exception::GeomAlgoAPI_Exception(std::string theMessageError)
: myMessageError(theMessageError)
{
}

//=================================================================================================
GeomAlgoAPI_Exception::~GeomAlgoAPI_Exception() throw()
{
}

//=================================================================================================
const char* GeomAlgoAPI_Exception::what() const throw()
{ 
  return myMessageError.c_str();
}