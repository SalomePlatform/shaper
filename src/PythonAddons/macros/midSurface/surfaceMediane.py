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
"""Obtention des surfaces médianes à partir d'un objet GEOM ou SHAPER

On sait traiter les faces :
  . planes
  . cylindriques
  . sphériques
  . toriques
  . coniques

Pour un objet complexe, on crée l'objet final comme étant la partition de toutes
les surfaces médianes.

Version initiale par :
alexandre.prunie@blastsolutions.io
guillaume.schweitzer@blastsolutions.io

Gérald NICOLAS
"""

__revision__ = "V11.07"

#========================= Les imports - Début ===================================

import os
import sys
import tempfile

import salome

salome.salome_init()

import SALOMEDS
from SketchAPI import *
from salome.shaper import model
from salome.shaper import geom
from GeomAPI import *
from GeomAlgoAPI import *

import numpy as np

#========================== Les imports - Fin ====================================

D_FMT = dict()
D_FMT["stp"] = ["stp", "step"]
D_FMT["igs"] = ["igs", "iges"]
for CLE in ("brep", "xao"):
  D_FMT[CLE] = [CLE]

# statut = 0 si pas encore traité, 1 si traité avec succès, 2 si trop mince, -1 si pas assez mince, -2 si impossible.
D_COLOR_R = dict()
D_COLOR_G = dict()
D_COLOR_B = dict()
D_COLOR_R[-2] = 255
D_COLOR_G[-2] = 255
D_COLOR_B[-2] = 0
D_COLOR_R[-1] = 255
D_COLOR_G[-1] = 0
D_COLOR_B[-1] = 0
D_COLOR_R[0] = 50
D_COLOR_G[0] = 50
D_COLOR_B[0] = 50
D_COLOR_R[1] = 170
D_COLOR_G[1] = 255
D_COLOR_B[1] = 120
D_COLOR_R[2] = 0
D_COLOR_G[2] = 0
D_COLOR_B[2] = 255
# Transparence des solides traités correctement
TRANSPARENCE = 0.7

# Limite basse de l'épaisseur pour pouvoir faire les intersections
EP_MIN = 0.0001
#EP_MIN = 0.1
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

def import_cao (part_doc, ficcao, nom_objet=None, verbose=False):
  """Importation d'une cao

Entrées :
  :part_doc: part
  :ficcao: le fichier de la CAO
  :nom_objet: nom à donner à l'objet lu, éventuellement
Sorties :
  :objet: l'objet importé dans SHAPER
"""

  nom_fonction = __name__ + "/import_cao"
  blabla = "Dans {} :\n".format(nom_fonction)
  message_0 = "Fichier : {}\n".format(ficcao)
  if verbose:
    message = blabla + message_0
    message += "nom_objet : {}".format(nom_objet)
    print (message)
  message = message_0

  erreur = 0

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
    exec_nom (objet, nom_objet)
    model.do()

    if verbose:
      texte  = "Objet   : '{}'\n".format(objet.result().name())
      texte += "De type : '{}'".format(objet.result().shapeType())
      print (texte)

  return erreur, message, objet

#=========================  Fin de la fonction ===================================

#========================= Début de la fonction ==================================

def print_tab (nb_tab, message, argu=None, saut_av=False, saut_ap=False):
  """Imprime avec des tabulations

Entrées :
  :nb_tab: nombre de tabulations à appliquer
  :message: message principal
  :argu: argument du format
  :saut_av: saut de ligne avant le texte
  :saut_ap: saut de ligne après le texte
"""

  texte = ""

  if saut_av:
    texte += "\n"

  for _ in range(nb_tab):
    texte += "\t"

  texte += message
  if ( argu is not None ):
    texte += "{}".format(argu)

  if saut_ap:
    texte += "\n"

  print (texte)

#=========================  Fin de la fonction ===================================

#========================= Début de la fonction ==================================

def nommage (objet, nom, couleur=None):
  """Nomme un objet et son résultat

Entrées :
  :objet: objet à traiter
  :nom: nom à attribuer
  :couleur: éventuellement couleur
"""

  objet.setName(nom)
  objet.result().setName(nom)

  if ( couleur is not None ):
    objet.result().setColor(couleur[0], couleur[1], couleur[2])

#=========================  Fin de la fonction ===================================

#========================= Début de la fonction ==================================

def exec_nom (fonction, nom=None):
  """Execute la fonction après l'avoir nommée et nommé son résultat

Entrées :
  :fonction: fonction à traiter
  :nom: nom à attribuer éventuellement
"""

  if ( nom is not None ):
    nommage (fonction, nom)

  fonction.execute(True)

#=========================  Fin de la fonction ===================================


#=================================== La classe ===================================

class SurfaceMediane (object):

  """Calcul des surfaces médianes de solides minces

L'objectif de ce programme est de créer les surfaces médianes, encore appelées fibres neutres, pour \
une structure qui est un solide ou un assemblage de solides (compound).
Pour réaliser l'opération, trois façons de faire :

1. On lance le script en précisant le fichier à analyser dans la zone d'auto-test.

2. Si on part d'un script qui manipule un fichier au format CAO, on crée une instance de la classe SurfaceMediane \
puis on appelle la méthode surf_fic_cao avec ce fichier en argument.

3. Si on part d'un script qui crée un objet SHAPER, on crée une instance de la classe SurfaceMediane \
puis on appelle la méthode surf_objet_shaper avec cet objet en argument.


Le programme crée les surfaces sous réserve que pour le solide envisagé, il a réussi à trouver deux faces \
de taille identique et supérieure aux tailles des autres faces du solide. \
Cela fonctionne pour des surfaces planes ou de forme canonique.
Il crée alors une surface au milieu de ces deux grandes faces. \
Cette face est coloriée en vert, le solide est en vert et transparent.

On sait traiter les faces :
  . planes
  . cylindriques
  . sphériques
  . toriques
  . coniques

Si la création n'a pas eu lieu, un message est émis et les solides sont mis en couleur :
. Rouge : le solide n'est pas assez mince.
. Bleu : le solide est trop mince, vis-à-vis de la précision de SHAPER.
. Orange : la forme de la face n'est pas reconnue.

Options obligatoires
********************
Aucune

Options facultatives
********************
. Exportation finale dans un fichier step. Par défaut, pas d'export.
-export_step/-no_export_step

Arborescence :
surf_fic_cao --> import_cao
             --> surf_objet_shaper (récursif) --> _nom_sous_objets
                                              --> _surf_objet_shaper_0
                                              --> surf_solide_shaper --> _isole_solide
                                                                     --> _traitement_objet --> face_mediane_solide --> _faces_du_solide
                                                                                                                   --> _tri_faces
                                                                                                                   --> _cree_face_mediane

_cree_face_mediane --> _cree_face_mediane_plane
                   --> _cree_face_mediane_cylindre
                   --> _cree_face_mediane_sphere
                   --> _cree_face_mediane_tore
                   --> _cree_face_mediane_cone
                   --> _cree_face_mediane_0

"""

# A. La base

  message_info = ""
  _verbose = 0
  _verbose_max = 0
  affiche_aide_globale = 0

# B. Les variables

  _choix_objet = 0
  _export_step = False
  nom_solide = None
  nom_solide_aux = None
  _epsilon = 5.e-2
  part_doc = None

  ficcao = None
  rep_step = None
  objet_principal = None
  # Pour chaque sous-objet dans l'ordre de l'arborescence : nom
  l_noms_so = list()
  # Statut de chaque sous-objet connu par son nom :
  # 0 si pas encore traité, 1 si traité avec succès, 2 si trop mince, -1 si pas assez mince, -2 si impossible.
  d_statut_so = dict()
  # Liste des faces médianes créées et des fonctions initiales
  l_faces_m = list()
  # La fonction initiale
  fonction_0 = None

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
      elif saux == "-EXPORT_STEP":
        self._export_step = True
      elif saux == "-NO_EXPORT_STEP":
        self._export_step = False

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def __del__(self):
    """A la suppression de l'instance de classe"""
    if self._verbose_max:
      print ("Suppression de l'instance de la classe.")

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _nom_sous_objets (self, objet, lecture, n_recur=0, rang=0):
    """Gère les noms des sous_objets solides

Entrées :
  :objet: objet à traiter
  :lecture: vrai pour lire les noms, faux pour les attribuer
  :n_recur: niveau de récursivité
  :rang: rang du sous-objet

Sorties :
  :rang: rang du sous-objet
"""

    nom_fonction = __name__ + "/_nom_sous_objets"
    blabla = "Dans {} :\n".format(nom_fonction)

    if self._verbose_max:
      prefixe = ""
      for _ in range(n_recur):
        prefixe += "\t"
      texte = "\n{}{}".format(prefixe,blabla)
      texte += "{}n_recur = {}".format(prefixe,n_recur)
      texte += "\n{}lecture = {}".format(prefixe,lecture)
      print (texte)

# 1. Au premier passage, il faut garder la référence au résultat principal

    if ( n_recur ==  0 ):
      objet_0 = objet.result()
      if self._verbose_max:
        print ("d_statut_so = {}".format(self.d_statut_so))
    else:
      objet_0 = objet

# 2. On descend dans l'arborescence des sous-objets jusqu'à en trouver un qui n'en n'a pas

    nb_sub_results = objet_0.numberOfSubs()

    if self._verbose_max:
      texte = "{}Examen de l'objet '{}' ".format(prefixe,objet_0.name())
      texte += "de type '{}'".format(objet_0.shapeType())
      texte += "\n{}objet.result().numberOfSubs() : {}".format(prefixe,nb_sub_results)
      print (texte)

    for n_sobj in range(nb_sub_results):

# 2.1. Exploration récursive de l'arborescence

      rang = self._nom_sous_objets ( objet_0.subResult(n_sobj), lecture, n_recur+1, rang )

# 2.2. Cet objet n'a pas de sous-objets. Si c'est un solide, on le traite

    if ( objet_0.shapeType() == "SOLID" ):
      # A la lecture, on enregistre le nom
      if lecture:
        nom = objet_0.name()
        self.l_noms_so.append(nom)
        self.d_statut_so[nom] = 0
      # A la récupération, on redonne le nom et on affecte une couleur dépendant de l'état
      else:
        nom = self.l_noms_so[rang]
        objet_0.setName(nom)
        etat = self.d_statut_so[nom]
        objet_0.setColor (D_COLOR_R[etat],D_COLOR_G[etat],D_COLOR_B[etat])
        if ( etat == 1 ):
          objet_0.setTransparency (TRANSPARENCE)
        rang += 1

    return rang

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _couleur_objet (self, objet, n_recur=0, coul_r=1, coul_g=0, coul_b=0):
    """Appliquer une couleur à un objet et à ses sous_objets

Entrées :
  :objet: objet à traiter
  :n_recur: niveau de récursivité
  :coul_r,coul_g,coul_b: code RGB de la couleur à appliquer

Sorties :
  :rang: rang du sous-objet
"""

    nom_fonction = __name__ + "/_couleur_objet"
    blabla = "Dans {} :".format(nom_fonction)

    if self._verbose_max:
      print (blabla)
      print_tab(n_recur, "objet : ", objet.name())
      print_tab(n_recur, "RGB = ({},{},{})".format(coul_r,coul_g,coul_b))

# 1. Au premier passage, il faut garder la référence au résultat principal

    if ( n_recur == 0 ):
      objet_0 = objet.result()
    else:
      objet_0 = objet

