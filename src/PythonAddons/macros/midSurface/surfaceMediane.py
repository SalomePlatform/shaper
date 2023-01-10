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

__revision__ = "V10.48"

#========================= Les imports - Début ===================================

import os
import sys
import tempfile

import salome

salome.standalone()
salome.salome_init()

import SALOMEDS
from SketchAPI import *
from salome.shaper import model
from GeomAPI import *
from GeomAlgoAPI import *

from salome.geom import geomBuilder

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

    if nom_objet is not None:
      objet.result().setName(nom_objet)

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

Si la création n'a pas eu lieu, un message est émis et les solides sont mise en couleur :
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
  objet_geom = None
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
    blabla = "Dans {} :\n".format(nom_fonction)

    if self._verbose_max:
      prefixe = ""
      for _ in range(n_recur):
        prefixe += "\t"
      texte = "\n{}{}".format(prefixe,blabla)
      texte += "{}n_recur = {}".format(prefixe,n_recur)
      texte += "\n{}RGB = ({},{},{})".format(prefixe,coul_r,coul_g,coul_b)
      print (texte)

# 1. Au premier passage, il faut garder la référence au résultat principal

    if ( n_recur ==  0 ):
      objet_0 = objet.result()
    else:
      objet_0 = objet

# 2. On descend dans l'arborescence des sous-objets jusqu'à en trouver un qui n'en n'a pas

    nb_sub_results = objet_0.numberOfSubs()

    if self._verbose_max:
      texte = "{}Examen de l'objet '{}' ".format(prefixe,objet_0.name())
      texte += "de type '{}' ".format(objet_0.shapeType())
      texte += "et de {} sous-objets".format(nb_sub_results)
      print (texte)

    for n_sobj in range(nb_sub_results):

# 2.1. Exploration récursive de l'arborescence

      self._couleur_objet ( objet_0.subResult(n_sobj), n_recur+1, coul_r, coul_g, coul_b )

# 2.2. Cet objet n'a pas de sous-objets : on le colore
    if self._verbose_max:
      texte = "{}Couleur affectée à l'objet '{}' ".format(prefixe,objet_0.name())
      print (texte)
    objet_0.setColor (int(coul_r),int(coul_g),int(coul_b))

    #print ("sortie de {}".format(nom_fonction))

    return

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _isole_solide ( self, solide ):
    """Isole le solide de son arboresence

Entrées :
  :solide: le solide à traiter

Sorties :
  :objet: le solide isolé
  :recover: la fonction de récupération
    """

    nom_fonction = __name__ + "/_isole_solide"
    blabla = "\nDans {} :\n".format(nom_fonction)
    if self._verbose_max:
      texte = blabla
      texte += "Pour le solide '{}' ".format(solide.name())
      texte += "de l'objet principal '{}'".format(self.objet_principal.name())
      print (texte)

    if ( solide.name() != self.objet_principal.name() ):

      if self._verbose_max:
        print (". Extraction du solide")

# 1. Extraction du solide
      remove_subshapes = model.addRemoveSubShapes(self.part_doc, model.selection("COMPOUND", self.objet_principal.name()))
      remove_subshapes.setSubShapesToKeep([model.selection("SOLID", solide.name())])

      self.nom_solide_aux = "{}_S".format(solide.name())
      remove_subshapes.result().setName(self.nom_solide_aux)

      self.fonction_0 = remove_subshapes

# 2. Récupération de l'objet principal
      recover = model.addRecover(self.part_doc, remove_subshapes, [self.objet_principal])
      recover.result().setName(self.objet_principal.name())

      objet = remove_subshapes.result()

    else:

      if self._verbose_max:
        print (". Mise en place du solide")

      objet = solide
      self.nom_solide_aux = self.objet_principal.name()
      self.fonction_0 = None
      recover = None

    if self._verbose_max:
      texte = "objet final : '{}'\n".format(objet.name())
      texte += "fonction_0 : {}".format(self.fonction_0)
      texte += "recover : {}".format(recover)
      print (texte)

    return objet, recover

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

      if self._verbose_max:
        print (".. Traitement du solide '{}'".format(self.nom_solide))
        longueur, aire, volume = geompy.BasicProperties(solide)
        texte = "{}".format(geompy.WhatIs(solide))
        texte += ". longueur, aire, volume : {}, {}, {}".format(longueur,aire,volume)
        print (texte)

# Liste des faces qui composent le solide
      l_faces = geompy.ExtractShapes(solide, geompy.ShapeType["FACE"], True)
      #if self._verbose_max:
        #print ("Liste des {} faces qui composent le solide :".format(len(l_faces)))
        #for iaux, face in enumerate(l_faces):
          #print ("Face n° {} :\n {}".format(iaux,geompy.WhatIs(face)))

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
  :tb_caract: tableau des caractéristiques géométriques des faces

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

  def _verif_epaisseur ( self, epaisseur ):
    """Contrôle de la validité de l'épaisseur

Entrées :
  :epaisseur: épaisseur du solide
    """

    nom_fonction = __name__ + "/_verif_epaisseur"
    blabla = "\nDans {} :\n".format(nom_fonction)

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

    return erreur

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _cree_face_mediane ( self, solide, geompy, caract_face_1, caract_face_2 ):
    """Crée la face médiane entre deux autres

Entrées :
  :solide: solide SHAPER à traiter
  :geompy: environnement de GEOM
  :caract_face_1, caract_face_2: les caractéristiques des 2 faces les plus grandes

Sorties :
  :face: la face médiane créée
    """

    nom_fonction = __name__ + "/_cree_face_mediane"
    blabla = "\nDans {} :\n".format(nom_fonction)

    if self._verbose_max:
      texte = blabla
      texte += "face_1 : {}\n".format(caract_face_1)
      texte += "face_2 : {}".format(caract_face_2)
      print (texte)

    erreur = 0
    face =  None

# 1. Forme de la face
    forme = caract_face_1[2][0]
    if self._verbose_max:
      print ("forme = {}".format(forme) )

# 2. Traitement selon la forme de la face
# 2.1. Face plane
    if forme in ( geompy.kind.DISK_CIRCLE, geompy.kind.DISK_ELLIPSE, geompy.kind.POLYGON, geompy.kind.PLANE, geompy.kind.PLANAR):
      erreur, face = self._cree_face_mediane_plane ( geompy, solide, caract_face_1, caract_face_2 )

# 2.2. Face cylindrique
    elif forme == geompy.kind.CYLINDER2D:
      erreur, face = self._cree_face_mediane_cylindre ( solide, caract_face_1, caract_face_2 )

