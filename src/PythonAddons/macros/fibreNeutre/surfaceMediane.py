# -*- coding: utf-8 -*-
# Copyright (C) 2016-2021  CEA/DEN, EDF R&D
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
"""Obtention des surfaces médianes à partir d'un objet GEOM ou SHAPER

On sait traiter les faces :
  . planes
  . cylindriques
  . sphériques
  . toriques
  . coniques

Version initiale par :
alexandre.prunie@blastsolutions.io
guillaume.schweitzer@blastsolutions.io

Gérald NICOLAS
+33.1.78.19.43.52
"""

__revision__ = "V10.09"

#========================= Les imports - Début ===================================

import os
import sys
import tempfile

import salome
import SALOMEDS
from salome.shaper import model
from salome.geom import geomBuilder
from salome.geom import geomtools
from salome.kernel.studyedit import getStudyEditor

import numpy as np

#========================== Les imports - Fin ====================================

D_FMT = dict()
D_FMT["stp"] = ["stp", "step"]
D_FMT["igs"] = ["igs", "iges"]
for CLE in ("brep", "xao"):
  D_FMT[CLE] = [CLE]

#========================= Début de la fonction ==================================

def decode_cao (fmt_cao):
  """Décode le format de la cao

Entrées :
  :fmt_cao: format du fichier, step, iges, etc.
Sorties :
  :fmt_cao_0: format décodé
  """

  fmt_cao_0 = ""

  fmt_cao_low = fmt_cao.lower()

  for cle, l_aux in D_FMT.items():
    if ( fmt_cao_low in l_aux ):
      fmt_cao_0 = cle
      break

  return fmt_cao_0

#=========================  Fin de la fonction ===================================

#========================= Début de la fonction ==================================

def import_cao (part_doc, ficcao, verbose=False):
  """Importation d'une cao

Entrées :
  :part_doc: part
  :ficcao: le fichier de la CAO
Sorties :
  :objet: l'objet importé dans SHAPER
  """


  erreur = 0
  message = "Fichier '{}'\n".format(ficcao)
  if verbose:
    print (message)

  objet = None

  laux = ficcao.split(".")
  fmt_cao_0 = decode_cao (laux[-1])

  if ( fmt_cao_0 not in ("stp", "brep", "igs", "xao") ):
    message += "Le format de CAO est inconnu"
    erreur = 1

  elif not ficcao:
    message += "Le fichier de CAO n'a pas été décodé correctement."
    erreur = 2

  elif not os.path.isfile(ficcao):
    message += "Le fichier de CAO est inconnu."
    erreur = 3

  else:

    message = ""
    objet = model.addImport(part_doc, ficcao)
    objet.execute(True)
    model.do()

    if verbose:
      texte  = "Objet   : '{}'\n".format(objet.result().name())
      texte += "De type : '{}'".format(objet.result().shapeType())
      print (texte)


  return erreur, message, objet

#=========================  Fin de la fonction ===================================

#=================================== La classe ===================================

class SurfaceMediane (object):

  """Calcul des surfaces médianes de solides minces

L'objectif de ce programme est de créer les surfaces médianes, encore appelées fibres neutres, pour \
une structure qui est un solide ou un assemblage de solides (compound).
Pour réaliser l'opération, deux façons de faire :

1. On sélectionne la structure dans l'arbre d'étude ou dans la fenêtre graphique de GEOM, puis on lance le script.

2. On écrit un script python dans lequel on réalise la création ou l'importation d'une CAO. \
On insère cette classe dans le script, puis on lance surf_fic_cao, surf_objet_shaper ou surf_objet_geom selon le point de départ.

Le programme crée les surfaces sous réserve que pour le solide envisagé, il a réussi à trouver deux faces \
de taille identique et supérieure aux autres faces du solide pour des polyèdres ou \
s'il a reconnu des formes canoniques.
Il crée alors une surface au milieu de ces deux grandes faces. Cette face est coloriée en vert.

Si les 2 faces les plus grandes sont planes mais que leurs tailles ne sont pas identiques, le programme \
crée néanmoins une face basée sur la plus grande de ces faces. Un message est émis et cette face médiane \
est coloriée en bleu. Le volume correspondant n'est pas détruit et est colorié en rouge.

On sait traiter les faces :
  . planes
  . cylindriques
  . sphériques
  . toriques
  . coniques

Options obligatoires
********************
Aucune

Options facultatives
********************
. Suppression des solides et faces créés par l'explosion des objets. Par défaut, la suppression est faite.
-menage/-no_menage

. Retour dans Shaper. Par défaut, les faces restent dans GEOM.
-retour_shaper/-no_retour_shaper

  """

# A. La base

  message_info = ""
  _verbose = 0
  _verbose_max = 0
  affiche_aide_globale = 0
  _menage = True

# B. Les variables

  _choix_objet = 0
  _retour_shaper = False
  nom_solide = None
  epsilon = 5.e-3
  part_doc = None

  ficcao = None
  rep_trav = None
  objet_geom = None

  l_faces_trans = list()

  faces_pb_nb = 0
  faces_pb_msg = ""

#=========================== Début de la méthode =================================

  def __init__ ( self, liste_option ):

    """Le constructeur de la classe SurfaceMediane

Décodage des arguments
On cherche ici les arguments généraux : aide, verbeux
    """

    for option in liste_option :

      #print (option)
      if isinstance(option,str):
        saux = option.upper()
      #print (saux)
      if saux in ( "-H", "-HELP" ):
        self.affiche_aide_globale = 1
      elif saux == "-V" :
        self._verbose = 1
      elif saux == "-VMAX" :
        self._verbose = 1
        self._verbose_max = 1
      elif saux == "-MENAGE" :
        self._menage = True
      elif saux == "-NO_MENAGE" :
        self._menage = False
      elif saux == "-RETOUR_SHAPER":
        self._retour_shaper = True
      elif saux == "-NO_RETOUR_SHAPER":
        self._retour_shaper = False

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def __del__(self):
    """A la suppression de l'instance de classe"""
    if self._verbose_max:
      print ("Suppression de l'instance de la classe.")

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _selection_objet_graphique ( self, geompy ):
    """Sélectionne l'objet dans la fenêtre graphique

Entrées :
  :geompy: environnement de GEOM

Sorties :
  :objet: objet à traiter
    """

    nom_fonction = __name__ + "/_selection_objet_graphique"
    blabla = "\nDans {} :\n".format(nom_fonction)

    erreur = 0
    message = ""
    objet = None

    while ( not erreur ):

# 1. Contrôle du nombre d'objets sélectionnés graphiquement
      nb_objet = salome.sg.SelectedCount()
      if self._verbose_max:
        print (blabla+"Nombre d'objets sélectionnés : {}.".format(nb_objet))

      if ( nb_objet != 1 ):
        message = "Nombre d'objets sélectionnés : {}.\nIl en faut un et un seul.".format(nb_objet)
        erreur = 1
        break

# 2. Récupération de l'ID de l'objet en cours
      entry = salome.sg.getSelected(0)
#     Récupération de l'objet
      objet = salome.myStudy.FindObjectID( entry ).GetObject()
      if self._verbose_max:
        print (geompy.WhatIs(objet))

      break

    return erreur, message, objet

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _les_solides ( self, geompy, objet ):
    """Les solides de l'objet à traiter

Entrées :
  :geompy: environnement de GEOM
  :objet: l'objet à traiter

Sorties :
  :l_solides: liste des solides de la sélection
    """

    nom_fonction = __name__ + "/_les_solides"
    blabla = "\nDans {} :\n".format(nom_fonction)
    if self._verbose_max:
      print (blabla)

    erreur = 0
    message = ""
    l_solides = list()

    while ( not erreur ):