# 2. On descend dans l'arborescence des sous-objets jusqu'à en trouver un qui n'en n'a pas

    nb_sub_results = objet_0.numberOfSubs()

    if self._verbose_max:
      print_tab(n_recur, "Examen de l'objet ",objet_0.name())
      texte = "de type '{}' ".format(objet_0.shapeType())
      texte += "et de {} sous-objets".format(nb_sub_results)
      print_tab(n_recur, texte)

    for n_sobj in range(nb_sub_results):

# 2.1. Exploration récursive de l'arborescence

      self._couleur_objet ( objet_0.subResult(n_sobj), n_recur+1, coul_r, coul_g, coul_b )

# 2.2. Cet objet n'a pas de sous-objets : on le colore
    if self._verbose_max:
      print_tab(n_recur, "Couleur affectée à l'objet ",objet_0.name())
    objet_0.setColor (int(coul_r),int(coul_g),int(coul_b))

    #print ("sortie de {}".format(nom_fonction))

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _isole_solide ( self, solide, n_recur ):
    """Isole le solide de son arboresence

Entrées :
  :solide: le solide à traiter
  :n_recur: numéro de la récurrence

Sorties :
  :objet: le solide isolé
  :recover: la fonction de récupération
"""

    nom_fonction = __name__ + "/_isole_solide"
    blabla = "Dans {} :".format(nom_fonction)
    if self._verbose_max:
      print_tab (n_recur, blabla)
      texte = "Pour le solide '{}' ".format(solide.name())
      texte += "de l'objet principal '{}'".format(self.objet_principal.name())
      print_tab (n_recur, texte)

    if ( solide.name() != self.objet_principal.name() ):
      objet, recover = self._isole_solide_a ( solide, n_recur )

    else:
      objet, recover = self._isole_solide_b ( solide, n_recur )

    if self._verbose_max:
      print_tab (n_recur, "objet final : ", objet.name())
      print_tab (n_recur, "fonction_0 : {}".format(self.fonction_0))
      print_tab (n_recur, "recover : {}".format(recover))

    return objet, recover

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _isole_solide_a ( self, solide, n_recur ):
    """Isole le solide de son arboresence

Entrées :
  :solide: le solide à traiter
  :n_recur: numéro de la récurrence

Sorties :
  :objet: le solide isolé
  :recover: la fonction de récupération
"""

    nom_fonction = __name__ + "/_isole_solide_a"
    blabla = "Dans {} :".format(nom_fonction)
    if self._verbose_max:
      print_tab (n_recur, blabla)
      texte = "Pour le solide '{}' ".format(solide.name())
      texte += "de l'objet principal '{}'".format(self.objet_principal.name())
      print_tab (n_recur, texte)

    if self._verbose_max:
      print_tab (n_recur, ". Extraction du solide '{}'".format(self.objet_principal.name()))

# 1. Extraction du solide
    remove_subshapes = model.addRemoveSubShapes(self.part_doc, model.selection("COMPOUND", self.objet_principal.name()))
    exec_nom (remove_subshapes)
    remove_subshapes.setSubShapesToKeep([model.selection("SOLID", solide.name())])

    self.nom_solide_aux = "{}_S".format(solide.name())
    if self._verbose_max:
      print_tab (n_recur, "\tAttribution à remove_subshapes.result() du nom '{}'".format(self.nom_solide_aux))
    remove_subshapes.result().setName(self.nom_solide_aux)

    self.fonction_0 = remove_subshapes

# 2. Récupération de l'objet principal
    recover = model.addRecover(self.part_doc, remove_subshapes, [self.objet_principal])
    if self._verbose_max:
      print_tab (n_recur, "\tAttribution à recover du nom '{}'".format(self.objet_principal.name()))
    exec_nom (recover,self.objet_principal.name())

    return remove_subshapes.result(), recover

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _isole_solide_b ( self, solide, n_recur ):
    """Isole le solide de son arboresence

Entrées :
  :solide: le solide à traiter
  :n_recur: numéro de la récurrence

Sorties :
  :objet: le solide isolé
  :recover: la fonction de récupération
"""

    nom_fonction = __name__ + "/_isole_solide_b"
    blabla = "Dans {} :".format(nom_fonction)
    if self._verbose_max:
      print_tab (n_recur, blabla)
      texte = "Pour le solide '{}' ".format(solide.name())
      texte += "de l'objet principal '{}'".format(self.objet_principal.name())
      print_tab (n_recur, texte)

    if self._verbose_max:
      print_tab (n_recur, ". Mise en place du solide")

    self.nom_solide_aux = self.objet_principal.name()
    self.fonction_0 = None

    return solide, None

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _faces_du_solide ( self, solide, n_recur=0 ):
    """Détermine les faces d'un solide

Entrées :
  :solide: solide SHAPER à traiter
  :n_recur: niveau de récursivité

Sorties :
  :l_faces_car: pour chaque face du solide (surface,caractéristiques)
"""

    nom_fonction = __name__ + "/_faces_du_solide"
    blabla = "Dans {} :".format(nom_fonction)
    if self._verbose_max:
      print_tab (n_recur, blabla, saut_av=True)

    l_faces = list()

    if self._verbose_max:
      print_tab (n_recur, ".. Traitement du solide ", self.nom_solide)
      print_tab (n_recur, ".. shapeType : ", solide.shapeType())

    #print (dir(solide))
    #print_tab (n_recur, "volume = ", GeomAlgoAPI_ShapeTools.volume(solide.shape()))
# 1. Repérage des faces
    objResult = solide.resultSubShapePair()[0]
    l_faces = list()
    exp = GeomAPI_ShapeExplorer(objResult.shape(), GeomAPI_Shape.FACE)
    while exp.more():
      l_faces.append(exp.current().face())
      exp.next()

# 2. Calcul
    l_faces_car = list()
    for iface, face in enumerate(l_faces):
      surf = GeomAlgoAPI_ShapeTools.area(face)
      caract = geom.shapeInfo(face)
      if self._verbose_max:
        print_tab (n_recur, "\tFace n°{} ; ".format(iface), "surface = {}, caractéristiques = {}".format(surf,caract))
      l_faces_car.append((surf,caract))

    return l_faces_car

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _calcul_caract_faces ( self, solide, n_recur ):
    """Calcule les caractéristiques géométriques des faces du solide

Entrées :
  :solide: solide SHAPER à traiter
  :n_recur: niveau de récursivité

Sorties :
  :tb_caract: tableau des caractéristiques géométriques des faces
"""

    nom_fonction = __name__ + "/_calcul_caract_faces"
    blabla = "Dans {} :".format(nom_fonction)
    if self._verbose_max:
      print_tab (n_recur, blabla, saut_av=True)
      print_tab (n_recur, ".. Traitement du solide ", self.nom_solide)
      print_tab (n_recur, ".. shapeType : ", solide.shapeType())

# 1. Repérage des faces
    objResult = solide.resultSubShapePair()[0]
    l_faces = list()
    exp = GeomAPI_ShapeExplorer(objResult.shape(), GeomAPI_Shape.FACE)
    while exp.more():
      l_faces.append(exp.current().face())
      exp.next()

# 2. Caractéristiques
    nb_faces = len(l_faces)
    tb_caract = np.zeros((nb_faces,3), dtype = 'object')
    for iaux, face in enumerate(l_faces):
      surf = GeomAlgoAPI_ShapeTools.area(face)
      caract = geom.shapeInfo(face)
      if self._verbose_max:
        print_tab (n_recur, "\tFace n°{} ; ".format(iaux), "surface = {}, caractéristiques = {}".format(surf,caract))

      tb_caract [iaux][0] = face
      tb_caract [iaux][1] = surf
      tb_caract [iaux][2] = caract

    #if self._verbose_max:
      #for iaux in range(nb_faces):
        #print ("\t. tb_caract : {} {}".format(surf,tb_caract[iaux][2]))

    return tb_caract

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _tri_faces ( self, tb_caract, n_recur ):
    """Trie les faces en fonction de leurs surfaces

Entrées :
  :tb_caract: tableau des caractéristiques géométriques des faces
  :n_recur: niveau de récursivité

Sorties :
  :tb_caract_1[-1], tb_caract_1[-2]: les caractéristiques des 2 faces les plus grandes
"""

    erreur = 0
    message = ""

    nom_fonction = __name__ + "/_tri_faces"
    blabla = "Dans {} :".format(nom_fonction)

# Tri du tableau en fonction des surfaces
    if self._verbose_max:
      print_tab (n_recur, blabla)
      print_tab (n_recur, "tb_caract brut : ", tb_caract)
    tb_caract_1 = sorted(tb_caract, key=lambda colonnes: colonnes[1])
    if self._verbose_max:
      print_tab (n_recur, "tb_caract trié :", tb_caract_1)

    if self._verbose_max:
      texte  = "\tSurface de la plus grande face      : {}, de caractéristiques {}\n".format(tb_caract_1[-1][1],tb_caract_1[-1][2])
      texte += "\tSurface de la face suivante         : {}, de caractéristiques {}".format(tb_caract_1[-2][1],tb_caract_1[-2][2])
      if self._verbose_max:
        texte += "\n\tSurface de la 3ème face suivante    : {}, de caractéristiques {}".format(tb_caract_1[-3][1],tb_caract_1[-3][2])
      print (texte)

# La surface suivante doit être différente, sinon ce n'est pas un solide mince
    ecart = np.abs((tb_caract_1[-1][1]-tb_caract_1[-3][1])/tb_caract_1[-1][1])
    if ( ecart < self._epsilon ):
      message  = "\nSolide '{}'\n".format(self.nom_solide)
      message += ". Surface de la plus grande face   : {}\n".format(tb_caract_1[-1][1])
      message += ". Surface de la 1ère face suivante : {}\n".format(tb_caract_1[-2][1])
      message += ". Surface de la 2ème face suivante : {}\n".format(tb_caract_1[-3][1])
      if self._verbose_max:
        message += ". Ecart relatif :{:4.1f}%\n".format(ecart*100.)
      message += "L'écart est trop faible par rapport à la limite de {}%.\n".format(self._epsilon*100.)
      message += "==> Impossible de créer la face médiane car le solide n'est pas assez mince.\n"
      erreur = -1
      self.d_statut_so[self.nom_solide] = -1
      self.faces_pb_nb += 1
      self.faces_pb_msg += message

    return erreur, message, tb_caract_1[-1], tb_caract_1[-2]

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _verif_epaisseur ( self, epaisseur ):
    """Contrôle de la validité de l'épaisseur

Entrées :
  :epaisseur: épaisseur du solide
"""

    nom_fonction = __name__ + "/_verif_epaisseur"
    blabla = "Dans {} :\n".format(nom_fonction)

    if self._verbose_max:
      texte = blabla
      texte += ". Epaisseur du solide : {}\n".format(epaisseur)
      texte += ". EP_MIN              : {}".format(EP_MIN)
      print (texte)

    if ( epaisseur <= EP_MIN ):
      message  = "\nSolide '{}'\n".format(self.nom_solide)
      message += ". Epaisseur : {}\n".format(epaisseur)
      message += "L'épaisseur est trop faible par rapport à la limite de {}.\n".format(EP_MIN)
      message += "==> Impossible de créer la face médiane car le solide est trop mince.\n"
      erreur = 2
      self.d_statut_so[self.nom_solide] = 2
      self.faces_pb_nb += 1
      self.faces_pb_msg += message

    else:
      erreur = 0
    #print ("erreur = {}".format(erreur))

    return erreur

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _cree_face_mediane ( self, solide, caract_face_1, caract_face_2, n_recur ):
    """Crée la face médiane entre deux autres

Entrées :
  :solide: solide SHAPER à traiter
  :caract_face_1, caract_face_2: les caractéristiques des 2 faces les plus grandes
  :n_recur: niveau de récursivité

Sorties :
  :face: la face médiane créée
"""

    nom_fonction = __name__ + "/_cree_face_mediane"
    blabla = "Dans {} :".format(nom_fonction)

    if self._verbose_max:
      print_tab (n_recur, blabla)
      print_tab (n_recur, "face_1 : " ,caract_face_1)
      print_tab (n_recur, "face_2 : " ,caract_face_2)

    erreur = 0
    face =  None

