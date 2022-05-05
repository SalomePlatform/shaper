# -*- coding: utf-8 -*-
# Copyright (C) 2016-2022  CEA/DEN, EDF R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

"""pipeNetwork Feature
Author: Nathalie GORE - Gérald NICOLAS
Remarque : la fonction de partitionnement pour un futur maillage en hexa est désactivée.
"""

__revision__ = "V02.17"

from salome.shaper import model
import ModelAPI
from GeomAPI import *

import numpy as np

# Si erreur :

def raiseException(texte):
    """En cas d'erreur"""
    print (texte)

def printverbose (texte, nb=0, verbose=False):
    """Impression controlée"""
    if verbose:
        if nb:
            texte_a = ""
            for _ in range(nb):
                texte_a += "="
            print (texte_a)
        print (texte)

class pipeNetwork(model.Feature):
    """Creation of a network of pipes"""
    lfeatures = list()
    ledges = list()
    folder = None
    isHexa = False
    twopartwo = "2par2"
    parligne = "par_ligne"
    radius = 0.5
    infoPoints = dict()
    connectivities = dict()

    _verbose = False
    _verbose_max = False

# Feature initializations

    def __init__(self):
        """x.__init__(...) initializes x; see x.__class__.__doc__ for signature"""
        model.Feature.__init__(self)

    @staticmethod
    def ID():
        """Return Id of the Feature."""
        return "pipeNetwork"

    @staticmethod
    def FILE_ID():
        """Returns ID of the file select parameter."""
        return "file_path"

    #@staticmethod
    #def HEXAS_ID():
        #"""Returns ID of the radius parameter."""
        #return "blocking"

    def getKind(self):
        """Override Feature.getKind()"""
        return pipeNetwork.ID()


#====================================================================================
# Initialization of the dialog panel

    def initAttributes(self):
        """Override Feature.initAttributes()"""
        # Creating the input argument of the feature
        self.data().addAttribute(self.FILE_ID(), ModelAPI.ModelAPI_AttributeString_typeId())
        #self.data().addAttribute(self.HEXAS_ID(), ModelAPI.ModelAPI_AttributeBoolean_typeId())

#====================================================================================
# Retrieve parent pipe

    def decodingCode(self, code):
        """decodingCode"""
        splitCode = code.split(".")
        if len(splitCode) <= 1:
            previousCode = ""
        else:
            previousCode = code[:len(code)-len(splitCode[-1])-1]
        return previousCode

#====================================================================================

    def readNodeInfo(self, line):
        """Lecture des noeuds

La ligne à décoder est formée des informations :
. l'identifiant du noeud
. si les coordonnées sont données en absolu : "-" suivi des 3 coordonnées
. si les coordonnées sont données en relatif : l'identifiant du noeud de départ, suivi des 3 coordonnées de la translation
Par défaut, on supposera que la connection est angulaire et que ce n'est pas une extrémité.
        """
        #print(line)
        texte = line
        splitLine = line.split()
        if ( len(splitLine) != 5 ):
            diagno = 1
        elif splitLine[0] in self.infoPoints:
            texte += "\nThis node was already declared."
            diagno = 2
        elif ( splitLine[1] not in self.infoPoints ) and ( splitLine[1] != "-" ):
            texte += "\nThe starting point was not seen before."
            diagno = 3
        else:
            diagno = 0
            self.infoPoints[splitLine[0]] = dict()
            self.infoPoints[splitLine[0]]["Ref"] = splitLine[1]
            if splitLine[1] == "-":
                self.infoPoints[splitLine[0]]["X"] = float(splitLine[2])
                self.infoPoints[splitLine[0]]["Y"] = float(splitLine[3])
                self.infoPoints[splitLine[0]]["Z"] = float(splitLine[4])
            else :
                self.infoPoints[splitLine[0]]["X"] = self.infoPoints[splitLine[1]]["X"] + float(splitLine[2])
                self.infoPoints[splitLine[0]]["Y"] = self.infoPoints[splitLine[1]]["Y"] + float(splitLine[3])
                self.infoPoints[splitLine[0]]["Z"] = self.infoPoints[splitLine[1]]["Z"] + float(splitLine[4])
            printverbose ("Enregistrement du point ({},{},{})".format(self.infoPoints[splitLine[0]]["X"],self.infoPoints[splitLine[0]]["Y"],self.infoPoints[splitLine[0]]["Z"]), verbose=self._verbose)
            self.infoPoints[splitLine[0]]["Fillet"] = "angular_connection"
            self.infoPoints[splitLine[0]]["isEnd"] = False
        #print ("Retour de readNodeInfo = {}".format(diagno))
        return diagno, texte