# 2.3. Face sphérique
    elif forme == geompy.kind.SPHERE2D:
      erreur, face = self._cree_face_mediane_sphere ( caract_face_1, caract_face_2 )

# 2.4. Face torique
    elif forme == geompy.kind.TORUS2D:
      erreur, face = self._cree_face_mediane_tore ( caract_face_1, caract_face_2 )

# 2.5. Face conique
    elif forme == geompy.kind.CONE2D:
      erreur, face = self._cree_face_mediane_cone ( geompy, caract_face_1, caract_face_2 )

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
      self._cree_face_mediane_0 ( face )

    return erreur, face

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _cree_face_mediane_0 ( self, face ):
    """Gestion de la face médiane créée entre deux autres

Entrées :
  :face: la face médiane créée
    """

    nom_fonction = __name__ + "/_cree_face_mediane_0"
    blabla = "\nDans {} :\n".format(nom_fonction)

    if self._verbose_max:
      texte = blabla
      print (texte)

# 1. Nom de la face
    nom_face = self.nom_solide+"_M"
    #if ( self.nom_solide_aux != self.objet_principal.name() ):
      #nom_face += "S"
    face.setName(nom_face)
    face.result().setName(nom_face)

# 2. Mémorisation de la face et de la fonction initiale
    self.l_faces_m.append((face, self.fonction_0))

# 3. Couleur verte pour la face
    self._couleur_objet (face, coul_r=0, coul_g=170, coul_b=0)

# 4. Changement de statut pour le solide
    self.d_statut_so[self.nom_solide] = 1

    return

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _cree_face_mediane_plane ( self, geompy, solide, caract_face_1, caract_face_2 ):
    """Crée la face médiane entre deux autres - cas des surfaces planes

Entrées :
  :geompy: environnement de GEOM
  :solide: l'objet solide à traiter
  :caract_face_1, caract_face_2: les caractéristiques des 2 faces les plus grandes

Sorties :
  :face: la face médiane
    """

    nom_fonction = __name__ + "/_cree_face_mediane_plane"
    blabla = "\nDans {} :\n".format(nom_fonction)
    if self._verbose_max:
      print (blabla)

#   Caractéristiques des surfaces
    coo_x, coo_y, coo_z, vnor_x, vnor_y, vnor_z, taille, d_face_1_2 = self._cree_face_mediane_plane_0 ( geompy, solide, caract_face_1, caract_face_2 )

#   Contrôle de la validité de l'épaisseur
    erreur = self._verif_epaisseur ( d_face_1_2 )

#   Création de la face
    if not erreur:
      face = self._cree_face_mediane_plane_1 ( solide, coo_x, coo_y, coo_z, vnor_x, vnor_y, vnor_z, taille, d_face_1_2 )
    else:
      face = None

    return erreur, face

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _cree_face_mediane_plane_0 ( self, geompy, solide, caract_face_1, caract_face_2 ):
    """Crée la face médiane entre deux autres - cas des surfaces planes

Décodage des caractéristiques

Entrées :
  :geompy: environnement de GEOM
  :solide: l'objet solide à traiter
  :caract_face_1, caract_face_2: les caractéristiques des 2 faces les plus grandes

Sorties :
  :coo_x, coo_y, coo_z: coordonnées du centre de la base
  :vnor_x, vnor_y, vnor_z: coordonnées du vecteur normal
  :taille: estimation de la taille de la future face
  :d_face_1_2: la distance entre les deux faces
    """

    nom_fonction = __name__ + "/_cree_face_mediane_plane_0"
    blabla = "\nDans {} :\n".format(nom_fonction)

    if self._verbose_max:
      texte = blabla
      texte += "face_1 : {}\n".format(caract_face_1)
      texte += "face_2 : {}".format(caract_face_2)
      print (texte)

# 1. Caractéristiques de la base
#   Coordonnées du centre de la base
    coo_x = caract_face_1[2][1]
    coo_y = caract_face_1[2][2]
    coo_z = caract_face_1[2][3]
#   Coordonnées du vecteur normal
    vnor_x = caract_face_1[2][4]
    vnor_y = caract_face_1[2][5]
    vnor_z = caract_face_1[2][6]
#   taille : la diagonale de la boîte englobante permet d'être certain de tout prendre
    l_diag = self._calcul_boite_englobante ( solide )
    taille = 10.*l_diag
    if self._verbose_max:
      print ("Taille englobante : {}".format(taille))

# 2. Distance entre les deux faces
    face_1 = caract_face_1[0]
    face_2 = caract_face_2[0]
    d_face_1_2 = geompy.MinDistance(face_1, face_2)
    if self._verbose_max:
      print ("Distance entre les deux faces = {}".format(d_face_1_2))

    return coo_x, coo_y, coo_z, vnor_x, vnor_y, vnor_z, taille, d_face_1_2

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _cree_face_mediane_plane_1 ( self, solide, coo_x, coo_y, coo_z, vnor_x, vnor_y, vnor_z, taille, d_face_1_2 ):
    """Crée la face médiane entre deux autres - cas des surfaces planes

Création des objets temporaires et de la face médiane

Entrées :
  :solide: l'objet solide à traiter
  :coo_x, coo_y, coo_z: coordonnées du centre de la base
  :vnor_x, vnor_y, vnor_z: coordonnées du vecteur normal
  :taille: estimation de la taille de la future face
  :d_face_1_2: la distance entre les deux faces

Sorties :
  :face: la face médiane
    """

    nom_fonction = __name__ + "/_cree_face_mediane_plane_1"
    blabla = "\nDans {} :\n".format(nom_fonction)
    if self._verbose_max:
      texte = blabla
      texte += "Centre   : ({}, {}, {})\n".format(coo_x, coo_y, coo_z)
      texte += "Normale  : ({}, {}, {})\n".format(vnor_x, vnor_y, vnor_z)
      texte += "Taille   : {}\n".format(taille)
      texte += "Distance entre les deux faces : {}".format(d_face_1_2)
      print (texte)

#   Création de paramètres
    nom_par_1 = "{}_taille".format(self.nom_solide)
    model.addParameter(self.part_doc, "{}".format(nom_par_1), "{}".format(taille))

#   Création du point central
    centre = model.addPoint(self.part_doc, coo_x, coo_y, coo_z)
    nom_centre = "{}_centre".format(self.nom_solide)
    centre.result().setName(nom_centre)

#   Création du vecteur normal
    v_norm = model.addAxis(self.part_doc, vnor_x, vnor_y, vnor_z)
    nom_normal = "{}_normale".format(self.nom_solide)
    v_norm.result().setName(nom_normal)