# 1. Nombre de solides composant l'objet à traiter
      d_shape_info = geompy.ShapeInfo(objet)
      n_solides = d_shape_info["SOLID"]
      if self._verbose_max:
        print ("Nombre de solides : {}".format(n_solides))

      nom_objet = objet.GetName()
      if self._verbose:
        print (". Traitement de l'objet '{}'".format(nom_objet))

# 2. Liste des solides qui composent l'objet
      if ( n_solides == 0 ):
        message = "Aucun solide dans l'objet sélectionné."
        erreur = 1
        break
      elif ( n_solides == 1 ):
        l_solides.append(objet)
      else:
        l_solides = geompy.ExtractShapes(objet, geompy.ShapeType["SOLID"], True)
        for iaux, solide in enumerate(l_solides):
          geompy.addToStudyInFather( objet, solide, "{}_{:03d}".format(nom_objet,iaux+1) )
        if self._verbose_max:
          print ("l_solides : {}.".format(l_solides))
          for iaux, solide in enumerate(l_solides):
            print (geompy.WhatIs(solide))

      break

    return erreur, message, l_solides

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _faces_du_solide ( self, geompy, solide ):
    """Détermine les faces d'un solide

Entrées :
  :geompy: environnement de GEOM
  :solide: le solide à traiter

Sorties :
  :l_faces: liste des faces du solide
    """

    nom_fonction = __name__ + "/_faces_du_solide"
    blabla = "\nDans {} :\n".format(nom_fonction)
    if self._verbose_max:
      print (blabla)

    erreur = 0
    message = ""
    l_faces = list()

    while ( not erreur ):

      self.nom_solide = solide.GetName()
      if self._verbose:
        print (".. Traitement du solide '{}'".format(self.nom_solide))

# 1. Le solide est-il un solide unique ?
      if self._verbose_max:
        print (geompy.WhatIs(solide))
        longueur, aire, volume = geompy.BasicProperties(solide)
        if self._verbose_max:
          print (". longueur, aire, volume : {}, {}, {}".format(longueur,aire,volume))
#     Contrôle du solide
      d_shape_info = geompy.ShapeInfo(solide)
      n_solides = d_shape_info["SOLID"]
      if self._verbose_max:
        print ("Nombre de solides : {}".format(n_solides))
      if ( n_solides != 1 ):
        message = "Nombre de solides : {}.\nIl en faut un et un seul.".format(n_solides)
        erreur = 1
        break

# 2. Liste des faces qui composent le solide
      l_faces = geompy.ExtractShapes(solide, geompy.ShapeType["FACE"], True)
      for iaux, face in enumerate(l_faces):
        geompy.addToStudyInFather( solide, face, "Face_{}".format(iaux+1) )
      if self._verbose_max:
        print ("l_faces : {}".format(l_faces))
        for iaux, face in enumerate(l_faces):
          print (geompy.WhatIs(face))

      break

    return erreur, message, l_faces

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _calcul_caract_faces ( self, geompy, l_faces ):
    """Calcule les caractéristiques géométriques des faces

Entrées :
  :geompy: environnement de GEOM
  :l_faces: liste des faces du solide

Sorties :
  :tb_caract: tableau des caractéristiques géométriques des faces
    """

    nom_fonction = __name__ + "/_calcul_caract_faces"
    blabla = "\nDans {} :\n".format(nom_fonction)

    nb_faces = len(l_faces)
    if self._verbose_max:
      print (blabla+"Nombre de faces : {}.".format(nb_faces))

    tb_caract = np.zeros((nb_faces,3), dtype = 'object')
    for iaux, face in enumerate(l_faces):
      _, aire, _ = geompy.BasicProperties(face)
      #longueur, aire, volume = geompy.BasicProperties(face)
      if self._verbose_max:
        texte = "\t. Face numéro {}".format(iaux)
        #texte += "\n\t. longueur, aire, volume : {}, {}, {}".format(longueur,aire,volume)
        print (texte)

      tb_caract [iaux][0] = face
      tb_caract [iaux][1] = aire
      tb_caract [iaux][2] = geompy.KindOfShape(face)
      if self._verbose_max:
        print ("\t. tb_caract : {} {}".format(aire,tb_caract[iaux][2]))

    return tb_caract

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _tri_faces ( self, tb_caract ):
    """Trie les faces en fonction de leurs surfaces

Entrées :
  :geompy: environnement de GEOM

Sorties :
  :tb_caract_1[-1], tb_caract_1[-2]: les caractéristiques des 2 faces les plus grandes
    """

    erreur = 0
    message = ""

    nom_fonction = __name__ + "/_tri_faces"
    blabla = "\nDans {} :\n".format(nom_fonction)

# Tri du tableau en fonction des surfaces
    if self._verbose_max:
      print (blabla+"tb_caract brut : {}".format(tb_caract))
    tb_caract_1 = sorted(tb_caract, key=lambda colonnes: colonnes[1])
    if self._verbose_max:
      print ("tb_caract trié : {}".format(tb_caract_1))

    if self._verbose:
      texte  = "\tSurface de la plus grande face : {}, de caractéristiques {}\n".format(tb_caract_1[-1][1],tb_caract_1[-1][2])
      texte += "\tSurface de la face suivante    : {}, de caractéristiques {}".format(tb_caract_1[-2][1],tb_caract_1[-2][2])
      print (texte)

# La surface suivante doit être différente, sinon ce n'est pas un solide mince
    if ( np.abs((tb_caract_1[-1][1]-tb_caract_1[-3][1])/tb_caract_1[-1][1]) < self.epsilon ):
      message += ". Surface de la plus grande face   : {}\n".format(tb_caract_1[-1][1])
      message += ". Surface de la face suivante      : {}\n".format(tb_caract_1[-2][1])
      message += ". Surface de la 3ème face suivante : {}\n".format(tb_caract_1[-3][1])
      message += "==> L'écart est trop faible.\n"
      message += "Impossible de créer la face médiane pour le solide '{}' ".format(self.nom_solide)
      message += "car le solide n'est pas assez mince."
      erreur = -1

    return erreur, message, tb_caract_1[-1], tb_caract_1[-2]

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _calcul_caract_aretes_face ( self, geompy, caract_face ):
    """Détermine les caractéristiques des arêtes d'une face

Entrées :
  :geompy: environnement de GEOM
  :caract_face: les caractéristiques de la face

Sorties :
  :caract_arete_face: les caractéristiques des arêtes de la face
    """

    nom_fonction = __name__ + "/_calcul_caract_aretes_face"
    blabla = "\nDans {} :\n".format(nom_fonction)

    if self._verbose_max:
      texte = blabla
      texte += "face : {}\n".format(caract_face)
      print (texte)

# Détermination des arêtes pour chaque face
    face = caract_face[0]
    l_aretes = geompy.ExtractShapes(face, geompy.ShapeType["EDGE"], True)
    caract_arete_face = list()
    for arete in l_aretes:
      caract_arete_face.append(geompy.KindOfShape(arete))

    if self._verbose_max:
      print ("Aretes de la face : {}".format(caract_arete_face))

    return caract_arete_face

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _cree_face_mediane ( self, geompy, solide, caract_face_1, caract_face_2 ):
    """Crée la face médiane entre deux autres

Entrées :
  :geompy: environnement de GEOM
  :solide: l'objet solide à traiter
  :caract_face_1, caract_face_2: les caractéristiques des 2 faces les plus grandes

Sorties :
  :face: la face médiane
    """
    erreur = 0
    message = ""
    face =  None

    nom_fonction = __name__ + "/_cree_face_mediane"
    blabla = "\nDans {} :\n".format(nom_fonction)

    if self._verbose_max:
      texte = blabla
      texte += "face_1 : {}\n".format(caract_face_1)
      texte += "face_2 : {}".format(caract_face_2)
      print (texte)

    while not erreur:

# 1. Forme de la face
      forme = caract_face_1[2][0]
      if self._verbose_max:
        print ("forme = {}".format(forme) )

# 2. Traitement selon la forme de la face
# 2.1. Face plane
      if forme in ( geompy.kind.DISK_CIRCLE, geompy.kind.DISK_ELLIPSE, geompy.kind.POLYGON, geompy.kind.PLANE, geompy.kind.PLANAR):
        erreur, message, face = self._cree_face_mediane_polygone ( geompy, caract_face_1, caract_face_2 )

# 2.2. Face cylindrique
      elif forme == geompy.kind.CYLINDER2D:
        face = self._cree_face_mediane_cylindre ( geompy, solide, caract_face_1, caract_face_2 )

# 2.3. Face sphérique
      elif forme == geompy.kind.SPHERE2D:
        face = self._cree_face_mediane_sphere ( geompy, solide, caract_face_1, caract_face_2 )

# 2.4. Face torique
      elif forme == geompy.kind.TORUS2D:
        face = self._cree_face_mediane_tore ( geompy, solide, caract_face_1, caract_face_2 )

# 2.5. Face conique
      elif forme == geompy.kind.CONE2D:
        face = self._cree_face_mediane_cone ( geompy, solide, caract_face_1, caract_face_2 )

# 2.N. Face de forme inconnue
      else:
        self.faces_pb_msg += "Impossible de créer la face médiane pour le solide '{}' ".format(self.nom_solide)
        self.faces_pb_msg += "car sa face la plus grande est de forme : {}\n".format(forme)
        self.faces_pb_nb += 1

# 3. Gestion de la face produite

      if face is not None:
        erreur, message = self._cree_face_mediane_0 ( geompy, face )

      break

    return erreur, message, face

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _cree_face_mediane_0 ( self, geompy, face ):
    """Gestion de la face médiane créée entre deux autres

Entrées :
  :geompy: environnement de GEOM
  :face: la face médiane créée
    """
    erreur = 0
    message = ""

    nom_fonction = __name__ + "/_cree_face_mediane_0"
    blabla = "\nDans {} :\n".format(nom_fonction)

    if self._verbose_max:
      texte = blabla
      print (texte)

    while not erreur:

# 3.1. Insertion dans l'étude
      nom_face = self.nom_solide+"_M"
      geompy.addToStudy (face,nom_face)

# 3.2. Transfert éventuel de GEOM vers SHAPER
      if self._retour_shaper:

# 3.2.1. Le fichier doit être différent à chaque fois
        #print ("l_faces_trans = {}".format(self.l_faces_trans))
        nom_fic = nom_face
        iaux = 0
        while True:
          if nom_fic in self.l_faces_trans:
            nom_fic = "{}_{}".format(nom_face,iaux)
            iaux += 1
          else:
            break
        self.l_faces_trans.append(nom_fic)
        fichier = os.path.join(self.rep_trav, "{}.stp".format(nom_fic))

# 3.2.2. Exportation depuis GEOM
        if self._verbose_max:
          texte = "Exportation depuis GEOM de la face médiane '{}' dans le fichier {}".format(nom_face,fichier)
          print (texte)
        try:
          geompy.ExportSTEP (face, fichier)
        except OSError as err:
          print (err)
          message += "Impossible d'exporter la face médiane '{}' dans le fichier {}".format(nom_face,fichier)
          erreur = 1
          break

# 3.2.3. Importation dans SHAPER
        if self._verbose_max:
          texte = "Importation dans SHAPER de la face médiane '{}' depuis le fichier {}".format(nom_face,fichier)
          print (texte)
        face_mediane = model.addImportSTEP(self.part_doc, fichier, False, False, False)
        face_mediane.execute(True)
        model.do()

#       Le nom
        face_mediane.setName(nom_face)
        face_mediane.result().setName(nom_face)

#       La couleur. Attention aux conventions différents entr GEOM et SHAPER
        while True:
          tbaux = np.array(face.GetColor()._tuple())
          np_aux = (tbaux<0.).nonzero()
          if np.any(np_aux):
            break
          np_aux = (tbaux>1.).nonzero()
          if np.any(np_aux):
            break
          tbaux *= 255.
          face_mediane.result().setColor(int(tbaux[0]), int(tbaux[1]), int(tbaux[2]))
          break

      break

    return erreur, message

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _cree_face_mediane_polygone ( self, geompy, caract_face_1, caract_face_2 ):
    """Crée la face médiane entre deux autres - cas des polygones

Entrées :
  :geompy: environnement de GEOM
  :caract_face_1, caract_face_2: les caractéristiques des 2 faces les plus grandes

Sorties :
  :face: la face médiane
    """
    erreur = 0
    message = ""
    face =  None

    nom_fonction = __name__ + "/_cree_face_mediane_polygone"
    blabla = "\nDans {} :\n".format(nom_fonction)

    while not erreur:

#     Les deux faces
      face_1 = caract_face_1[0]
      face_2 = caract_face_2[0]
      if self._verbose_max:
        texte = blabla
        texte += "face_1 : {}\n".format(caract_face_1)
        texte += "face_2 : {}".format(caract_face_2)
        print (texte)

#     Les 2 surfaces doivent être les mêmes, sinon c'est qu'elles ne sont pas similaires
      ecart = np.abs((caract_face_1[1]-caract_face_2[1])/caract_face_1[1])
      if ( ecart > self.epsilon ):
        if self._verbose:
          message += ". Surface de la plus grande face : {}\n".format(caract_face_1[1])
          message += ". Surface de la face suivante    : {}\n".format(caract_face_2[1])

#     Distance entre les deux faces
      d_face_1_2 = geompy.MinDistance(face_1, face_2)
      if self._verbose:
        print ("\tDistance entre les deux faces = {}".format(d_face_1_2))

#     Copie de la plus grande face
      face_1_copie= geompy.MakeCopy(face_1)
      if self._verbose_max:
        geompy.addToStudy(face_1_copie, face_1.GetName()+"_copie" )

#     On la translate d'une demi épaisseur
#     Attention : les normales des faces issues de l'explosion du solide sont dirigées vers l'extérieur.
#                 Comme il faut translater la face vers l'intérieur, on le fait avec une distance négative.
      vnorm_face_1_copie = geompy.GetNormal(face_1_copie)
      face = geompy.TranslateVectorDistance (face_1_copie, vnorm_face_1_copie, -0.5*d_face_1_2)

