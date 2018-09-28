
Fuse Faces
==========

The feature **Fuse Faces** eliminates the edges between adjacent same-domain faces lying on the same geometrical surface. This feature is useful for processing the results of **Fuse**, **Union** or **Partition** operations.

To perform operation **Fuse Faces** in the active part:

#. select in the Main Menu *Features - > Fuse Faces* item  or
#. click **Fuse Faces** button in the toolbar

.. image:: images/fusion_faces.png 
   :align: center

.. centered::
   **Fuse Faces**  button 

The following property panel appears:

.. image:: images/FuseFaces.png 
  :align: center

.. centered::
  **Fuse Faces**  property panel

Input fields:  

- **Shape** contains object selected in the Object Browser or in the Viewer, on which faces will be fused.

**TUI Command**:  *model.addFusionFaces(Part_doc, shape)*

**Arguments**:   Part +  shape in format *model.selection(TYPE, shape)*.

Result
""""""

The Result of the operation will be a shape with eliminated edges:

.. image:: images/CreatedFuseFaces.png
	   :align: center

.. centered::
   Shape before and after operation **Fuse Faces**

**See Also** a sample TUI Script of a :ref:`tui_create_fuse_faces` operation.
