#!/usr/bin/env python
#=============================================================================
# This script was written in march and april 2020 during the beginning in France
# of the coronavirus pandemia. It's generating a parametric geometric model of the virus
# in Shaper.
# Author : Raphaël MARC, EDF R&D France, with the precious help of Artem ZHIDKOV from OpenCascade company.
#=============================================================================
import numpy as np
from salome.shaper import model
from SketchAPI import *
from GeomAPI import *

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
#=============================================================================
# Paramètres du modèle SHAPER :
#=============================================================================
Rext = model.addParameter(Part_1_doc, "Rext", "10")
#print(Rext.value())
model.addParameter(Part_1_doc, "eps", "0.5")
Rint = model.addParameter(Part_1_doc, "Rint", "Rext-eps")
model.addParameter(Part_1_doc, "marge", "0.3")
Rint_m = model.addParameter(Part_1_doc, "Rint_m", "Rint-marge")
longueur = model.addParameter(Part_1_doc, "long_tube", "4")
Rtube = model.addParameter(Part_1_doc, "Rtube", "0.8")
ep_tube = model.addParameter(Part_1_doc, "ep_tube", "0.3")
# ratio diamètre cercle milieu tube / diamètre cercle de base (bottom) :
coef1 = model.addParameter(Part_1_doc, "coef1", "0.5")
# ratio diamètre cercle haut (top) du tube / diamètre cercle de base (bottom) :
coef2 = model.addParameter(Part_1_doc, "coef2", "1.2")
fillet_radius_top = model.addParameter(Part_1_doc, "fillet_radius_top", "0.12")
fillet_radius_bottom = model.addParameter(Part_1_doc, "fillet_radius_bottom", "0.8")
# méthodes dispo sur les paramètres : .value() et .setValue()
bruit = 0.012 # bruit dans la position des tubes (qui est régulière si pas de bruit)

#=============================================================================
# Début réel de la création du modèle géométrique SHAPER :
#=============================================================================
Sphere_ext = model.addSphere(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), "Rext")
Sphere_ext.setName("Sphere_ext")
# Création de num_pts points uniformément répartis sur la sphère, qui serviront à positionner les tubes :
num_pts = 50
indices = np.arange(0, num_pts, dtype=float) + 0.5
phi0 = np.arccos(1 - 2*indices/num_pts)
theta0 = np.pi * (1 + 5**0.5) * indices
# ajout de bruit Gaussien pour rendre légèrement aléatoire les positions des tubes :
bruit=np.pi*np.random.normal(0, bruit, num_pts)  # std = np.pi*0.05
phi = phi0+bruit
theta = theta0+bruit
#print(max(np.abs((phi-phi0)/phi0)))
#print(max(np.abs((theta-theta0)/theta0)))

listeX, listeY, listeZ = Rint_m.value()*np.cos(theta) * np.sin(phi), \
			Rint_m.value()*np.sin(theta) * np.sin(phi),  \
			Rint_m.value()*np.cos(phi);
