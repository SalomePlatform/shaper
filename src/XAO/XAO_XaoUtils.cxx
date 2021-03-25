// Copyright (C) 2013-2021  CEA/DEN, EDF R&D
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


std::string XaoUtils::intToString(int value)
{
    std::ostringstream str;
    str << value;
    return str.str();
}

int XaoUtils::stringToInt(const std::string& value)
{
    int res;
    std::istringstream convert(value);
    if ( !(convert >> res) )
        throw XAO_Exception(MsgBuilder() << "Cannot convert '" << value << "' to integer.");
    return res;
}

std::string XaoUtils::doubleToString(double value)
{
    std::ostringstream str;
    str << value;
    return str.str();
}

double XaoUtils::stringToDouble(const std::string& value)
{
    double res;
    std::istringstream convert(value);
    if ( !(convert >> res) )
        throw XAO_Exception(MsgBuilder() << "Cannot convert '" << value << "' to double.");
    return res;
}

std::string XaoUtils::booleanToString(bool value)
{
    return value ? "true" : "false";
}

bool XaoUtils::stringToBoolean(const std::string& value)
{
    if (value == "true" || value == "1")
        return true;
    if (value == "false" || value == "0")
        return false;

    throw XAO_Exception(MsgBuilder() << "Invalid boolean value: " << value);
}

std::string XaoUtils::dimensionToString(XAO::Dimension dimension)
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

XAO::Dimension XaoUtils::stringToDimension(const std::string& dimension)
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

std::string XaoUtils::fieldTypeToString(XAO::Type type)
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

XAO::Type XaoUtils::stringToFieldType(const std::string& type)
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

std::string XaoUtils::shapeFormatToString(XAO::Format format)
{
    if (format == XAO::BREP)
        return "BREP";
    if (format == XAO::STEP)
        return "STEP";

    throw XAO_Exception(MsgBuilder() << "Bad format: " << format);
}

XAO::Format XaoUtils::stringToShapeFormat(const std::string& format)
{
    if (format == "BREP")
        return XAO::BREP;
    if (format == "STEP")
        return XAO::STEP;

    throw XAO_Exception(MsgBuilder() << "Bad format: " << format);
}