#   Création du plan perpendiculaire au vecteur normal
    plan = model.addPlane(self.part_doc, model.selection("EDGE", nom_normal), model.selection("VERTEX", nom_centre), True)
    nom_plan = "{}_plan".format(self.nom_solide)
    plan.result().setName(nom_plan)

#   Création d'un sketch
    sketch = model.addSketch(self.part_doc, model.selection("FACE", nom_plan))

    SketchProjection_1 = sketch.addProjection(model.selection("VERTEX", nom_centre), False)
    SketchPoint_1 = SketchProjection_1.createdFeature()

    ### Create SketchLine
    SketchLine_1 = sketch.addLine(-taille/2., taille/2., taille/2., taille/2.)

    ### Create SketchLine
    SketchLine_2 = sketch.addLine(taille/2., taille/2., taille/2., -taille/2.)

    ### Create SketchLine
    SketchLine_3 = sketch.addLine(taille/2., -taille/2., -taille/2., -taille/2.)

    ### Create SketchLine
    SketchLine_4 = sketch.addLine(-taille/2., -taille/2., -taille/2., taille/2.)
    sketch.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
    sketch.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
    sketch.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
    sketch.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())

    ### Create SketchLine
    SketchLine_5 = sketch.addLine(-taille/2., taille/2., taille/2., -taille/2.)
    SketchLine_5.setAuxiliary(True)

    ### Create SketchLine
    SketchLine_6 = sketch.addLine(taille/2., taille/2., -taille/2., -taille/2.)
    SketchLine_6.setAuxiliary(True)
    sketch.setCoincident(SketchLine_1.startPoint(), SketchLine_5.startPoint())
    sketch.setCoincident(SketchLine_2.startPoint(), SketchLine_6.startPoint())
    sketch.setCoincident(SketchLine_3.startPoint(), SketchLine_5.endPoint())
    sketch.setCoincident(SketchLine_4.startPoint(), SketchLine_6.endPoint())
    sketch.setCoincident(SketchAPI_Point(SketchPoint_1).coordinates(), SketchLine_5.result())
    sketch.setCoincident(SketchAPI_Point(SketchPoint_1).coordinates(), SketchLine_6.result())
    sketch.setHorizontal(SketchLine_1.result())
    sketch.setVertical(SketchLine_2.result())
    sketch.setHorizontal(SketchLine_3.result())
    sketch.setVertical(SketchLine_4.result())
    sketch.setLength(SketchLine_3.result(), nom_par_1)
    sketch.setEqual(SketchLine_3.result(), SketchLine_4.result())

    model.do()
    nom_sketch = "{}_esquisse".format(self.nom_solide)
    sketch.setName(nom_sketch)
    sketch.result().setName(nom_sketch)

    ### Create LinearCopy
    LinearCopy_1 = model.addMultiTranslation(self.part_doc, [model.selection("SOLID", self.nom_solide_aux)], model.selection("EDGE", "PartSet/OX"), 0, 1, keepSubResults = True)
    LinearCopy_1.result().subResult(0).setName("{}_0".format(self.nom_solide_aux))

    ### Create Recover
    Recover_1 = model.addRecover(self.part_doc, LinearCopy_1, [solide])
    Recover_1.result().setName("{}_1".format(self.nom_solide_aux))

    # Création d'une face ; on la translate d'une demi-épaisseur.
    for iaux in range(2):

      distance = -0.5*d_face_1_2*float(2*iaux-1)
      nom_solide = "{}_{}".format(self.nom_solide_aux,iaux)
      face = self._cree_face_mediane_plane_2 ( nom_sketch, nom_normal, nom_solide, distance, iaux )

      if face.results():
        # Si on traite un objet solide unique, on le récupère
        if ( self.nom_solide_aux == self.objet_principal.name() ):
          if self._verbose_max:
            print ("On traite un objet solide unique ==> on le récupère.")
          Recover_2 = model.addRecover(self.part_doc, face, [Recover_1.result()])
          Recover_2.result().setName("{}_S".format(self.nom_solide_aux))
        nb_inter = face.result().numberOfSubs()
        if self._verbose_max:
          print ("Nombre d'intersections : {}".format(nb_inter))
        # Une seule intersection : c'est la bonne
        # Sinon, c'est que le solide est trop mince. On fusionnerait les faces.
        if (nb_inter > 1 ):
          face = self._cree_face_mediane_plane_3 ( face )
        break
      # Si l'intersection est vide, on la translate dans l'autre sens
      else:
        if self._verbose_max:
          print ("L'intersection est vide.")
        face = None

    return face

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _cree_face_mediane_plane_2 ( self, nom_sketch, nom_normal, nom_solide, distance, icpt=0 ):
    """Crée la face médiane entre deux autres - cas des surfaces planes

Intersection de la face avec le solide

Entrées :
  :nom_sketch: nom du sketch
  :nom_normal: nom du vecteur normal
  :nom_solide: nom du solide à intersecter
  :distance: la distance de translation
  :icpt: numéro de la tentative

Sorties :
  :face: la face médiane
    """

    nom_fonction = __name__ + "/_cree_face_mediane_plane_2"
    blabla = "\nDans {} :\n".format(nom_fonction)
    if self._verbose_max:
      texte = blabla
      texte += "nom_sketch   : {}\n".format(nom_sketch)
      texte += "nom_normal   : {}\n".format(nom_normal)
      texte += "nom_solide   : {}\n".format(nom_solide)
      texte += "distance : {}".format(distance)
      print (texte)

    # Création d'une face
    Face_1 = model.addFace(self.part_doc, [model.selection("COMPOUND", "all-in-{}".format(nom_sketch))])
    nom_face_1 = "{}_face_1_{}".format(self.nom_solide_aux,icpt)
    Face_1.result().setName(nom_face_1)

#   On la translate
    Translation_1 = model.addTranslation(self.part_doc, [model.selection("FACE", nom_face_1)], axis = model.selection("EDGE", nom_normal), distance = distance, keepSubResults = True)
    nom_trans = "{}_trans_{}".format(self.nom_solide_aux,icpt)
    Translation_1.setName(nom_trans)
    Translation_1.result().setName(nom_trans)
    Translation_1.result().setColor(85, 0, 255)

