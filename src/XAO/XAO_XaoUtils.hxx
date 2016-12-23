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

#ifndef __XAO_UTILS_HXX__
#define __XAO_UTILS_HXX__

#include <sstream>
#include <string>
#include <exception>

#include "XAO.hxx"
#include "XAO_Exception.hxx"

#ifdef WIN32
#pragma warning(disable:4290) // Warning Exception ...
#endif

namespace XAO
{
    /**
     * @enum Format
     */
    enum Format
    {
        BREP,
        STEP
    };

    /**
     * @enum Dimension
     */
    enum Dimension
    {
        VERTEX = 0,//!< VERTEX
        EDGE = 1,  //!< EDGE
        FACE = 2,  //!< FACE
        SOLID = 3, //!< SOLID
        WHOLE = -1 //!< WHOLE
    };

    /**
     * @enum Type
     */
    enum Type
    {
        BOOLEAN = 0,//!< BOOLEAN
        INTEGER = 1,//!< INTEGER
        DOUBLE = 2, //!< DOUBLE
        STRING = 3  //!< STRING
    };

    /**
     * \class XaoUtils
     * Utilities class to convert types.
     */
    class XAO_EXPORT XaoUtils
    {
    public:
        /**
         * Converts an integer into a string.
         * \param value the integer to convert.
         * \return the string.
         */
        static const std::string intToString(const int& value);

        /**
         * Converts a string into an integer.
         * \param value the string to convert.
         * \return the integer value.
         * \throw XAO_Exception if value cannot be converted to string.
         */
        static const int stringToInt(const std::string& value) throw(XAO_Exception);

        /**
         * Converts a double into a string.
         * \param value the double to convert.
         * \return the string.
         */
        static const std::string doubleToString(const double& value);
        /**
         * Converts a string into a double.
         * \param value the string to convert.
         * \return the double value.
         * \throw XAO_Exception if value cannot be converted to string.
         */
        static const double stringToDouble(const std::string& value) throw(XAO_Exception);

        /**
         * Converts a boolean into a string.
         * \param value the boolean to convert.
         * \return the string.
         */
        static const std::string booleanToString(const bool& value);
        /**
         * Converts a string into a boolean.
         * \param value the string to convert.
         * \return the boolean value.
         * \throw XAO_Exception if value cannot be converted to boolean.
         * \note accepted values are "true", "1", "false", "0".
         */
        static const bool stringToBoolean(const std::string& value) throw(XAO_Exception);

        /**
         * Converts a Dimension to string.
         * \param dimension the Dimension to convert.
         * \return the dimension as a string.
         * \throw XAO_Exception
         */
        static const std::string dimensionToString(const XAO::Dimension& dimension) throw(XAO_Exception);

        /**
         * Converts a string into a Dimension.
         * \param dimension the dimension as a string.
         * \return the converted Dimension.
         * \throw XAO_Exception if dimension cannot be converted.
         */
        static const XAO::Dimension stringToDimension(const std::string& dimension) throw(XAO_Exception);

        /**
         * Converts a Type to string.
         * \param type the Type to convert.
         * \return the Type as a string.
         * \throw XAO_Exception
         */
        static const std::string fieldTypeToString(const XAO::Type& type) throw(XAO_Exception);

        /**
         * Converts a string into a Type.
         * \param type the Type as a string.
         * \return the converted Type.
         * \throw XAO_Exception if type cannot be converted.
         */
        static const XAO::Type stringToFieldType(const std::string& type) throw(XAO_Exception);

        /**
         * Converts a Format to string.
         * \param format the Format to convert.
         * \return the Format as a string.
         * \throw XAO_Exception
         */
        static const std::string shapeFormatToString(const XAO::Format& format) throw(XAO_Exception);

        /**
         * Converts a string into a Format.
         * \param format the Format as a string.
         * \return the converted Format.
         * \throw XAO_Exception if format cannot be converted.
         */
        static const XAO::Format stringToShapeFormat(const std::string& format) throw(XAO_Exception);
    };

    /**
     * @class MsgBuilder
     * MsgBuilder can be use to easily create messages.
     */
    class MsgBuilder
    {
    public:
        /** Constructor. */
        MsgBuilder() {};
        /** Destructor. */
        ~MsgBuilder() {};

#ifndef SWIG
        /** Stream operator. */
        template <typename T>
        MsgBuilder& operator <<(const T& t)
        {
            m_stream << t;
            return *this;
        }

        /**
         * Conversion operator to char*.
         */
        operator const char*() const   { return m_stream.str().c_str(); }
#endif

    private :
        std::stringstream m_stream;
    };

}



#endif /* __XAO_UTILS_HXX__ */