# 1. Forme de la face
    forme = caract_face_1[2][0]

# 2. Traitement selon la forme de la face
# 2.1. Face plane
    if forme in ( "Disk" , "Plane", "Rectangle"):
      erreur, face = self._cree_face_mediane_plane ( solide, caract_face_1, caract_face_2, n_recur )

# 2.2. Face cylindrique
    elif ( forme == "Cylinder" ):
      erreur, face = self._cree_face_mediane_cylindre ( solide, caract_face_1, caract_face_2, n_recur )

# 2.3. Face sphérique
    elif ( forme == "Sphere" ):
      erreur, face = self._cree_face_mediane_sphere ( caract_face_1, caract_face_2, n_recur )

# 2.4. Face torique
    elif ( forme == "Torus" ):
      erreur, face = self._cree_face_mediane_tore ( caract_face_1, caract_face_2, n_recur )

# 2.5. Face conique
    elif ( forme == "Cone" ):
      erreur, face = self._cree_face_mediane_cone ( caract_face_1, caract_face_2, n_recur )

# 2.N. Face de forme inconnue
    else:
      message  = "\nSolide '{}'\n".format(self.nom_solide)
      message += "sa face la plus grande est de forme : {}\n".format(forme)
      message += "==> Impossible de créer la face médiane.\n"
      erreur = -2
      self.d_statut_so[self.nom_solide] = -2
      self.faces_pb_nb += 1
      self.faces_pb_msg += message

# 3. Gestion de la face produite

    if face is not None:
      self._cree_face_mediane_0 ( face, n_recur )

    return erreur, face

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _cree_face_mediane_0 ( self, face, n_recur ):
    """Gestion de la face médiane créée entre deux autres

Entrées :
  :face: la face médiane créée
  :n_recur: niveau de récursivité
"""

    nom_fonction = __name__ + "/_cree_face_mediane_0"
    blabla = "Dans {} :".format(nom_fonction)

    if self._verbose_max:
      print_tab (n_recur, blabla)

# 1. Nom de la face
    nom_face = self.nom_solide+"_M"
    if self._verbose_max:
      print_tab (n_recur,"Nom de la face créée : ", nom_face)
    #if ( self.nom_solide_aux != self.objet_principal.name() ):
      #nom_face += "S"
    nommage (face, nom_face)

# 2. Mémorisation de la face et de la fonction initiale
    self.l_faces_m.append((face, self.fonction_0))

# 3. Couleur verte pour la face
    self._couleur_objet (face, coul_r=0, coul_g=170, coul_b=0)

# 4. Changement de statut pour le solide
    self.d_statut_so[self.nom_solide] = 1

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _cree_face_mediane_plane ( self, solide, caract_face_1, caract_face_2, n_recur ):
    """Crée la face médiane entre deux autres - cas des surfaces planes

Entrées :
  :solide: l'objet solide à traiter
  :caract_face_1, caract_face_2: les caractéristiques des 2 faces les plus grandes
  :n_recur: niveau de récursivité

Sorties :
  :face: la face médiane
"""

    nom_fonction = __name__ + "/_cree_face_mediane_plane"
    blabla = "Dans {} :".format(nom_fonction)
    if self._verbose_max:
      print_tab (n_recur, blabla)

#   Caractéristiques des surfaces
    coo_x, coo_y, coo_z, vnor_x, vnor_y, vnor_z, taille, d_face_1_2 = self._cree_face_mediane_plane_0 ( solide, caract_face_1, caract_face_2, n_recur )

#   Contrôle de la validité de l'épaisseur
    erreur = self._verif_epaisseur ( d_face_1_2 )

#   Création de la face
    if not erreur:
      face = self._cree_face_mediane_plane_1 ( solide, (coo_x, coo_y, coo_z),(vnor_x, vnor_y, vnor_z), taille, d_face_1_2, n_recur )
    else:
      face = None

    return erreur, face

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _cree_face_mediane_plane_0 ( self, solide, caract_face_1, caract_face_2, n_recur ):
    """Crée la face médiane entre deux autres - cas des surfaces planes

Décodage des caractéristiques

Entrées :
  :solide: l'objet solide à traiter
  :caract_face_1, caract_face_2: les caractéristiques des 2 faces les plus grandes
  :n_recur: niveau de récursivité

Sorties :
  :coo_x, coo_y, coo_z: coordonnées du centre de la base
  :vnor_x, vnor_y, vnor_z: coordonnées du vecteur normal
  :taille: estimation de la taille de la future face
  :d_face_1_2: la distance entre les deux faces
"""

    nom_fonction = __name__ + "/_cree_face_mediane_plane_0"
    blabla = "Dans {} :".format(nom_fonction)

    if self._verbose_max:
      print_tab (n_recur, blabla)
      print_tab (n_recur, "caract_face_1 : ", caract_face_1)
      print_tab (n_recur, "caract_face_2 : ", caract_face_2)

# 1. Caractéristiques de la base
#   Coordonnées du centre de la base
    coo_x = caract_face_1[2][1]
    coo_y = caract_face_1[2][2]
    coo_z = caract_face_1[2][3]
#   Coordonnées du vecteur normal
    vnor_x = caract_face_1[2][4]
    vnor_y = caract_face_1[2][5]
    vnor_z = caract_face_1[2][6]
#   taille : une longueur caractéristique pour être certain de tout prendre
    l_diag = self._calcul_lg_caract ( solide, n_recur )
    taille = 10.*l_diag
    if self._verbose_max:
      print_tab (n_recur, "Taille englobante : ",taille)

# 2. Distance entre les deux faces
    face_1 = caract_face_1[0]
    face_2 = caract_face_2[0]
    d_face_1_2 = GeomAlgoAPI_ShapeTools.minimalDistance(face_1, face_2)
    if self._verbose_max:
      print_tab (n_recur, "Distance entre les deux faces = ", d_face_1_2)

    return coo_x, coo_y, coo_z, vnor_x, vnor_y, vnor_z, taille, d_face_1_2

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _cree_face_mediane_plane_1 ( self, solide, coo_c, vnor, taille, d_face_1_2, n_recur ):
    """Crée la face médiane entre deux autres - cas des surfaces planes

Création des objets de construction et de la face médiane

Entrées :
  :solide: l'objet solide à traiter
  :coo_c: coordonnées du centre de la base
  :vnor: coordonnées du vecteur normal
  :taille: estimation de la taille de la future face
  :d_face_1_2: la distance entre les deux faces
  :n_recur: niveau de récursivité

Sorties :
  :face: la face médiane
"""

    nom_fonction = __name__ + "/_cree_face_mediane_plane_1"
    blabla = "Dans {} :".format(nom_fonction)
    if self._verbose_max:
      print_tab (n_recur, blabla)
      print_tab (n_recur, "Centre   : ({}, {}, {})".format(coo_c[0], coo_c[1], coo_c[2]))
      print_tab (n_recur, "Normale  : ({}, {}, {})".format(vnor[0], vnor[1], vnor[2]))
      print_tab (n_recur, "Taille   : ", taille)
      print_tab (n_recur, "Distance entre les deux faces : ", d_face_1_2)

# 1. Préalables
    _, v_norm, plan = self._cree_centre_axe_plan ( coo_c, vnor, self.nom_solide, n_recur )

# 2. Création de l'esquisse
    sketch = self._cree_face_mediane_plane_1_a ( plan, taille, n_recur )

