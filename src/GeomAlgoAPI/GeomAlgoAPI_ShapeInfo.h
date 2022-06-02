// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#ifndef GeomAlgoAPI_ShapeInfo_H_
#define GeomAlgoAPI_ShapeInfo_H_

#include "GeomAlgoAPI.h"

#include <GeomAPI_Shape.h>
#include <GeomAPI_Vertex.h>
#include <GeomAPI_Pnt.h>
#include <GeomAPI_Edge.h>
#include <GeomAPI_Face.h>

/// Keeps values of different possible types, used in python shapeInfo.
class GeomAlgoAPI_InfoValue
{
  int myType;        //< type stored in this Value
  std::string myStr; //< type 0
  double myDouble;   //< type 1
  bool myBool;       //< type 2

public:
  GeomAlgoAPI_InfoValue() : myType(-1) {} // invalid case
  GeomAlgoAPI_InfoValue(const char* theVal) : myType(0), myStr(theVal) {}
  GeomAlgoAPI_InfoValue(const std::string theVal) : myType(0), myStr(theVal) {}
  GeomAlgoAPI_InfoValue(const double theVal) : myType(1), myDouble(theVal) {}
  GeomAlgoAPI_InfoValue(const bool theVal) : myType(2), myBool(theVal) {}

  const int& type() const { return myType; }
  const std::string& string() const { return myStr; }
  const double& real() const { return myDouble; }
  const bool& boolean() const { return myBool; }
};

/// \class GeomAlgoAPI_ShapeInfo
/// \ingroup DataAlgo
/// \brief Provides information about shapes in textual (HTML for Information Panel)  or
///        a list of parameters order (for python "shapeInfo" command).
///
/// At least the following geometrical items are supported:
/// 0D: Vertex,
/// 1D : Line segment, Polygon, Circle, Arc, Ellipse, Arc of ellipse,
/// 2D : Rectangle, Polygon, Disc with circular or elliptical base, Sphere, Cylinder, Cone, Torus
///
/// It is initialized by a shape and fors the needed output depending on the requested format.
class GeomAlgoAPI_ShapeInfo
{
  /// Keeps the shape, source of the produced information.
  std::shared_ptr<GeomAPI_Shape> myShape;
  std::string myShapeType; //< detected shape type
public:

  /// Initializes a parameters input by an input shape.
  GEOMALGOAPI_EXPORT GeomAlgoAPI_ShapeInfo(const std::shared_ptr<GeomAPI_Shape> theShape)
   : myShape(theShape) {}

  /// A translator to the current language is needed for HTML export.
  /// It is provided by a caller of this class.
  class Translator
  {
  public:
    Translator() {}
    GEOMALGOAPI_EXPORT virtual std::string translate(const char* theSource) { return theSource; }
  };

  /// Returns an HTML text of a shape info.
  GEOMALGOAPI_EXPORT std::string html(Translator* theTranslator);

  /// Returns the detected shape type, or empty string.
  GEOMALGOAPI_EXPORT const std::string& shapeType() { return myShapeType; }

  /// Sets the shape type
  void setShapeType(const std::string theType) { myShapeType = theType; }

  /// Returns a list of shape information values.
  GEOMALGOAPI_EXPORT std::list<GeomAlgoAPI_InfoValue> values();

private:
  /// Possible types of export values
  enum InfoType {
    TYPE_HTML,
    TYPE_VALS
  };

  /// Allows to input values in a defined format, then - to export them.
  class Values
  {
    /// A type of stored shape info.
    InfoType myType;
    /// An HTML string container.
    std::string myStr;
    Translator* myTr; //< an external translator
    /// A container of values: type of the value -> value
    std::list<GeomAlgoAPI_InfoValue> myVals;
  public:

    /// Initialization of an empty container, default translator
    Values(const InfoType theType);

    /// Returns HTML representation of values
    const std::string& html() const { return myStr; }

    void setTranslator(Translator* theTranslator) { myTr = theTranslator; }
    Translator* translator() { return myTr; }

    /// Returns list of values representation.
    const std::list<GeomAlgoAPI_InfoValue>& values() const { return myVals; }

    /// Adds next value
    Values& operator<< (double theValue);
    /// Adds next value
    Values& operator<< (bool theValue);
    /// Adds string
    Values& operator<< (const char* theStr);
    /// Adds point coordinates
    void addPoint(const char* theTitle, const GeomPointPtr& thePoint);
    /// Adds directon coordinates
    void addDirection(const char* theTitle, const GeomDirPtr& thePoint);
    /// Adds named value
    void addNamedValue(const char* theName, const double theValue);
    /// Adds named value
    void addNamedValue(const char* theName, const bool theValue);
    /// Adds a name (title) of the group
    void addGroupName(const char* theName);
  };

  /// Gets information from the shape
  void processShape(Values& theVals);

  /// Gets particual information from the edge
  void processEdge(Values& theVals, GeomEdgePtr theEdge);

  /// Gets particual information from face
  void processFace(Values& theVals, GeomFacePtr theFace);
};

#endif
