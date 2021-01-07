// Copyright (C) 2007-2020  CEA/DEN, EDF R&D
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
        static Geometry* createGeometry(XAO::Format format) ;

        /**
         * Constructor.
         * Creates a geometry.
         * @name name the name of the geometry.
         * @return the created geometry.
         */
        static Geometry* createGeometry(XAO::Format format, const std::string& name);

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
        virtual XAO::Format getFormat() = 0;

        virtual const std::string getShapeString() = 0;
        virtual void setShapeString(const std::string& shape) = 0;
        virtual void writeShapeFile(const std::string& fileName) = 0;
        virtual void readShapeFile(const std::string& fileName) = 0;

        int countElements(XAO::Dimension dim) const ;
        int countVertices() const { return m_vertices.getSize(); }
        int countEdges() const { return m_edges.getSize(); }
        int countFaces() const { return m_faces.getSize(); }
        int countSolids() const { return m_solids.getSize(); }

        void setCountVertices(int nb);
        void setCountEdges(int nb);
        void setCountFaces(int nb);
        void setCountSolids(int nb);

        const std::string getVertexName(int index)  { return m_vertices.getName(index); }
        const std::string getEdgeName(int index)  { return m_edges.getName(index); }
        const std::string getFaceName(int index)  { return m_faces.getName(index); }
        const std::string getSolidName(int index)  { return m_solids.getName(index); }

        void setVertexName(int index, const std::string& name)  { m_vertices.setName(index, name); }
        void setEdgeName(int index, const std::string& name)  { m_edges.setName(index, name); }
        void setFaceName(int index, const std::string& name)  { m_faces.setName(index, name); }
        void setSolidName(int index, const std::string& name)  { m_solids.setName(index, name); }

        bool hasVertexName(int index)  { return m_vertices.hasName(index); }
        bool hasEdgeName(int index)  { return m_edges.hasName(index); }
        bool hasFaceName(int index)  { return m_faces.hasName(index); }
        bool hasSolidName(int index)  { return m_solids.hasName(index); }

        const std::string getVertexReference(int index)  { return m_vertices.getReference(index); }
        const std::string getEdgeReference(int index)  { return m_edges.getReference(index); }
        const std::string getFaceReference(int index)  { return m_faces.getReference(index); }
        const std::string getSolidReference(int index)  { return m_solids.getReference(index); }
        const std::string getElementReference(XAO::Dimension dim, int index) ;

        void setVertexReference(int index, const std::string& reference) ;
        void setEdgeReference(int index, const std::string& reference) ;
        void setFaceReference(int index, const std::string& reference) ;
        void setSolidReference(int index, const std::string& reference) ;

        void setVertex(int index, const std::string& name, const std::string& reference) ;
        void setEdge(int index, const std::string& name, const std::string& reference) ;
        void setFace(int index, const std::string& name, const std::string& reference) ;
        void setSolid(int index, const std::string& name, const std::string& reference) ;

        int getVertexIndexByReference(const std::string& reference) { return m_vertices.getIndexByReference(reference); }
        int getEdgeIndexByReference(const std::string& reference) { return m_edges.getIndexByReference(reference); }
        int getFaceIndexByReference(const std::string& reference) { return m_faces.getIndexByReference(reference); }
        int getSolidIndexByReference(const std::string& reference) { return m_solids.getIndexByReference(reference); }
        int getElementIndexByReference(XAO::Dimension dim, const std::string& reference) ;

        GeometricElementList::iterator begin(XAO::Dimension dim) ;
        GeometricElementList::iterator end(XAO::Dimension dim) ;

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
        void checkReadOnly() ;

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