# 3. La face
    face = self._cree_face_mediane_plane_1_b ( solide, sketch, v_norm, d_face_1_2, n_recur )

    #print ("fin de {}".format(nom_fonction))

    return face

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _cree_face_mediane_plane_1_a ( self, plan, taille, n_recur ):
    """Crée la face médiane entre deux autres - cas des surfaces planes - l'esquisse

Entrées :
  :plan: plan
  :taille: estimation de la taille de la future face
  :n_recur: niveau de récursivité

Sorties :
  :sketch: l'esquisse
"""

    nom_fonction = __name__ + "/_cree_face_mediane_plane_1_a"
    blabla = "Dans {} :".format(nom_fonction)
    if self._verbose_max:
      print_tab (n_recur, blabla)
      print_tab (n_recur, "Plan      : {}".format(plan.name()))
      print_tab (n_recur, "taille    : {}".format(taille))

    sketch = model.addSketch(self.part_doc, model.selection("FACE", plan.name()))
    sketch.execute(True)

    ### Create SketchLine
    SketchLine_1 = sketch.addLine(-taille/2., taille/2., taille/2., taille/2.)
    SketchLine_1.execute(True)

    ### Create SketchLine
    SketchLine_2 = sketch.addLine(taille/2., taille/2., taille/2., -taille/2.)
    SketchLine_2.execute(True)

    ### Create SketchLine
    SketchLine_3 = sketch.addLine(taille/2., -taille/2., -taille/2., -taille/2.)
    SketchLine_3.execute(True)

    ### Create SketchLine
    SketchLine_4 = sketch.addLine(-taille/2., -taille/2., -taille/2., taille/2.)
    SketchLine_4.execute(True)

    model.do()

    nom_sketch = "{}_esquisse".format(self.nom_solide)
    exec_nom (sketch,nom_sketch)

    #print ("fin de {}".format(nom_fonction))

    return sketch

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _cree_face_mediane_plane_1_b ( self, solide, sketch, v_norm, d_face_1_2, n_recur ):
    """Crée la face médiane entre deux autres - cas des surfaces planes

Création de la face médiane

Entrées :
  :solide: l'objet solide à traiter
  :sketch: l'esquisse
  :v_norm: vecteur normal
  :d_face_1_2: la distance entre les deux faces
  :n_recur: niveau de récursivité

Sorties :
  :face: la face médiane
"""

    nom_fonction = __name__ + "/_cree_face_mediane_plane_1_b"
    blabla = "Dans {} :".format(nom_fonction)
    if self._verbose_max:
      print_tab (n_recur, blabla)
      print_tab (n_recur, "Esquisse : ", sketch.name())
      print_tab (n_recur, "Distance entre les deux faces : ", d_face_1_2)

    ### Create LinearCopy
    LinearCopy_1 = model.addMultiTranslation(self.part_doc, [model.selection("SOLID", self.nom_solide_aux)], model.selection("EDGE", "PartSet/OX"), 0, 1, keepSubResults = True)
    nom = "{}_0".format(self.nom_solide_aux)
    exec_nom (LinearCopy_1,nom)
    if self._verbose_max:
      print_tab (n_recur, "Après LinearCopy_1 : ", nom)

    ### Create Recover
    Recover_1 = model.addRecover(self.part_doc, LinearCopy_1, [solide])
    nom = "{}_1".format(self.nom_solide_aux)
    exec_nom (Recover_1,nom)
    if self._verbose_max:
      print_tab (n_recur, "Après Recover_1 : ", nom)

    # Création d'une face ; on la translate d'une demi-épaisseur.
    for iaux in range(2):

      d_trans = -0.5*d_face_1_2*float(2*iaux-1)
      nom_solide = "{}_{}".format(self.nom_solide_aux,iaux)
      face = self._cree_face_mediane_plane_2 ( sketch.name(), v_norm.name(), nom_solide, d_trans, iaux, n_recur )

      if face.results():
        face = self._cree_face_mediane_plane_11 ( face, Recover_1, n_recur )
        break
      # Si l'intersection est vide, on la translate dans l'autre sens
      else:
        if self._verbose_max:
          print_tab (n_recur, "L'intersection est vide.")
        face = None

    #print ("fin de {}".format(nom_fonction))

    return face

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _cree_face_mediane_plane_11 ( self, face, Recover_1, n_recur ):
    """Crée la face médiane entre deux autres - cas des surfaces planes

Création des objets de construction et de la face médiane

Entrées :
  :face: la face médiane
  :Recover_1: la récupératiuon du solide
  :n_recur: niveau de récursivité

Sorties :
  :face: la face médiane
"""

    nom_fonction = __name__ + "/_cree_face_mediane_plane_11"
    blabla = "Dans {} :".format(nom_fonction)
    if self._verbose_max:
      print_tab (n_recur, blabla)
      print_tab (n_recur, "face : ", face.name())

    # Si on traite un objet solide unique, on le récupère
    if ( self.nom_solide_aux == self.objet_principal.name() ):
      if self._verbose_max:
        print_tab (n_recur, "On traite un objet solide unique ==> on le récupère.")
      Recover_2 = model.addRecover(self.part_doc, face, [Recover_1.result()])
      nom = "{}_S".format(self.nom_solide_aux)
      exec_nom (Recover_2,nom)

    nb_inter = face.result().numberOfSubs()
    if self._verbose_max:
      print_tab (n_recur, "Nombre d'intersections : ", nb_inter)

    if (nb_inter > 1 ):
      face = self._cree_face_mediane_plane_3 ( face )

    return face

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _cree_face_mediane_plane_2 ( self, nom_sketch, nom_normal, nom_solide, d_trans, icpt, n_recur ):
    """Crée la face médiane entre deux autres - cas des surfaces planes

Intersection de la face avec le solide

Entrées :
  :nom_sketch: nom de l'esquisse
  :nom_normal: nom du vecteur normal
  :nom_solide: nom du solide à intersecter
  :d_trans: la distance de translation
  :icpt: numéro de la tentative

Sorties :
  :face: la face médiane
"""

    nom_fonction = __name__ + "/_cree_face_mediane_plane_2"
    blabla = "Dans {} :".format(nom_fonction)
    if self._verbose_max:
      print_tab (n_recur, blabla)
      print_tab (n_recur, "nom_sketch : ", nom_sketch)
      print_tab (n_recur, "nom_normal : ", nom_normal)
      print_tab (n_recur, "nom_solide : ", nom_solide)
      print_tab (n_recur, "d_trans    : ", d_trans)

    # Création d'une face
    Face_1 = model.addFace(self.part_doc, [model.selection("COMPOUND", "all-in-{}".format(nom_sketch))])
    nom_face_1 = "{}_face_1_{}".format(self.nom_solide_aux,icpt)
    exec_nom (Face_1,nom_face_1)
    if self._verbose_max:
      print_tab (n_recur, "Après Face_1 : ", nom_face_1)

#   On la translate
    Translation_1 = model.addTranslation(self.part_doc, [model.selection("FACE", nom_face_1)], axis = model.selection("EDGE", nom_normal), distance = d_trans, keepSubResults = True)
    nom_trans = "{}_trans_{}".format(self.nom_solide_aux,icpt)
    exec_nom (Translation_1,nom_trans)
    if self._verbose_max:
      print_tab (n_recur, "Après Translation_1 : ", nom_trans)
    Translation_1.result().setColor(85, 0, 255)

#   Intersection de cette face avec le solide initial
    face = model.addCommon(self.part_doc, [model.selection("SOLID", nom_solide), model.selection("FACE", nom_trans)], keepSubResults = True)
    face.execute(True)

    return face

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _cree_face_mediane_plane_3 ( self, face ):
    """Crée la face médiane entre deux autres - cas des surfaces planes

Fusion des 2 intersections

Entrées :
  :face: la face médiane composée de plusieurs intersections

Sorties :
  :face_m: la face médiane
"""

    nom_fonction = __name__ + "/_cree_face_mediane_plane_3"
    blabla = "Dans {} :\n".format(nom_fonction)
    if self._verbose_max:
      texte = blabla
      print (texte)

# Nommage des sous-objets
    l_fuse = list()
    for iaux in range(face.result().numberOfSubs()):
      nom = "{}_common_{}".format(self.nom_solide_aux,iaux)
      face.result().subResult(iaux).setName(nom)
      l_fuse.append(model.selection("FACE", '{}'.format(nom)))

#   Fusion
    if self._verbose_max:
      print ("Fusion de {} faces.".format(len(l_fuse)))
    face_m = model.addFuse(self.part_doc, l_fuse, keepSubResults = True)
    face_m.execute(True)

    return face_m

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _cree_face_mediane_cylindre ( self, solide, caract_face_1, caract_face_2, n_recur ):
    """Crée la face médiane entre deux autres - cas des cylindres

Entrées :
  :solide: solide SHAPER à traiter
  :caract_face_1, caract_face_2: les caractéristiques des 2 faces les plus grandes
  :n_recur: niveau de récursivité

Sorties :
  :face: la face médiane
"""

    nom_fonction = __name__ + "/_cree_face_mediane_cylindre"
    blabla = "Dans {} :".format(nom_fonction)

#   Les deux faces
    if self._verbose_max:
      print_tab (n_recur, blabla)
      print_tab (n_recur, "face_1 : ", caract_face_1)
      print_tab (n_recur, "face_2 : ", caract_face_2)

#   Caractéristiques des cylindres
    coo_x, coo_y, coo_z, axe_x, axe_y, axe_z, rayon, hauteur, epaisseur = self._cree_face_mediane_cylindre_0 ( solide, caract_face_1, caract_face_2, n_recur )

#   Contrôle de la validité de l'épaisseur
    erreur = self._verif_epaisseur ( epaisseur )

#   Création de la face
    if not erreur:
      face = self._cree_face_mediane_cylindre_1 ( (coo_x, coo_y, coo_z), (axe_x, axe_y, axe_z), rayon, hauteur, n_recur )
    else:
      self._couleur_objet (solide, n_recur, coul_r=0, coul_g=0, coul_b=255)
      face = None

    return erreur, face

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _cree_face_mediane_cylindre_0 ( self, solide, caract_face_1, caract_face_2, n_recur ):
    """Crée la face médiane entre deux autres - cas des cylindres

Décodage des caractéristiques

Entrées :
  :solide: l'objet solide à traiter
  :caract_face_1, caract_face_2: les caractéristiques des 2 faces les plus grandes
  :n_recur: niveau de récursivité

Sorties :
  :coo_x, coo_y, coo_z: coordonnées du centre de la base
  :axe_x, axe_y, axe_z: coordonnées de l'axe
  :rayon: rayon moyen entre les deux faces
  :hauteur: hauteur du cylindre
  :epaisseur: épaisseur de l'interface entre les deux faces
"""

    nom_fonction = __name__ + "/_cree_face_mediane_cylindre_0"
    blabla = "Dans {} :".format(nom_fonction)

    if self._verbose_max:
      print_tab (n_recur, blabla)
      print_tab (n_recur, "face_1 : ", caract_face_1)
      print_tab (n_recur, "face_2 : ", caract_face_2)

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
#   Hauteur : une longueur caractéristique pour être certain de tout prendre
    l_diag = self._calcul_lg_caract ( solide, n_recur )
    hauteur = 10.*l_diag
    if self._verbose_max:
      print_tab (n_recur, "Hauteur englobante : ", hauteur)
#   Epaisseur
    epaisseur = np.abs(caract_face_2[2][7]-caract_face_1[2][7])

    return coo_x, coo_y, coo_z, axe_x, axe_y, axe_z, rayon, hauteur, epaisseur

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _cree_face_mediane_cylindre_1 ( self, coo_c, v_axe, rayon, hauteur, n_recur ):
    """Crée la face médiane entre deux autres - cas des cylindres

Création des objets temporaires et de la face externe du cylindre support

Entrées :
  :coo_x, coo_y, coo_z: coordonnées du centre de la base
  :axe_x, axe_y, axe_z: coordonnées de l'axe
  :rayon: rayon moyen entre les deux faces
  :hauteur: hauteur du cylindre
  :n_recur: niveau de récursivité

Sorties :
  :face: la face médiane
"""
    nom_fonction = __name__ + "/_cree_face_mediane_cylindre_1"
    blabla = "Dans {} :\n".format(nom_fonction)

#   Les caractéristiques du cylindre à créer
    if self._verbose_max:
      print_tab (n_recur, blabla)
      print_tab (n_recur, "Centre  : ({}, {}, {})".format(coo_c[0], coo_c[1], coo_c[2]))
      print_tab (n_recur, "Axe     : ({}, {}, {})".format(v_axe[0], v_axe[1], v_axe[2]))
      print_tab (n_recur, "Rayon   : ", rayon)
      print_tab (n_recur, "Hauteur : ", hauteur)

# 1. Objets préalables
    nom_par_1 = "{}_R".format(self.nom_solide)
    model.addParameter(self.part_doc, "{}".format(nom_par_1), "{}".format(rayon))
    nom_par_2 = "{}_H".format(self.nom_solide)
    model.addParameter(self.part_doc, "{}".format(nom_par_2), "{}".format(hauteur))

    centre, _, plan = self._cree_centre_axe_plan ( coo_c, v_axe, self.nom_solide, n_recur )

# 2. Création de l'esquisse
    sketch = self._cree_face_mediane_cylindre_1_a ( plan, centre, rayon, n_recur )

# 3. La face
    face = self._cree_face_mediane_cylindre_1_b ( sketch, nom_par_2, n_recur )

    return face

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _cree_face_mediane_cylindre_1_a ( self, plan, centre, rayon, n_recur ):
    """Crée la face médiane entre deux autres - cas des cylindres

Création des objets temporaires et de la face externe du cylindre support

Entrées :
  :coo_x, coo_y, coo_z: coordonnées du centre de la base
  :axe_x, axe_y, axe_z: coordonnées de l'axe
  :rayon: rayon moyen entre les deux faces
  :n_recur: niveau de récursivité

Sorties :
  :face: la face médiane
"""
    nom_fonction = __name__ + "/_cree_face_mediane_cylindre_1_a"
    blabla = "Dans {} :\n".format(nom_fonction)

