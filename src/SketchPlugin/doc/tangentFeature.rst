.. |tangent.icon|    image:: images/Tangent.png

Tangent constraint
==================

Tangent constraint fixes two curves so that the first curve is tangent to the second one.

To create a Tangent in the active Sketch:

#. select in the Main Menu *Sketch - > Tangent* item  or
#. click |tangent.icon| **Tangent** button in Sketch toolbar:

Property panel:

.. figure:: images/Tangent_panel.png
   :align: center

Input fields:

- **First object** is a line, a circle or another curve selected in the view.
- **Second object** is a line, a circle or another curve selected in the view.

Note that tangency is not available between two lines or two splines.

After the objects are selected, a special sign will be added close to the tangency point in the view.

**TUI Command**:

.. py:function:: Sketch_1.setTangent(Curve1, Curve2)

    :param object: The first curve (line/circle/arc/...).
    :param object: The second curve (line/circle/arc/...).
    :return: Result object.

Result
""""""

Created Tangent constraint appears in the view.

.. figure:: images/Tangent_res.png
   :align: center

   Created tangent constraint

**See Also** a sample TUI Script of :ref:`tui_create_tangent` operation.
