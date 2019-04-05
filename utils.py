# Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

import os, os.path

#SHAPER_SRC = os.environ['SHAPER_SRC']
SHAPER_SRC = os.environ['PWD']
# VERBOSE_FLAG = os.environ['VERBOSE_FLAG']
VERBOSE_FLAG = ""
ENCODING = "utf-8"
OPERATORS=[">", "<", "+", "=", "-", "*", "/"]

def create_warning_generator(fichier):
	"""
	fichier est le fichier de log cpplint ouvert à la position courante.
	Un trou dans le fichier résulte en l'arrêt de la génération
	"""
	ligne = "-"
	ligne = fichier.readline()
	i = 1
	while ligne.strip("\n") :
		ligne = ligne.decode(ENCODING)
		liste = ligne.split(":")
		print("Size of liste = ", len(liste))
		print("ligne = ", ligne)
		print("n° ligne = ", i)
		print("XXX : ", ligne.find('Done processing '))

		if len(liste) < 3:
			if ligne.find('Done processing ') == -1 and ligne.find('Category ') == -1 and ligne.find('Total errors found: ') == -1:
				raise Exception("Fichier de log mal formé")
			else:
				ligne = fichier.readline()
				i += 1
				continue
		elif len(liste) != 3:
			item1=liste[0]
			item2=liste[1]
			merge = ""
			sep = ""
			for item in liste[2:]:
				merge += sep
				merge += item
				sep = ":"
			liste = [item1, item2, merge]
		ligne = fichier.readline()
		i += 1
		yield liste[0], int(liste[1]), liste[2]

def get_src_path(path):
	return os.path.normpath(os.path.join(SHAPER_SRC, path))

def get_line_no(path, nol):
	"""retourne la ligne No nol du fichier path (relatif à DST_SRC_PARENT) sous forme d'un unicode"""
	ligne = ""
	fic = open(get_src_path(path), "r")
	for i in range(nol):
		ligne = fic .readline()
	fic.close()
	ligne_u = ligne.decode(ENCODING)
	return ligne_u

def fic_readlines(fic):
	tmp=fic.readlines()
	liste=[]
	for ligne in tmp:
		liste.append(ligne.decode(ENCODING))
	return liste

def fic_writelines(liste):
	liste2=[]
	for ligne in liste:
		liste2.append(ligne.encode(ENCODING))
	return liste2