#     Si les 2 surfaces ne sont pas similaires, on a quand même tenté la création mais on le signale
      if ( ecart > self.epsilon ):
        face.SetColor(SALOMEDS.Color(0,0,1))
        message += "Problème pour créer la face médiane pour le solide '{}' : ".format(self.nom_solide)
        message += "ce n'est pas un véritable polyèdre."
        if self._verbose:
          message += "\nL'écart de surface entre les 2 plus grandes faces est trop grand : {:5.2f}%.\n".format(ecart*100.)
          message += "Tentative de création à partir de la plus grande des faces du solide."
        erreur = -2
      else:
        face.SetColor(SALOMEDS.Color(0,1,0))

      break

    return erreur, message, face

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _cree_face_mediane_cylindre ( self, geompy, solide, caract_face_1, caract_face_2 ):
    """Crée la face médiane entre deux autres - cas des cylindres

Entrées :
  :geompy: environnement de GEOM
  :solide: l'objet solide à traiter
  :caract_face_1, caract_face_2: les caractéristiques des 2 faces les plus grandes

Sorties :
  :face: la face médiane
    """
    face =  None

    nom_fonction = __name__ + "/_cree_face_mediane_cylindre"
    blabla = "\nDans {} :\n".format(nom_fonction)

#   Les deux faces
    if self._verbose_max:
      texte = blabla
      texte += "face_1 : {}\n".format(caract_face_1)
      texte += "face_2 : {}".format(caract_face_2)
      print (texte)

#   Caractéristiques des cylindres
    coo_x, coo_y, coo_z, axe_x, axe_y, axe_z, rayon, hauteur = self._cree_face_mediane_cylindre_0 ( geompy, solide, caract_face_1, caract_face_2 )

#   Création de la face
    centre, axe, cylindre = self._cree_face_mediane_cylindre_1 ( geompy, coo_x, coo_y, coo_z, axe_x, axe_y, axe_z, rayon, hauteur )

#   Gestion de l'intersection avec le solide initial
    face = self._cree_face_mediane_cylindre_2 ( geompy, solide, centre, axe, cylindre )

#   Couleur verte
    face.SetColor(SALOMEDS.Color(0,1,0))

    return face

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _cree_face_mediane_cylindre_0 ( self, geompy, solide, caract_face_1, caract_face_2 ):
    """Crée la face médiane entre deux autres - cas des cylindres

Décodage des caractéristiques

Entrées :
  :geompy: environnement de GEOM
  :solide: l'objet solide à traiter
  :caract_face_1, caract_face_2: les caractéristiques des 2 faces les plus grandes

Sorties :
  :coo_x, coo_y, coo_z: coordonnées du centre de la base
  :axe_x, axe_y, axe_z: coordonnées de l'axe
  :rayon: rayon moyen entre les deux faces
  :hauteur: hauteur du cylindre
    """

    nom_fonction = __name__ + "/_cree_face_mediane_cylindre_0"
    blabla = "\nDans {} :\n".format(nom_fonction)

#   Les deux faces
    if self._verbose_max:
      texte = blabla
      texte += "face_1 : {}\n".format(caract_face_1)
      texte += "face_2 : {}".format(caract_face_2)
      print (texte)

#   Coordonnées du centre de la base
    coo_x = caract_face_1[2][1]
    coo_y = caract_face_1[2][2]
    coo_z = caract_face_1[2][3]
#   Coordonnées de l'axe
    axe_x = caract_face_1[2][4]
    axe_y = caract_face_1[2][5]
    axe_z = caract_face_1[2][6]
#   Rayons
    rayon = (caract_face_2[2][7]+caract_face_1[2][7])/2.
#   Hauteur : la diagonale de la boîte englobante permet d'être certain de tout prendre
    hauteur = self._calcul_hauteur_englobante ( geompy, solide )

    return coo_x, coo_y, coo_z, axe_x, axe_y, axe_z, rayon, hauteur

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _cree_face_mediane_cylindre_1 ( self, geompy, coo_x, coo_y, coo_z, axe_x, axe_y, axe_z, rayon, hauteur ):
    """Crée la face médiane entre deux autres - cas des cylindres

Création des objets temporaires et de la face externe du cylindre support

Entrées :
  :geompy: environnement de GEOM
  :coo_x, coo_y, coo_z: coordonnées du centre de la base
  :axe_x, axe_y, axe_z: coordonnées de l'axe
  :rayon: rayon moyen entre les deux faces
  :hauteur: hauteur du cylindre

Sorties :
  :centre: le centre de la base du cylindre médian
  :axe: l'axe des cylindres
  :cylindre: le cylindre support
    """
    nom_fonction = __name__ + "/_cree_face_mediane_cylindre_1"
    blabla = "\nDans {} :\n".format(nom_fonction)

#   Les deux faces
    if self._verbose_max:
      texte = blabla
      texte += "Centre : ({}, {}, {})\n".format(coo_x, coo_y, coo_z)
      texte += "Axe    : ({}, {}, {})\n".format(axe_x, axe_y, axe_z)
      texte += "Rayon : {}\n".format(rayon)
      texte += "Hauteur : {}".format(hauteur)
      print (texte)

#   Création du point central
    centre = geompy.MakeVertex(coo_x, coo_y, coo_z)
    #geompy.addToStudy( centre, "centre" )

#   Création de l'axe
    axe = geompy.MakeVectorDXDYDZ(axe_x, axe_y, axe_z)
    #geompy.addToStudy( axe, "axe" )

#   Création d'un cylindre
    cylindre = geompy.MakeCylinder(centre, axe, rayon, hauteur)
    if self._verbose_max:
      geompy.addToStudy( cylindre, "cylindre médian" )

    return centre, axe, cylindre

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _cree_face_mediane_cylindre_2 ( self, geompy, solide, centre, axe, cylindre ):
    """Crée la face médiane entre deux autres - cas des cylindres

Intersection de la face cylindrique avec le solide initial

Entrées :
  :geompy: environnement de GEOM
  :solide: l'objet solide à traiter
  :centre: le centre de la base du cylindre médian
  :axe: l'axe des cylindres
  :cylindre: le cylindre support

Sorties :
  :face: la face médiane
    """
    face =  None

    nom_fonction = __name__ + "/_cree_face_mediane_cylindre_2"
    blabla = "\nDans {} :\n".format(nom_fonction)

    if self._verbose_max:
      print (blabla)

#   Récupération de la bonne face après intersection du cylindre avec le solide initial
    face = self._creation_face_inter ( geompy, solide, cylindre, 1 )

#   Pour des raisons inconnues de moi, il arrive que l'axe donné par les caractéristiques soit dans le mauvais sens.
#   Dans ce cas l'intersection est vide, et il faut retourner la face créée précédemment et recalculer l'intersection.
    tb_caract = geompy.KindOfShape(face)
    if ( tb_caract[1] == 0 ):
      if self._verbose_max:
        print ("On opère un retournement du cylindre.")
      plan_de_base = geompy.MakePlane(centre, axe, 100)
      #geompy.addToStudy( plan_de_base, "plan_de_base" )
      geompy.MirrorByPlane(cylindre, plan_de_base)
#     Récupération de la bonne face après intersection du cylindre retourné avec le solide initial
      face = self._creation_face_inter ( geompy, solide, cylindre, 1 )

    return face

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _cree_face_mediane_sphere ( self, geompy, solide, caract_face_1, caract_face_2 ):
    """Crée la face médiane entre deux autres - cas des sphères

Entrées :
  :geompy: environnement de GEOM
  :solide: l'objet solide à traiter
  :caract_face_1, caract_face_2: les caractéristiques des 2 faces les plus grandes

Sorties :
  :face: la face médiane
    """
    face =  None

    nom_fonction = __name__ + "/_cree_face_mediane_sphere"
    blabla = "\nDans {} :\n".format(nom_fonction)

#   Les deux faces
    if self._verbose_max:
      texte = blabla
      texte += "face_1 : {}\n".format(caract_face_1)
      texte += "face_2 : {}".format(caract_face_2)
      print (texte)

