// Copyright (C) 2013-2024  CEA, EDF
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

#include <iostream>
#include "XAO_XaoUtils.hxx"
#include "XAO_Xao.hxx"
#include "XAO_Geometry.hxx"
#include "XAO_Group.hxx"
#include "XAO_Field.hxx"
#include "XAO_IntegerField.hxx"
#include "XAO_BooleanField.hxx"
#include "XAO_DoubleField.hxx"
#include "XAO_StringField.hxx"
#include "XAO_XaoExporter.hxx"

#include <libxml/xmlstring.h>

using namespace XAO;

const xmlChar* C_XAO_VERSION = (xmlChar*)"1.0";

Xao::Xao()
{
    m_author = "";
    m_version = (char*)C_XAO_VERSION;
    m_geometry = NULL;
}

Xao::Xao(const std::string& author, const std::string& version)
{
    m_author = author;
    m_version = version;
    m_geometry = NULL;
}

Xao::~Xao()
{
    if (m_geometry != NULL)
    {
        delete m_geometry;
        m_geometry = NULL;
    }

    for (std::list<Group*>::iterator it = m_groups.begin(); it != m_groups.end(); ++it)
    {
        delete (*it);
    }

    for (std::list<Field*>::iterator it = m_fields.begin(); it != m_fields.end(); ++it)
    {
        delete (*it);
    }
}

int Xao::countGroups() const
{
    return m_groups.size();
}

Group* Xao::getGroup(int index)

{
    checkGroupIndex(index);

    int i = 0;
    for (std::list<Group*>::iterator it = m_groups.begin(); it != m_groups.end(); ++it, ++i)
    {
        if (i == index)
            return (*it);
    }

    return NULL;
}

Group* Xao::addGroup(XAO::Dimension dim, const std::string& name)

{
    checkGeometry();
    checkGroupDimension(dim);

    Group* group = new Group(dim, m_geometry->countElements(dim), name);
    m_groups.push_back(group);
    return group;
}

bool Xao::removeGroup(Group* group)
{
    int nb = countGroups();
    m_groups.remove(group);

    bool res = (nb-1 == countGroups());
    if (res)
    {
        delete group;
        group = NULL;
    }

    return res;
}

int Xao::countFields() const
{
    return m_fields.size();
}

XAO::Type Xao::getFieldType(int index)

{
    return getField(index)->getType();
}

Field* Xao::getField(int index)

{
    checkFieldIndex(index);

    int i = 0;
    for (std::list<Field*>::iterator it = m_fields.begin(); it != m_fields.end(); ++it, ++i)
    {
        if (i == index)
            return (*it);
    }

    throw XAO_Exception("Field not found.");
}

BooleanField* Xao::getBooleanField(int index)

{
    Field* field = getField(index);
    if (field->getType() != XAO::BOOLEAN)
        throw XAO_Exception(MsgBuilder() << "Field " << index << " is not a boolean field.");
    return (BooleanField*)field;
}

DoubleField* Xao::getDoubleField(int index)

{
    Field* field = getField(index);
    if (field->getType() != XAO::DOUBLE)
        throw XAO_Exception(MsgBuilder() << "Field " << index << " is not a double field.");
    return (DoubleField*)field;
}

IntegerField* Xao::getIntegerField(int index)

{
    Field* field = getField(index);
    if (field->getType() != XAO::INTEGER)
        throw XAO_Exception(MsgBuilder() << "Field " << index << " is not an integer field.");
    return (IntegerField*)field;
}

StringField* Xao::getStringField(int index)

{
    Field* field = getField(index);
    if (field->getType() != XAO::STRING)
        throw XAO_Exception(MsgBuilder() << "Field " << index << " is not a string field.");
    return (StringField*)field;
}

Field* Xao::addField(XAO::Type type, XAO::Dimension dim, int nbComponents, const std::string& name)

{
    checkGeometry();
    int nbElts = m_geometry->countElements(dim);
    Field* field = Field::createField(type, dim, nbElts, nbComponents, name);
    m_fields.push_back(field);
    return field;
}

IntegerField* Xao::addIntegerField(XAO::Dimension dim, int nbComponents, const std::string& name)

{
    checkGeometry();
    int nbElts = m_geometry->countElements(dim);
    IntegerField* field = new IntegerField(dim, nbElts, nbComponents, name);
    m_fields.push_back(field);
    return field;
}
BooleanField* Xao::addBooleanField(XAO::Dimension dim, int nbComponents, const std::string& name)

{
    checkGeometry();
    int nbElts = m_geometry->countElements(dim);
    BooleanField* field = new BooleanField(dim, nbElts, nbComponents, name);
    m_fields.push_back(field);
    return field;
}
DoubleField* Xao::addDoubleField(XAO::Dimension dim, int nbComponents, const std::string& name)

{
    checkGeometry();
    int nbElts = m_geometry->countElements(dim);
    DoubleField* field = new DoubleField(dim, nbElts, nbComponents, name);
    m_fields.push_back(field);
    return field;
}
StringField* Xao::addStringField(XAO::Dimension dim, int nbComponents, const std::string& name)

{
    checkGeometry();
    int nbElts = m_geometry->countElements(dim);
    StringField* field = new StringField(dim, nbElts, nbComponents, name);
    m_fields.push_back(field);
    return field;
}



bool Xao::removeField(Field* field)
{
    int nb = countFields();
    m_fields.remove(field);

    bool res = (nb-1 == countFields());
    if (res)
    {
        delete field;
        field = NULL;
    }

    return res;
}

bool Xao::exportXAO(const std::string& fileName, const std::string& shapeFileName)
{
    return XaoExporter::saveToFile(this, fileName, shapeFileName);
}

const std::string Xao::getXML()
{
    return XaoExporter::saveToXml(this);
}

bool Xao::importXAO(const std::string& fileName)
{
    return XaoExporter::readFromFile(fileName, this);
}

bool Xao::setXML(const std::string& xml)
{
    return XaoExporter::setXML(xml, this);
}

void Xao::checkGeometry() const
{
    if (m_geometry == NULL)
        throw XAO_Exception("Geometry is null");
}

void Xao::checkGroupIndex(int index) const
{
    if (index >= 0 && index < countGroups())
        return;

    throw XAO_Exception(MsgBuilder() << "Group index is out of range [0, "
                                     << countGroups()-1 << "]: " << index);
}

void Xao::checkFieldIndex(int index) const
{
    if (index >= 0 && index < countFields())
        return;

    throw XAO_Exception(MsgBuilder() << "Field index is out of range [0, "
                                     << countFields()-1 << "]: " << index);
}

void Xao::checkGroupDimension(XAO::Dimension dim) const
{
    if (dim == XAO::WHOLE)
        throw XAO_Exception(MsgBuilder() << "Invalid dimension for group: " << dim);
}
