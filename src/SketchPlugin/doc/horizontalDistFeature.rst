
Horizontal distance constraint
==============================

Horizontal Distance constraint fixes the distance between two points along the horizontal axis.

The constraint can be defined between two points such as point object, line or arc end point, center of circle or arc.

To create a Horizontal Distance constraint in the active Sketch:

#. select in the Main Menu *Sketch - > Horizontal Distance* item  or
#. click **Horizontal Distance** button in Sketch toolbar:

.. image:: images/distance_h.png
   :align: center

.. centered::
   **Horizontal Distance**  button

Property panel:

.. image:: images/HorizontalDistance_panel.png
   :align: center

Input fields:

- **First point** is the first point selected in the view;
- **Second point** is the second point selected in the view;
- **Value** is a distance between the objects, can be modified to set the desirable value;
- **Text location** is a position of the distance value label relating to extension line (in the view):
   .. image:: images/location_left.png
      :align: left
   **Left** inserts text to the left of the distance extension line;

   .. image:: images/location_automatic.png
      :align: left
   **Automatic** inserts text in the middle of the distance extension line if it has enough length, otherwise - to the left;

   .. image:: images/location_right.png
      :align: left
   **Right** inserts text to the right of the distance extension line.

When both objects are selected, the horizontal distance value is displayed in the property panel and in the view.

When creating the constraint, after selection of two objects for the first time:

- drag the horizontal distance presentation in the view to the desired position and click once;
- set desirable horizontal distance value in the input field in the view and press **Enter** or just press **Enter** to keep the current distance.

.. image:: images/HorizontalDistance_field_view.png
   :align: center

.. centered::
   Horizontal Distance input in the view

**TUI Command**: *Sketch_1.setHorizontalDistance(FirstObject, SecondObject, Value)*

**Arguments**:  2 objects + horizontal distance value

Result
""""""

Created Horizontal Distance appears in the view.

.. image:: images/HorizontalDistance_res.png
	   :align: center

.. centered::
   Horizontal Distance created

**See Also** a sample TUI Script of :ref:`tui_create_hdistance` operation.