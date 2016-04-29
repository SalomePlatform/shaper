// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAPI_Wire.cpp
// Created:     28 April 2016
// Author:      Dmitry Bobylev

#include "GeomAPI_Wire.h"

#include <BRep_Builder.hxx>
#include <TopoDS_Wire.hxx>

//==================================================================================================
GeomAPI_Wire::GeomAPI_Wire()
{
  TopoDS_Wire* aWire = new TopoDS_Wire();

  BRep_Builder aBuilder;
  aBuilder.MakeWire(*aWire);

  this->setImpl(aWire);
}
