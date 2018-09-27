
Translation
===========

The feature **Translation** makes translation of a selected shape.

To create Translation in the active part:

#. select in the Main Menu *Part - > Translation* item  or
#. click **Translation** button in the toolbar

.. image:: images/translation_vector_32x32.png   
   :align: center

.. centered::
   **Translation** button 

The Vector of translation can be defined in three different ways:

  .. image:: images/translation_vector_32x32.png    
    :align: left
  by by axis and distance

  .. image:: images/translation_dxyz_32x32.png      
    :align: left
  by vector 

  .. image:: images/translation_2pt_32x32.png    
    :align: left
  by two points

Translation by axis and distance
--------------------------------

.. image:: images/Translation2.png
  :align: center

.. centered::
  Translation by axis and distance property panel

Input fields:

- **Main objects** panel contains shapes to be translated. Shapes are selected in 3D OCC viewer or object browser;
- **Axis**  defines vector along which the object will be translated. Vector is edge, axis selected in 3D OCC viewer or object browser;
- **Distance** defines the distance along the **Vector of translation**.

**TUI Command**:  *model.addTranslation(Part_doc, [shape], axis, dist)*

**Arguments**: part + list of shapes in format *model.selection(TYPE, shape)* + axis in format *model.selection(TYPE, shape)*+ real (distance value).

Result
""""""

Result of operation is shifted initial shape.

.. image:: images/translation_XYZ.png
   :align: center

.. centered::
   Translation by axis and distance

**See Also** a sample TUI Script of a :ref:`tui_translation_vector` operation.

Translation by vector
---------------------

.. image:: images/Translation1.png
  :align: center

.. centered::
  Translation by vector property panel

Input fields:

- **Main objects** panel contains shapes to be translated. Shapes are selected in 3D OCC viewer or object browser;
- **DX**, **DY**, **DZ**  define vector using coordinates along the axis.

**TUI Command**:  *model.addTranslation(Part_1_doc, [shape], DX, DY, DZ)*

**Arguments**: part + list of shapes in format *model.selection(TYPE, shape)* + 3 real (coordinate values).

Result
""""""

Result of operation is shifted initial shape.

.. image:: images/translation_vector.png
   :align: center

.. centered::
   Translation by vector

**See Also** a sample TUI Script of a :ref:`tui_translation_DXDYDZ` operation.


Translation by two points
-------------------------

.. image:: images/Translation3.png
  :align: center

.. centered::
  Translation by two points property panel

Input fields:

- **Main objects** panel contains shapes to be translated. Shapes are selected in 3D OCC viewer or object browser;
- **Start point**  defines start vector point as point or vertex selected in 3D OCC viewer or object browser;
- **End point**  defines end vector point as point or vertex selected in 3D OCC viewer or object browser;
  
**TUI Command**:  *model.addTranslation(Part_doc, [shape], point1, point2)*

**Arguments**: part + list of shapes in format *model.selection(TYPE, shape)* + 2 points in format *model.selection(TYPE, shape)*.

Result
""""""

Result of operation is shifted initial shape.

.. image:: images/translation_2points.png
   :align: center

.. centered::
   Translation by two points

**See Also** a sample TUI Script of a :ref:`tui_translation_2points` operation.
  
