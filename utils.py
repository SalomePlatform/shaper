# coding: utf-8
import os, os.path

#SHAPER_SRC = os.environ['SHAPER_SRC']
SHAPER_SRC = os.environ['PWD']
# VERBOSE_FLAG = os.environ['VERBOSE_FLAG']
VERBOSE_FLAG = ""
ENCODING = u"utf-8"
OPERATORS=[u">", u"<", u"+", u"=", u"-", u"*", u"/"]

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
		print "Size of liste = ", len(liste)
		print "ligne = ", ligne
		print "n° ligne = ", i
		print "XXX : ", ligne.find(u'Done processing ')

		if len(liste) < 3:
			if ligne.find(u'Done processing ') == -1 and ligne.find(u'Category ') == -1 and ligne.find(u'Total errors found: ') == -1:
				raise Exception(u"Fichier de log mal formé")
			else:
				ligne = fichier.readline()
				i += 1
				continue
		elif len(liste) != 3:
			item1=liste[0]
			item2=liste[1]
			merge = u""
			sep = u""
			for item in liste[2:]:
				merge += sep
				merge += item
				sep = u":"
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
	for i in xrange(nol):
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