#====================================================================================

    def readConnectivity(self, line, method):
        """Lecture des connectivités

La ligne à décoder est formée des informations :
. si la méthode est par ligne : la liste des identifiants des noeuds formant le trajet
. si la méthode est 2 par 2 : chaque tronçon est décrit par les identifiants des 2 noeuds
Par défaut, on supposera que la méthode est par ligne.
        """
        splitLine = line.split()
        printverbose ("Enregistrement du tronçon : {}".format(splitLine),verbose=self._verbose)
        diagno = 0
        if ( method == self.twopartwo ):
            if self.connectivities:
                # Recherche si le tronçon existe déjà ou s'il est nouveau
                existe = False
                for key, val in self.connectivities.items():
                    #print(key, " ******* {}".format(val))
                    if val['chainage'][-1] == splitLine[0]:
                        # Le tronçon existe
                        val['chainage'].append(splitLine[1])
                        #print("On complète le tronçon")
                        existe = True
                        break
                # Le tronçon n'existe pas
                if not existe:
                    #print("On démarre un nouveau tronçon - Cas 2")
                    self.newConnectivity(splitLine[0], splitLine)
            else :
                #print("On démarre un nouveau tronçon - Cas 1")
                self.newConnectivity(splitLine[0], splitLine)
        else :
            self.newConnectivity(splitLine[0], splitLine)
        #print ("Retour de readConnectivity = {}".format(diagno))

        return diagno

#====================================================================================

    def correctConnectivity(self):
        """Correction des connectivités pour tenir compte de points alignés

Si 3 points sont alignés sur une ligne et qu'aucune ligne ne part du point central,
il faut scinder la ligne au niveau de ce point pour traiter correctement la suite : on se mettrait
à créer un plan avec ces 3 points alignés et tout s'effondre ensuite.
        """
        while True:
            # On explore toutes les lignes et on cherche un cas où sur une ligne 3 points consécutifs sont alignés
            for _, value in self.connectivities.items():
                # Sur cette ligne, a-t-on 3 points consécutifs alignés ?
                modif = self.correctConnectivity_a(value["chainage"])
                # Si on a modifié la description des connectivités, on recommence l'analyse
                if modif:
                    break
            # Si plus rien n'a été modifié, c'est fini
            if not modif:
                break

    def correctConnectivity_a(self, l_noeuds):
        """On explore toutes les lignes et on cherche un cas où sur une ligne 3 points consécutifs sont alignés

Entrées :
  :l_noeuds: liste des noeuds de la ligne

Sorties :
  :modif: on a modifié ou non la description des lignes
        """
        modif = False
        nb_points = len(l_noeuds)
        printverbose ("Analyse de {}".format(l_noeuds), verbose=self._verbose_max)
        #print ("nb_points = {}".format(nb_points))

        indice = 0
        nb_test = nb_points - 2
        for iaux in range(nb_test):
            # Calcul de l'angle entre les 3 points de la séquence
            vect = list()
            #print ("({},{},{}".format(l_noeuds[iaux],l_noeuds[iaux+1],l_noeuds[iaux+2]))
            for jaux in range(3):
                coox = self.infoPoints[l_noeuds[iaux+jaux]]["X"]
                cooy = self.infoPoints[l_noeuds[iaux+jaux]]["Y"]
                cooz = self.infoPoints[l_noeuds[iaux+jaux]]["Z"]
                vect.append(np.array((coox,cooy,cooz),np.float))
            cosinus = np.dot(vect[1]-vect[0],vect[1]-vect[2])/(np.linalg.norm(vect[1]-vect[0])* np.linalg.norm(vect[1]-vect[2]))
            #print ("cosinus = {}".format(cosinus))
            # Si l'angle est plat, c'est que les 3 points sont alignés : on arrête... sauf si ce point est un départ d'une autre !
            if ( (1.-np.abs(cosinus)) < 1.e-4 ):
                indice = iaux+1
                if l_noeuds[indice] not in self.connectivities:
                    break
                else:
                    indice = 0
        # Si un angle plat a été trouvé, on scinde la ligne
        if indice:
            #print ("id_noeud_debut = {}, {}".format(l_noeuds[0], l_noeuds[:indice+1]))
            #print ("id_noeud_new   = {}, {}".format(l_noeuds[indice], l_noeuds[indice:]))
            self.newConnectivity(l_noeuds[0], l_noeuds[:indice+1])
            self.newConnectivity(l_noeuds[indice], l_noeuds[indice:])
            modif = True

        return modif