#   Les caractéristiques du cylindre à créer
    if self._verbose_max:
      print_tab (n_recur, blabla)
      print_tab (n_recur, "Plan      : {}".format(plan.name()))
      print_tab (n_recur, "Centre    : {}".format(centre.name()))
      print_tab (n_recur, "Rayon   : ", rayon)

    sketch = model.addSketch(self.part_doc, model.selection("FACE", plan.name()))
    sketch.execute(True)

    SketchProjection_1 = sketch.addProjection(model.selection("VERTEX", centre.name()), False)
    SketchPoint_1 = SketchProjection_1.createdFeature()

    SketchCircle_1 = sketch.addCircle(0., 0., rayon)
    sketch.setCoincident(SketchPoint_1.result(), SketchCircle_1.center())
    model.do()

    nom_sketch = "{}_esquisse".format(self.nom_solide)
    exec_nom (sketch, nom_sketch)

    return sketch

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _cree_face_mediane_cylindre_1_b ( self, sketch, nom_par_2, n_recur ):
    """Crée la face médiane entre deux autres - cas des cylindres

Création des objets de construction et de la face externe du cylindre support

Entrées :
  :sketch: l'esquisse
  :n_recur: niveau de récursivité

Sorties :
  :face: la face médiane
"""
    nom_fonction = __name__ + "/_cree_face_mediane_cylindre_1_b"
    blabla = "Dans {} :\n".format(nom_fonction)

#   Les caractéristiques du cylindre à créer
    if self._verbose_max:
      print_tab (n_recur, blabla)
      print_tab (n_recur, "Esquisse : ", sketch.name())
      print_tab (n_recur, "nom_par_2 : ", nom_par_2)

#   Création du cylindre complet
    cylindre = model.addExtrusion(self.part_doc, [model.selection("COMPOUND", "all-in-{}".format(sketch.name()))], model.selection(), nom_par_2, nom_par_2, "Edges")
    cylindre.execute(True)

    nom_cylindre = "{}_cylindre".format(self.nom_solide)
    nommage (cylindre, nom_cylindre, (85, 0, 255))

#   Intersection de la face cylindrique avec le solide initial
    face = self._creation_face_inter ( nom_cylindre )

    return face

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _cree_face_mediane_sphere ( self, caract_face_1, caract_face_2, n_recur ):
    """Crée la face médiane entre deux autres - cas des sphères

Entrées :
  :caract_face_1, caract_face_2: les caractéristiques des 2 faces les plus grandes
  :n_recur: niveau de récursivité

Sorties :
  :face: la face médiane
"""

    nom_fonction = __name__ + "/_cree_face_mediane_sphere"
    blabla = "Dans {} :".format(nom_fonction)

#   Les deux faces
    if self._verbose_max:
      print_tab (n_recur, blabla)
      print_tab (n_recur, "face_1 : ", caract_face_1)
      print_tab (n_recur, "face_2 : ", caract_face_2)

#   Caractéristiques des sphères
    coo_x, coo_y, coo_z, rayon, epaisseur = self._cree_face_mediane_sphere_0 ( caract_face_1, caract_face_2, n_recur )

#   Contrôle de la validité de l'épaisseur
    erreur = self._verif_epaisseur ( epaisseur )

#   Création de la face
    if not erreur:
      face = self._cree_face_mediane_sphere_1 ( coo_x, coo_y, coo_z, rayon )
    else:
      face = None

    return erreur, face

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _cree_face_mediane_sphere_0 ( self, caract_face_1, caract_face_2, n_recur ):
    """Crée la face médiane entre deux autres - cas des sphères

Décodage des caractéristiques

Entrées :
  :caract_face_1, caract_face_2: les caractéristiques des 2 faces les plus grandes
  :n_recur: niveau de récursivité

Sorties :
  :coo_x, coo_y, coo_z: coordonnées du centre de la sphère
  :rayon: rayon moyen entre les deux faces
  :epaisseur: épaisseur de l'interface entre les deux faces
"""

    nom_fonction = __name__ + "/_cree_face_mediane_sphere_0"
    blabla = "Dans {} :".format(nom_fonction)

#   Les deux faces
    if self._verbose_max:
      print_tab (n_recur, blabla)
      print_tab (n_recur, "face_1 : ", caract_face_1)
      print_tab (n_recur, "face_2 : ", caract_face_2)

#   Coordonnées du centre de la sphère
    coo_x = caract_face_1[2][1]
    coo_y = caract_face_1[2][2]
    coo_z = caract_face_1[2][3]
#   Rayons
    rayon = (caract_face_2[2][4]+caract_face_1[2][4])/2.
#   Epaisseur
    epaisseur = np.abs(caract_face_2[2][4]-caract_face_1[2][4])

    return coo_x, coo_y, coo_z, rayon, epaisseur

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _cree_face_mediane_sphere_1 ( self, coo_x, coo_y, coo_z, rayon ):
    """Crée la face médiane entre deux autres - cas des sphères

Création des objets de construction et de la face externe de la sphère support

Entrées :
  :coo_x, coo_y, coo_z: coordonnées du centre de la sphère
  :rayon: rayon moyen entre les deux faces

Sorties :
  :face: la face externe de la sphère support
"""

    nom_fonction = __name__ + "/_cree_face_mediane_sphere_1"
    blabla = "Dans {} :\n".format(nom_fonction)

#   Les caractéristiques de la sphère à créer
    if self._verbose_max:
      texte = blabla
      texte += "Centre : ({}, {}, {})\n".format(coo_x, coo_y, coo_z)
      texte += "Rayon : {}".format(rayon)
      print (texte)

#   Création du point central
    centre = model.addPoint(self.part_doc, coo_x, coo_y, coo_z)
    nom_centre = "{}_centre".format(self.nom_solide)
    exec_nom (centre,nom_centre)

#   Création d'un plan passant par ce centre et l'axe OX
    plan = model.addPlane(self.part_doc, model.selection("EDGE", "PartSet/OX"), model.selection("VERTEX", nom_centre), False)
    nom_plan = "{}_plan".format(self.nom_solide)
    exec_nom (plan,nom_plan)

#   Création de l'esquisse
    nom_par_1 = "{}_R".format(self.nom_solide)
    model.addParameter(self.part_doc, "{}".format(nom_par_1), "{}".format(rayon))

    sketch = model.addSketch(self.part_doc, model.selection("FACE", nom_plan))
    sketch.execute(True)

    SketchProjection_1 = sketch.addProjection(model.selection("VERTEX", nom_centre), False)
    SketchPoint_1 = SketchProjection_1.createdFeature()

    ### Create SketchArc
    SketchArc_1 = sketch.addArc(coo_x, coo_y, coo_x-rayon, coo_y, coo_x+rayon, coo_y, False)
    sketch.setRadius(SketchArc_1.results()[1], nom_par_1)
    sketch.setCoincident(SketchPoint_1.result(), SketchArc_1.center())

    ### Create SketchLine
    SketchLine_1 = sketch.addLine(coo_x-rayon, coo_y, coo_x+rayon, coo_y)
    nom_ligne = "{}_ligne".format(self.nom_solide)
    SketchLine_1.setName(nom_ligne)
    SketchLine_1.result().setName(nom_ligne)
    SketchLine_1.setAuxiliary(True)
    sketch.setHorizontal(SketchLine_1.result())
    sketch.setCoincident(SketchArc_1.startPoint(), SketchLine_1.startPoint())
    sketch.setCoincident(SketchArc_1.endPoint(), SketchLine_1.endPoint())
    sketch.setCoincident(SketchAPI_Point(SketchPoint_1).coordinates(), SketchLine_1.result())

    model.do()

    nom_sketch = "{}_esquisse".format(self.nom_solide)
    exec_nom (sketch,nom_sketch)

#   Création de la sphère complète
    sphere = model.addRevolution(self.part_doc, [model.selection("COMPOUND", nom_sketch)], model.selection("EDGE", "{}/{}".format(nom_sketch,nom_ligne)), 360, 0, "Edges")
    sphere.execute(True)

    nom_sphere = "{}_sphere".format(self.nom_solide)
    nommage (sphere, nom_sphere, (85, 0, 255))

#   Intersection de la face sphérique avec le solide initial
    face = self._creation_face_inter ( nom_sphere )

    return face

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _cree_face_mediane_tore ( self, caract_face_1, caract_face_2, n_recur ):
    """Crée la face médiane entre deux autres - cas des tores

Entrées :
  :caract_face_1, caract_face_2: les caractéristiques des 2 faces les plus grandes
  :n_recur: niveau de récursivité

Sorties :
  :face: la face médiane
"""

    nom_fonction = __name__ + "/_cree_face_mediane_tore"
    blabla = "Dans {} :".format(nom_fonction)

#   Les deux faces
    if self._verbose_max:
      print_tab (n_recur, blabla)
      print_tab (n_recur, "face_1 : ", caract_face_1)
      print_tab (n_recur, "face_2 : ", caract_face_2)

#   Caractéristiques des tores
    coo_x, coo_y, coo_z, axe_x, axe_y, axe_z, rayon_1, rayon_2 = self._cree_face_mediane_tore_0 ( caract_face_1, caract_face_2, n_recur )

#   Contrôle de la validité de l'épaisseur (bidon)
    erreur = self._verif_epaisseur ( EP_MIN*10. )

#   Création de la face
    if not erreur:
      face = self._cree_face_mediane_tore_1 ( coo_x, coo_y, coo_z, axe_x, axe_y, axe_z, rayon_1, rayon_2 )
    else:
      face = None

    return erreur, face

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _cree_face_mediane_tore_0 ( self, caract_face_1, caract_face_2, n_recur ):
    """Crée la face médiane entre deux autres - cas des tores

Décodage des caractéristiques

Entrées :
  :caract_face_1, caract_face_2: les caractéristiques des 2 faces les plus grandes
  :n_recur: niveau de récursivité

Sorties :
  :coo_x, coo_y, coo_z: coordonnées du centre du tore
  :axe_x, axe_y, axe_z: coordonnées de l'axe
  :rayon_1 : rayon principal
  :rayon_2 : rayon secondaire
"""

    nom_fonction = __name__ + "/_cree_face_mediane_tore_0"
    blabla = "Dans {} :".format(nom_fonction)

#   Les deux faces
    if self._verbose_max:
      print_tab (n_recur, blabla)
      print_tab (n_recur, "face_1 : ", caract_face_1)
      print_tab (n_recur, "face_2 : ", caract_face_2)

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

  def _cree_face_mediane_tore_1 ( self, coo_x, coo_y, coo_z, axe_x, axe_y, axe_z, rayon_1, rayon_2 ):
    """Crée la face médiane entre deux autres - cas des tores

Création des objets de construction et de la face externe du tore support

Entrées :
  :coo_x, coo_y, coo_z: coordonnées du centre du tore
  :axe_x, axe_y, axe_z: coordonnées de l'axe
  :rayon_1 : rayon principal
  :rayon_2 : rayon secondaire

Sorties :
  :face: la face externe du tore support
"""

    nom_fonction = __name__ + "/_cree_face_mediane_tore_1"
    blabla = "Dans {} :\n".format(nom_fonction)

#   Les deux faces
    if self._verbose_max:
      texte = blabla
      texte += "Centre : ({}, {}, {})\n".format(coo_x, coo_y, coo_z)
      texte += "Axe    : ({}, {}, {})\n".format(axe_x, axe_y, axe_z)
      texte += "Rayon principal : {}\n".format(rayon_1)
      texte += "Rayon secondaire : {}".format(rayon_2)
      print (texte)

#   Création du point central
    centre = model.addPoint(self.part_doc, coo_x, coo_y, coo_z)
    nom_centre = "{}_centre".format(self.nom_solide)
    exec_nom (centre,nom_centre)

#   Création de l'axe
    axe = model.addAxis(self.part_doc, axe_x, axe_y, axe_z)
    nom_axe = "{}_axe".format(self.nom_solide)
    exec_nom (axe,nom_axe)