#   Caractéristiques des sphères
    coo_x, coo_y, coo_z, rayon = self._cree_face_mediane_sphere_0 ( caract_face_1, caract_face_2 )

#   Création de la face
    face = self._cree_face_mediane_sphere_1 ( geompy, solide, coo_x, coo_y, coo_z, rayon )

#   Couleur verte
    face.SetColor(SALOMEDS.Color(0,1,0))

    return face

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _cree_face_mediane_sphere_0 ( self, caract_face_1, caract_face_2 ):
    """Crée la face médiane entre deux autres - cas des sphères

Décodage des caractéristiques

Entrées :
  :caract_face_1, caract_face_2: les caractéristiques des 2 faces les plus grandes

Sorties :
  :coo_x, coo_y, coo_z: coordonnées du centre de la sphère
  :rayon: rayon moyen entre les deux faces
    """

    nom_fonction = __name__ + "/_cree_face_mediane_sphere_0"
    blabla = "\nDans {} :\n".format(nom_fonction)

#   Les deux faces
    if self._verbose_max:
      texte = blabla
      texte += "face_1 : {}\n".format(caract_face_1)
      texte += "face_2 : {}".format(caract_face_2)
      print (texte)

#   Coordonnées du centre de la sphère
    coo_x = caract_face_1[2][1]
    coo_y = caract_face_1[2][2]
    coo_z = caract_face_1[2][3]
#   Rayons
    rayon = (caract_face_2[2][4]+caract_face_1[2][4])/2.

    return coo_x, coo_y, coo_z, rayon

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _cree_face_mediane_sphere_1 ( self, geompy, solide, coo_x, coo_y, coo_z, rayon ):
    """Crée la face médiane entre deux autres - cas des sphères

Création des objets temporaires et de la face externe de la sphère support

Entrées :
  :geompy: environnement de GEOM
  :solide: l'objet solide à traiter
  :coo_x, coo_y, coo_z: coordonnées du centre de la sphère
  :rayon: rayon moyen entre les deux faces

Sorties :
  :face: la face externe de la sphère support
    """
    face =  None

    nom_fonction = __name__ + "/_cree_face_mediane_sphere_1"
    blabla = "\nDans {} :\n".format(nom_fonction)

#   Les deux faces
    if self._verbose_max:
      texte = blabla
      texte += "Centre : ({}, {}, {})\n".format(coo_x, coo_y, coo_z)
      texte += "Rayon : {}".format(rayon)
      print (texte)

#   Création du point central
    centre = geompy.MakeVertex(coo_x, coo_y, coo_z)
    #geompy.addToStudy( centre, "centre" )

#   Création d'une sphère médiane
    sphere = geompy.MakeSpherePntR(centre, rayon)
    if self._verbose_max:
      geompy.addToStudy( sphere, "sphère médiane" )

#   Récupération de la bonne face après intersection avec le solide initial
    face = self._creation_face_inter ( geompy, solide, sphere, 0 )

    return face

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _cree_face_mediane_tore ( self, geompy, solide, caract_face_1, caract_face_2 ):
    """Crée la face médiane entre deux autres - cas des tores

Entrées :
  :geompy: environnement de GEOM
  :solide: l'objet solide à traiter
  :caract_face_1, caract_face_2: les caractéristiques des 2 faces les plus grandes

Sorties :
  :face: la face médiane
    """
    face =  None

    nom_fonction = __name__ + "/_cree_face_mediane_tore"
    blabla = "\nDans {} :\n".format(nom_fonction)

#   Les deux faces
    if self._verbose_max:
      texte = blabla
      texte += "face_1 : {}\n".format(caract_face_1)
      texte += "face_2 : {}".format(caract_face_2)
      print (texte)

#   Caractéristiques des tores
    coo_x, coo_y, coo_z, axe_x, axe_y, axe_z, rayon_1, rayon_2 = self._cree_face_mediane_tore_0 ( caract_face_1, caract_face_2 )

#   Création de la face
    face = self._cree_face_mediane_tore_1 ( geompy, solide, coo_x, coo_y, coo_z, axe_x, axe_y, axe_z, rayon_1, rayon_2 )

#   Couleur verte
    face.SetColor(SALOMEDS.Color(0,1,0))

    return face

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _cree_face_mediane_tore_0 ( self, caract_face_1, caract_face_2 ):
    """Crée la face médiane entre deux autres - cas des tores

Décodage des caractéristiques

Entrées :
  :caract_face_1, caract_face_2: les caractéristiques des 2 faces les plus grandes

Sorties :
  :coo_x, coo_y, coo_z: coordonnées du centre du tore
  :axe_x, axe_y, axe_z: coordonnées de l'axe
  :rayon_1 : rayon principal
  :rayon_2 : rayon secondaire
    """

    nom_fonction = __name__ + "/_cree_face_mediane_tore_0"
    blabla = "\nDans {} :\n".format(nom_fonction)

#   Les deux faces
    if self._verbose_max:
      texte = blabla
      texte += "face_1 : {}\n".format(caract_face_1)
      texte += "face_2 : {}".format(caract_face_2)
      print (texte)

#   Coordonnées du centre du tore
    coo_x = caract_face_1[2][1]
    coo_y = caract_face_1[2][2]
    coo_z = caract_face_1[2][3]
#   Coordonnées de l'axe
    axe_x = caract_face_1[2][4]
    axe_y = caract_face_1[2][5]
    axe_z = caract_face_1[2][6]
#   Rayons
    rayon_1 = caract_face_2[2][7]
    rayon_2 = (caract_face_2[2][8]+caract_face_1[2][8])/2.

    return coo_x, coo_y, coo_z, axe_x, axe_y, axe_z, rayon_1, rayon_2

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _cree_face_mediane_tore_1 ( self, geompy, solide, coo_x, coo_y, coo_z, axe_x, axe_y, axe_z, rayon_1, rayon_2 ):
    """Crée la face médiane entre deux autres - cas des tores

Création des objets temporaires et de la face externe du tore support

Entrées :
  :geompy: environnement de GEOM
  :solide: l'objet solide à traiter
  :coo_x, coo_y, coo_z: coordonnées du centre du tore
  :axe_x, axe_y, axe_z: coordonnées de l'axe
  :rayon_1 : rayon principal
  :rayon_2 : rayon secondaire

Sorties :
  :face: la face externe du tore support
    """
    face =  None

    nom_fonction = __name__ + "/_cree_face_mediane_tore_1"
    blabla = "\nDans {} :\n".format(nom_fonction)

#   Les deux faces
    if self._verbose_max:
      texte = blabla
      texte += "Centre : ({}, {}, {})\n".format(coo_x, coo_y, coo_z)
      texte += "Axe    : ({}, {}, {})\n".format(axe_x, axe_y, axe_z)
      texte += "Rayon principal : {}\n".format(rayon_1)
      texte += "Rayon secondaire : {}".format(rayon_2)
      print (texte)

#   Création du point central
    centre = geompy.MakeVertex(coo_x, coo_y, coo_z)
    #geompy.addToStudy( centre, "centre" )

#   Création de l'axe
    axe = geompy.MakeVectorDXDYDZ(axe_x, axe_y, axe_z)
    #geompy.addToStudy( axe, "axe" )

#   Création d'un tore médian
    tore = geompy.MakeTorus(centre, axe, rayon_1, rayon_2)
    if self._verbose_max:
      geompy.addToStudy( tore, "tore médian" )

