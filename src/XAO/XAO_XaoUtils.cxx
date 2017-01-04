// Copyright (C) 2013-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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
// Author : Frederic Pons (OpenCascade)

#include <sstream>

#include "XAO_Exception.hxx"
#include "XAO_Xao.hxx"
#include "XAO_XaoUtils.hxx"

using namespace XAO;


const std::string XaoUtils::intToString(const int& value)
{
    std::ostringstream str;
    str << value;
    return str.str();
}

const int XaoUtils::stringToInt(const std::string& value)
throw(XAO_Exception)
{
    int res;
    std::istringstream convert(value);
    if ( !(convert >> res) )
        throw XAO_Exception(MsgBuilder() << "Cannot convert '" << value << "' to integer.");
    return res;
}

const std::string XaoUtils::doubleToString(const double& value)
{
    std::ostringstream str;
    str << value;
    return str.str();
}

const double XaoUtils::stringToDouble(const std::string& value)
throw(XAO_Exception)
{
    double res;
    std::istringstream convert(value);
    if ( !(convert >> res) )
        throw XAO_Exception(MsgBuilder() << "Cannot convert '" << value << "' to double.");
    return res;
}

const std::string XaoUtils::booleanToString(const bool& value)
{
    if (value)
        return "true";
    return "false";
}

const bool XaoUtils::stringToBoolean(const std::string& value)
throw(XAO_Exception)
{
    if (value == "true" || value == "1")
        return true;
    if (value == "false" || value == "0")
        return false;

    throw XAO_Exception(MsgBuilder() << "Invalid boolean value: " << value);
}

const std::string XaoUtils::dimensionToString(const XAO::Dimension& dimension)
throw(XAO_Exception)
{
    if (dimension == XAO::VERTEX)
        return "vertex";
    if (dimension == XAO::EDGE)
        return "edge";
    if (dimension == XAO::FACE)
        return "face";
    if (dimension == XAO::SOLID)
        return "solid";
    if (dimension == XAO::WHOLE)
        return "whole";

    throw XAO_Exception(MsgBuilder() << "Bad dimension: " << dimension);
}

const XAO::Dimension XaoUtils::stringToDimension(const std::string& dimension)
throw(XAO_Exception)
{
    if (dimension == "vertex")
        return XAO::VERTEX;
    if (dimension == "edge")
        return XAO::EDGE;
    if (dimension == "face")
        return XAO::FACE;
    if (dimension == "solid")
        return XAO::SOLID;
    if (dimension == "whole")
        return XAO::WHOLE;

    throw XAO_Exception(MsgBuilder() << "Bad dimension: " << dimension);
}

const std::string XaoUtils::fieldTypeToString(const XAO::Type& type)
throw(XAO_Exception)
{
    if (type ==XAO:: BOOLEAN)
        return "boolean";
    if (type == XAO::INTEGER)
        return "integer";
    if (type == XAO::DOUBLE)
        return "double";
    if (type == XAO::STRING)
        return "string";

    throw XAO_Exception(MsgBuilder() << "Bad type: " << type);
}

const XAO::Type XaoUtils::stringToFieldType(const std::string& type)
throw(XAO_Exception)
{
    if (type == "boolean")
        return XAO::BOOLEAN;
    if (type == "integer")
        return XAO::INTEGER;
    if (type == "double")
        return XAO::DOUBLE;
    if (type == "string")
        return XAO::STRING;

    throw XAO_Exception(MsgBuilder() << "Bad type: " << type);
}

const std::string XaoUtils::shapeFormatToString(const XAO::Format& format)
throw(XAO_Exception)
{
    if (format == XAO::BREP)
        return "BREP";
    if (format == XAO::STEP)
        return "STEP";

    throw XAO_Exception(MsgBuilder() << "Bad format: " << format);
}

const XAO::Format XaoUtils::stringToShapeFormat(const std::string& format)
throw(XAO_Exception)
{
    if (format == "BREP")
        return XAO::BREP;
    if (format == "STEP")
        return XAO::STEP;

    throw XAO_Exception(MsgBuilder() << "Bad format: " << format);
}