#   Création d'un plan passant par ce centre et cet axe
    plan = model.addPlane(self.part_doc, model.selection("EDGE", nom_axe), model.selection("VERTEX", nom_centre), False)
    nom_plan = "{}_plan".format(self.nom_solide)
    exec_nom (plan,nom_plan)

#   Création de l'esquisse
    nom_par_1 = "{}_R_1".format(self.nom_solide)
    model.addParameter(self.part_doc, "{}".format(nom_par_1), "{}".format(rayon_1))
    nom_par_2 = "{}_R_2".format(self.nom_solide)
    model.addParameter(self.part_doc, "{}".format(nom_par_2), "{}".format(rayon_2))

    sketch = model.addSketch(self.part_doc, model.selection("FACE", nom_plan))
    sketch.execute(True)

    SketchProjection_1 = sketch.addProjection(model.selection("VERTEX", nom_centre), False)
    SketchPoint_1 = SketchProjection_1.createdFeature()

    SketchProjection_2 = sketch.addProjection(model.selection("EDGE", nom_axe), False)
    SketchLine_1 = SketchProjection_2.createdFeature()

    SketchPoint_2 = sketch.addPoint(rayon_1, 0.)
    sketch.setDistance(SketchPoint_1.result(), SketchPoint_2.coordinates(), nom_par_1, True)

    SketchLine_2 = sketch.addLine(0., 0., rayon_1, 0.)
    SketchLine_2.setAuxiliary(True)
    sketch.setCoincident(SketchAPI_Point(SketchPoint_1).coordinates(), SketchLine_2.startPoint())
    sketch.setCoincident(SketchPoint_2.coordinates(), SketchLine_2.endPoint())
    sketch.setPerpendicular(SketchLine_1.result(), SketchLine_2.result())

    SketchCircle_1 = sketch.addCircle(0., 0., rayon_2)
    sketch.setCoincident(SketchPoint_2.result(), SketchCircle_1.center())
    sketch.setRadius(SketchCircle_1.results()[1], nom_par_2)

    model.do()

    nom_sketch = "{}_esquisse".format(self.nom_solide)
    exec_nom (sketch,nom_sketch)

#   Création du tore complet
    nom_tore = "{}_tore".format(self.nom_solide)
    self._cree_revolution ( nom_sketch, nom_centre, coo_x, coo_y, coo_z, axe_x, axe_y, axe_z, nom_tore )

#   Intersection de la face torique avec le solide initial
    face = self._creation_face_inter ( nom_tore )

    return face

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _cree_face_mediane_cone ( self, caract_face_1, caract_face_2, n_recur ):
    """Crée la face médiane entre deux autres - cas des cones

Entrées :
  :caract_face_1, caract_face_2: les caractéristiques des 2 faces les plus grandes
  :n_recur: niveau de récursivité

Sorties :
  :face: la face médiane
"""

    nom_fonction = __name__ + "/_cree_face_mediane_cone"
    blabla = "Dans {} :".format(nom_fonction)

#   Les deux faces
    if self._verbose_max:
      print_tab (n_recur, blabla)
      print_tab (n_recur, "face_1 : ", caract_face_1)
      print_tab (n_recur, "face_2 : ", caract_face_2)

#   Caractéristiques des cones
    coo_x, coo_y, coo_z, axe_x, axe_y, axe_z, rayon_1, rayon_2, hauteur = self._cree_face_mediane_cone_0 ( caract_face_1, caract_face_2, n_recur )

#   Contrôle de la validité de l'épaisseur (bidon)
    erreur = self._verif_epaisseur ( EP_MIN*10. )

#   Création de la face
    if not erreur:
      face = self._cree_face_mediane_cone_1 ( coo_x, coo_y, coo_z, axe_x, axe_y, axe_z, rayon_1, rayon_2, hauteur )
    else:
      face = None

    return erreur, face

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _cree_face_mediane_cone_0 ( self, caract_face_1, caract_face_2, n_recur ):
    """Crée la face médiane entre deux autres - cas des cones

Décodage des caractéristiques

Entrées :
  :caract_face_1, caract_face_2: les caractéristiques des 2 faces les plus grandes
  :n_recur: niveau de récursivité

Sorties :
  :coo_x, coo_y, coo_z: coordonnées du centre de la base
  :axe_x, axe_y, axe_z: coordonnées de l'axe
  :rayon_1, rayon_2: rayons moyens du côté de la base et à l'opposé
  :hauteur: hauteur du cone
"""

    nom_fonction = __name__ + "/_cree_face_mediane_cone_0"
    blabla = "Dans {} :".format(nom_fonction)

#   Les deux faces
    if self._verbose_max:
      print_tab (n_recur, blabla)
      print_tab (n_recur, "face_1 : ", caract_face_1)
      print_tab (n_recur, "face_2 : ", caract_face_2)

#   Coordonnées du centre de la base
    coo_x = caract_face_1[2][1]
    coo_y = caract_face_1[2][2]
    coo_z = caract_face_1[2][3]
#   Coordonnées de l'axe
    axe_x = caract_face_1[2][4]
    axe_y = caract_face_1[2][5]
    axe_z = caract_face_1[2][6]
#   Rayons
    rayon_1 = (caract_face_2[2][7]+caract_face_1[2][7])/2.
    rayon_2 = (caract_face_2[2][8]+caract_face_1[2][8])/2.
#   Hauteur
    hauteur = caract_face_1[2][9]

#   Bilan
    if self._verbose_max:
      print_tab (n_recur, "coo_x : ", coo_x)
      print_tab (n_recur, "coo_y : ", coo_y)
      print_tab (n_recur, "coo_z : ", coo_z)
      print_tab (n_recur, "axe_x : ", axe_x)
      print_tab (n_recur, "axe_y : ", axe_y)
      print_tab (n_recur, "axe_z : ", axe_z)
      print_tab (n_recur, "rayon_1 : ", rayon_1)
      print_tab (n_recur, "rayon_2 : ", rayon_2)
      print_tab (n_recur, "hauteur : ", hauteur)

    return coo_x, coo_y, coo_z, axe_x, axe_y, axe_z, rayon_1, rayon_2, hauteur

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _cree_face_mediane_cone_1 ( self, coo_x, coo_y, coo_z, axe_x, axe_y, axe_z, rayon_1, rayon_2, hauteur ):
    """Crée la face médiane entre deux autres - cas des cones

Création des objets de construction et de la face externe du cone support

Entrées :
  :coo_x, coo_y, coo_z: coordonnées du centre de la base
  :axe_x, axe_y, axe_z: coordonnées de l'axe
  :rayon_1, rayon_2: rayons moyens du côté de la base et à l'opposé
  :hauteur: hauteur du cone

Sorties :
  :face: la face externe du cone support
"""
    nom_fonction = __name__ + "/_cree_face_mediane_cone_1"
    blabla = "Dans {} :\n".format(nom_fonction)

#   Les deux faces
    if self._verbose_max:
      texte = blabla
      texte += "Centre : ({}, {}, {})\n".format(coo_x, coo_y, coo_z)
      texte += "Axe    : ({}, {}, {})\n".format(axe_x, axe_y, axe_z)
      texte += "Rayons : {}, {}\n".format(rayon_1, rayon_2)
      texte += "Hauteur : {}".format(hauteur)
      print (texte)

#   1. Création du point central de la base, côté rayon_1
    centre_1 = model.addPoint(self.part_doc, coo_x, coo_y, coo_z)
    nom_centre_1 = "{}_centre_1".format(self.nom_solide)
    exec_nom (centre_1,nom_centre_1)

#   2. Création du point central, du côté de rayon_2
    centre_2 = model.addPoint(self.part_doc, coo_x+hauteur*axe_x, coo_y+hauteur*axe_y, coo_z+hauteur*axe_z)
    nom_centre_2 = "{}_centre_2".format(self.nom_solide)
    exec_nom (centre_2,nom_centre_2)

#   3. Création de l'axe
    axe = model.addAxis(self.part_doc, model.selection("VERTEX", nom_centre_1), model.selection("VERTEX", nom_centre_2))
    nom_axe = "{}_axe".format(self.nom_solide)
    exec_nom (axe,nom_axe)

#   4. Création d'un plan passant par le centre de la base et l'axe
#   4.1. Création d'un vecteur perpendiculaire à l'axe
    coeff = 10.
    v_perp = model.addAxis(self.part_doc, -coeff*axe_y, coeff*axe_x, 0.)
    nom_v_perp = "{}_v_perp".format(self.nom_solide)
    exec_nom (v_perp,nom_v_perp)
#   4.2. Création du plan
    plan = model.addPlane(self.part_doc, model.selection("EDGE",nom_v_perp), model.selection("VERTEX", nom_centre_1), True)
    nom_plan = "{}_plan".format(self.nom_solide)
    exec_nom (plan,nom_plan)

#   5. Paramétrage
    nom_par_1 = "{}_R_1".format(self.nom_solide)
    model.addParameter(self.part_doc, "{}".format(nom_par_1), "{}".format(rayon_1))
    nom_par_2 = "{}_R_2".format(self.nom_solide)
    model.addParameter(self.part_doc, "{}".format(nom_par_2), "{}".format(rayon_2))
    nom_par_3 = "{}_H".format(self.nom_solide)
    model.addParameter(self.part_doc, "{}".format(nom_par_3), "{}".format(hauteur))

#   6. Création de l'esquisse

    sketch = model.addSketch(self.part_doc, model.selection("FACE", nom_plan))
    sketch.execute(True)

#   6.1. Projection des centres et de l'axe
    SketchProjection_1 = sketch.addProjection(model.selection("VERTEX", nom_centre_1), False)
    SketchPoint_1 = SketchProjection_1.createdFeature()
    sk_coo_x_1 = SketchAPI_Point(SketchPoint_1).coordinates().x()
    sk_coo_y_1 = SketchAPI_Point(SketchPoint_1).coordinates().y()

    SketchProjection_2 = sketch.addProjection(model.selection("VERTEX", nom_centre_2), False)
    SketchPoint_2 = SketchProjection_2.createdFeature()
    sk_coo_x_2 = SketchAPI_Point(SketchPoint_2).coordinates().x()
    sk_coo_y_2 = SketchAPI_Point(SketchPoint_2).coordinates().y()

    SketchProjection_3 = sketch.addProjection(model.selection("EDGE", nom_axe), False)
    SketchLine_0 = SketchProjection_3.createdFeature()

#   6.2. Lignes perpendiculaires à l'axe passant par les centres
    SketchLine_1 = sketch.addLine(sk_coo_x_1, sk_coo_y_1, sk_coo_x_1+rayon_1, sk_coo_y_1)
    SketchLine_1.setAuxiliary(True)
    sketch.setCoincident(SketchAPI_Point(SketchPoint_1).coordinates(), SketchLine_1.startPoint())
    sketch.setPerpendicular(SketchLine_0.result(), SketchLine_1.result())
    sketch.setLength(SketchLine_1.result(), nom_par_1)

    SketchLine_2 = sketch.addLine(sk_coo_x_2, sk_coo_y_2, sk_coo_x_2+rayon_2, sk_coo_y_2)
    SketchLine_2.setAuxiliary(True)
    sketch.setCoincident(SketchAPI_Point(SketchPoint_2).coordinates(), SketchLine_2.startPoint())
    sketch.setPerpendicular(SketchLine_0.result(), SketchLine_2.result())
    sketch.setLength(SketchLine_2.result(), nom_par_2)