#   Récupération de la bonne face après intersection avec le solide initial
    face = self._creation_face_inter ( geompy, solide, tore, 0 )

    return face

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _cree_face_mediane_cone ( self, geompy, solide, caract_face_1, caract_face_2 ):
    """Crée la face médiane entre deux autres - cas des cones

Entrées :
  :geompy: environnement de GEOM
  :solide: l'objet solide à traiter
  :caract_face_1, caract_face_2: les caractéristiques des 2 faces les plus grandes

Sorties :
  :face: la face médiane
    """
    face =  None

    nom_fonction = __name__ + "/_cree_face_mediane_cone"
    blabla = "\nDans {} :\n".format(nom_fonction)

#   Les deux faces
    if self._verbose_max:
      texte = blabla
      texte += "face_1 : {}\n".format(caract_face_1)
      texte += "face_2 : {}".format(caract_face_2)
      print (texte)

#   Caractéristiques des cones
    coo_x, coo_y, coo_z, axe_x, axe_y, axe_z, rayon_1, rayon_2, hauteur = self._cree_face_mediane_cone_0 ( geompy, caract_face_1, caract_face_2 )

#   Création de la face
    centre, axe, cone = self._cree_face_mediane_cone_1 ( geompy, coo_x, coo_y, coo_z, axe_x, axe_y, axe_z, rayon_1, rayon_2, hauteur )

#   Gestion de l'intersection avec le solide initial
    face = self._cree_face_mediane_cone_2 ( geompy, solide, centre, axe, cone )

#   Couleur verte
    face.SetColor(SALOMEDS.Color(0,1,0))

    return face

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _cree_face_mediane_cone_0 ( self, geompy, caract_face_1, caract_face_2 ):
    """Crée la face médiane entre deux autres - cas des cones

Décodage des caractéristiques

Entrées :
  :geompy: environnement de GEOM
  :caract_face_1, caract_face_2: les caractéristiques des 2 faces les plus grandes

Sorties :
  :coo_x, coo_y, coo_z: coordonnées du centre de la base
  :axe_x, axe_y, axe_z: coordonnées de l'axe
  :rayon_1, rayon_2: rayons moyens du côté de la base et à l'opposé
  :hauteur: hauteur du cone
    """

    nom_fonction = __name__ + "/_cree_face_mediane_cone_0"
    blabla = "\nDans {} :\n".format(nom_fonction)

#   Les deux faces
    if self._verbose_max:
      texte = blabla
      texte += "face_1 : {}\n".format(caract_face_1)
      texte += "face_2 : {}".format(caract_face_2)
      print (texte)

#   Coordonnées du centre de la base
    coo_x = caract_face_1[2][1]
    coo_y = caract_face_1[2][2]
    coo_z = caract_face_1[2][3]
#   Coordonnées de l'axe
    axe_x = caract_face_1[2][4]
    axe_y = caract_face_1[2][5]
    axe_z = caract_face_1[2][6]
#   Rayons
#   Pour un cone complet, les caractéristiques forunies par GEOM sont correctes
#   Mais s'il est découpé, malheureusement,bug dans GEOM et caract_face_2[2][8] est toujours nul !
#   Alors on passe par le décodage des arêtes
    #rayon_1 = (caract_face_2[2][7]+caract_face_1[2][7])/2.
    #rayon_2 = (caract_face_2[2][8]+caract_face_1[2][8])/2.
    caract_arete_face_1 = self._calcul_caract_aretes_face ( geompy, caract_face_1 )
    caract_arete_face_2 = self._calcul_caract_aretes_face ( geompy, caract_face_2 )
    rayon_1 = 0.
    rayon_2 = 0.
    for caract_aretes_face in [caract_arete_face_1,caract_arete_face_2]:
      prem = True
      for l_aux in caract_aretes_face:
        if ( l_aux[0] in ( geompy.kind.CIRCLE, geompy.kind.ARC_CIRCLE ) ):
          #print ("R =",l_aux[7])
          if prem:
            rayon_1 += l_aux[7]
            prem = False
          else:
            rayon_2 += l_aux[7]
    rayon_1 *= 0.5
    rayon_2 *= 0.5
#   Hauteur
    hauteur = caract_face_1[2][9]

    return coo_x, coo_y, coo_z, axe_x, axe_y, axe_z, rayon_1, rayon_2, hauteur

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _cree_face_mediane_cone_1 ( self, geompy, coo_x, coo_y, coo_z, axe_x, axe_y, axe_z, rayon_1, rayon_2, hauteur ):
    """Crée la face médiane entre deux autres - cas des cones

Création des objets temporaires et de la face externe du cone support

Entrées :
  :geompy: environnement de GEOM
  :coo_x, coo_y, coo_z: coordonnées du centre de la base
  :axe_x, axe_y, axe_z: coordonnées de l'axe
  :rayon_1, rayon_2: rayons moyens du côté de la base et à l'opposé
  :hauteur: hauteur du cone

Sorties :
  :centre: le centre de la base du cone médian
  :axe: l'axe des cones
  :cone: le cone support
    """
    nom_fonction = __name__ + "/_cree_face_mediane_cone_1"
    blabla = "\nDans {} :\n".format(nom_fonction)

#   Les deux faces
    if self._verbose_max:
      texte = blabla
      texte += "Centre : ({}, {}, {})\n".format(coo_x, coo_y, coo_z)
      texte += "Axe    : ({}, {}, {})\n".format(axe_x, axe_y, axe_z)
      texte += "Rayons : {}, {}\n".format(rayon_1, rayon_2)
      texte += "Hauteur : {}".format(hauteur)
      print (texte)

#   Création du point central
    centre = geompy.MakeVertex(coo_x, coo_y, coo_z)
    #geompy.addToStudy( centre, "centre" )

#   Création de l'axe
    axe = geompy.MakeVectorDXDYDZ(axe_x, axe_y, axe_z)
    #geompy.addToStudy( axe, "axe" )

#   Création d'un cone
    cone = geompy.MakeCone(centre, axe, rayon_1, rayon_2, hauteur)
    if self._verbose_max:
      geompy.addToStudy( cone, "cone médian" )

    return centre, axe, cone

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _cree_face_mediane_cone_2 ( self, geompy, solide, centre, axe, cone ):
    """Crée la face médiane entre deux autres - cas des cones

Intersection de la face cylindrique avec le solide initial

Entrées :
  :geompy: environnement de GEOM
  :solide: l'objet solide à traiter
  :centre: le centre de la base du cone médian
  :axe: l'axe des cones
  :cone: le cone support

Sorties :
  :face: la face médiane
    """
    face =  None

    nom_fonction = __name__ + "/_cree_face_mediane_cone_2"
    blabla = "\nDans {} :\n".format(nom_fonction)

    if self._verbose_max:
      print (blabla)

#   Récupération de la bonne face après intersection du cone avec le solide initial
    face = self._creation_face_inter ( geompy, solide, cone, 1 )

#   Pour des raisons inconnues de moi, il arrive que l'axe donné par les caractéristiques soit dans le mauvais sens.
#   Dans ce cas l'intersection est vide, et il faut retourner la face créée précédemment et recalculer l'intersection.
    tb_caract = geompy.KindOfShape(face)
    if ( tb_caract[1] == 0 ):
      if self._verbose_max:
        print ("On opère un retournement du cone.")
      plan_de_base = geompy.MakePlane(centre, axe, 100)
      #geompy.addToStudy( plan_de_base, "plan_de_base" )
      geompy.MirrorByPlane(cone, plan_de_base)
