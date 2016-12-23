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

#ifndef __XAO_XAOEXPORTER_HXX__
#define __XAO_XAOEXPORTER_HXX__

#include "XAO.hxx"
#include "XAO_Xao.hxx"
#include "XAO_Geometry.hxx"
#include "XAO_Group.hxx"
#include "XAO_Field.hxx"

#ifdef WIN32
#pragma warning(disable:4290) // Warning Exception ...
#endif

namespace XAO
{
    /**
     * @class XaoExporter
     * Util class for import/export XAO.
     */
    class XAO_EXPORT XaoExporter
    {
    public:
        /**
         * Saves the XAO object to a file.
         * @param xaoObject the object to export.
         * @param fileName the path of the file to create.
         * @return true if the export was successful, false otherwise.
         */
        static const bool saveToFile(Xao* xaoObject, const std::string& fileName)
        throw (XAO_Exception);

        /**
         * Saves the XAO object to a XML string.
         * @param xaoObject the object to export.
         * @return the XML string.
         */
        static const std::string saveToXml(Xao* xaoObject)
        throw (XAO_Exception);

        /**
         * Reads a XAO object from a file.
         * @param fileName the path of the file to read.
         * @param xaoObject the XAO object.
         * @return true if the XAO object was read successful, false otherwise.
         */
        static const bool readFromFile(const std::string& fileName, Xao* xaoObject)
        throw (XAO_Exception);

        /**
         * Reads a XAO object from an XML string.
         * @param xml the XML string.
         * @param xaoObject the XAO object.
         * @return true if the XAO object was read successful, false otherwise.
         */
        static const bool setXML(const std::string& xml, Xao* xaoObject)
        throw (XAO_Exception);
    };
}


#endif /* __XAO_XAOEXPORTER_HXX__ */
