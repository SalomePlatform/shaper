Shaper textual interface documentation
======================================

Shaper provides two different textual interfaces that allows you to create
models through python scripts:

* a "direct" API, which allows you to create geometrical objects
  without any parametric considerations

* a "parametric" API, which is very close to what is done
  in the graphical interface and allows you to recalculate a model
  when a parameter is changed

Parametric API
--------------

This API is implemented in the model Python package. A script written
with the parametric API will typically begin with a code like below:

.. doctest:: 

   >>> import model
   >>> model.begin()
   >>> partset = model.moduleDocument()
   >>> part = model.addPart(partset).document()
   >>> plane = model.defaultPlane("XOY")
   >>> sketch = model.addSketch(part, plane)
   >>> line = sketch.addLine(0, 0, 0, 1)

Features
........

This API provides functions for creating the features listed below.
These functions return an interface to the feature that allow
to modify the feature and retrieve data from it.

.. toctree::
   :maxdepth: 1

   sketcher

   connection
   construction
   exchange

   boolean
   partition
   extrusion
   revolution
   placement
   rotation
   translation
   group

   parameter
   partset

Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