#     Récupération de la bonne face après intersection du cone retourné avec le solide initial
      face = self._creation_face_inter ( geompy, solide, cone, 1 )

    return face

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _calcul_hauteur_englobante ( self, geompy, solide ):
    """Crée la hauteur englobant à coup sûr le solide

Décodage des caractéristiques

Entrées :
  :geompy: environnement de GEOM
  :solide: l'objet solide à traiter

Sorties :
  :hauteur: hauteur englobante
    """

    nom_fonction = __name__ + "/_calcul_hauteur_englobante"
    blabla = "\nDans {} :\n".format(nom_fonction)

#   Les deux faces
    if self._verbose_max:
      print (blabla[:-1])

#   Hauteur : la diagonale de la boîte englobante permet d'être certain de tout prendre
    bounding_box = geompy.BoundingBox(solide)
    if self._verbose_max:
      texte = "Boîte englobante du solide '{}'\n".format(self.nom_solide)
      texte += "\tXmin = {}, Xmax = {}\n".format(bounding_box[0],bounding_box[1])
      texte += "\tYmin = {}, Ymax = {}\n".format(bounding_box[2],bounding_box[3])
      texte += "\tZmin = {}, Zmax = {}".format(bounding_box[4],bounding_box[5])
      print(texte)

    coo_1 = np.array([bounding_box[0],bounding_box[2],bounding_box[4]])
    coo_2 = np.array([bounding_box[1],bounding_box[3],bounding_box[5]])
    longueur = np.linalg.norm(coo_2-coo_1)
    if self._verbose_max:
      print ("Longueur de la diagonale {}".format(longueur))
    hauteur = 1.2*longueur

    return hauteur

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _creation_face_inter ( self, geompy, solide, objet, numero ):
    """Crée la face

. Repère la face principale de l'objet support
. Réalise l'intersection avec le solide initial

Entrées :
  :geompy: environnement de GEOM
  :solide: l'objet solide à traiter
  :objet: l'objet support de la face
  :numero: numero de la face dans l'explosion de l'objet support

Sorties :
  :face: la face externe de l'objet support intersecté avec le solide initial
    """

    nom_fonction = __name__ + "/_creation_face_inter"
    blabla = "\nDans {} :\n".format(nom_fonction)

#   Les deux faces
    if self._verbose_max:
      print (blabla[:-1])

#   Récupération de la bonne face
    l_aux = geompy.ExtractShapes(objet, geompy.ShapeType["FACE"], True)
    face_1 = geompy.MakeShell([l_aux[numero]])
    #geompy.addToStudy( face_1, "face externe" )

#   Intersection entre la face externe du solide médian et le solide initial
    face = geompy.MakeCommonList([solide, face_1], True)
    #geompy.addToStudy( face, "face" )

    return face

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _menage_faces ( self, gst, l_faces ):
    """Fait le ménage des faces dans l'étude

Entrées :
  :gst: environnement de GeomStudyTools
  :l_faces: liste des faces du solide
    """

    nom_fonction = __name__ + "/_menage_faces"
    blabla = "\nDans {} :\n".format(nom_fonction)

    if self._verbose_max:
      texte = blabla
      texte += "Nombre de faces à supprimer : {}\n".format(len(l_faces))
      print (texte)

# Suppression des faces incluses dans l'étude
    if self._menage:
      for face in l_faces:
        if self._verbose_max:
          print ("\tSuppression de {}".format(face.GetName()))
        gst.deleteShape(face.GetStudyEntry())

# Rafraichissement de l'affichage
    salome.sg.updateObjBrowser()

    return

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def face_mediane_solide (self, geompy, gst, solide):

    """Calcul de la face médiane pour un solide

Entrées :
  :geompy: environnement de GEOM
  :gst: environnement de GeomStudyTools
  :solide: l'objet solide à traiter

Sorties :
  :erreur: code d'erreur
  :message: message d'erreur
    """

    nom_fonction = __name__ + "/face_mediane_solide"
    blabla = "\nDans {} :\n".format(nom_fonction)

    if self._verbose_max:
      print (blabla)
    if self._verbose:
      print ("Traitement du solide '{}'".format(solide.GetName()))

# 1. Préalables

    erreur = 0
    message = ""

    while not erreur :

# 2. Explosion du solide en faces

      erreur, message, l_faces = self._faces_du_solide ( geompy, solide )
      if erreur:
        break

# 3. Calcul des caractéristiques géométriques des faces

      tb_caract = self._calcul_caract_faces ( geompy, l_faces )

# 4. Tri des faces en fonction de leurs caractéristiques géométriques

      erreur, message, caract_face_1, caract_face_2 = self._tri_faces ( tb_caract )
      if erreur:
        break

# 5. Création de la face médiane

      erreur, message, face = self._cree_face_mediane ( geompy, solide, caract_face_1, caract_face_2 )
      if erreur:
        break

# 6. Ménage des faces

      if salome.sg.hasDesktop():
        if self._retour_shaper:
          l_faces.append(face)
        self._menage_faces ( gst, l_faces )

      break

# 7. La fin

    if ( erreur and self._verbose_max ):
      print (blabla, message)

    return erreur, message

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _menage_solides ( self, gst, l_solides, l_solides_m, type_selection, objet ):
    """Fait le ménage des solides dans l'étude

Entrées :
  :gst: environnement de GeomStudyTools
  :l_solides: liste des solides de l'objet
  :l_solides_m: liste des solides de l'objet dont on veut le ménage
  :type_selection: type de sélection de l'objet à traiter (0: graphique, 1: GEOM, 2;SHAPER)
  :objet: l'objet à traiter
    """

    nom_fonction = __name__ + "/_menage_solides"
    blabla = "\nDans {} :\n".format(nom_fonction)

    if self._verbose_max:
      texte = blabla
      texte += "Nombre de solides de l'objet  : {}\n".format(len(l_solides))
      texte += "Nombre de solides à supprimer : {}".format(len(l_solides_m))
      print (texte)

    if self._menage:

# Suppression des solides inclus dans l'étude dans le cas d'un assemblage explosé
      if ( len(l_solides) > 1 ):
        for solide in l_solides_m:
          if self._verbose_max:
            print ("\tSuppression de {}".format(solide.GetName()))
          gst.deleteShape(solide.GetStudyEntry())

# Suppression de l'objet initial quand il vient d'une exportation depuis SHAPER si ça s'est mal passé
      if ( type_selection == 2 ):
        if ( len(l_solides) != len(l_solides_m) ):
          pass
        else:
          if self._verbose_max:
            print ("\tSuppression de {}".format(objet.GetName()))
          gst.deleteShape(objet.GetStudyEntry())

# Mise en évidence des solides non détruits
      for solide in l_solides:
        if solide not in l_solides_m:
          solide.SetColor(SALOMEDS.Color(1,0,0))

# Rafraichissement de l'affichage
    salome.sg.updateObjBrowser()

    return

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _traitement_objet (self, type_selection=0, objet=None ):
    """Traitement d'un objet

Entrées :
  :type_selection: type de sélection de l'objet à traiter (0: graphique, 1: GEOM, 2;SHAPER)
  :objet: l'objet à traiter quand il passe par argument

Sorties :
  :erreur: code d'erreur
  :message: message d'erreur
    """

    nom_fonction = __name__ + "/_traitement_objet"
    blabla = "\nDans {} :\n".format(nom_fonction)

    if self._verbose_max:
      texte = blabla
      texte += "type_selection = {}".format(type_selection)
      print (texte)

# 1. Préalables

    erreur = 0
    message = ""

    while not erreur :

# 2. L'aide

      if self.affiche_aide_globale :
        break

