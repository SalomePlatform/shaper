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

Geometry* Geometry::createGeometry(XAO::Format format)
{
    return createGeometry(format, "");
}

Geometry* Geometry::createGeometry(XAO::Format format, const std::string& name)
{
    if (format == XAO::BREP)
        return new BrepGeometry(name);

    throw XAO_Exception(MsgBuilder() << "Geometry format not supported: " << format);
}

Geometry::~Geometry()
{
}

void Geometry::checkReadOnly()
{
    if (m_readOnly)
        throw XAO_Exception("Geometry is read only.");
}

int Geometry::countElements(XAO::Dimension dim) const
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

const std::string Geometry::getElementReference(XAO::Dimension dim, int index)
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

int Geometry::getElementIndexByReference(XAO::Dimension dim, const std::string& reference)
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

GeometricElementList::iterator Geometry::begin(XAO::Dimension dim)
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

GeometricElementList::iterator Geometry::end(XAO::Dimension dim)
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

void Geometry::setCountVertices(int nb) 
{
    checkReadOnly();
    m_vertices.setSize(nb);
}
void Geometry::setCountEdges(int nb) 
{
    checkReadOnly();
    m_edges.setSize(nb);
}
void Geometry::setCountFaces(int nb) 
{
    checkReadOnly();
    m_faces.setSize(nb);
}
void Geometry::setCountSolids(int nb) 
{
    checkReadOnly();
    m_solids.setSize(nb);
}

void Geometry::setVertexReference(int index, const std::string& reference) 
{
    checkReadOnly();
    m_vertices.setReference(index, reference);
}
void Geometry::setEdgeReference(int index, const std::string& reference) 
{
    checkReadOnly();
    m_edges.setReference(index, reference);
}
void Geometry::setFaceReference(int index, const std::string& reference) 
{
    checkReadOnly();
    m_faces.setReference(index, reference);
}
void Geometry::setSolidReference(int index, const std::string& reference) 
{
    checkReadOnly();
    m_solids.setReference(index, reference);
}

void Geometry::setVertex(int index, const std::string& name, const std::string& reference) 
{
    checkReadOnly();
    m_vertices.setElement(index, name, reference);
}
void Geometry::setEdge(int index, const std::string& name, const std::string& reference) 
{
    checkReadOnly();
    m_edges.setElement(index, name, reference);
}
void Geometry::setFace(int index, const std::string& name, const std::string& reference) 
{
    checkReadOnly();
    m_faces.setElement(index, name, reference);
}
void Geometry::setSolid(int index, const std::string& name, const std::string& reference) 
{
    checkReadOnly();
    m_solids.setElement(index, name, reference);
}
