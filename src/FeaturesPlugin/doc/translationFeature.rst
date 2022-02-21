.. _featureTranslation:
.. |translation_vector_32x32.icon|    image:: images/translation_vector_32x32.png
   :height: 16px

Translation
===========

**Translation** feature translates a selected shape.

To create a Translation in the active part:

#. select in the Main Menu *Part - > Translation* item  or
#. click |translation_vector_32x32.icon| **Translation** button in the toolbar

The Vector of translation can be defined in three different ways:

.. figure:: images/translation_vector_32x32.png    
   :align: left
   :height: 24px

by by axis and distance

.. figure:: images/translation_dxyz_32x32.png      
   :align: left
   :height: 24px

by vector 

.. figure:: images/translation_2pt_32x32.png    
   :align: left
   :height: 24px

by two points

--------------------------------------------------------------------------------

Translation by axis and distance
--------------------------------

.. figure:: images/Translation2.png
   :align: center

   Translation by axis and distance property panel

Input fields:

- **Main objects** panel contains shapes to be translated. The shapes are selected in 3D OCC viewer or object browser; :ref:`transformation_subshapes`
- **Axis**  defines a vector along which the object will be translated. The vector is an edge or axis selected in 3D OCC viewer or object browser;
- **Distance** defines the distance along the **Vector of translation**.

**TUI Command**:

.. py:function:: model.addTranslation(Part_doc, [shape], axis, dist)

    :param part: The current part object.
    :param list: A list of shapes in format *model.selection(TYPE, shape)*.
    :param object: axis in format *model.selection(TYPE, shape)*.
    :param real: Distance value.
    :return: Result object.

Result
""""""

The result of operation is a shifted initial shape.

.. figure:: images/translation_XYZ.png
   :align: center

   Translation by axis and distance

**See Also** a sample TUI Script of a :ref:`tui_translation_vector` operation.

Translation by vector
---------------------

.. figure:: images/Translation1.png
   :align: center

   Translation by vector property panel

Input fields:

- **Main objects** panel contains shapes to be translated. The shapes are selected in 3D OCC viewer or object browser; :ref:`transformation_subshapes`
- **DX**, **DY**, **DZ**  define vector using coordinates along the axis.

**TUI Command**:

.. py:function:: model.addTranslation(Part_1_doc, [shape], DX, DY, DZ)

    :param part: The current part object.
    :param list: A list of shapes in format *model.selection(TYPE, shape)*.
    :param real: dX value.
    :param real: dY value.
    :param real: dZ value.
    :return: Result object.

Result
""""""

The result of operation is a shifted initial shape.

.. figure:: images/translation_vector.png
   :align: center

   Translation by vector

**See Also** a sample TUI Script of :ref:`tui_translation_DXDYDZ` operation.


Translation by two points
-------------------------

.. figure:: images/Translation3.png
   :align: center

   Translation by two points property panel

Input fields:

- **Main objects** panel contains shapes to be translated. Shapes are selected in 3D OCC viewer or object browser; :ref:`transformation_subshapes`
- **Start point**  defines start vector point as point or vertex selected in 3D OCC viewer or object browser;
- **End point**  defines end vector point as point or vertex selected in 3D OCC viewer or object browser;
  
**TUI Command**:

.. py:function:: model.addTranslation(Part_doc, [shape], point1, point2)

    :param part: The current part object.
    :param list: A list of shapes in format *model.selection(TYPE, shape)*.
    :param object: A point in format *model.selection(TYPE, shape)*.
    :param object: A point in format *model.selection(TYPE, shape)*.
    :return: Result object.

Result
""""""

The result of operation is a shifted initial shape.

.. figure:: images/translation_2points.png
   :align: center

   Translation by two points

**See Also** a sample TUI Script of :ref:`tui_translation_2points` operation.


  .. _transformation_subshapes:

Transformation of COMPSOLID subshapes is prohibited. Read more about transformation of COMPOUND subshapes.
======================

If you select one or more subshapes of COMPOUND as transformation **Main objects**, all other subshapes will be also included in result at their original locations.

Transformation of COMPSOLID subshapes is prohibited, because we cannot guarantee the result still be a COMPSOLID after moving/duplicating some parts of it.