# 3. Les imports pour salomé
      geompy = geomBuilder.New()
      gst = geomtools.GeomStudyTools(getStudyEditor())

# 4. Sélection de l'objet
      if ( type_selection == 0 ):
        erreur, message, objet = self._selection_objet_graphique ( geompy )
      elif ( type_selection in (1,2) ):
        pass
      else:
        message = "Le type d'objet {} est inconnu.".format(type_selection)
        erreur = 4
      if erreur:
        break

# 5. En cas de retour vers shaper, répertoire de travail associé à l'éventuel fichier de départ
      if self._retour_shaper:
        if self.ficcao is None:
          self.rep_trav = tempfile.mkdtemp(prefix="{}_".format(objet.GetName()))
        else:
          self.rep_trav = os.path.join(os.path.dirname(self.ficcao),"{}_M".format(objet.GetName()))
          if not os.path.isdir(self.rep_trav):
            os.mkdir(self.rep_trav)
        if self._verbose_max:
          print ("Les fichiers CAO des surfaces seront dans le répertoire {}".format(self.rep_trav))

# 6. Liste des solides qui composent l'objet
      erreur, message, l_solides = self._les_solides ( geompy, objet )
      if erreur:
        break

# 7. Calcul des surfaces médianes pour chaque solide
      l_solides_m = list()
      for solide in l_solides:
        erreur, message = self.face_mediane_solide (geompy, gst, solide)
        if erreur:
          break
        l_solides_m.append(solide)
      if erreur:
        break

# 8. Ménage des solides

      if salome.sg.hasDesktop():
        self._menage_solides ( gst, l_solides, l_solides_m, type_selection, objet )

# 9. Informations sur les faces à problème
      if self.faces_pb_nb:
        if ( self.faces_pb_nb == 1 ):
          texte = "1 face pose"
        else:
          texte = "{} faces posent".format(self.faces_pb_nb)
        print ("{} problème.\n{}".format(texte,self.faces_pb_msg))

# 10. Final
      print ("Les fichiers CAO des surfaces sont dans le répertoire {}".format(self.rep_trav))

      break

    return erreur, message

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def surf_fic_cao (self, ficcao):
    """Calcule la surface médiane pour un objet dans un fichier passé en argument

Entrées :
  :ficcao: fichier de l'objet à traiter

Sorties :
  :erreur: code d'erreur
  :message: message d'erreur
    """

    nom_fonction = __name__ + "/surf_fic_cao"
    blabla = "\nDans {} :\n".format(nom_fonction)

    if self._verbose_max:
      print (blabla)

    erreur = 0
    message = ""

    while not erreur :

# 1. Définition de la pièce

      self.part_doc = model.activeDocument()

# 2. Import de la CAO

      self.ficcao = ficcao
      print ("Traitement du fichier {}".format(self.ficcao))

      erreur, message, objet = import_cao (self.part_doc, self.ficcao, self._verbose_max)
      if erreur:
        break

# 3. Calcul des surfaces

      erreur, message = self.surf_objet_shaper ( objet.result().name(), objet.result().shapeType() )

      if ( erreur and self._verbose_max ):
        print (blabla, message)

      break

    return erreur, message

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def surf_objet_shaper (self, nom_objet, type_objet):
    """Calcule la surface médiane pour un objet SHAPER passé en argument

Entrées :
  :nom_objet: nom de l'objet à traiter
  :type_objet: type de l'objet à traiter "SOLID"/"COMPOUND"

Sorties :
  :erreur: code d'erreur
  :message: message d'erreur
    """

    nom_fonction = __name__ + "/surf_objet_shaper"
    blabla = "\nDans {} :\n".format(nom_fonction)

    if self._verbose_max:
      print (blabla)

    erreur = 0
    message = ""

    while not erreur :

# 1. Transfert vers GEOM
# 1.1. Le départ est-il un fichier au format step ?
      deja_step = False
      if self.ficcao is not None:
        laux = self.ficcao.split(".")
        fmt_cao_0 = decode_cao (laux[-1])
        if ( fmt_cao_0 == "stp" ):
          deja_step = True
      if self._verbose_max:
        print ("deja_step = {}".format(deja_step))

# 1.1. Exportation si le fichier de départ n'est pas au format step
      if deja_step:
        fichier = self.ficcao
      else:
        fichier = tempfile.mkstemp(suffix=".stp")[1]
        if self._verbose_max:
          print ('fichier    = {}'.format(fichier))
          print ('nom_objet  = {}'.format(nom_objet))
          print ('type_objet = {}'.format(type_objet))
        export = model.exportToFile(self.part_doc, fichier, [model.selection(type_objet, nom_objet)])
        export.execute(True)
        model.do()
        taille = os.path.getsize(fichier)
        if ( taille <= 0 ):
          message = "Export de SHAPER vers GEOM impossible pour l'objet '{}' de type '{}'\n".format(nom_objet, type_objet)
          message += "Le fichier {} est de taille {}".format(fichier,taille)
          erreur = 2
          break

# 1.2. Importation
      geompy = geomBuilder.New()
      objet = geompy.ImportSTEP(fichier, False, True)
      geompy.addToStudy( objet, nom_objet )
      if not deja_step:
        os.remove(fichier)

# 2. Traitement de l'objet GEOM correspondant
      erreur, message = self._traitement_objet ( 2, objet )

      if ( erreur and self._verbose_max ):
        print (blabla, message)

      break

    return erreur, message

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def surf_objet_geom (self, objet):
    """Calcule la surface médiane pour un objet GEOM passé en argument

Entrées :
  :objet: l'objet à traiter

Sorties :
  :erreur: code d'erreur
  :message: message d'erreur
    """

    nom_fonction = __name__ + "/surf_objet_geom"
    blabla = "\nDans {} :\n".format(nom_fonction)

    if self._verbose_max:
      print (blabla)

    erreur, message = self._traitement_objet ( 1, objet )

    if ( erreur and self._verbose_max ):
      print (blabla, message)

    return erreur, message

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def lancement (self):

    """Lancement

Sorties :
  :erreur: code d'erreur
  :message: message d'erreur
    """

    nom_fonction = __name__ + "/lancement"
    blabla = "\nDans {} :\n".format(nom_fonction)

    if self._verbose_max:
      print (blabla)

    erreur, message = self._traitement_objet ( )

    if ( erreur and self._verbose_max ):
      print (blabla, message)

    return erreur, message

#===========================  Fin de la méthode ==================================

#==========================  Fin de la classe ====================================

#==================================================================================
# Lancement
#==================================================================================

if __name__ == "__main__" :

# 1. Options

  L_OPTIONS = list()
  #L_OPTIONS.append("-h")
  #L_OPTIONS.append("-v")
  #L_OPTIONS.append("-vmax")
  #L_OPTIONS.append("-no_menage")
  #L_OPTIONS.append("-retour_shaper")

# 2. Lancement de la classe

  #print ("L_OPTIONS :", L_OPTIONS)
  SURFACE_MEDIANE = SurfaceMediane(L_OPTIONS)
  if SURFACE_MEDIANE.affiche_aide_globale:
    sys.stdout.write(SURFACE_MEDIANE.__doc__+"\n")
  else:
    ERREUR, MESSAGE_ERREUR = SURFACE_MEDIANE.lancement()
    if ERREUR:
      MESSAGE_ERREUR += "\n Code d'erreur : %d\n" % ERREUR
      sys.stderr.write(MESSAGE_ERREUR)

  del SURFACE_MEDIANE

  #sys.exit(0)
