
Radius constraint
=================

Radius constraint fixes the radius of circle or arc.

To create a Radius constraint in the active Sketch:

#. select in the Main Menu *Sketch - > Radius* item  or
#. click **Radius** button in Sketch toolbar:

.. image:: images/radius_constr.png
   :align: center

.. centered::
   **Radius**  button

Property panel:

.. image:: images/Radius_panel.png
   :align: center

Input fields:

- **Circle or Arc** is the circle or arc selected in the view; 
- **Value** is a radius of the selected object, could be modified to set the desirable value
- **Text location** is a position of the radius value label relating to extension line (in the view)
   .. image:: images/location_left.png
      :align: left
   **Left** inserts text at the left of the radius extension line.

   .. image:: images/location_automatic.png
      :align: left
   **Automatic** inserts text at the middle of the radius extension line if it has enough length, otherwise - to the left.

   .. image:: images/location_right.png
      :align: left
   **Right** inserts text to the right of the radius extension line.

When circle or arc is selected, the radius value is displayed in the property panel and in the view.

When creating the constraint, after selection of the circle or arc at the first time :

- drag the radius presentation in the view to the desired position and click once;
- set desirable radius value in the input field in the view and press **Enter** or just press **Enter** to keep the current radius.

.. image:: images/Radius_field_view.png
   :align: center

.. centered::
   Radius input in the view

**TUI Command**: *Sketch_1.setRadius(CircleOrArc, Value)*

**Arguments**:  circle or arc circumference object + radius value

Result
""""""

Created Radius appears in the view.

.. image:: images/Radius_res.png
	   :align: center

.. centered::
   Radius created

**See Also** a sample TUI Script of :ref:`tui_create_radius` operation.
