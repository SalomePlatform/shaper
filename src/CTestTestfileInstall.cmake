# Copyright (C) 2020-2021  CEA/DEN, EDF R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

SET(COMPONENT_NAME SHAPER)
set(SALOME_TEST_LABEL_ADV SHAPR_ADV)

# Add all test subdirs
SUBDIRS(ConnectorAPI
        HDFs
        FeaturesPlugin
        ConstructionPlugin
        SketchPlugin
        ModelAPI
        FiltersPlugin
        CollectionPlugin
        BuildPlugin
        ExchangePlugin
        PythonAPI
        GeomAPI
        ModelHighAPI
        ParametersPlugin
        PythonAddons
        SketchAPI
        ConstructionAPI
        PartSetAPI
        GeomDataAPI
        Config
        ExchangeAPI
        ModelGeomAlgo
        Locale
        test_API
)
