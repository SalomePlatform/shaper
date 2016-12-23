// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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
// Author : Nathalie Gore (OpenCascade)

#ifndef __XAO_GEOMETRY_HXX__
#define __XAO_GEOMETRY_HXX__

#include <string>

#include "XAO.hxx"
#include "XAO_XaoUtils.hxx"
#include "XAO_Exception.hxx"
#include "XAO_GeometricElement.hxx"

#ifdef WIN32
#pragma warning(disable:4290) // Warning Exception ...
#endif


namespace XAO
{
    /**
     * @class Geometry
     * Base class for geometries.
     */
    class XAO_EXPORT Geometry
    {
    protected:
        /**
         * Constructor.
         * @param name the name of the Geometry.
         */
        Geometry(const std::string& name);

    public:

        /**
         * Creates a geometry.
         * @param format the format of the geometry.
         * @return the created geometry.
         */
        static Geometry* createGeometry(const XAO::Format& format) throw (XAO_Exception);

        /**
         * Constructor.
         * Creates a geometry.
         * @name name the name of the geometry.
         * @return the created geometry.
         */
        static Geometry* createGeometry(const XAO::Format& format, const std::string& name)
        throw (XAO_Exception);

        /** Destructor. */
        virtual ~Geometry();

        /**
         * Gets the name of the geometry.
         * @return the name of the geometry.
         */
        const std::string getName()
        {
            return m_name;
        }
        /**
         * Sets the name of the geometry.
         * @param name the name to set.
         */
        void setName(const std::string& name)
        {
            m_name = name;
        }

        /**
         * Gets the format of the geometry.
         * @return the format of the geometry.
         */
        virtual const XAO::Format getFormat() = 0;

        virtual const std::string getShapeString() = 0;
        virtual void setShapeString(const std::string& shape) = 0;

        const int countElements(const XAO::Dimension& dim) const throw (XAO_Exception);
        const int countVertices() const { return m_vertices.getSize(); }
        const int countEdges() const { return m_edges.getSize(); }
        const int countFaces() const { return m_faces.getSize(); }
        const int countSolids() const { return m_solids.getSize(); }

        void setCountVertices(const int& nb) throw (XAO_Exception);
        void setCountEdges(const int& nb) throw (XAO_Exception);
        void setCountFaces(const int& nb) throw (XAO_Exception);
        void setCountSolids(const int& nb) throw (XAO_Exception);

        const std::string getVertexName(const int& index) throw (XAO_Exception) { return m_vertices.getName(index); }
        const std::string getEdgeName(const int& index) throw (XAO_Exception) { return m_edges.getName(index); }
        const std::string getFaceName(const int& index) throw (XAO_Exception) { return m_faces.getName(index); }
        const std::string getSolidName(const int& index) throw (XAO_Exception) { return m_solids.getName(index); }

        void setVertexName(const int& index, const std::string& name) throw (XAO_Exception) { m_vertices.setName(index, name); }
        void setEdgeName(const int& index, const std::string& name) throw (XAO_Exception) { m_edges.setName(index, name); }
        void setFaceName(const int& index, const std::string& name) throw (XAO_Exception) { m_faces.setName(index, name); }
        void setSolidName(const int& index, const std::string& name) throw (XAO_Exception) { m_solids.setName(index, name); }

        const bool hasVertexName(const int& index) throw (XAO_Exception) { return m_vertices.hasName(index); }
        const bool hasEdgeName(const int& index) throw (XAO_Exception) { return m_edges.hasName(index); }
        const bool hasFaceName(const int& index) throw (XAO_Exception) { return m_faces.hasName(index); }
        const bool hasSolidName(const int& index) throw (XAO_Exception) { return m_solids.hasName(index); }

        const std::string getVertexReference(const int& index) throw (XAO_Exception) { return m_vertices.getReference(index); }
        const std::string getEdgeReference(const int& index) throw (XAO_Exception) { return m_edges.getReference(index); }
        const std::string getFaceReference(const int& index) throw (XAO_Exception) { return m_faces.getReference(index); }
        const std::string getSolidReference(const int& index) throw (XAO_Exception) { return m_solids.getReference(index); }
        const std::string getElementReference(const XAO::Dimension& dim, const int& index) throw (XAO_Exception);

        void setVertexReference(const int& index, const std::string& reference) throw (XAO_Exception);
        void setEdgeReference(const int& index, const std::string& reference) throw (XAO_Exception);
        void setFaceReference(const int& index, const std::string& reference) throw (XAO_Exception);
        void setSolidReference(const int& index, const std::string& reference) throw (XAO_Exception);

        void setVertex(const int& index, const std::string& name, const std::string& reference) throw (XAO_Exception);
        void setEdge(const int& index, const std::string& name, const std::string& reference) throw (XAO_Exception);
        void setFace(const int& index, const std::string& name, const std::string& reference) throw (XAO_Exception);
        void setSolid(const int& index, const std::string& name, const std::string& reference) throw (XAO_Exception);

        const int getVertexIndexByReference(const std::string& reference) { return m_vertices.getIndexByReference(reference); }
        const int getEdgeIndexByReference(const std::string& reference) { return m_edges.getIndexByReference(reference); }
        const int getFaceIndexByReference(const std::string& reference) { return m_faces.getIndexByReference(reference); }
        const int getSolidIndexByReference(const std::string& reference) { return m_solids.getIndexByReference(reference); }
        const int getElementIndexByReference(const XAO::Dimension& dim, const std::string& reference) throw (XAO_Exception);

        GeometricElementList::iterator begin(const XAO::Dimension& dim) throw (XAO_Exception);
        GeometricElementList::iterator end(const XAO::Dimension& dim) throw (XAO_Exception);

        /**
         * Verifies if the geometry is read only.
         * @return true if the geometry is read only.
         */
        bool isReadOnly() { return m_readOnly; }

        /**
         * Sets the geometry read only.
         */
        void setReadOnly() { m_readOnly = true; }

    protected:
        void checkReadOnly() throw (XAO_Exception);

    protected:
        std::string m_name;
        GeometricElementList m_vertices;
        GeometricElementList m_edges;
        GeometricElementList m_faces;
        GeometricElementList m_solids;
        bool m_readOnly;

    };
}

#endif
