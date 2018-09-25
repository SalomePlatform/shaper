
Angular copy
============

The feature **Angular copy** makes a compound of several rotated shapes basing on the initial shape. 

To create Angular copy in the active part:

#. select in the Main Menu *Part - > Angular copy* item  or
#. click **Angular copy** button in the toolbar

.. image:: images/multirotation.png  
   :align: center

.. centered::
   **Angular copy** button 

The following property panel appears.

.. image:: images/AngularCopy.png
  :align: center

.. centered::
  Angular copy property panel

Input fields:

- **Main objects** panel contains shapes to be rotated. Shapes are selected in 3D OCC viewer or object browser;
- **Vector of rotation**  defines the axis of rotation. Vector is edge, axis selected in 3D OCC viewer or object browser;
- **Angular Step** check-box turns on/off definition of the angle by which the object is rotated. By default (if the checkbox is not checked), **Angular Step** = 2 * PI / NbTimes;
- **Nb. Copies** defines the number of rotated shape copies in the resulting compound. If **Nb. Copies** = 1, the result contains only the initial shape.

**TUI Command**:  *model.addMultiRotation(Part_doc, [shape], axis, step, Nb)*

**Arguments**: part + list of shapes in format *model.selection(TYPE, shape)* + axis in format *model.selection(TYPE, shape)*+ real (step value) + integer (Nb. Copies).

Result
""""""
Result of cylinder rotation is compound.

.. image:: images/angularcopy.png
   :align: center

.. centered::
   Angular copy

**See Also** a sample TUI Script of a :ref:`tui_create_angularcopy` operation.  