#   Intersection de cette face avec le solide initial
    face = model.addCommon(self.part_doc, [model.selection("SOLID", nom_solide), model.selection("FACE", nom_trans)], keepSubResults = True)

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
    blabla = "\nDans {} :\n".format(nom_fonction)
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

    return face_m

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _cree_face_mediane_cylindre ( self, solide, caract_face_1, caract_face_2 ):
    """Crée la face médiane entre deux autres - cas des cylindres

Entrées :
  :solide: solide SHAPER à traiter
  :caract_face_1, caract_face_2: les caractéristiques des 2 faces les plus grandes

Sorties :
  :face: la face médiane
    """

    nom_fonction = __name__ + "/_cree_face_mediane_cylindre"
    blabla = "\nDans {} :\n".format(nom_fonction)

#   Les deux faces
    if self._verbose_max:
      texte = blabla
      texte += "face_1 : {}\n".format(caract_face_1)
      texte += "face_2 : {}".format(caract_face_2)
      print (texte)

#   Caractéristiques des cylindres
    coo_x, coo_y, coo_z, axe_x, axe_y, axe_z, rayon, hauteur, epaisseur = self._cree_face_mediane_cylindre_0 ( solide, caract_face_1, caract_face_2 )

#   Contrôle de la validité de l'épaisseur
    erreur = self._verif_epaisseur ( epaisseur )

#   Création de la face
    if not erreur:
      face = self._cree_face_mediane_cylindre_1 ( coo_x, coo_y, coo_z, axe_x, axe_y, axe_z, rayon, hauteur )
    else:
      self._couleur_objet (solide, coul_r=0, coul_g=0, coul_b=255)
      face = None

    return erreur, face

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _cree_face_mediane_cylindre_0 ( self, solide, caract_face_1, caract_face_2 ):
    """Crée la face médiane entre deux autres - cas des cylindres

Décodage des caractéristiques

Entrées :
  :solide: l'objet solide à traiter
  :caract_face_1, caract_face_2: les caractéristiques des 2 faces les plus grandes

Sorties :
  :coo_x, coo_y, coo_z: coordonnées du centre de la base
  :axe_x, axe_y, axe_z: coordonnées de l'axe
  :rayon: rayon moyen entre les deux faces
  :hauteur: hauteur du cylindre
  :epaisseur: épaisseur de l'interface entre les deux faces
    """

    nom_fonction = __name__ + "/_cree_face_mediane_cylindre_0"
    blabla = "\nDans {} :\n".format(nom_fonction)

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
    l_diag = self._calcul_boite_englobante ( solide )
    hauteur = 10.*l_diag
    if self._verbose_max:
      print ("Hauteur englobante : {}".format(hauteur))
#   Epaisseur
    epaisseur = np.abs(caract_face_2[2][7]-caract_face_1[2][7])

    return coo_x, coo_y, coo_z, axe_x, axe_y, axe_z, rayon, hauteur, epaisseur

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _cree_face_mediane_cylindre_1 ( self, coo_x, coo_y, coo_z, axe_x, axe_y, axe_z, rayon, hauteur ):
    """Crée la face médiane entre deux autres - cas des cylindres

Création des objets temporaires et de la face externe du cylindre support

Entrées :
  :coo_x, coo_y, coo_z: coordonnées du centre de la base
  :axe_x, axe_y, axe_z: coordonnées de l'axe
  :rayon: rayon moyen entre les deux faces
  :hauteur: hauteur du cylindre

Sorties :
  :face: la face médiane
    """
    nom_fonction = __name__ + "/_cree_face_mediane_cylindre_1"
    blabla = "\nDans {} :\n".format(nom_fonction)

#   Les caractéristiques du cylindre à créer
    if self._verbose_max:
      texte = blabla
      texte += "Centre : ({}, {}, {})\n".format(coo_x, coo_y, coo_z)
      texte += "Axe    : ({}, {}, {})\n".format(axe_x, axe_y, axe_z)
      texte += "Rayon : {}\n".format(rayon)
      texte += "Hauteur : {}".format(hauteur)
      print (texte)

#   Création du point central
    centre = model.addPoint(self.part_doc, coo_x, coo_y, coo_z)
    nom_centre = "{}_centre".format(self.nom_solide)
    centre.result().setName(nom_centre)

#   Création de l'axe
    axe = model.addAxis(self.part_doc, axe_x, axe_y, axe_z)
    nom_axe = "{}_axe".format(self.nom_solide)
    axe.result().setName(nom_axe)

#   Création du plan perpendiculaire à l'axe
    plan = model.addPlane(self.part_doc, model.selection("EDGE", nom_axe), model.selection("VERTEX", nom_centre), True)
    nom_plan = "{}_plan".format(self.nom_solide)
    plan.result().setName(nom_plan)

#   Création d'un sketch
    nom_par_1 = "{}_R".format(self.nom_solide)
    model.addParameter(self.part_doc, "{}".format(nom_par_1), "{}".format(rayon))
    nom_par_2 = "{}_H".format(self.nom_solide)
    model.addParameter(self.part_doc, "{}".format(nom_par_2), "{}".format(hauteur))

    sketch = model.addSketch(self.part_doc, model.selection("FACE", nom_plan))

    SketchProjection_1 = sketch.addProjection(model.selection("VERTEX", nom_centre), False)
    SketchPoint_1 = SketchProjection_1.createdFeature()

    SketchCircle_1 = sketch.addCircle(0., 0., rayon)
    sketch.setCoincident(SketchPoint_1.result(), SketchCircle_1.center())
    sketch.setRadius(SketchCircle_1.results()[1], nom_par_1)
    model.do()
    nom_sketch = "{}_esquisse".format(self.nom_solide)
    sketch.setName(nom_sketch)
    sketch.result().setName(nom_sketch)

#   Création du cylindre complet
    cylindre = model.addExtrusion(self.part_doc, [model.selection("COMPOUND", "all-in-{}".format(nom_sketch))], model.selection(), nom_par_2, nom_par_2, "Edges")
    nom_cylindre = "{}_cylindre".format(self.nom_solide)
    cylindre.setName(nom_cylindre)
    cylindre.result().setName(nom_cylindre)
    cylindre.result().setColor(85, 0, 255)

#   Intersection de la face cylindrique avec le solide initial
    face = self._creation_face_inter ( nom_cylindre )

    return face

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _cree_face_mediane_sphere ( self, caract_face_1, caract_face_2 ):
    """Crée la face médiane entre deux autres - cas des sphères

Entrées :
  :caract_face_1, caract_face_2: les caractéristiques des 2 faces les plus grandes

Sorties :
  :face: la face médiane
    """

    nom_fonction = __name__ + "/_cree_face_mediane_sphere"
    blabla = "\nDans {} :\n".format(nom_fonction)

