.. |feature_subshapes.icon|    image:: images/feature_subshapes.png

Sub-shapes
==========

The feature **Sub-shapes** modifies the selected entity by creating a non-manifold topology. It gives the possibility to:

* add/remove vertices lying on edges of a wire;
* add/remove edges and vertices lying on a face.

To create Sub-shapes in the active part:

#. select in the Main Menu *Build - > Sub-shapes* item  or
#. click |feature_subshapes.icon| **Sub-shapes** button in the toolbar

The following property panel appears.

.. figure:: images/SubShapes.png
  :align: center

  Sub-shapes property panel

Input fields:

- **Shape** panel contains the shape to be modified. The shape is selected in 3D OCC viewer or object browser. For today it has to be a wire or a face;
- **Sub-shapes** panel contains the list of sub-shapes which are already included into result. A sub-shape can be deleted from the list or the list can be extended with new shapes selected in the view.

**TUI Command**:

.. py:function:: model.addSubShapes(Part_doc, shape, sub-shapes)

    :param part: The current part object.
    :param object: A shape in format *model.selection(TYPE, shape)*.
    :param list: A list of sub-shapes in format *model.selection(TYPE, subshape)*.
    :return: Result object.

Result
""""""
The result replaces the original shape with its copy with added/removed sub-shapes.

.. figure:: images/CreateSubShapes.png
   :align: center

   Sub-shapes

**See Also** a sample TUI Script of :ref:`tui_create_subshape` operation.


  