#====================================================================================

    def newConnectivity(self, key, value):
        """newConnectivity"""
        self.connectivities[key] = dict()
        self.connectivities[key]['chainage'] = value

#====================================================================================

    def readFillet(self, line):
        """Décodage des caractéristiques de la connection entre deux tuyaux

La ligne est formée de deux informations :
. l'identifiant du noeud
. la caractérisation de la connection : "angular_connection" ou "radius=xxx"
        """
        splitLine = line.split()
        if len(splitLine) != 2:
            print(line)
            diagno = 1
        elif not splitLine[0] in self.infoPoints:
            print(line)
            diagno = 2
        elif splitLine[1] == "angular_connection":
            self.infoPoints[splitLine[0]]["Fillet"] = "angular_connection"
            diagno = 0
        elif splitLine[1][:7] == "radius=":
            self.infoPoints[splitLine[0]]["Fillet"] = "radius"
            self.infoPoints[splitLine[0]]["Radius"] = float(splitLine[1][7:])
            diagno = 0
        else:
            print(line)
            diagno = 3
        #print ("Retour de readFillet = {}".format(diagno))
        return diagno

#====================================================================================

    def retrieveSubshapesforWire(self, copy, key, ind):
        """retrieveSubshapesforWire"""
        exp = GeomAPI_ShapeExplorer(copy.defaultResult().shape(), GeomAPI_Shape.EDGE)

        end = False
        subshapesForWire = list()
        currentInd = 0
        isPipe = True
        #print("Current chainage : {}".format(self.connectivities[key]['chainage'][ind:]))
        #print("Indice de démarrage = {}".format(ind))

        while exp.more() and not end :
            #print("Analyse Edge n°", currentInd)
            #print(" => ", self.connectivities[key]['chainage'][currentInd], " - ", self.connectivities[key]['chainage'][currentInd+1])
            #print(" ==> ", self.infoPoints[self.connectivities[key]['chainage'][currentInd]]["isAngular"], " - ", self.infoPoints[self.connectivities[key]['chainage'][currentInd+1]]["isAngular"])
            cur = exp.current().edge()
            if currentInd < ind:
                #print("Edge non prise en compte")
                #print("test si fillet : ", currentInd+1, ind, self.infoPoints[self.connectivities[key]['chainage'][currentInd+1]]["Fillet"])
                if currentInd+1 <= ind and self.infoPoints[self.connectivities[key]['chainage'][currentInd+1]]["Fillet"] == "radius" and not self.infoPoints[self.connectivities[key]['chainage'][currentInd]]["isAngular"]:
                    #print("Fillet à ne pas prendre en compte")
                    exp.next()
                    cur = exp.current().edge()
            else :
                subshapesForWire.append(model.selection(copy.defaultResult(), cur))
                #print("Mode normal - Nb segments dans le wire : {}".format(len(subshapesForWire)))
                # Cas du fillet : on récupère l'edge suivante
                if self.infoPoints[self.connectivities[key]['chainage'][currentInd]]["isAngular"] or self.infoPoints[self.connectivities[key]['chainage'][currentInd+1]]["isAngular"]:
                    end = True
                    #print("Nb segments dans le wire : {}".format(len(subshapesForWire)))
                    if ( len(subshapesForWire) == 1 ):
                        #print("Coude droit en cours")
                        currentInd += 1
                        isPipe = False
                    else :
                        #print("Coude droit à venir")
                        subshapesForWire = subshapesForWire[:-1]
                elif self.infoPoints[self.connectivities[key]['chainage'][currentInd]]["Fillet"] == "radius":
                    #print("Ajout edge start Fillet")
                    exp.next()
                    cur = exp.current().edge()
                    subshapesForWire.append(model.selection(copy.defaultResult(), cur))
                    #currentInd = currentInd+1
                    #print("Mode Fillet - Nb segments dans le wire : {}".format(len(subshapesForWire)))
                elif self.infoPoints[self.connectivities[key]['chainage'][currentInd+1]]["Fillet"] == "radius":
                    #print("Ajout edge end Fillet")
                    exp.next()
                    cur = exp.current().edge()
                    subshapesForWire.append(model.selection(copy.defaultResult(), cur))
                    #print("Mode Fillet - Nb segments dans le wire : {}".format(len(subshapesForWire)))
                else :
                    if self.infoPoints[self.connectivities[key]['chainage'][currentInd+1]]["isEnd"]:
                        #print("Fin détecte")
                        currentInd = currentInd+1
                        end = True
                    #else :
                        #print("Branchement")
            if not end:
                currentInd = currentInd+1
            exp.next()
            #print("End = {} {}".format(end,self.connectivities[key]['chainage'][currentInd]))

        return subshapesForWire, currentInd, isPipe, self.connectivities[key]['chainage'][currentInd]