#   Les deux faces
    if self._verbose_max:
      texte = blabla
      texte += "face_1 : {}\n".format(caract_face_1)
      texte += "face_2 : {}".format(caract_face_2)
      print (texte)

#   Caractéristiques des sphères
    coo_x, coo_y, coo_z, rayon, epaisseur = self._cree_face_mediane_sphere_0 ( caract_face_1, caract_face_2 )

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

  def _cree_face_mediane_sphere_0 ( self, caract_face_1, caract_face_2 ):
    """Crée la face médiane entre deux autres - cas des sphères

Décodage des caractéristiques

Entrées :
  :caract_face_1, caract_face_2: les caractéristiques des 2 faces les plus grandes

Sorties :
  :coo_x, coo_y, coo_z: coordonnées du centre de la sphère
  :rayon: rayon moyen entre les deux faces
  :epaisseur: épaisseur de l'interface entre les deux faces
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
#   Epaisseur
    epaisseur = np.abs(caract_face_2[2][4]-caract_face_1[2][4])

    return coo_x, coo_y, coo_z, rayon, epaisseur

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _cree_face_mediane_sphere_1 ( self, coo_x, coo_y, coo_z, rayon ):
    """Crée la face médiane entre deux autres - cas des sphères

Création des objets temporaires et de la face externe de la sphère support

Entrées :
  :coo_x, coo_y, coo_z: coordonnées du centre de la sphère
  :rayon: rayon moyen entre les deux faces

Sorties :
  :face: la face externe de la sphère support
    """

    nom_fonction = __name__ + "/_cree_face_mediane_sphere_1"
    blabla = "\nDans {} :\n".format(nom_fonction)

#   Les caractéristiques de la sphère à créer
    if self._verbose_max:
      texte = blabla
      texte += "Centre : ({}, {}, {})\n".format(coo_x, coo_y, coo_z)
      texte += "Rayon : {}".format(rayon)
      print (texte)

#   Création du point central
    centre = model.addPoint(self.part_doc, coo_x, coo_y, coo_z)
    nom_centre = "{}_centre".format(self.nom_solide)
    centre.result().setName(nom_centre)

#   Création d'un plan passant par ce centre et cet axe
    plan = model.addPlane(self.part_doc, model.selection("EDGE", "PartSet/OX"), model.selection("VERTEX", nom_centre), False)
    nom_plan = "{}_plan".format(self.nom_solide)
    plan.result().setName(nom_plan)

#   Création d'un sketch
    nom_par_1 = "{}_R".format(self.nom_solide)
    model.addParameter(self.part_doc, "{}".format(nom_par_1), "{}".format(rayon))

    sketch = model.addSketch(self.part_doc, model.selection("FACE", nom_plan))

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
    sketch.setName(nom_sketch)
    sketch.result().setName(nom_sketch)

#   Création de la sphère complète
    sphere = model.addRevolution(self.part_doc, [model.selection("COMPOUND", nom_sketch)], model.selection("EDGE", "{}/{}".format(nom_sketch,nom_ligne)), 360, 0, "Edges")
    nom_sphere = "{}_sphere".format(self.nom_solide)
    sphere.setName(nom_sphere)
    sphere.result().setName(nom_sphere)
    sphere.result().setColor(85, 0, 255)

#   Intersection de la face sphérique avec le solide initial
    face = self._creation_face_inter ( nom_sphere )

    return face

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _cree_face_mediane_tore ( self, caract_face_1, caract_face_2 ):
    """Crée la face médiane entre deux autres - cas des tores

Entrées :
  :caract_face_1, caract_face_2: les caractéristiques des 2 faces les plus grandes

Sorties :
  :face: la face médiane
    """

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

  def _cree_face_mediane_tore_1 ( self, coo_x, coo_y, coo_z, axe_x, axe_y, axe_z, rayon_1, rayon_2 ):
    """Crée la face médiane entre deux autres - cas des tores

Création des objets temporaires et de la face externe du tore support

Entrées :
  :coo_x, coo_y, coo_z: coordonnées du centre du tore
  :axe_x, axe_y, axe_z: coordonnées de l'axe
  :rayon_1 : rayon principal
  :rayon_2 : rayon secondaire

Sorties :
  :face: la face externe du tore support
    """

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
    centre = model.addPoint(self.part_doc, coo_x, coo_y, coo_z)
    nom_centre = "{}_centre".format(self.nom_solide)
    centre.result().setName(nom_centre)

#   Création de l'axe
    axe = model.addAxis(self.part_doc, axe_x, axe_y, axe_z)
    nom_axe = "{}_axe".format(self.nom_solide)
    axe.result().setName(nom_axe)

#   Création d'un plan passant par ce centre et cet axe
    plan = model.addPlane(self.part_doc, model.selection("EDGE", nom_axe), model.selection("VERTEX", nom_centre), False)
    nom_plan = "{}_plan".format(self.nom_solide)
    plan.result().setName(nom_plan)

#   Création d'un sketch
    nom_par_1 = "{}_R_1".format(self.nom_solide)
    model.addParameter(self.part_doc, "{}".format(nom_par_1), "{}".format(rayon_1))
    nom_par_2 = "{}_R_2".format(self.nom_solide)
    model.addParameter(self.part_doc, "{}".format(nom_par_2), "{}".format(rayon_2))

    sketch = model.addSketch(self.part_doc, model.selection("FACE", nom_plan))

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
    sketch.setName(nom_sketch)
    sketch.result().setName(nom_sketch)

#   Création du tore complet
    nom_tore = "{}_tore".format(self.nom_solide)
    self._cree_revolution ( nom_sketch, nom_centre, coo_x, coo_y, coo_z, axe_x, axe_y, axe_z, nom_tore )

#   Intersection de la face torique avec le solide initial
    face = self._creation_face_inter ( nom_tore )

    return face

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _cree_face_mediane_cone ( self, geompy, caract_face_1, caract_face_2 ):
    """Crée la face médiane entre deux autres - cas des cones

Entrées :
  :geompy: environnement de GEOM
  :caract_face_1, caract_face_2: les caractéristiques des 2 faces les plus grandes

Sorties :
  :face: la face médiane
    """

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
#   Pour un cone complet, les caractéristiques fournies par GEOM sont correctes
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

  def _cree_face_mediane_cone_1 ( self, coo_x, coo_y, coo_z, axe_x, axe_y, axe_z, rayon_1, rayon_2, hauteur ):
    """Crée la face médiane entre deux autres - cas des cones

Création des objets temporaires et de la face externe du cone support

Entrées :
  :coo_x, coo_y, coo_z: coordonnées du centre de la base
  :axe_x, axe_y, axe_z: coordonnées de l'axe
  :rayon_1, rayon_2: rayons moyens du côté de la base et à l'opposé
  :hauteur: hauteur du cone

Sorties :
  :face: la face externe du cone support
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
    centre = model.addPoint(self.part_doc, coo_x, coo_y, coo_z)
    nom_centre = "{}_centre".format(self.nom_solide)
    centre.result().setName(nom_centre)

#   Création de l'axe
    axe = model.addAxis(self.part_doc, axe_x, axe_y, axe_z)
    nom_axe = "{}_axe".format(self.nom_solide)
    axe.result().setName(nom_axe)

#   Création d'un plan passant par ce centre et cet axe
    plan = model.addPlane(self.part_doc, model.selection("EDGE", nom_axe), model.selection("VERTEX", nom_centre), False)
    nom_plan = "{}_plan".format(self.nom_solide)
    plan.result().setName(nom_plan)

#   Création d'un sketch
    nom_par_1 = "{}_R_1".format(self.nom_solide)
    model.addParameter(self.part_doc, "{}".format(nom_par_1), "{}".format(rayon_1))
    nom_par_2 = "{}_R_2".format(self.nom_solide)
    model.addParameter(self.part_doc, "{}".format(nom_par_2), "{}".format(rayon_2))
    nom_par_3 = "{}_H".format(self.nom_solide)
    model.addParameter(self.part_doc, "{}".format(nom_par_3), "{}".format(hauteur))

    sketch = model.addSketch(self.part_doc, model.selection("FACE", nom_plan))

    SketchProjection_1 = sketch.addProjection(model.selection("VERTEX", nom_centre), False)
    SketchPoint_1 = SketchProjection_1.createdFeature()

    SketchProjection_2 = sketch.addProjection(model.selection("EDGE", nom_axe), False)
    SketchLine_1 = SketchProjection_2.createdFeature()

    SketchLine_2 = sketch.addLine(coo_x, coo_y, coo_x+rayon_1, coo_y+hauteur)
    SketchLine_2.setAuxiliary(True)
    sketch.setCoincident(SketchAPI_Point(SketchPoint_1).coordinates(), SketchLine_2.startPoint())
    sketch.setParallel(SketchLine_1.result(), SketchLine_2.result())
    sketch.setLength(SketchLine_2.result(), nom_par_3)

    SketchLine_3 = sketch.addLine(coo_x+rayon_1, coo_y, coo_x+rayon_1, coo_y+hauteur)
    sketch.setDistance(SketchLine_2.startPoint(), SketchLine_3.result(), nom_par_1, True)
    sketch.setDistance(SketchLine_2.endPoint(), SketchLine_3.result(), nom_par_2, True)
    sketch.setLength(SketchLine_3.result(), "2.5*{}".format(nom_par_3))

    SketchPoint_2 = sketch.addPoint(coo_x, coo_y)
    sketch.setCoincident(SketchPoint_2.coordinates(), SketchLine_3.result())
    sketch.setMiddlePoint(SketchPoint_2.coordinates(), SketchLine_3.result())

    SketchLine_4 = sketch.addLine(coo_x, coo_y, 1.2*coo_x, 1.2*coo_y)
    SketchLine_4.setAuxiliary(True)
    sketch.setCoincident(SketchAPI_Point(SketchPoint_1).coordinates(), SketchLine_4.startPoint())
    sketch.setCoincident(SketchPoint_2.coordinates(), SketchLine_4.endPoint())
    sketch.setHorizontal(SketchLine_4.result())

    model.do()
    nom_sketch = "{}_esquisse".format(self.nom_solide)
    sketch.setName(nom_sketch)
    sketch.result().setName(nom_sketch)

#   Création du cone complet
    nom_cone = "{}_cone".format(self.nom_solide)
    self._cree_revolution ( nom_sketch, nom_centre, coo_x, coo_y, coo_z, axe_x, axe_y, axe_z, nom_cone )

#   Intersection de la face conique avec le solide initial
    face = self._creation_face_inter ( nom_cone )

    return face

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _calcul_boite_englobante ( self, objet ):
    """Crée la hauteur englobant à coup sûr l'objet

Entrées :
  :objet: l'objet à traiter

Sorties :
  :l_diag: longueur de la diagonale de la boîte englobante
    """

    nom_fonction = __name__ + "/_calcul_boite_englobante"
    blabla = "\nDans {} :\n".format(nom_fonction)

    if self._verbose_max:
      texte = blabla[:-1]
      print (texte)

#   Hauteur : la diagonale de la boîte englobante permet d'être certain de tout prendre
    if self._verbose_max:
      texte = "Création de la boite englobante pour l'objet '{}' ".format(objet.name())
      texte += "de type '{}'".format(objet.shapeType())
      print (texte)
    bbox = model.getBoundingBox(self.part_doc, model.selection("{}".format(objet.shapeType()), "{}".format(objet.name())))

    bbox_nom = bbox.name()
    if self._verbose_max:
      print ("Boîte englobante : '{}' '{}'".format(bbox.name(), bbox.result().name()))

    if self._verbose_max:
      coo_min = model.getPointCoordinates(self.part_doc, \
      model.selection("VERTEX", "[{}_1/Back][{}_1/Left][{}_1/Bottom]".format(bbox_nom,bbox_nom,bbox_nom)))
      coo_max = model.getPointCoordinates(self.part_doc, \
      model.selection("VERTEX", "[{}_1/Front][{}_1/Right][{}_1/Top]".format(bbox_nom,bbox_nom,bbox_nom)))
      texte = "\tXmin = {}, Xmax = {}\n".format(coo_min[0],coo_max[0])
      texte += "\tYmin = {}, Ymax = {}\n".format(coo_min[1],coo_max[1])
      texte += "\tZmin = {}, Zmax = {}".format(coo_min[2],coo_max[2])
      print(texte)

    l_diag = model.measureDistance(self.part_doc, \
      model.selection("VERTEX", "[{}_1/Back][{}_1/Left][{}_1/Bottom]".format(bbox_nom,bbox_nom,bbox_nom)), \
      model.selection("VERTEX", "[{}_1/Front][{}_1/Right][{}_1/Top]".format(bbox_nom,bbox_nom,bbox_nom)) )
    if self._verbose_max:
      print ("Longueur de la diagonale : {}".format(l_diag))

    return l_diag

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _cree_revolution ( self, nom_sketch, nom_centre, coo_x, coo_y, coo_z, axe_x, axe_y, axe_z, nom_objet ):
    """Crée un volume de révolution

Entrées :
  :nom_sketch: nom du sketch à révolutionner
  :nom_centre: nom du point associé au centre du volume de révolution
  :coo_x, coo_y, coo_z: coordonnées du centre du tore
  :axe_x, axe_y, axe_z: coordonnées de l'axe
  :rayon_1 : rayon principal
  :rayon_2 : rayon secondaire
  :nom_objet: nom de l'objet 2D créé
    """

    nom_fonction = __name__ + "/_cree_revolution"
    blabla = "\nDans {} :\n".format(nom_fonction)

    if self._verbose_max:
      texte = blabla
      texte += "Centre : ({}, {}, {})\n".format(coo_x, coo_y, coo_z)
      texte += "Axe    : ({}, {}, {})\n".format(axe_x, axe_y, axe_z)
      print (texte)

#   Création d'un point décalé par rapport au point central
    point = model.addPoint(self.part_doc, coo_x+axe_x, coo_y+axe_y, coo_z+axe_z)
    nom_point = "{}_point".format(self.nom_solide)
    point.result().setName(nom_point)

#   Création de l'axe de la rotation
    axe_r = model.addAxis(self.part_doc, model.selection("VERTEX", nom_centre), model.selection("VERTEX", nom_point))
    nom_axe_r = "{}_axe_r".format(self.nom_solide)
    axe_r.result().setName(nom_axe_r)

#   Création de l'objet complet
    objet = model.addRevolution(self.part_doc, [model.selection("COMPOUND", nom_sketch)], model.selection("EDGE", nom_axe_r), 360, 0, "Edges")
    objet.setName(nom_objet)
    objet.result().setName(nom_objet)
    objet.result().setColor(85, 0, 255)

    return

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
    blabla = "\nDans {} :\n".format(nom_fonction)

    if self._verbose_max:
      print (blabla)

    face = model.addCommon(self.part_doc, [model.selection("SOLID", self.nom_solide_aux), model.selection("FACE", nom_objet)], keepSubResults = True)

    return face

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def face_mediane_solide (self, solide, geompy, objet_geom):
    """Calcul de la face médiane pour un solide

Entrées :
  :solide: solide SHAPER à traiter
  :geompy: environnement de GEOM
  :objet_geom: l'objet solide au format GEOM à traiter

Sorties :
  :erreur: code d'erreur
  :message: message d'erreur
    """

    nom_fonction = __name__ + "/face_mediane_solide"
    blabla = "\nDans {} :\n".format(nom_fonction)

    if self._verbose_max:
      print (blabla)
    if self._verbose:
      print ("Traitement du solide '{}'".format(solide.name()))

# 1. Préalables

    erreur = 0
    message = ""

    while not erreur :

# 2. Explosion du solide en faces

      erreur, message, l_faces = self._faces_du_solide ( geompy, objet_geom )
      if erreur:
        break

# 3. Calcul des caractéristiques géométriques des faces

      tb_caract = self._calcul_caract_faces ( geompy, l_faces )

# 4. Tri des faces en fonction de leurs caractéristiques géométriques

      erreur, message, caract_face_1, caract_face_2 = self._tri_faces ( tb_caract )
      if erreur:
        break

# 5. Création de la face médiane

      erreur, face = self._cree_face_mediane ( solide, geompy, caract_face_1, caract_face_2 )
      if erreur:
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

  def _traitement_objet (self, solide=None, objet_geom=None):
    """Traitement d'un objet

Entrées :
  :solide: solide SHAPER à traiter
  :objet_geom: l'objet GEOM équivalent

Sorties :
  :erreur: code d'erreur
  :message: message d'erreur
    """

    nom_fonction = __name__ + "/_traitement_objet"
    blabla = "\nDans {} :\n".format(nom_fonction)

    if self._verbose_max:
      print (blabla)

# 1. Préalables

    erreur = 0
    message = ""

    while not erreur :

# 2. L'aide

      if self.affiche_aide_globale :
        break

# 3. Les imports pour salomé
      geompy = geomBuilder.New()

# 4. En cas d'exportation step, répertoire de travail associé à l'éventuel fichier de départ
#    Attention à ne pas recréer le répertoire à chaque fois
      if self._export_step:

        if self._verbose_max:
          print ("Préparation de l'export STEP")

        if self.rep_step is None:
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
          print ("Les fichiers CAO des surfaces seront dans le répertoire {}".format(self.rep_step))

# 5. Calcul réel de la face médiane

      if solide is None:
        self.nom_solide = objet_geom.GetName()

      erreur, message = self.face_mediane_solide (solide, geompy, objet_geom)
      if erreur:
        break

      break

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
    blabla = "\nDans {} :\n".format(nom_fonction)

    if self._verbose_max:
      print (blabla)

    erreur = 0
    message = ""

    model.begin()

    while not erreur :

# 1. Définition de la pièce

      self.part_doc = model.activeDocument()

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
    blabla = "Dans {} :\n".format(nom_fonction)

    if self._verbose_max:
      print (blabla)

# 1. Acquisition de la liste des noms des sous-objets solides

    self.d_statut_so = dict()
    self.l_noms_so = list()
    self.l_faces_m = list()

    _ = self._nom_sous_objets (objet, True)
    if self._verbose_max:
      print ("Noms des sous-objets : {}".format(self.l_noms_so))

# 2. Les faces médianes

    erreur, message = self._surf_objet_shaper_0 ( objet )

# 3. Gestion des faces créées

    self._surf_objet_shaper_1 ( )

# 4. Futur message pour le résultat

    if ( self._export_step and not erreur ):
      message = "Les fichiers des CAO des surfaces sont dans le répertoire {}".format(self.rep_step)

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
    blabla = "Dans {} :\n".format(nom_fonction)

    if self._verbose_max:
      prefixe = ""
      for _ in range(n_recur):
        prefixe += "\t"
      texte = "\n{}{}".format(prefixe,blabla)
      texte += "{}n_recur = {}".format(prefixe,n_recur)
      print (texte)

    erreur = 0
    message = ""

    while not erreur :

# 1. Au premier passage, il faut récupérer la pièce et garder la référence au résultat principal

      if ( n_recur == 0 ):
        self.part_doc = model.activeDocument()
        objet_0 = objet.result()
        self.objet_principal = objet_0
      else:
        objet_0 = objet

# 2. On descend dans l'arborescence des sous-objets jusqu'à en trouver un qui n'en n'a pas

      nb_sub_results = objet_0.numberOfSubs()

      if self._verbose_max:
        texte = "Examen de l'objet '{}' ".format(objet_0.name())
        texte += "de type '{}'".format(objet_0.shapeType())
        texte += "\nobjet.result().numberOfSubs() : {}".format(nb_sub_results)
        print (texte)

      for n_sobj in range(nb_sub_results):

# 2.1. Exploration récursive de l'arborescence

        erreur, message = self._surf_objet_shaper_0 ( objet_0.subResult(n_sobj), n_recur+1 )
        if erreur:
          break

# 2.2. Cet objet n'a pas de sous-objets. Si c'est un solide, on le traite

      if ( objet_0.shapeType() == "SOLID" ):
        erreur, message = self.surf_solide_shaper ( objet_0 )
        if erreur:
          break

      if erreur:
        break

# 3. Futur message pour le résultat

      if self._export_step:
        message = "Les fichiers STEP des surfaces sont dans le répertoire {}".format(self.rep_step)

      break

    return erreur, message

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def _surf_objet_shaper_1 (self):
    """Gestion des surfaces médianes créées

Sorties :
  :erreur: code d'erreur
  :message: message d'erreur
    """

    nom_fonction = __name__ + "/_surf_objet_shaper_1"
    blabla = "Dans {} :\n".format(nom_fonction)

    if self._verbose_max:
      print (blabla)

# 1. Informations sur les faces à problème

    if self.faces_pb_nb:
      if ( self.faces_pb_nb == 1 ):
        texte = "1 face pose"
      else:
        texte = "{} faces posent".format(self.faces_pb_nb)
      print ("\n{} problème.\n{}".format(texte,self.faces_pb_msg))

# 2. Si plus d'une face a été créée
    if ( len(self.l_faces_m) > 1 ):

# 2.1. Partition du paquet de faces

      if self._verbose_max:
        print ("Partitionnnement des faces créées.")

      l_objets = list()
      for (face,_) in self.l_faces_m:
        l_objets.append(model.selection("COMPOUND", "all-in-{}".format(face.name())))

      Partition_1 = model.addPartition(self.part_doc, l_objets, keepSubResults = True)

      Partition_1.result().setName("{}_M".format(self.objet_principal.name()))
      for iaux, (face,_) in enumerate(self.l_faces_m):
        Partition_1.result().subResult(iaux).setName("{}".format(face.name()))
      self._couleur_objet (Partition_1, coul_r=0, coul_g=170, coul_b=0)

# 2.2. Récupération des faces individuelles

      if self._verbose_max:
        print ("Récupération des faces individuelles.")

      l_objets = list()
      for iaux, (face,_) in enumerate(self.l_faces_m):
        l_objets.append(face.result())

      Recover_1 = model.addRecover(self.part_doc, Partition_1, l_objets)
      for iaux, (face,_) in enumerate(self.l_faces_m):
        Recover_1.results()[iaux].setName("{}".format(face.name()))
        Recover_1.results()[iaux].setColor(0, 170, 0)

# 2.3. Mise en dossier

      if self._verbose_max:
        print ("Mise en dossier.")

      for (face,fonction_0) in self.l_faces_m:
        dossier = model.addFolder(self.part_doc, fonction_0, face)
        dossier.setName(face.name()[:-2])

    return

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def surf_solide_shaper (self, solide):
    """Calcule les surfaces médianes pour un solide SHAPER solide passé en argument

Entrées :
  :solide: solide SHAPER à traiter

Sorties :
  :erreur: code d'erreur
  :message: message d'erreur
    """

    nom_fonction = __name__ + "/surf_solide_shaper"
    blabla = "Dans {} :".format(nom_fonction)

    if self._verbose_max:
      print (blabla)

    erreur = 0
    message = ""

    while not erreur :

      self.nom_solide = solide.name()
      if self._verbose_max:
        print ("solide '{}'".format(self.nom_solide))

# 1. Isolement du solide
      solide_aux, recover = self._isole_solide ( solide )

# 2. Exportation dans un fichier step pour traitement dans GEOM

      fichier = tempfile.mkstemp(suffix=".stp")[1]
      if self._verbose_max:
        print ("fichier = {}".format(fichier))
        print ("solide  = {}".format(solide_aux.name()))
        print ("de type = {}".format(solide_aux.shapeType()))
      export = model.exportToFile(self.part_doc, fichier, [model.selection(solide_aux.shapeType(), solide_aux.name())])
      export.execute(True)
      model.do()

      taille = os.path.getsize(fichier)
      if ( taille <= 0 ):
        message = "Export de SHAPER vers GEOM impossible pour le solide '{}' de type '{}'\n".format(solide_aux.name(), solide_aux.shapeType())
        message += "Le fichier {} est de taille {}".format(fichier,taille)
        erreur = 2
        break

# 3. Importation dans GEOM
      geompy = geomBuilder.New()
      objet_geom = geompy.ImportSTEP(fichier, False, True)
      os.remove(fichier)

# 4. Traitement de l'objet correspondant
      erreur, message = self._traitement_objet ( solide=solide_aux, objet_geom=objet_geom )

      if ( erreur and self._verbose_max ):
        print (blabla, message)

# 5. Mise en forme de l'objet principal récupéré
      if ( recover is not None ):
        _ = self._nom_sous_objets (recover, False)

# 6. Neutralisation des erreurs dues à l'épaisseur
      if ( erreur in (-2,-1,2) ):
        erreur = 0
        message = ""

      break

    return erreur, message

#===========================  Fin de la méthode ==================================

#=========================== Début de la méthode =================================

  def surf_objet_geom (self, objet_geom):
    """Calcule la surface médiane pour un objet GEOM passé en argument

Entrées :
  :objet_geom: l'objet GEOM à traiter

Sorties :
  :erreur: code d'erreur
  :message: message d'erreur
    """

    nom_fonction = __name__ + "/surf_objet_geom"
    blabla = "\nDans {} :\n".format(nom_fonction)

    if self._verbose_max:
      print (blabla)

    erreur, message = self._traitement_objet ( objet_geom=objet_geom )

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
  L_OPTIONS.append("-vmax")
  #L_OPTIONS.append("-export_step")
  FIC_CAO = os.path.join(os.getenv("SHAPER_ROOT_DIR"), "bin", "salome", "macros", "midSurface", "midSurface.stp")
  #FIC_CAO = os.path.join(os.getenv("HOME"), "salome-dev", "DEV_package", "modules", "src", "SHAPER", "src", "PythonAddons", "macros", "midSurface", "Objet_1.stp")

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

  del SURFACE_MEDIANE

  #sys.exit(0)