#   6.3. Ligne joignant les extrémités des précédentes et point milieu
    SketchLine_3 = sketch.addLine(sk_coo_x_1+rayon_1, sk_coo_y_1, sk_coo_x_2+rayon_2, sk_coo_y_2)
    sketch.setCoincident(SketchLine_3.startPoint(), SketchLine_1.endPoint())
    sketch.setCoincident(SketchLine_3.endPoint(), SketchLine_2.endPoint())
    SketchLine_3.setAuxiliary(True)
    SketchPoint_3 = sketch.addPoint(sk_coo_x_1, sk_coo_y_1)
    sketch.setMiddlePoint(SketchLine_3.result(), SketchPoint_3.coordinates())

#   6.4. Ligne support de la future révolution
    SketchLine_4 = sketch.addLine(sk_coo_x_1+rayon_1, sk_coo_y_1, sk_coo_x_2+rayon_2, sk_coo_y_2)
    sketch.setMiddlePoint(SketchLine_4.result(), SketchPoint_3.coordinates())
    sketch.setCoincident(SketchLine_1.endPoint(), SketchLine_4.result())
    sketch.setLength(SketchLine_4.result(), "1.2*{}".format(nom_par_3))

    model.do()

    nom_sketch = "{}_esquisse".format(self.nom_solide)
    exec_nom (sketch,nom_sketch)

#   Création du cone complet
    nom_cone = "{}_cone".format(self.nom_solide)
    self._cree_revolution ( nom_sketch, nom_centre_1, coo_x, coo_y, coo_z, axe_x, axe_y, axe_z, nom_cone )

#   Intersection de la face conique avec le solide initial
    face = self._creation_face_inter ( nom_cone )

    return face

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _cree_centre_axe_plan ( self, coo_c, vnor, prefixe, n_recur ):
    """Crée un centre, un axe, un plan

Préparatifs

Entrées :
  :coo_c: coordonnées du centre de la base
  :vnor: coordonnées du vecteur normal
  :prefix: prefixe du nom des objets
  :n_recur: niveau de récursivité

Sorties :
  :centre: centre
  :normal: vecteur normal
  :plan: plan
"""

    nom_fonction = __name__ + "/_cree_centre_axe_plan"
    blabla = "Dans {} :".format(nom_fonction)
    if self._verbose_max:
      print_tab (n_recur, blabla)
      print_tab (n_recur, "Centre   : ({}, {}, {})".format(coo_c[0], coo_c[1], coo_c[2]))
      print_tab (n_recur, "Normale  : ({}, {}, {})".format(vnor[0], vnor[1], vnor[2]))

#   Création du point central
    centre = model.addPoint(self.part_doc, coo_c[0], coo_c[1], coo_c[2])
    nom_centre = "{}_centre".format(prefixe)
    exec_nom (centre,nom_centre)

#   Création du vecteur normal
    v_norm = model.addAxis(self.part_doc, vnor[0], vnor[1], vnor[2])
    nom_vnorm = "{}_normale".format(prefixe)
    exec_nom (v_norm,nom_vnorm)

#   Création du plan perpendiculaire au vecteur normal
    plan = model.addPlane(self.part_doc, model.selection("EDGE", v_norm.name()), model.selection("VERTEX", centre.name()), True)
    nom_plan = "{}_plan".format(prefixe)
    exec_nom (plan,nom_plan)

    #print ("fin de {}".format(nom_fonction))

    return centre, v_norm, plan

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _calcul_lg_caract ( self, objet, n_recur ):
    """Crée une longueur caractéristique de l'objet

Entrées :
  :objet: l'objet à traiter
  :n_recur: niveau de récursivité

Sorties :
  :l_caract: longueur caractéristique de l'objet
"""

    nom_fonction = __name__ + "/_calcul_lg_caract"
    blabla = "Dans {} :".format(nom_fonction)

    if self._verbose_max:
      print_tab (n_recur, blabla)

    properties = model.getGeometryCalculation(self.part_doc,model.selection("{}".format(objet.shapeType()), "{}".format(objet.name())))
    l_caract = properties[0]

    if self._verbose_max:
      print_tab (n_recur, "Longueur caractéristique : ", l_caract)

    return l_caract

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _cree_revolution ( self, nom_sketch, nom_centre, coo_x, coo_y, coo_z, axe_x, axe_y, axe_z, nom_objet ):
    """Crée un volume de révolution

Entrées :
  :nom_sketch: nom de l'esquisse à révolutionner
  :nom_centre: nom du point associé au centre du volume de révolution
  :coo_x, coo_y, coo_z: coordonnées du centre du tore
  :axe_x, axe_y, axe_z: coordonnées de l'axe
  :rayon_1 : rayon principal
  :rayon_2 : rayon secondaire
  :nom_objet: nom de l'objet 2D créé
"""

    nom_fonction = __name__ + "/_cree_revolution"
    blabla = "Dans {} :\n".format(nom_fonction)

    if self._verbose_max:
      texte = blabla
      texte += "Centre : ({}, {}, {})\n".format(coo_x, coo_y, coo_z)
      texte += "Axe    : ({}, {}, {})\n".format(axe_x, axe_y, axe_z)
      print (texte)

#   Création d'un point décalé par rapport au point central
    point = model.addPoint(self.part_doc, coo_x+axe_x, coo_y+axe_y, coo_z+axe_z)
    nom_point = "{}_point".format(self.nom_solide)
    exec_nom (point,nom_point)

#   Création de l'axe de la rotation
    axe_r = model.addAxis(self.part_doc, model.selection("VERTEX", nom_centre), model.selection("VERTEX", nom_point))
    nom_axe_r = "{}_axe_r".format(self.nom_solide)
    exec_nom (axe_r,nom_axe_r)

#   Création de l'objet complet
    objet = model.addRevolution(self.part_doc, [model.selection("COMPOUND", nom_sketch)], model.selection("EDGE", nom_axe_r), 360, 0, "Edges")
    objet.execute(True)

    nommage (objet, nom_objet, (85, 0, 255))

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _creation_face_inter ( self, nom_objet ):
    """Crée la face par intersection entre l'objet initial et une face complète

. Repère la face principale de l'objet support
. Réalise l'intersection avec le solide initial

Entrées :
  :nom_objet: nom de l'objet 2D créé

Sorties :
  :face: la face externe de l'objet support intersecté avec le solide initial
"""

    nom_fonction = __name__ + "/_creation_face_inter"
    blabla = "Dans {} :\n".format(nom_fonction)

    if self._verbose_max:
      print (blabla)

    face = model.addCommon(self.part_doc, [model.selection("SOLID", self.nom_solide_aux), model.selection("FACE", nom_objet)], keepSubResults = True)
    face.execute(True)

    return face

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def face_mediane_solide (self, solide, n_recur=0):
    """Calcul de la face médiane pour un solide

Entrées :
  :solide: solide SHAPER à traiter
  :n_recur: niveau de récursivité

Sorties :
  :erreur: code d'erreur
  :message: message d'erreur
"""

    nom_fonction = __name__ + "/face_mediane_solide"
    blabla = "Dans {} :".format(nom_fonction)

    if self._verbose_max:
      print_tab (n_recur, blabla, saut_av=True)
    if self._verbose:
      print_tab (n_recur, "Traitement du solide ", solide.name())

# 1. Préalables

    erreur = 0
    message = ""

    while not erreur :

# 2. Calcul des caractéristiques géométriques des faces

      tb_caract = self._calcul_caract_faces ( solide, n_recur )

# 3. Tri des faces en fonction de leurs caractéristiques géométriques

      erreur, message, caract_face_1, caract_face_2 = self._tri_faces ( tb_caract, n_recur )
      if erreur:
        break

# 4. Création de la face médiane

      erreur, face = self._cree_face_mediane ( solide, caract_face_1, caract_face_2, n_recur )
      if ( erreur or ( face is None ) ):
        break

# 6. Exportation step

      if self._export_step:
        fichier = os.path.join(self.rep_step, "{}.stp".format(face.result().name()))
        export = model.exportToFile(self.part_doc, fichier, [model.selection(face.result().shapeType(), face.result().name())])
        export.execute(True)
        model.do()

# 7. La fin

      break

    if ( erreur and self._verbose_max ):
      print (blabla, message)

    return erreur, message

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _traitement_objet (self, solide, n_recur=0):
    """Traitement d'un objet

Entrées :
  :solide: solide SHAPER à traiter
  :n_recur: niveau de récursivité

Sorties :
  :erreur: code d'erreur
  :message: message d'erreur
"""

    nom_fonction = __name__ + "/_traitement_objet"
    blabla = "Dans {} :".format(nom_fonction)

    if self._verbose_max:
      print_tab (n_recur, blabla, saut_av=True)
      texte = "solide = {} ".format(solide.name())
      print_tab (n_recur, texte, solide)

# 1. En cas d'exportation step, répertoire de travail associé à l'éventuel fichier de départ
#    Attention à ne pas recréer le répertoire à chaque fois
    if self._export_step:

      if self.rep_step is None:

        if self._verbose_max:
          print_tab (n_recur, "Préparation de l'export STEP")

        if self.ficcao is None:
          self.rep_step = tempfile.mkdtemp(prefix="{}_".format(self.objet_principal.name()))
        else:
          self.rep_step = os.path.join(os.path.dirname(self.ficcao),"{}_M".format(self.objet_principal.name()))
          if os.path.isdir(self.rep_step):
            l_aux = os.listdir(self.rep_step)
            for nomfic in l_aux:
              os.remove(os.path.join(self.rep_step,nomfic))
          else:
            os.mkdir(self.rep_step)

      if self._verbose_max:
        print_tab (n_recur, "Les fichiers CAO des surfaces seront dans le répertoire {}".format(self.rep_step))

# 2. Calcul réel de la face médiane

    erreur, message = self.face_mediane_solide (solide, n_recur)

    return erreur, message

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def surf_fic_cao (self, ficcao, nom_objet=None):
    """Calcule la surface médiane pour un objet dans un fichier passé en argument

Entrées :
  :ficcao: fichier de l'objet à traiter
  :nom_objet: un nom à donner à l'objet à traiter

Sorties :
  :erreur: code d'erreur
  :message: message d'erreur
"""

    nom_fonction = __name__ + "/surf_fic_cao"
    blabla = "Dans {} :".format(nom_fonction)

    if self._verbose_max:
      print (blabla)

    erreur = 0
    message = ""

    model.begin()

    while not erreur :

      if self.affiche_aide_globale:
        break

# 1. Définition de la pièce

      self.part_doc = model.activeDocument()
      if ( self.part_doc.kind() == "PartSet" ):
        part = model.addPart(self.part_doc)
        self.part_doc = part.document()

# 2. Import de la CAO

      self.ficcao = ficcao
      print ("Traitement du fichier {}".format(ficcao))

      erreur, message, objet = import_cao (self.part_doc, ficcao, nom_objet, self._verbose_max)
      if erreur:
        break

# 3. Calcul des surfaces

      erreur, message = self.surf_objet_shaper ( objet )
      if erreur:
        break
      #print (message)

      break

    model.end()

    if ( erreur and self._verbose_max ):
      print (blabla, message)

    return erreur, message

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def surf_objet_shaper (self, objet):
    """Calcule les surfaces médianes pour un objet SHAPER passé en argument

Entrées :
  :objet: objet à traiter

Sorties :
  :erreur: code d'erreur
  :message: message d'erreur
"""

    nom_fonction = __name__ + "/surf_objet_shaper"
    blabla = "Dans {} :".format(nom_fonction)

    if self._verbose_max:
      print (blabla)

    erreur = 0
    message = ""

    while not erreur :

      if self.affiche_aide_globale:
        break