#====================================================================================

    def retrieveLastElement(self, obj, typeOfElement):
        """retrieveLastElement"""
        exp = GeomAPI_ShapeExplorer(obj.defaultResult().shape(), typeOfElement)
        while exp.more():
            cur = None
            if typeOfElement == GeomAPI_Shape.VERTEX :
                cur = exp.current().vertex()
            elif typeOfElement == GeomAPI_Shape.EDGE :
                cur = exp.current().edge()
            elif typeOfElement == GeomAPI_Shape.FACE :
                cur = exp.current().face()
            elif typeOfElement == GeomAPI_Shape.SOLID :
                cur = exp.current().solid()
            if cur is not None:
                exp.next()
                cur = model.selection(obj.defaultResult(), cur)
        return cur

#====================================================================================

    def retrieveFirstElement(self, obj, typeOfElement):
        """retrieveFirstElement"""
        exp = GeomAPI_ShapeExplorer(obj.defaultResult().shape(), typeOfElement)
        cur = None
        if typeOfElement == GeomAPI_Shape.VERTEX :
            cur = exp.current().vertex()
        elif typeOfElement == GeomAPI_Shape.EDGE :
            cur = exp.current().edge()
        elif typeOfElement == GeomAPI_Shape.FACE :
            cur = exp.current().face()
        elif typeOfElement == GeomAPI_Shape.SOLID :
            cur = exp.current().solid()
        if cur is not None:
            exp.next()
            cur = model.selection(obj.defaultResult(), cur)
        return cur

