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


.. toctree::
   :maxdepth: 2

   sketcher
   extrusion



Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
