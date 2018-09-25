
Sub-shapes
==========

The feature **Sub-shapes** lets to modify selected entity creating a non-manifold topology. It provides:

* add/remove vertices laying on edges of a wire;
* add/remove edges and vertices laying on a face.

To create Sub-shapes in the active part:

#. select in the Main Menu *Build - > Sub-shapes* item  or
#. click **Sub-shapes** button in the toolbar

.. image:: images/feature_subshapes.png 
   :align: center

.. centered::
   **Sub-shapes** button 

The following property panel appears.

.. image:: images/SubShapes.png
  :align: center

.. centered::
  Sub-shapes property panel

Input fields:

- **Shape** panel contains a shape to be modified. Shape is selected in 3D OCC viewer or object browser. For today it has to be wire or face;
- **Sub-shapes** panel contains list of sub-shapes which are already included into result. A sub-shape can be deleted from the list or the list can be extended with new shapes selected in the view.

**TUI Command**:  *model.addSubShapes(Part_doc, shape, sub-shapes)*

**Arguments**: 1 part + shape in format *model.selection(TYPE, shape)* + list of sub-shapes in format *model.selection(TYPE, subshape)*.

Result
""""""
Result replaces original shape with the same one with added/removed sub-shapes.

.. image:: images/CreateSubShapes.png
   :align: center

.. centered::
   Sub-shapes

**See Also** a sample TUI Script of a :ref:`tui_create_subshape` operation.


  