#====================================================================================

    def createPipe(self, part, connectivityInfos):
        """createPipe"""
        lPipes = list()
        startFace = None
        fuse = None
        for ind in range(len(connectivityInfos['paths'])):
            printverbose ("Step = {}".format(ind), 80, verbose=self._verbose_max)
            if ind == 0:
                startFace = connectivityInfos['sketch']
            if connectivityInfos['isPipe'][ind] :
                pipe = model.addPipe(part, [startFace], connectivityInfos['paths'][ind].result())
            else :
                # recherche du plan
                if self.infoPoints[connectivityInfos['ends'][ind]]['isAngular']:
                    pipe = model.addExtrusion(part, [startFace], model.selection(), self.infoPoints[connectivityInfos['ends'][ind]]['plane'], 0, model.selection(), 0, "Faces|Wires")
                else :
                    # le plan cible n'existe pas
                    edge = model.addAxis(part, self.infoPoints[connectivityInfos['starts'][ind]]['point'], self.infoPoints[connectivityInfos['ends'][ind]]['point'])
                    edge.execute(True)
                    self.lfeatures.append(edge)# self.retrieveFirstElement(connectivityInfos['paths'][ind], GeomAPI_Shape.EDGE)
                    self.ledges.append(edge)
                    point = self.retrieveLastElement(connectivityInfos['paths'][ind], GeomAPI_Shape.VERTEX)
                    plane = model.addPlane(part, edge.result(), point, True)
                    plane.execute(True)
                    self.lfeatures.append(plane)
                    pipe = model.addExtrusion(part, [startFace], edge.result(), plane.result(), 0, model.selection(), 0, "Faces|Wires")
            pipe.execute(True)
            self.lfeatures.append(pipe)
            lPipes.append(pipe.result())
            if ( ind < len(connectivityInfos['paths'])-1 ):
                copy = model.addCopy(part, [model.selection(pipe.defaultResult())], 1)
                copy.execute(True)
                self.lfeatures.append(copy)
                startFace = self.retrieveLastElement(copy, GeomAPI_Shape.FACE)

        if len(lPipes) > 1 :
            fuse = model.addFuse(part, lPipes, False)
            fuse.execute(True)
            self.lfeatures.append(fuse)
        else :
            return pipe
        return fuse

#==========================================================

#==========================================================
# Création des différents éléments
    def createPoints(self, part):
        """Création des points

Le point est créé en tant qu'objet de construction avec ses coordonnées.
Il est nommé conformément au texte donné dans le fichier de données. Cela n'a qu'un intérêt graphique mais agréable en débogage.
"""
        print("========================= Création des noeuds ============================")
        for key, value in self.infoPoints.items():
            printverbose ("Noeud : '{}'".format(key), verbose=self._verbose)
            point = model.addPoint(part, value['X'], value['Y'], value['Z'])
            point.execute(True)
            point.setName(key)
            point.result().setName(key)
            self.lfeatures.append(point)
            value["point"] = point.result()

    def createPolylines(self, part):
        """Création des polylines

La polyligne est créée en tant que résultat en enchaînant ses points.
Elle est nommée conformément aux 1er et dernier noeud. Cela n'a qu'un intérêt graphique mais agréable en débogage.
"""
        print("========================= Création des polylines =========================")
        for key, value in self.connectivities.items():
            printverbose ("Ligne : {}".format(value['chainage']), verbose=self._verbose)
            lPoints = list()
            for id_noeud in value['chainage']:
                lPoints.append(self.infoPoints[id_noeud]["point"])
                id_noeud_fin = id_noeud
            polyline = model.addPolyline3D(part, lPoints, False)
            polyline.execute(True)
            nom = "L_{}_{}".format(key,id_noeud_fin)
            polyline.setName(nom)
            polyline.result().setName(nom)
            self.lfeatures.append(polyline)
            value["polyline"] = polyline

    def createFillets(self, part):
        """Création des fillets

Le fillet est créé en tant que résultat.
Il est nommé conformément au noeud d'application. Cela n'a qu'un intérêt graphique mais agréable en débogage.
"""
        print("========================= Création des fillets ===========================")
        for key, value in self.connectivities.items():
            printverbose ("Examen de la ligne démarrant sur le noeud '{}'".format(key), verbose=self._verbose)
            # recherche des noeuds fillets
            value["fillet"] = value["polyline"]
            for id_noeud in value['chainage']:
                if self.infoPoints[id_noeud]["Fillet"] == "radius" :
                    printverbose ("\tFillet sur le noeud '{}'".format(id_noeud), verbose=self._verbose)
                    fillet1D = model.addFillet(part, [model.selection("VERTEX", (self.infoPoints[id_noeud]["X"],self.infoPoints[id_noeud]["Y"],self.infoPoints[id_noeud]["Z"]))], self.infoPoints[id_noeud]["Radius"])
                    fillet1D.execute(True)
                    nom = "F_{}".format(id_noeud)
                    fillet1D.setName(nom)
                    fillet1D.result().setName(nom)
                    self.lfeatures.append(fillet1D)
                    value["fillet"] = fillet1D