# 1. Acquisition de la liste des noms des sous-objets solides

      self.d_statut_so = dict()
      self.l_noms_so = list()
      self.l_faces_m = list()

      _ = self._nom_sous_objets (objet, True)
      if self._verbose_max:
        print ("Noms des sous-objets : {}".format(self.l_noms_so))

# 2. Les faces médianes

      erreur, message = self._surf_objet_shaper_0 ( objet )
      if erreur:
        break

# 3. Gestion des faces créées

      self._surf_objet_shaper_1 ( )

# 4. Futur message pour le résultat

      if ( self._export_step and not erreur ):
        message = "Les fichiers des CAO des surfaces sont dans le répertoire {}".format(self.rep_step)

      break

    return erreur, message

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _surf_objet_shaper_0 (self, objet, n_recur=0):
    """Calcule les surfaces médianes pour un objet SHAPER passé en argument

Entrées :
  :objet: objet à traiter
  :n_recur: niveau de récursivité

Sorties :
  :erreur: code d'erreur
  :message: message d'erreur
"""

    nom_fonction = __name__ + "/_surf_objet_shaper_0"
    blabla = "Dans {} :".format(nom_fonction)

    if self._verbose_max:
      print_tab (n_recur, blabla)
      print_tab (n_recur, "n_recur = ", n_recur)

    erreur = 0
    message = ""

    while not erreur :

# 1. Au premier passage, il faut récupérer la pièce SHAPER et garder la référence au résultat principal

      if ( n_recur == 0 ):
        self.part_doc = model.activeDocument()
        objet_0 = objet.result()
        self.objet_principal = objet_0
        objet_bis = objet.defaultResult().shape()
        if self._verbose_max:
          print_tab (0, "Examen de l'objet initial ", objet.result().name(), saut_av=True)
          print_tab (0, "Type python : ", type(objet))
          print_tab (0, "Type {} / {} ; volume = ".format(objet_bis.shapeType(),objet_bis.shapeTypeStr()), GeomAlgoAPI_ShapeTools.volume(objet_bis))
      else:
        objet_0 = objet
        if self._verbose_max:
          print_tab (n_recur, "Examen de l'objet ", objet.name(), saut_av=True)
          print_tab (n_recur, "Type python : ", type(objet))
          print_tab (n_recur, "shapeType : ", objet.shapeType())

# 2. On descend dans l'arborescence des sous-objets jusqu'à en trouver un qui n'en n'a pas

      nb_sub_results = objet_0.numberOfSubs()

      if self._verbose_max:
        print_tab (n_recur, "Examen de l'objet '{}' de type ".format(objet_0.name()), objet_0.shapeType(), saut_av=True)
        print_tab (n_recur, "objet.result().numberOfSubs() : ", nb_sub_results)

      for n_sobj in range(nb_sub_results):

# 2.1. Exploration récursive de l'arborescence

        erreur, message = self._surf_objet_shaper_0 ( objet_0.subResult(n_sobj), n_recur+1 )
        if erreur:
          break

      if erreur:
        break

# 2.2. Cet objet n'a pas de sous-objets. Si c'est un solide, on le traite

      if ( objet_0.shapeType() == "SOLID" ):
        erreur, message = self.surf_solide_shaper ( objet_0, n_recur )
        if erreur:
          break

# 3. Futur message pour le résultat

      if self._export_step:
        message = "Les fichiers STEP des surfaces sont dans le répertoire {}".format(self.rep_step)

      break

    return erreur, message

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _surf_objet_shaper_1 (self, n_recur=0):
    """Gestion des surfaces médianes créées

Entrées :
  :n_recur: niveau de récursivité

Sorties :
  :erreur: code d'erreur
  :message: message d'erreur
"""

    nom_fonction = __name__ + "/_surf_objet_shaper_1"
    blabla = "Dans {} :\n".format(nom_fonction)

    if self._verbose_max:
      print_tab (n_recur, blabla)

# 1. Informations sur les faces à problème

    if self.faces_pb_nb:
      if ( self.faces_pb_nb == 1 ):
        texte = "1 face pose"
      else:
        texte = "{} faces posent".format(self.faces_pb_nb)
      print_tab (n_recur, "{} problème.".format(texte), self.faces_pb_msg, saut_av=True)

# 2. Si plus d'une face a été créée
    if ( len(self.l_faces_m) > 1 ):

# 2.1. Partition du paquet de faces

      if self._verbose_max:
        print_tab (n_recur, "Partitionnnement des faces créées.")

# 2.1.1. Pour une raison mystérieuse, il faut commencer par les faces entières, puis mettre les sous-faces éventuelles
      d_faces = dict()
      for (face,_) in self.l_faces_m:
        if not face.result().numberOfSubs():
          d_faces[face.name()] = [face.name()]
      for (face,_) in self.l_faces_m:
        nb_sub_results = face.result().numberOfSubs()
        if nb_sub_results:
          laux = list()
          for n_sobj in range(nb_sub_results):
            laux.append(face.result().subResult(n_sobj).name())
          d_faces[face.name()] = laux

      l_objets = list()
      for _, laux in d_faces.items():
        for s_face_n in laux:
          l_objets.append(model.selection("FACE", s_face_n))

      Partition_1 = model.addPartition(self.part_doc, l_objets, keepSubResults = True)
      nom = "{}_M".format(self.objet_principal.name())
      exec_nom (Partition_1,nom)

      iaux = 0
      for face_n, laux in d_faces.items():
        Partition_1.result().subResult(iaux).setName("{}".format(face_n))
        if ( len(laux) > 1 ):
          for jaux, s_face_n in enumerate(laux):
            Partition_1.result().subResult(iaux).subResult(jaux).setName("{}_M".format(s_face_n))
        iaux += 1
      self._couleur_objet (Partition_1, n_recur, coul_r=0, coul_g=170, coul_b=0)

# 2.2. Récupération des faces individuelles

      if self._verbose_max:
        print_tab (n_recur, "Récupération des faces individuelles.")

      l_objets = list()
      for iaux, (face,_) in enumerate(self.l_faces_m):
        l_objets.append(face.result())

      Recover_1 = model.addRecover(self.part_doc, Partition_1, l_objets)
      for iaux, (face,_) in enumerate(self.l_faces_m):
        Recover_1.results()[iaux].setName("{}".format(face.name()))
        Recover_1.results()[iaux].setColor(0, 170, 0)
        nb_sub_results = Recover_1.results()[iaux].numberOfSubs()
        for n_sobj in range(nb_sub_results):
          Recover_1.results()[iaux].subResult(n_sobj).setName("{}_{}".format(face.name(),n_sobj))
          Recover_1.results()[iaux].subResult(n_sobj).setColor(0, 170, 0)

# 2.3. Mise en dossier

      if self._verbose_max:
        print_tab (n_recur, "Mise en dossier.")

      for (face,fonction_0) in self.l_faces_m:
        dossier = model.addFolder(self.part_doc, fonction_0, face)
        dossier.execute(True)
        dossier.setName(face.name()[:-2])
      dossier = model.addFolder(self.part_doc, Partition_1, Recover_1)
      dossier.execute(True)
      dossier.setName(self.objet_principal.name())

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def surf_solide_shaper (self, solide, n_recur):
    """Calcule les surfaces médianes pour un solide SHAPER solide passé en argument

Entrées :
  :solide: solide SHAPER à traiter
  :n_recur: numéro de la récurrence

Sorties :
  :erreur: code d'erreur
  :message: message d'erreur
"""

    nom_fonction = __name__ + "/surf_solide_shaper"
    blabla = "Dans {} :".format(nom_fonction)

    if self._verbose_max:
      print_tab (n_recur, blabla, saut_av=True)

    erreur = 0
    message = ""

    while not erreur :

      self.nom_solide = solide.name()
      if self._verbose_max:
        print_tab (n_recur, "solide : ", self.nom_solide)

# 1. Isolement du solide
      solide_aux, recover = self._isole_solide ( solide, n_recur )

# 2. Traitement de l'objet correspondant
      erreur, message = self._traitement_objet ( solide_aux, n_recur=n_recur )

      if ( erreur and self._verbose_max ):
        print (blabla, message)

# 3. Neutralisation des erreurs dues à l'épaisseur
      if ( erreur in (-2,-1,2) ):
        erreur = 0
        message = ""
      if erreur:
        break

# 4. Mise en forme de l'objet principal récupéré
      if ( recover is not None ):
        _ = self._nom_sous_objets (recover, False)

      break

    return erreur, message

#===========================  Fin de la méthode ==================================

#==========================  Fin de la classe ====================================

#==================================================================================
# Lancement
#==================================================================================

if __name__ == "__main__" :

  HOME_SH_SM_EXCHANGE = os.environ["HOME_SH_SM_EXCHANGE"]
  L_FIC_CAO = list()
  #L_FIC_CAO.append(os.path.join(HOME_SH_SM_EXCHANGE, "SHAPER", "SurfaceMediane", "Solid_7.step"))
  L_FIC_CAO.append(os.path.join(HOME_SH_SM_EXCHANGE, "SHAPER", "SurfaceMediane", "CAO_STEP", "Objet_1.stp"))
  #L_FIC_CAO.append(os.path.join(HOME_SH_SM_EXCHANGE, "SHAPER", "SurfaceMediane", "CAO_STEP", "Naval_007.stp"))
  ##L_FIC_CAO.append(os.path.join(HOME_SH_SM_EXCHANGE, "SHAPER", "SurfaceMediane", "CAO_STEP", "Naval.stp"))
  #L_FIC_CAO.append(os.path.join(HOME_SH_SM_EXCHANGE, "SHAPER", "SurfaceMediane", "CAO_STEP", "Partition_2.step"))
  #L_FIC_CAO.append(os.path.join(HOME_SH_SM_EXCHANGE, "SHAPER", "SurfaceMediane", "CAO_STEP", "Partition_2_1_22.step"))
  #L_FIC_CAO.append(os.path.join(HOME_SH_SM_EXCHANGE, "SHAPER", "SurfaceMediane", "CAO_STEP", "Partition_2_1_5.step"))
  #L_FIC_CAO.append(os.path.join(HOME_SH_SM_EXCHANGE, "SHAPER", "SurfaceMediane", "CAO_STEP", "Observatoire.stp"))

  for FIC_CAO in L_FIC_CAO:

# 1. Options

    L_OPTIONS = list()
    #L_OPTIONS.append("-h")
    #L_OPTIONS.append("-v")
    L_OPTIONS.append("-vmax")
    L_OPTIONS.append("-export_step")

# 2. Lancement de la classe

    #print ("L_OPTIONS :", L_OPTIONS)

    SURFACE_MEDIANE = SurfaceMediane(L_OPTIONS)
    if SURFACE_MEDIANE.affiche_aide_globale:
      sys.stdout.write(SURFACE_MEDIANE.__doc__+"\n")
    else:
      model.begin()
      PARTSET = model.moduleDocument()
      _ = model.addPart(PARTSET)
      ERREUR, MESSAGE_ERREUR = SURFACE_MEDIANE.surf_fic_cao(FIC_CAO)
      if ERREUR:
        MESSAGE_ERREUR += "\n Code d'erreur : %d\n" % ERREUR
        sys.stderr.write(MESSAGE_ERREUR)
        break

    del SURFACE_MEDIANE

  #sys.exit(0)