x,y,z = (listeX[0], listeY[0], listeZ[0])
#=============================================================================
# On construit le premier tube que l'on copiera puis rotations ensuite :
# 3 sketchs successifs composés de 2 cercles // puis un filling (remplissage)
#=============================================================================
Point_init = model.addPoint(Part_1_doc, x,y,z)
Axis = model.addAxis(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), Point_init.result())
Plane_1 = model.addPlane(Part_1_doc, Axis.result(), Point_init.result(), True)
Sketch_1 = model.addSketch(Part_1_doc, Plane_1.result())
SketchProjection_1 = Sketch_1.addProjection(Point_init.result(), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchCircle_1_int = Sketch_1.addCircle(0, 0, 1.1)
Sketch_1.setCoincident(SketchAPI_Point(SketchPoint_1).coordinates(), SketchCircle_1_int.center())
SketchCircle_1_ext = Sketch_1.addCircle(0, 0, 1.)
Sketch_1.setCoincident(SketchAPI_Point(SketchPoint_1).coordinates(), SketchCircle_1_ext.center())
Sketch_1.setRadius(SketchCircle_1_int.results()[1], "Rtube")
Sketch_1.setRadius(SketchCircle_1_ext.results()[1], "Rtube+ep_tube")
model.do()

Plane_2 = model.addPlane(Part_1_doc, Plane_1.result(), "long_tube", False)
Sketch_2 = model.addSketch(Part_1_doc, Plane_2.result())
SketchProjection_2 = Sketch_2.addProjection(Point_init.result(), False)
SketchPoint_2 = SketchProjection_2.createdFeature()
SketchCircle_2_int = Sketch_2.addCircle(0, 0, 0.4)
Sketch_2.setCoincident(SketchAPI_Point(SketchPoint_2).coordinates(), SketchCircle_2_int.center())
SketchCircle_2_ext = Sketch_2.addCircle(0, 0, 0.7)
Sketch_2.setCoincident(SketchAPI_Point(SketchPoint_2).coordinates(), SketchCircle_2_ext.center())
Sketch_2.setRadius(SketchCircle_2_int.results()[1], "Rtube*coef1")
Sketch_2.setRadius(SketchCircle_2_ext.results()[1], "(Rtube+ep_tube)*coef1")
model.do()

Plane_3 = model.addPlane(Part_1_doc, Plane_2.result(), "long_tube", False)
Sketch_3 = model.addSketch(Part_1_doc, Plane_3.result())
SketchProjection_4 = Sketch_3.addProjection(Point_init.result(), False)
SketchPoint_3 = SketchProjection_4.createdFeature()
SketchCircle_3_int = Sketch_3.addCircle(0, 0, 0.8)
SketchCircle_3_ext = Sketch_3.addCircle(0, 0, 1.1)
Sketch_3.setRadius(SketchCircle_3_int.results()[1], "Rtube*coef2")
Sketch_3.setRadius(SketchCircle_3_ext.results()[1], "(Rtube+ep_tube)*coef2")
Sketch_3.setCoincident(SketchCircle_3_ext.center(), SketchAPI_Point(SketchPoint_3).coordinates())
Sketch_3.setCoincident(SketchCircle_3_int.center(), SketchAPI_Point(SketchPoint_3).coordinates())
model.do()

Filling_1_objects = \
    [model.selection(Sketch_1.defaultResult(), SketchCircle_1_int.defaultResult().shape()), \
     model.selection(Sketch_2.defaultResult(), SketchCircle_2_int.defaultResult().shape()), \
     model.selection(Sketch_3.defaultResult(), SketchCircle_3_int.defaultResult().shape())]
Filling_1 = model.addFilling(Part_1_doc, Filling_1_objects, "curve_info", 2, 5, 0, 0.0001, 0.0001, False)
Copy_filling_1 = model.addCopy(Part_1_doc, [Filling_1.result()], 1)
Copy_filling_1.result().setName("Filling_1_copie")

Filling_2_objects = \
    [model.selection(Sketch_1.defaultResult(), SketchCircle_1_ext.defaultResult().shape()), \
     model.selection(Sketch_2.defaultResult(), SketchCircle_2_ext.defaultResult().shape()), \
     model.selection(Sketch_3.defaultResult(), SketchCircle_3_ext.defaultResult().shape())]
Filling_2 = model.addFilling(Part_1_doc, Filling_2_objects, "curve_info", 2, 5, 0, 0.0001, 0.0001, False)

Filling_3 = model.addFilling(Part_1_doc, \
	[model.selection(Sketch_1.defaultResult(), SketchCircle_1_int.defaultResult().shape()), \
	model.selection(Sketch_1.defaultResult(), SketchCircle_1_ext.defaultResult().shape())], \
	"curve_info", 2, 5, 0, 0.0001, 0.0001, False)

Filling_4 = model.addFilling(Part_1_doc, \
	[model.selection(Sketch_3.defaultResult(), SketchCircle_3_int.defaultResult().shape()), \
	model.selection(Sketch_3.defaultResult(), SketchCircle_3_ext.defaultResult().shape())], \
	"curve_info", 2, 5, 0, 0.0001, 0.0001, False)

Face_inf = model.addFace(Part_1_doc, [model.selection(Sketch_1.defaultResult(), SketchCircle_1_int.defaultResult().shape())])

Face_sup = model.addFace(Part_1_doc, [model.selection(Sketch_3.defaultResult(), SketchCircle_3_int.defaultResult().shape())])

liste_tube_ext = [Filling_1.result(), Filling_2.result(), Filling_3.result(), Filling_4.result()]
Solid_tube_ext = model.addSolid(Part_1_doc, liste_tube_ext)

# Recherche de la face du tube sur laquelle faire un congé de raccordement (fillet) :
face = Filling_4.defaultResult().shape().face()
exp = GeomAPI_ShapeExplorer(Solid_tube_ext.defaultResult().shape(), GeomAPI_Shape.FACE)
while exp.more():
   cur = exp.current().face()
   if face.isEqual(cur) : # and face.isSameGeometry(cur):
      res = cur
      break
   exp.next()
#print(type(res))
Fillet_1 = model.addFillet(Part_1_doc, [model.selection(Solid_tube_ext.defaultResult(), res)], "fillet_radius_top", keepSubResults = False)
Solid_tube_ext = Fillet_1

#=============================================================================
# Création du tube intérieur avec lequel on va couper la sphère extérieure :
#=============================================================================
liste_tube_int = [Copy_filling_1.result(), Face_inf.result(), Face_sup.result()]
Solid_tube_int = model.addSolid(Part_1_doc, liste_tube_int)
model.do()

# Copie des 2 tubes intérieur et extérieur avant qu'ils ne soient avalés par les
# différentes opérations ultérieures (cut, fuse) :
Sphere_ext.result().setName("Sphere_ext")
Solid_tube_ext.result().setName("Solid_tube_ext")
Solid_tube_int.result().setName("Solid_tube_int")
Copie_tube_ext = model.addCopy(Part_1_doc, [Solid_tube_ext.result()], 1)
Copie_tube_ext.result().setName("Tube_ext_1")
Copie_tube_int = model.addCopy(Part_1_doc, [Solid_tube_int.result()], 1)
Copie_tube_int.result().setName("Tube_int_1")

#=============================================================================
# Création d'un congé de raccordement à la base du premier tube :
#=============================================================================
# Calcul de l'intersection entre tube ext. et sphère de manière à identifier l'arête sur # laquelle on va faire un congé (fillet) :
Intersection_1 = model.addIntersection(Part_1_doc, \
		[model.selection("SOLID", "Sphere_ext"), model.selection("SOLID", "Solid_tube_ext")], keepSubResults = True)
Recover_1 = model.addRecover(Part_1_doc, Intersection_1, [Sphere_ext.result(), Solid_tube_ext.result()])
Solid_tube_ext = Recover_1.results()[0] #.setName("Tube_ext_1")
Sphere_ext = Recover_1.results()[1] #.setName("Sphere_ext")

# Coupe de la sphère par tube int. et fusion avec tube ext. :
Cut_1 = model.addCut(Part_1_doc, [Sphere_ext], [Solid_tube_int.result()], keepSubResults = False)
Fuse_1 = model.addFuse(Part_1_doc, [Solid_tube_ext, Cut_1.result()], [], keepSubResults = False)
model.do()
#=======================================
# Fonction de calcul d'un rayon sur un cercle ou arc de cercle à partir de 3 points :
# 2 points extrêmes et point milieu. Elle nous sert à sélectionner le cercle extérieur
# de l'intersection tube-sphère, qui correspond au cercle sur lequel on veut faire
# un congé de raccordement (fillet).
def radius(theEdge):
#=======================================
    p1 = theEdge.firstPoint()
    p2 = theEdge.middlePoint()
    p3 = theEdge.lastPoint()
    dir12 = GeomAPI_Dir(p1.xyz().decreased(p2.xyz()))
    dir32 = GeomAPI_Dir(p3.xyz().decreased(p2.xyz()))
    ang = 0.5 * dir12.angle(dir32)
    return 0.5 * p1.distance(p2) / cos(ang)
#=======================================

maxRad=-999
for i in range(Intersection_1.result().numberOfSubs()):
	# The shapes are not circles, but rather NURBS :
	rad = radius(GeomAPI_Edge(Intersection_1.result().subResult(i).resultSubShapePair()[1]))
	if rad > maxRad:
		imax=i
		maxRad = rad
#print(Intersection_1.result().subResult(imax).name())
edge = GeomAPI_Edge(Intersection_1.result().subResult(imax).resultSubShapePair()[1])
exp = GeomAPI_ShapeExplorer(Fuse_1.defaultResult().shape(), GeomAPI_Shape.EDGE)
while exp.more():
   cur = exp.current().edge()
   if edge.isEqual(cur) : # and edge.isSameGeometry(cur):
      resEdge = cur
      break
   exp.next()
#print(type(res))
Sphere_ext = model.addFillet(Part_1_doc, [model.selection(Fuse_1.defaultResult(), resEdge)], "fillet_radius_bottom", keepSubResults = False)
#Sphere_ext = Fillet_2
Sphere_ext.result().setName("Sphere_ext")
model.do()
#=============================================================================
# Fin création congé de racc. à la base du premier tube.
#=============================================================================
f2=Sphere_ext # mémorisation pour rangement dans folder à la fin du script

RemoveResults_1 = model.addRemoveResults(Part_1_doc, [Fillet_1.result()])
RemoveResults_2 = model.addRemoveResults(Part_1_doc, [Intersection_1.result()])
model.do()
#=============================================================================
# Boucle de répétition des tubes autour de la sphère :
#=============================================================================
i=1
CutTools = [] # liste pour accumulation des tubes int. et cut de la sphère ext. par tous ces tubes int.
IntTools = [Sphere_ext.result()] # liste pour accumulation de la sphère ext. et de tous les tubes ext. et calcul d'intersection entre tous ces objets (pour déterminer les arêtes sur lesquelles on mettra des fillets).
for x,y,z in zip(listeX[1:], listeY[1:], listeZ[1:]):
	i += 1
# on parcourt les listes à partir du 2ème élément car le premier point (Point_init) a déjà été créé.
	CurPoint = model.addPoint(Part_1_doc, x,y,z)
	if i==2:
		f3 = CurPoint # mémorisation de la feature pour insertion ultérieure dans les dossiers (folders)
#	print("Boucle",i,":") #," : (",round(x,1), round(y,1), round(z,1),")")
	Copie_tube_ext = model.addCopy(Part_1_doc, [Solid_tube_ext], 1)
	Copie_tube_ext.result().setName("Tube_ext_"+str(i))
	Copie_tube_int = model.addCopy(Part_1_doc, [Solid_tube_int.result()], 1)
	Copie_tube_int.result().setName("Tube_int_"+str(i))
	Rotation = model.addRotation(Part_1_doc, \
		[Copie_tube_ext.result(), Copie_tube_int.result()], \
		center = model.selection("VERTEX", "PartSet/Origin"), \
		start = Point_init.result(), \
		end = CurPoint.result(), keepSubResults = True)
	Copie_tube_ext = Rotation.results()[0]
	Copie_tube_int = Rotation.results()[1]

	model.do()

	CutTools.append(model.selection("SOLID", "Tube_int_"+str(i)))
	IntTools.append(Copie_tube_ext)

#======================================================================
# Création d'un congé de raccordement à la base du tube courant (n°i) :
#======================================================================
# Calcul de l'intersection entre tous les tubes ext. et la sphère extérieure
# de manière à identifier l'arête sur  laquelle on va faire un congé (fillet) :
Intersection_1 = model.addIntersection(Part_1_doc, IntTools, keepSubResults = True)
maxRad=-999
#print("=============================================================")
TOLERANCE = 1.e-5
EdgesForFillet = []
for j in range(Intersection_1.result().numberOfSubs()):
	edge = GeomAPI_Edge(Intersection_1.result().subResult(j).resultSubShapePair()[1])
	rad = radius(edge)
#	print("Edge ",j,":",Intersection_1.result().subResult(j).name(),"-- rayon =",rad)
	if rad > maxRad + TOLERANCE:
		jmax=j
		maxRad = rad
		EdgesForFillet = [edge]
	elif rad + TOLERANCE > maxRad:
		EdgesForFillet.append(edge)
# Restauration de la sphère ext. et de tous les tubes ext. :
Recover_1 = model.addRecover(Part_1_doc, Intersection_1, IntTools)
Sphere_ext = Recover_1.results()[0]
FuseTools = Recover_1.results()[1:]

# Coupe de la sphère par tous les tubes int. et fusion du résultat avec tous les tubes ext. :
Cut_2 = model.addCut(Part_1_doc, [Sphere_ext], CutTools, keepSubResults = False)
Fuse_2 = model.addFuse(Part_1_doc, [Cut_2.result()], FuseTools, keepSubResults = False)
model.do()

# Détermination des arêtes du bas des tubes sur lesquelles mettre des "fillets" :
#print("Edge n°",jmax,"avec le plus grand rayon :",Intersection_1.result().subResult(jmax).name(),"-- rayon=",maxRad)
FilletEdges = []
exp = GeomAPI_ShapeExplorer(Fuse_2.defaultResult().shape(), GeomAPI_Shape.EDGE)
while exp.more():
	cur = exp.current().edge()
	for edge in EdgesForFillet:
		if edge.isEqual(cur) : # and edge.isSameGeometry(cur):
			FilletEdges.append(model.selection(Fuse_2.defaultResult(), cur))
			EdgesForFillet.remove(edge)
			break
	exp.next()
#print(type(res))
Fillet_2 = model.addFillet(Part_1_doc, FilletEdges, "fillet_radius_bottom", keepSubResults = False)
Fillet_2.setName("Fillets_bottom_tubes")
Fillet_2.result().setName("Resultat_1_tube")
Sphere_ext = Fillet_2
model.addRemoveResults(Part_1_doc, [Intersection_1.result()])
model.do()
##	#======================================================================
##	# Fin de la création du congé de raccordement
##	#======================================================================
##	Sphere_ext.result().setName("Resultat_"+str(i))
##	i+=1
##	model.do()
###=====================================================
### Fin de la boucle de répétition des tubes.
### A ce stade, on a une sphère fusionnée avec les num_pts tubes.
###=====================================================

# Pour faire le ménage dans les résultats et ne laisser que l'unique solide résultant,
# on supprime les 2 tubes initiaux qui ont servi à la copie dans la boucle de répétition :
model.addRemoveResults(Part_1_doc, [model.selection("SOLID", "Tube_ext_1")])
model.addRemoveResults(Part_1_doc, [model.selection("SOLID", "Tube_int_1")])

## Finalement, on crée la sphère intérieure avec laquelle on va creuser (cut) la sphère fusionnée
## avec les num_pts tubes :
Sphere_int = model.addSphere(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), "Rint")
Sphere_int.setName("Sphere_int")
Cut_2 = model.addCut(Part_1_doc, [Sphere_ext.result()], [Sphere_int.result()], keepSubResults = False)
Cut_2.result().setName("Resultat_final")
Cut_2.result().setColor(0,0,150)
#Cut_2.result().setTransparency(0.5)
model.do()