#==========================================================

    def searchRightConnections(self, part):
        """Recherche des coudes droits"""
        print("========================= Recherche des coudes droits ====================")
        for key, value in self.connectivities.items():
            printverbose ("Examen de la ligne démarrant sur le noeud '{}'".format(key), verbose=self._verbose)
            # recherche des noeuds fillets
            for ind, id_noeud in enumerate(value['chainage']):
                printverbose ("\tNoeud '{}' : {}".format(id_noeud,self.infoPoints[id_noeud]["Fillet"]), verbose=self._verbose)
                if ( ( ind == 0 ) or ( ind == len(value['chainage'])-1 ) ):
                    self.infoPoints[id_noeud]["isAngular"] = False
                else :
                    if self.infoPoints[id_noeud]["Fillet"] == "radius" :
                        self.infoPoints[id_noeud]["isAngular"] = False
                    else :
                        if id_noeud in self.connectivities:
                            self.infoPoints[id_noeud]["isAngular"] = False
                        else :
                            self.infoPoints[id_noeud]["isAngular"] = True
                            # Axe d'extrusion
                            #print(ind-1, ind, ind+1)
                            printverbose ("\t\tCréation du plan passant par les points : ('{}','{}','{}')".format(value["chainage"][ind-1], id_noeud, value["chainage"][ind+1]), verbose=self._verbose)
                            #print(self.infoPoints[value["chainage"][ind-1]]["point"])

                            tmpPlane = model.addPlane(part, self.infoPoints[value["chainage"][ind-1]]["point"], self.infoPoints[id_noeud]["point"], self.infoPoints[value["chainage"][ind+1]]["point"])
                            tmpPlane.execute(True)
                            self.lfeatures.append(tmpPlane)
                            axis =  model.addAxis(part, tmpPlane.result(), self.infoPoints[id_noeud]["point"])
                            axis.execute(True)
                            self.lfeatures.append(axis)
                            self.infoPoints[id_noeud]["axis"] = axis.result()

                            # Edge à extruder
                            tmpEdge = model.addEdge(part, self.infoPoints[id_noeud]["point"], self.infoPoints[value["chainage"][ind+1]]["point"])
                            tmpEdge.execute(True)
                            self.lfeatures.append(tmpEdge)
                            length = model.measureDistance(part, self.infoPoints[value["chainage"][ind-1]]["point"], self.infoPoints[id_noeud]["point"])
                            point =  model.addPoint(part, tmpEdge.result(), length, False, False)
                            point.execute(True)
                            self.lfeatures.append(point)
                            baseEdge = model.addEdge(part, self.infoPoints[value["chainage"][ind-1]]["point"], point.result())
                            baseEdge.execute(True)
                            self.lfeatures.append(baseEdge)
                            middlePoint = model.addPoint(part, baseEdge.result(), 0.5, True, False)
                            middlePoint.execute(True)
                            self.lfeatures.append(middlePoint)
                            Edge = model.addEdge(part, self.infoPoints[id_noeud]["point"], middlePoint.result())
                            Edge.execute(True)
                            self.lfeatures.append(Edge)
                            self.ledges.append(Edge)

                            # Extrusion
                            plane = model.addExtrusion(part, [Edge.result()], axis.result(), 10, 0)
                            plane.execute(True)
                            self.lfeatures.append(plane)
                            self.infoPoints[id_noeud]["plane"] = plane.result()

    def createPaths(self, part):
        """Création des paths pour le pipeNetwork"""
        print("========================= Création des paths =============================")
        for key, value in self.connectivities.items():
            printverbose ("Ligne démarrant sur le noeud '{}'".format(key), verbose=self._verbose)
            # recherche des noeuds fillets
            value["paths"] = list()
            value["isPipe"] = list()
            value["starts"] = list()
            value["ends"] = list()
            ind = 0
            copy = value['fillet']
            while ind < len(value['chainage'])-1:
                value["starts"].append(self.connectivities[key]['chainage'][ind])
                objectsForPath, ind, isPipe, end_noeud = self.retrieveSubshapesforWire(copy, key, ind)
                if self._verbose_max:
                    print("************************* ind = {}".format(ind))
                    print("************************* objectsForPath = {}".format(objectsForPath))
                path = model.addWire(part, objectsForPath, False)
                path.execute(True)
                self.lfeatures.append(path)
                value["paths"].append(path)
                value["isPipe"].append(isPipe)
                value["ends"].append(end_noeud)
                if ind < len(value['chainage'])-1:
                    copy = model.addCopy(part, [model.selection(copy.defaultResult())], 1)
                    copy.execute(True)
                    self.lfeatures.append(copy)

    def createSketches(self, part):
        """Création des sketchs"""
        print("========================= Création des sketchs =========================")
        for key, value in self.connectivities.items():
            printverbose ("Ligne démarrant sur le noeud '{}'".format(key), verbose=self._verbose)
            # Creating sketch
            edge = model.addEdge(part, self.infoPoints[value["chainage"][0]]["point"], self.infoPoints[value["chainage"][1]]["point"])
            edge.execute(True)
            self.lfeatures.append(edge)
            plane = model.addPlane(part, edge.result(), self.infoPoints[value["chainage"][0]]["point"], True)
            plane.execute(True)
            self.lfeatures.append(plane)
            sketch = model.addSketch(part, plane.result())
            sketch.execute(True)
            self.lfeatures.append(sketch)
            SketchProjection = sketch.addProjection(self.infoPoints[value["chainage"][0]]["point"], False)
            SketchProjection.execute(True)
            SketchPoint = SketchProjection.createdFeature()
            SketchPoint.execute(True)
            SketchCircle = sketch.addCircle(0,0,self.radius)
            SketchCircle.execute(True)
            sketch.setCoincident(SketchPoint.result(), SketchCircle.center())
            sketch.setRadius(SketchCircle.results()[1], self.radius)
            sketch.execute(True)
            model.do()
            value["sketch"] = sketch.result()

    def createPipes(self, part, nameRes):
        """Création des pipes"""
        print("========================= Création des pipes =========================")
        for key, value in self.connectivities.items():
            printverbose ("Ligne démarrant sur le noeud '{}'".format(key), verbose=self._verbose)
            pipe = self.createPipe(part, value)
            value["pipe"] = pipe.result()

        # Fusion des pipes
        print("========================= Fusion des pipes =========================")
        lPipes = list()
        for key, value in self.connectivities.items():
            lPipes.append(value["pipe"])
        if len(lPipes) > 1 :
            fuse = model.addFuse(part, lPipes, False)
            fuse.execute(True)
            fuse.setName(nameRes)
            fuse.result().setName(nameRes)
        else:
            lPipes[0].setName(nameRes)

