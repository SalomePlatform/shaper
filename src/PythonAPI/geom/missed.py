# Direct Geometry API not yet swigged
# Author: Daniel Brunier-Coulin
# -----------------------------

from GeomAPI import *


class Ax3:

  def __init__(self, origin, normal, dirx):
    self.o  = origin
    self.n  = normal
    self.dx = dirx

  def location (self):
    return self.o

  def direction (self):
    return self.n

  def xDirection (self):
    return self.dx

  def yDirection (self):
    return self.n.cross(self.dx)