## Création de répertoires pour y ranger les features (et compacter l'OB) :
Folder_1 = model.addFolder(Part_1_doc, Point_init, f2)
Folder_1.setName("Tube_initial")
Folder_2 = model.addFolder(Part_1_doc, f3, Fillet_2)
Folder_2.setName("Boucle_copie_"+str(num_pts)+"_tubes")
model.do()
model.end()

#=====================================================
# Tests divers (paramétrage, vérification nombre de solides, faces) :
#=====================================================
model.begin()
Rext.setValue(11);model.do() # 10
longueur.setValue(6);model.do() # 4
Rtube.setValue(0.4);model.do() # 0.8
coef1.setValue(0.3);model.do() # 0.5
coef2.setValue(2);model.do() # 1.2
ep_tube.setValue(0.2);model.do() # 0.3
fillet_radius_top.setValue(0.06);model.do() # 0.12
fillet_radius_bottom.setValue(1);model.do() # 0.8

# Retour aux valeurs initiales sauf coef2 et fillet_radius_bottom :
Rext.setValue(10);model.do() # 10
longueur.setValue(4);model.do() # 4
Rtube.setValue(0.8);model.do() # 0.8
coef1.setValue(0.5);model.do() # 0.5
coef2.setValue(1.5);model.do() # 1.2
ep_tube.setValue(0.3);model.do() # 0.3
fillet_radius_top.setValue(0.12);model.do() # 0.12
fillet_radius_bottom.setValue(0.7);model.do() # 0.8

model.end()

#model.generateTests(Part_1, "Part_1")
model.testNbResults(Part_1, 1)
model.testNbSubResults(Part_1, [0])
model.testNbSubShapes(Part_1, GeomAPI_Shape.SOLID, [1])
#model.testNbSubShapes(Part_1, GeomAPI_Shape.FACE, [358])
model.testResultsVolumes(Part_1, [1000.4175], 5)
