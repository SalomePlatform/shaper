# Copyright (C) 2018-2019  CEA/DEN, EDF R&D
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

from ModelAPI import *
from ConfigAPI import *

# register boolean property
Config_PropManager().registerProp("TestSection", "PropBool", "PropTitle", Config_Prop.Boolean)
# check property once again
Config_PropManager().registerProp("TestSection", "PropBool", "PropTitle", Config_Prop.Boolean, "true")
# check property value
assert(Config_PropManager().boolean("TestSection", "PropBool"))

# register real property
Config_PropManager().registerProp("TestSection", "PropDouble", "PropTitle", Config_Prop.Double, "12,5")
# check property (',' should be substituted by ".")
assert(Config_PropManager().real("TestSection", "PropDouble") == 12.5)

# register color property
Config_PropManager().registerProp("TestSection", "PropColor", "PropTitle", Config_Prop.Color, "#B00F00")
assert(len(Config_PropManager().color("TestSection", "PropColor")) == 3)

# check sections and properties are not empty
assert(len(Config_PropManager().getSections()) > 0)
assert(len(Config_PropManager().getProperties()) > 0)

# verify the property
prop = Config_PropManager().findProp("TestSection", "PropDouble")
assert(prop is not None)
prop.setTitle("PropTitle")
assert(prop.title() == "PropTitle")
prop.setMin("0")
assert(prop.min() == "0")
prop.setMax("100")
assert(prop.max() == "100")
prop.setType(Config_Prop.String)
assert(prop.type() == Config_Prop.String)
