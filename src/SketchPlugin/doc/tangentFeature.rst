
Tangent constraint
==================

Tangent constraint fixes a circle (or an arc) and a line so that a line is tangent to a circle (or an arc).

To create a Tangent in the active Sketch:

#. select in the Main Menu *Sketch - > Tangent* item  or
#. click **Tangent** button in Sketch toolbar:

.. image:: images/tangent.png
   :align: center

.. centered::
   **Tangent**  button

Property panel:

.. image:: images/Tangent_panel.png
   :align: center

Input fields:

- **First object** is a line, a circle or an arc selected in the view.
- **Second object** is a line, a circle or an arc selected in the view.

Note that one of two objects should be a line.

After the objects are selected, a special sign will be added close to the tangency point in the view.

**TUI Command**: *Sketch_1.setTangent(Line, CircleOrArc)*

**Arguments**:  2 objects (the first is a line, the second one is a circle or an arc)

Result
""""""

Created Tangent constraint appears in the view.

.. image:: images/Tangent_res.png
	   :align: center

.. centered::
   Created tangent constraint

**See Also** a sample TUI Script of :ref:`tui_create_tangent` operation.
