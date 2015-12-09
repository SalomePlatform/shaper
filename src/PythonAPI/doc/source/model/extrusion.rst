Extrusion
=========

.. testsetup::

   import model
   model.begin()
   partset = model.moduleDocument()
   part = model.addPart(partset).document()
   plane = model.defaultPlane("XOY")
   sketch = model.addSketch(part, plane)
   p1 = sketch.addPoint(0, 0)
   p2 = sketch.addPoint(0, 1)
   p3 = sketch.addPoint(1, 1)
   p4 = sketch.addPoint(1, 0)
   sketch.addPolygon(p1.pointData(), p2.pointData(), p3.pointData(), p4.pointData())

Provided that a sketch has been created before an extrusion can then be created
by the following code:

.. doctest::
   
   >>> base = sketch.selectFace()
   >>> extrusion = model.addExtrusion(part, base, 10)
   >>> # Modification of the extrusion
   >>> extrusion.setSize(20)
   
All the ways to create an extrusion and the data access and modification methods
of the extrusion are documented below

Create an extrusion
-------------------

.. autofunction:: model.addExtrusion

Extrusion object
................

.. autoclass:: model.features.extrusion.Extrusion
   :members:

Create an Extrusion Bolean
--------------------------

.. autofunction:: model.addExtrusionCut
.. autofunction:: model.addExtrusionFuse

Extrusion Boolean object
........................

.. autoclass:: model.features.extrusion_boolean.ExtrusionBoolean
   :members:
   
Create an Extrusion Sketch
--------------------------

.. autofunction:: model.addExtrusionSketch

Extrusion Sketch object
.......................

.. autoclass:: model.features.extrusion_sketch.ExtrusionSketch
   :members:
