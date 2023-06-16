// Copyright (C) 2013-2023  CEA/DEN, EDF R&D
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
// Author : Nathalie Gore (OpenCascade), Frederic Pons (OpenCascade)

#ifndef __XAO_XAO_HXX__
#define __XAO_XAO_HXX__

#include <string>
#include <list>

#include "XAO.hxx"
#include "XAO_Exception.hxx"
#include "XAO_XaoUtils.hxx"
#include "XAO_Geometry.hxx"

#ifdef WIN32
#pragma warning(disable:4290) // Warning Exception ...
#pragma warning(disable:4251) // Warning dll-interface ...
#endif

namespace XAO
{
    class Group;
    class Field;
    class IntegerField;
    class DoubleField;
    class BooleanField;
    class StringField;

    /**
     * @class Xao
     * The Xao class describes the XAO format.
     */
    class XAO_EXPORT Xao
    {
    public:
        /**
         * Default constructor.
         */
        Xao();
        /**
         * Constructor with author and version.
         * \param author the author of the file.
         * \param version the version of the XAO format.
         */
        Xao(const std::string& author, const std::string& version);
        /**
         * Destructor.
         */
        virtual ~Xao();

        /**
         * Gets the author of the file.
         * \return the author of the file.
         */
        const std::string getAuthor() const
        {
            return m_author;
        }
        /**
         * Sets the author of the file.
         * \param author the author to set.
         */
        void setAuthor(const std::string& author)
        {
            m_author = author;
        }

        /**
         * Gets the version of the file.
         * \return the version of the file.
         */
        const std::string getVersion() const
        {
            return m_version;
        }
        /**
         * Sets the version of the file.
         * \param version the version to set.
         */
        void setVersion(const std::string& version)
        {
            m_version = version;
        }

        //
        // Geometry
        //

        /**
         * Gets the geometry.
         * \return the geometry.
         */
        Geometry* getGeometry() const
        {
            return m_geometry;
        }
        /**
         * Sets the geometry.
         * \param geometry the geometry to set.
         */
        void setGeometry(Geometry* geometry) 
        {
            if (m_geometry != NULL)
                throw XAO_Exception("Geometry already set.");
            m_geometry = geometry;
            m_geometry->setReadOnly();
        }

        //
        // Groups
        //

        /**
         * Gets the number of groups.
         * \return the number of groups.
         */
        int countGroups() const;
        /**
         * Gets a group.
         * \param index the index of the wanted group.
         * \return the group.
         */
        Group* getGroup(int index) ;
        /**
         * Adds a group.
         * \param dim the dimension of the group.
         * \param name the name of the group.
         * \return the created group.
         */
        Group* addGroup(XAO::Dimension dim, const std::string& name = std::string("")) ;
        /**
         * Removes a group.
         * \param group the group to remove.
         * \return true if the group has been removed, false otherwise.
         */
        bool removeGroup(Group* group);

        //
        // Fields
        //

        /**
         * Gets the number of fields.
         * \return the number of fields.
         */
        int countFields() const;

        /**
         * Gets the type of a field.
         * \param index the index of the wanted field.
         * \return the type of the field.
         */
        XAO::Type getFieldType(int index) ;

        /**
         * Gets a field.
         * \param index the index of the wanted field.
         * \return the field.
         */
        Field* getField(int index) ;

        BooleanField* getBooleanField(int index) ;
        DoubleField* getDoubleField(int index) ;
        IntegerField* getIntegerField(int index) ;
        StringField* getStringField(int index) ;

        /**
         * Adds a field.
         * \param type the type of the field.
         * \param dim the dimension of the field.
         * \param nbComponents the number of components in the field.
         * \param name the name of the field.
         * \return the created field.
         */
        Field* addField(XAO::Type type, XAO::Dimension dim, int nbComponents,
                const std::string& name = std::string(""))
        ;

        BooleanField* addBooleanField(XAO::Dimension dim, int nbComponents,
                const std::string& name = std::string("")) ;
        IntegerField* addIntegerField(XAO::Dimension dim, int nbComponents,
                const std::string& name = std::string("")) ;
        DoubleField* addDoubleField(XAO::Dimension dim, int nbComponents,
                const std::string& name = std::string("")) ;
        StringField* addStringField(XAO::Dimension dim, int nbComponents,
                const std::string& name = std::string("")) ;

        /**
         * Removes a field.
         * \param field the field to remove.
         * \return true if the field has been removed, false otherwise.
         */
        bool removeField(Field* field);

        //
        // Import / Export
        //
        /**
         * Exports this XAO object to a file.
         * \param fileName the name of the file to create.
         * \param shapeFileName if not empty, export the shape to this external file.
         * \return true is the export is successful.
         */
        bool exportXAO(const std::string& fileName, const std::string& shapeFileName);
        /**
         * Gets the XML corresponding to this XAO.
         * \return the XML as a string.
         */
        const std::string getXML();

        /**
         * Imports an XAO file into this object.
         * \param fileName the name of the file to import.
         * \return true if the import is successful.
         */
        bool importXAO(const std::string& fileName);
        /**
         * Sets an XML describing an XAO format to this object.
         * \param xml the XML to set.
         * \return true if the import is successful.
         */
        bool setXML(const std::string& xml);

    private:
        void checkGeometry() const ;
        void checkGroupIndex(int index) const ;
        void checkFieldIndex(int index) const ;
        void checkGroupDimension(XAO::Dimension dim) const ;

    private:
        /** The author of the file. */
        std::string m_author;
        /** The version of the file. */
        std::string m_version;
        /** The geometry. */
        Geometry* m_geometry;
        /** The list of groups. */
        std::list<Group*> m_groups;
        /** The list of fields. */
        std::list<Field*> m_fields;
    };

}

#endif
