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
// Author : Nathalie Gore (OpenCascade)

#include "XAO_XaoUtils.hxx"
#include "XAO_Geometry.hxx"
#include "XAO_BrepGeometry.hxx"

using namespace XAO;

Geometry::Geometry(const std::string& name)
    : m_name(name)
{
    m_readOnly = false;
}

Geometry* Geometry::createGeometry(const XAO::Format& format)
throw (XAO_Exception)
{
    return createGeometry(format, "");
}

Geometry* Geometry::createGeometry(const XAO::Format& format, const std::string& name)
throw (XAO_Exception)
{
    if (format == XAO::BREP)
        return new BrepGeometry(name);

    throw XAO_Exception(MsgBuilder() << "Geometry format not supported: " << format);
}

Geometry::~Geometry()
{
}

void Geometry::checkReadOnly()
throw (XAO_Exception)
{
    if (m_readOnly)
        throw XAO_Exception("Geometry is read only.");
}

const int Geometry::countElements(const XAO::Dimension& dim) const
throw (XAO_Exception)
{
    if (dim == XAO::VERTEX)
        return countVertices();
    if (dim == XAO::EDGE)
        return countEdges();
    if (dim == XAO::FACE)
        return countFaces();
    if (dim == XAO::SOLID)
        return countSolids();
    if (dim == XAO::WHOLE)
        return 1;

    throw XAO_Exception(MsgBuilder() << "Unknown dimension:" << dim);
}

const std::string Geometry::getElementReference(const XAO::Dimension& dim, const int& index)
throw (XAO_Exception)
{
    if (dim == XAO::VERTEX)
        return getVertexReference(index);
    if (dim == XAO::EDGE)
        return getEdgeReference(index);
    if (dim == XAO::FACE)
        return getFaceReference(index);
    if (dim == XAO::SOLID)
        return getSolidReference(index);

    throw XAO_Exception(MsgBuilder() << "Unknown dimension:" << dim);
}

const int Geometry::getElementIndexByReference(const XAO::Dimension& dim, const std::string& reference)
throw (XAO_Exception)
{
    if (dim == XAO::VERTEX)
        return getVertexIndexByReference(reference);
    if (dim == XAO::EDGE)
        return getEdgeIndexByReference(reference);
    if (dim == XAO::FACE)
        return getFaceIndexByReference(reference);
    if (dim == XAO::SOLID)
        return getSolidIndexByReference(reference);

    throw XAO_Exception(MsgBuilder() << "Unknown dimension:" << dim);
}

GeometricElementList::iterator Geometry::begin(const XAO::Dimension& dim)
throw (XAO_Exception)
{
    if (dim == XAO::VERTEX)
        return m_vertices.begin();
    if (dim == XAO::EDGE)
        return m_edges.begin();
    if (dim == XAO::FACE)
        return m_faces.begin();
    if (dim == XAO::SOLID)
        return m_solids.begin();

    throw XAO_Exception(MsgBuilder() << "Unknown dimension:" << dim);
}

GeometricElementList::iterator Geometry::end(const XAO::Dimension& dim)
throw (XAO_Exception)
{
    if (dim == XAO::VERTEX)
        return m_vertices.end();
    if (dim == XAO::EDGE)
        return m_edges.end();
    if (dim == XAO::FACE)
        return m_faces.end();
    if (dim == XAO::SOLID)
        return m_solids.end();

    throw XAO_Exception(MsgBuilder() << "Unknown dimension:" << dim);
}

void Geometry::setCountVertices(const int& nb) throw (XAO_Exception)
{
    checkReadOnly();
    m_vertices.setSize(nb);
}
void Geometry::setCountEdges(const int& nb) throw (XAO_Exception)
{
    checkReadOnly();
    m_edges.setSize(nb);
}
void Geometry::setCountFaces(const int& nb) throw (XAO_Exception)
{
    checkReadOnly();
    m_faces.setSize(nb);
}
void Geometry::setCountSolids(const int& nb) throw (XAO_Exception)
{
    checkReadOnly();
    m_solids.setSize(nb);
}

void Geometry::setVertexReference(const int& index, const std::string& reference) throw (XAO_Exception)
{
    checkReadOnly();
    m_vertices.setReference(index, reference);
}
void Geometry::setEdgeReference(const int& index, const std::string& reference) throw (XAO_Exception)
{
    checkReadOnly();
    m_edges.setReference(index, reference);
}
void Geometry::setFaceReference(const int& index, const std::string& reference) throw (XAO_Exception)
{
    checkReadOnly();
    m_faces.setReference(index, reference);
}
void Geometry::setSolidReference(const int& index, const std::string& reference) throw (XAO_Exception)
{
    checkReadOnly();
    m_solids.setReference(index, reference);
}

void Geometry::setVertex(const int& index, const std::string& name, const std::string& reference) throw (XAO_Exception)
{
    checkReadOnly();
    m_vertices.setElement(index, name, reference);
}
void Geometry::setEdge(const int& index, const std::string& name, const std::string& reference) throw (XAO_Exception)
{
    checkReadOnly();
    m_edges.setElement(index, name, reference);
}
void Geometry::setFace(const int& index, const std::string& name, const std::string& reference) throw (XAO_Exception)
{
    checkReadOnly();
    m_faces.setElement(index, name, reference);
}
void Geometry::setSolid(const int& index, const std::string& name, const std::string& reference) throw (XAO_Exception)
{
    checkReadOnly();
    m_solids.setElement(index, name, reference);
}