#==========================================================

    def print_info (self, verbose, comment=""):
        """Impression si verbose est valide. Avec un comentaire introductif éventuellement."""
        if verbose:
            texte = "\n+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
            texte += "\nRécapitulatif"
            if comment:
                texte += " {}".format(comment)
            texte += "\ninfos points ="
            for key, value in self.infoPoints.items():
                texte += "\n{} : {}".format(key, value)
            texte += "\nconnectivities ="
            for key, value in self.connectivities.items():
                texte += "\n{} : {}".format(key, value)
            texte += "\n+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
            print(texte+"\n")

#==========================================================

# Execution of the Import

    def execute(self):
        """F.execute() -- execute the Feature"""
        # Retrieving the user input
        apath    = self.string(self.FILE_ID())

        filepath = apath.value()
        if filepath != "" :

            # A. Initialisation
            part = model.activeDocument()

            if self.lfeatures :
                for feature in self.lfeatures:
                    part.removeFeature(feature.feature())
                self.lfeatures = list()
                model.removeFolder(self.folder)

            self.infoPoints = dict()
            self.connectivities = dict()

            from os.path import basename
            filename = basename(filepath)
            if ( "." in filename ):
                laux= filename.split(".")
                nameRes = laux[0]
                for saux in laux[1:-1]:
                    nameRes+="."+saux
            else:
                nameRes = filename

            # Creating the construction points in the current document

            # B. Traitement du fichier
            print ("\n=============== Traitement du fichier {}".format(filepath))
            error = 0
            while True:

                # B.1. Lecture du fichier
                with open(filepath) as afile:
                    summary = 0
                    method = self.parligne
                    for line in afile:
                        printverbose  (line[:-1], verbose=self._verbose_max)

                        # B.1.1. Repérages
                        if line == "\n":
                            printverbose ("========================= Saut de ligne =========================", verbose=self._verbose_max)
                            continue
                        if line[0] == "#" or line[:3] == "...":
                            continue
                        if summary == 0 and line[:-1] == "nodes section" :
                            printverbose ("========================= Lecture des coordonnées ==============================", 80, verbose=self._verbose)
                            summary = 1
                            continue
                        if summary == 1 and line[:-1] == "connectivity section" :
                            printverbose ("========================= Lecture de la connectivité ===========================", 80, verbose=self._verbose)
                            summary = 2
                            continue
                        if summary == 2 and line[:6] == "method" :
                            printverbose ("===================== summary == 2 method =========================", verbose=self._verbose_max)
                            method = line[7:-1]
                            printverbose ("Méthode : '{}'".format(method), verbose=self._verbose)
                            if method not in (self.twopartwo, self.parligne):
                                raiseException("Problem with type of connectivity")
                            continue
                        if summary == 2 and line[:-1] == "fillets section" :
                            printverbose ("========================= Lecture des fillets =========================", 80, verbose=self._verbose)
                            summary = 3
                            continue

                        # B.1.2. Enregistrement des données
                        if summary == 1:
                            printverbose ("===================== summary == 1 =========================", 80, verbose=self._verbose_max)
                            diagno, texte = self.readNodeInfo(line[:-1])
                            if diagno:
                                raiseException("{}\nProblem with description of nodes.".format(texte))
                            continue
                        if summary == 2:
                            printverbose ("===================== summary == 2 =========================", 80, verbose=self._verbose_max)
                            diagno = self.readConnectivity(line[:-1],method)
                            if diagno:
                                raiseException("Problem with description of connectivities")
                            continue
                        if summary == 3:
                            printverbose ("===================== summary == 3 =========================", 80, verbose=self._verbose_max)
                            diagno = self.readFillet(line[:-1])
                            if diagno:
                                raiseException("Problem with description of fillets")
                            continue

                        printverbose ("===================== Rien =========================", 80, verbose=self._verbose_max)
                        if diagno:
                            error = diagno
                            break

                if error:
                    break


                # B.2. Gestion des points alignés
                self.print_info (self._verbose_max, "avant gestion des points alignés")

                self.correctConnectivity ()

                # B.3. Signalement de la fin d'une chaine
                for _, value in self.connectivities.items():
                    self.infoPoints[value['chainage'][-1]]["isEnd"] = True

                self.print_info (self._verbose_max, "avant les création de points, etc.")

                # B.4. Creation des points
                self.createPoints(part)

                # B.5. Creation des polylines
                self.createPolylines(part)

                # B.6. Creation des fillets
                self.createFillets(part)

                # B.7. Recherche des coudes droits
                self.searchRightConnections(part)

                # B.8. Création des paths pour le pipeNetwork
                self.createPaths(part)

                # B.9. Création des sketchs pour le pipeNetwork
                self.createSketches(part)

                self.print_info (self._verbose_max, "après la création des sketchs")

                # B.10. Création des pipes
                self.createPipes(part, nameRes)

                # B.11. Dossier pour les opérations internes
                print("========================= Mise en dossier =========================")
                self.folder = model.addFolder(part, self.lfeatures[0], self.lfeatures[-1])
                self.folder.setName("{}_inter".format(nameRes))

                # B.12. Ménage des résultats inutiles
                print("========================= Ménage des résultats inutiles ==================")
                laux = list()
                for iaux in range(len(self.ledges)):
                    laux.append(model.selection("EDGE", "Edge_{}_1".format(iaux)))
                _ = model.addRemoveResults(part, laux)

                break

            return

    def isMacro(self):
        """Override Feature.initAttributes().
        F.isMacro() -> True

        pipeNetwork feature is macro: removes itself on the creation transaction
        finish.
        """
        return False
