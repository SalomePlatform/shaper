# coding: utf-8
import os, os.path, sys, re, argparse
from utils import *







#### définition des hooks.
# chaque hook doit :
# * avoir le prototype suivant : def <nom_hook>(<unicode>) avec <unicode> la ligne de texte à corriger
# * retourner deux valeurs : la ligne corrigée, en unicode et un booléen indiquant s'il faut faire un check humain avant de corriger
# * être inséré dans HOOKS_DICT en y faisant correspondre un dico explicitant les critères de séléction de ce hook
####

### whitespace/semicolon ###

def extra_space_before_last_semicolon(ligne, previous_ligne = None, warning=None):
	liste=ligne.split(u';\n')
	ligne2=liste[0]
	ligne3=ligne2.rstrip()
	ligne=ligne3 + u';\n' 
	return ligne, previous_ligne, False

def missing_space_after_semicolon(ligne, previous_ligne = None, warning=None):
	ligne=ligne.replace(u';',u'; ')
	return ligne, previous_ligne, False
	
#########################

### whitespace/operators ###

def missing_space_around_operator_egal(ligne, previous_ligne = None, warning=None):
	i=ligne.find(u'=')
	nextchar=ligne[i+1]
	previouschar=ligne[i-1]
	if nextchar not in OPERATORS and previouschar not in OPERATORS:
		if nextchar.isspace() == False and previouschar.isspace()==True:
			ligne=ligne.replace(u'=', u'= ')
		elif nextchar.isspace() == True and previouschar.isspace()==False:
			ligne=ligne.replace(u'=', u' =')
		elif nextchar.isspace() == False and previouschar.isspace()==False:
			ligne=ligne.replace(u'=', u' = ')
		else:
			pass
	return ligne, previous_ligne, False	

def extra_space_for_operator_add(ligne, previous_ligne = None, warning=None):			
	while ligne.find(u'++ ') > -1:
		ligne = ligne.replace(u'++ ',u'++')
	while ligne.find(u' ++') > -1 and ligne.find(u'; ++') == -1:
		ligne = ligne.replace(u' ++',u'++')
	return ligne, previous_ligne, False
	
def extra_space_for_operator_diff(ligne, previous_ligne = None, warning=None):			
	while ligne.find(u'! ') > -1:
		ligne = ligne.replace(u'! ',u'!')
	return ligne, previous_ligne, False

def missing_space_around_operator_double_chevron(ligne, previous_ligne = None, warning=None):
	i=ligne.find(u'<<')
	nextchar=ligne[i+1]
	previouschar=ligne[i-1]
	if nextchar.isspace() == False and previouschar.isspace()==True:
		ligne=ligne.replace(u'<<', u'<< ')
	elif nextchar.isspace() == True and previouschar.isspace()==False:
		ligne=ligne.replace(u'<<', u' <<')
	elif nextchar.isspace() == False and previouschar.isspace()==False:
		ligne=ligne.replace(u'<<', u' << ')
	else:
		pass
	return ligne, previous_ligne, False
	
def missing_space_around_operator_simple_chevron(ligne, previous_ligne = None, warning=None):
	i=ligne.find(u'<')
	nextchar=ligne[i+1]
	previouschar=ligne[i-1]
	if nextchar not in OPERATORS and previouschar not in OPERATORS: 
		if nextchar.isspace() == False and previouschar.isspace()==True:
			ligne=ligne.replace(u'<', u'< ')
		elif nextchar.isspace() == True and previouschar.isspace()==False:
			ligne=ligne.replace(u'<', u' <')
		elif nextchar.isspace() == False and previouschar.isspace()==False:
			ligne=ligne.replace(u'<', u' < ')
		else:
			pass
	return ligne, previous_ligne, False
	

def missing_space_around_operator_diff_egal(ligne, previous_ligne = None, warning=None):
	i=ligne.find(u'!=')
	nextchar=ligne[i+1]
	previouschar=ligne[i-1]
	if nextchar.isspace() == False and previouschar.isspace()==True:
		ligne=ligne.replace(u'!=', u'!= ')
	elif nextchar.isspace() == True and previouschar.isspace()==False:
		ligne=ligne.replace(u'!=', u' !=')
	elif nextchar.isspace() == False and previouschar.isspace()==False:
		ligne=ligne.replace(u'!=', u' != ')
	else:
		pass
	return ligne, previous_ligne, False
	
def missing_space_around_operator_double_egal(ligne, previous_ligne = None, warning=None):
	i=ligne.find(u'==')
	nextchar=ligne[i+1]
	previouschar=ligne[i-1]
	if nextchar.isspace() == False and previouschar.isspace()==True:
		ligne=ligne.replace(u'==', u'== ')
	elif nextchar.isspace() == True and previouschar.isspace()==False:
		ligne=ligne.replace(u'==', u' ==')
	elif nextchar.isspace() == False and previouschar.isspace()==False:
		ligne=ligne.replace(u'==', u' == ')
	else:
		pass
	return ligne, previous_ligne, False

	
#########################

### whitespace/comments ###

def space_between_comments_and_code(ligne, previous_ligne = None, warning=None):
	if ligne.find(u'//')>=0 :			
		ligne=ligne.replace(u'//', u'  //')
	elif ligne.find(u' //')>=0 :			
		ligne=ligne.replace(u' //', u'  //')
	else:
	    pass	
	return ligne, previous_ligne, False

def space_between_comments_and_double_slash(ligne, previous_ligne = None, warning=None):			
	return ligne.replace(u'//', u'// '), previous_ligne, False
	
#########################


### legal/copyright ###

def legal_copyright(ficpath, ficline, warning=None):
	replace_fic_copyright(ficpath)

#########################

### build/header_guard

def ifndef_header(ficpath, ficline, warning):
	replace_fic_ifndef(ficpath, ficline, warning, "wrong_style")
		
#def no_ifndef_header(ficpath, ficline, warning):
#	replace_fic_ifndef(ficpath, ficline, warning, "no_guard_found")
		
def endif_line(ligne, previous_ligne = None, warning=None):
	liste1=warning.split(u'// ')
	ligne1=liste1[1]
	liste2=ligne1.split(u'"')
	header_guard=liste2[0]
	ligne= u'#endif  // ' + header_guard +u'\n'		
	return ligne, previous_ligne, False

#########################

### whitespace/end_of_ligne ###

def whitespace_end_of_line(ligne, previous_ligne = None, warning=None):
	ligne = ligne.rstrip()+'\n'

	return ligne, previous_ligne, False

#########################

### whitespace/comma ###

def missing_space_after_comma(ligne, previous_ligne = None, warning=None):
	ligne=ligne.replace(u',',u', ')
	return ligne, previous_ligne, False

#########################

### readability/namespace ###

def namespace_should_be_terminated(ligne, previous_ligne = None, warning=None):
	ligne2=ligne.rstrip(u'\n')
	liste = warning.split(u'"')
	namespace=liste[1]
	if ligne.find(u'\r')>=0:
		ligne=ligne2+u'  '+namespace+u'\r\n'
	else:
		ligne=ligne2+u'  '+namespace+u'\n'
	return ligne, previous_ligne, False

def anonymous_namespace_should_be_terminated(ligne, previous_ligne = None, warning=None):
	ligne2=ligne.rstrip(u'\n')
	ligne=ligne2+u'  // namespace\n'
	return ligne, previous_ligne, False
	
#########################

### whitespace/parens ###

def extra_space_after_opening_paranthesis(ligne, previous_ligne = None, warning=None):
	while ligne.find(u'( ') > -1:
		ligne = ligne.replace(u'( ',u'(')
	return ligne, previous_ligne, False

def extra_space_before_closing_paranthesis(ligne, previous_ligne = None, warning=None):
	while ligne.find(u' )') > -1:
		ligne = ligne.replace(u' )',u')')
	return ligne, previous_ligne, False
	
def extra_space_before_opening_paranthesis_in_function_call(ligne, previous_ligne = None, warning=None):
	while ligne.find(u' (') > -1 and ligne.find(u', (')==-1:
		ligne = ligne.replace(u' (',u'(')
	return ligne, previous_ligne, False

def missing_space_before_opening_parenthesis_in_for(ligne, previous_ligne = None, warning=None):
	return ligne.replace(u'for(',u'for ('), previous_ligne, False

def missing_space_before_opening_parenthesis_in_if(ligne, previous_ligne = None, warning=None):
	return ligne.replace(u'if(',u'if ('), previous_ligne, False

def missing_space_before_opening_parenthesis_in_switch(ligne, previous_ligne = None, warning=None):
	return ligne.replace(u'switch(',u'switch ('), previous_ligne, False

def missing_space_before_opening_parenthesis_in_while(ligne, previous_ligne = None, warning=None):
	return ligne.replace(u'while(',u'while ('), previous_ligne, False

def mismatching_spaces_inside_paranthesis(ligne, previous_ligne = None, warning=None):
	while ligne.find(u'( ') > -1:
		ligne = ligne.replace(u'( ',u'(')
	while ligne.find(u' )') > -1:
		ligne = ligne.replace(u' )',u')')
	return ligne, previous_ligne, False
	
#########################

### whitespace/newline ###

def else_should_be_previous_line(ligne, previous_ligne=None, warning=None):
	return position_of_else(ligne, previous_ligne, warning)

#########################

### whitespace/indent ###

def missing_space_before_public_protected_private(ligne, previous_ligne = None, warning=None):
	# Ajout d'un espace avant public:, protected: et :private
	if re.match("public.*:|protected.*:|private.*:", ligne):
		ligne = " " + ligne

	return ligne, previous_ligne, False

def wrong_number_of_spaces_while_indenting(ligne, previous_ligne = None, warning=None):
	# Le nombre d'espaces pour l'indentation doit être pair
	if re.match("\s{1}\S+|\s{3}\S+|\s{5}\S+|\s{7}\S+|\s{9}\S+", ligne):
		if not re.match(" public.*:| protected.*:| private.*:| \*", ligne):
			ligne = " " + ligne
	# Suppression des espaces superflus sur une ligne sans code
	if ligne.isspace():
		ligne = "\n"

	# Remplacement des tabulations par 8 espaces en début de ligne
	ligne = ligne.replace("\t", "        ")

	return ligne, previous_ligne, False
#########################

### whitespace/ending_newline ###

def no_newline_at_the_end_of_file(ligne, previous_ligne = None, warning=None):
	ligne += '\n\n'
	return ligne, None, False

#################################

### build/storage_class ###

def order_of_storage_class_specifier(ligne, previous_ligne = None, warning=None):
	if re.match("\s*const static", ligne):
		# Inversion de const static en static const
		ligne = ligne.replace("const static", "static const")

	return ligne, previous_ligne, False

###########################

### whitespace/tab ###

def use_spaces_instead_of_tab(ligne, previous_ligne = None, warning=None):
	if re.match(".*\t", ligne):
		# Remplacement des tabulations par 8 espaces
		ligne = ligne.replace("\t", "        ")

	return ligne, previous_ligne, False

######################
	
### readability/braces ###

def no_semicolon_after_brace(ligne, previous_ligne = None, warning=None):
	return ligne.replace(u'};',u'}'), previous_ligne, False

def position_of_else(ligne, previous_ligne=None, warning=None):
	'''Remonte une ligne du type } else {'''
	
	if '}' not in previous_ligne:
		# Si on ne trouve pas de '}' sur la ligne précédente, on ne fait rien
		return ligne, None, False

	m = re.match("(.*)else(.*)", ligne)
	
	previous_ligne = previous_ligne.rstrip("\n") + ' else' + m.group(2) + '\n'
	ligne = ''
	return ligne, previous_ligne, False

##########################

### whitespace/braces ###

def position_of_opening_brace(ligne, previous_ligne=None, warning=None):
	'''Remonte le { sur la ligne du dessus'''
	
	m = re.match("(\s*){(.*)", ligne)
	
	# On ne fait rien si on trouve un commentaire sur la ligne précédente (ce sera une correction manuelle)
	if previous_ligne.find(u'//') == -1:
		previous_ligne = previous_ligne.rstrip("\n") + ' {' + m.group(2) + '\n'
		ligne = ''
	else:
		print "The previous line contains a comment, fixing has to be manual."

	return ligne, previous_ligne, False

def missing_space_before_opening_brace(ligne, previous_ligne = None, warning=None):
	m = re.match("(.+)(\S){(.*)", ligne)
	if(m):
		print 'group_0', m.group(0)
		print 'group_1', m.group(1)
		print 'group_2', m.group(2)
		print 'group_3', m.group(3)
		ligne = m.group(1) + m.group(2) + ' {' + m.group(3) + '\n'

	return ligne, previous_ligne, False


#########################	


def missing_space_before_else(ligne, previous_ligne = None, warning=None):
	m = re.match("(.+)(\S)else(.*)", ligne)
	if(m):
		print 'group_0', m.group(0)
		print 'group_1', m.group(1)
		print 'group_2', m.group(2)
		print 'group_3', m.group(3)
		ligne = m.group(1) + m.group(2) + ' else' + m.group(3) + '\n'

	return ligne, previous_ligne, False

### runtime/references ###

def make_const_reference(ficpath, ligne, previous_ligne = None, warning=None):
	""" Adding keyword 'const' """
	print "ficpath = ", ficpath
	print "ligne = ", ligne
	print "warning = ", warning
	m = re.match("(.+)Is this a non-const reference\? If so, make const or use a pointer: (.+)  (.+)", warning)
	if(m):
		print 'group_0', m.group(0)
		print 'group_1', m.group(1)
		print 'group_2', m.group(2)
		print 'group_3', m.group(3)
		arg_to_modify = m.group(2)
		ligne = ligne.replace(arg_to_modify, "const "+arg_to_modify)
		
		# Répercution des corrections dans le fichier ".cpp" correspondant si c'est un fichier ".h"
		if ficpath.find('.h') > -1:
			cpp_file_path = ficpath.replace('.h', '.cpp')
			make_const_reference_cpp_file(cpp_file_path, arg_to_modify)
			global H_FILE_MAKE_CONST_REFERENCE_MODIFIED
			H_FILE_MAKE_CONST_REFERENCE_MODIFIED = True
	else:
		print "ERROR : The following pattern was not found : 'Is this a non-const reference? If so, make const or use a pointer:'"

	return ligne, previous_ligne, False

def make_const_reference_cpp_file(ficpath, arg_to_modify):
	if not os.path.isfile(ficpath):
		print "WARNING : The file ", ficpath, " doesn't exist, manual fixing is required in methods of the file ", ficpath.replace('.cpp', '.h'), " in which arguments have been declared 'const'"
		return

	fic = open(get_src_path(ficpath),'r')
	liste = fic_readlines(fic)
	new_liste = []

	for ligne in liste:
		# Recherche de l'argument à modifier
		if(ligne.find(arg_to_modify) > -1 and ligne.find('const '+arg_to_modify) == -1):
			new_liste.append(ligne.replace(arg_to_modify, "const "+arg_to_modify))
		else:
			new_liste.append(ligne)
		
	newliste=fic_writelines(new_liste)
	fichier = open(get_src_path(ficpath), "w")
	fichier.writelines(newliste)




##########################

### runtime/int ###

def replace_short_by_int16(ligne, previous_ligne = None, warning=None):
	ligne = ligne.replace(u'short', u'int16_t')

	return ligne, None, False

def replace_long_by_int64(ligne, previous_ligne = None, warning=None):
	ligne = ligne.replace(u'long', u'int64_t')

	return ligne, None, False

###################

### runtime/explicit ###

def make_constructor_explicit(ligne, previous_ligne = None, warning=None):
	m = re.match("(\s*)(.+)", ligne)
	if(m):
		print 'group_0', m.group(0)
		print 'group_1', m.group(1)
		print 'group_2', m.group(2)
		ligne = ligne.replace(m.group(2), u'explicit '+m.group(2))

	return ligne, None, False

########################

### build/include ###

def cpp_file_should_include_h_file(ficpath, ficline, warning):
	fic = open(get_src_path(ficpath),'r')
	liste = fic_readlines(fic)
	new_liste = []

	m = re.match("(.+) should include its header file (.+)  (.+)", warning)
	if(m):
		print 'group_0', m.group(0)
		print 'group_1', m.group(1)
		print 'group_2', m.group(2)
		# Nouveau chemin du fichier .h
		new_h_file_path = m.group(2)

		# Nom du fichier .h
		h_file_name = os.path.basename(new_h_file_path)

		# Recherche de la ligne à modifier
		for ligne in liste:
			m2 = re.match("#include.*"+h_file_name+".*", ligne)
			if(m2):
				print "FOUND : ", ligne
				new_liste.append("#include \""+new_h_file_path+"\"\n")
			else:
				print "NOT FOUND : ", ligne
				new_liste.append(ligne)
	else:
		print "ERROR : Pattern not found : \"should include its header file\""

	newliste=fic_writelines(new_liste)
	fichier = open(get_src_path(ficpath), "w")
	fichier.writelines(newliste)
	
def _h_file_already_included(ligne, previous_ligne = None, warning=None):
	return "", None, False
		

#####################

### whitespace/blank_line ###

def do_not_leave_blank_line_after_public_protected_private(ficpath, ficline, warning):
	fic = open(get_src_path(ficpath),'r')
	liste = fic_readlines(fic)
	new_liste = []

	flag = False
	for ligne in liste:
		if re.match(".*public.*:|.*protected.*:|.*private.*:", ligne):
			# Détection d'une ligne public:, protected: ou :private
			flag = True
		if flag and ligne.isspace():
			# Supprimer les lignes vides après public:, protected: et :private
			print "Deleting empty line"
			new_liste.append("")
			continue
		if not ligne.isspace() and not re.match(".*public.*:|.*protected.*:|.*private.*:", ligne):
			flag = False
		new_liste.append(ligne)

	newliste=fic_writelines(new_liste)
	fichier = open(get_src_path(ficpath), "w")
	fichier.writelines(newliste)

def do_not_leave_blank_line_at_the_start_of_code_block(ficpath, ficline, warning):
	fic = open(get_src_path(ficpath),'r')
	liste = fic_readlines(fic)
	new_liste = []

	flag = False
	for ligne in liste:
		if re.match(".*{", ligne):
			# Détection d'un début de bloc
			flag = True
		if flag and ligne.isspace():
			# Supprimer les lignes vides après un début de bloc
			print "Deleting empty line"
			new_liste.append("")
			continue
		if not ligne.isspace() and not re.match(".*{", ligne):
			flag = False
		new_liste.append(ligne)

	newliste=fic_writelines(new_liste)
	fichier = open(get_src_path(ficpath), "w")
	fichier.writelines(newliste)

def do_not_leave_blank_line_at_the_end_of_code_block(ficpath, ficline, warning):
	fic = open(get_src_path(ficpath),'r')
	liste = fic_readlines(fic)
	new_liste = []

	nb_blank_lines = 0
	for ligne in liste:
		if re.match(".*}", ligne):
			# Détection d'une fin de bloc -> suppression des nb_blank_lines lignes vides précédentes
			for i in range(0, nb_blank_lines):
				new_liste.pop()
		if ligne.isspace():
			nb_blank_lines += 1
		else:
			nb_blank_lines = 0
		new_liste.append(ligne)

	newliste=fic_writelines(new_liste)
	fichier = open(get_src_path(ficpath), "w")
	fichier.writelines(newliste)

def add_blank_line_before_public(ligne, previous_ligne = None, warning=None):
	ligne = "\n" + ligne

	return ligne, None, False

def add_blank_line_before_protected(ligne, previous_ligne = None, warning=None):
	ligne = "\n" + ligne

	return ligne, None, False

def add_blank_line_before_private(ligne, previous_ligne = None, warning=None):
	ligne = "\n" + ligne

	return ligne, None, False

##############################

### build/include_what_you_use ###

def add_include_what_you_use(ficpath, ficline, warning):
	"""
	Ajoute le #include suggéré dans le warning
	"""

	fic = open(get_src_path(ficpath), "r")
	liste = fic_readlines(fic)
	
	m = re.match("\s*Add (.+) for (.+)", warning)
	if(m):
		print 'group_0', m.group(0)
		print 'group_1', m.group(1)
		include = m.group(1)

		# Recherche la ligne dans laquelle ajouter le #include
		# On l'ajoutera après le dernier "#include <..."
		num_ligne = 0
		num_ligne_include_system = 0
		num_ligne_include_local = 0
		for ligne in liste:
			num_ligne += 1
			if ligne.find('#include <') > -1:
				num_ligne_include_system = num_ligne
			elif ligne.find('#include "') > -1:
				num_ligne_include_local = num_ligne
		
		num_ligne_include = max(num_ligne_include_system, num_ligne_include_local)
		if num_ligne_include == 0:
			print "WARNING : #include not found in file ", ficpath
			return

		new_liste = []
		num_ligne = 0
		fic2 = open(get_src_path(ficpath), "r")
		liste2 = fic_readlines(fic2)
		for ligne in liste2:
			num_ligne += 1
			new_liste.append(ligne)
			if num_ligne == num_ligne_include:
				new_liste.append(include+'\n')
		
		newliste=fic_writelines(new_liste)
		fichier = open(get_src_path(ficpath), "w")
		fichier.writelines(newliste)
	else:
		print "ERROR : Pattern of include_what_you_use not found"
		return

##################################


HOOKS_DICT = {
        extra_space_before_last_semicolon:{'pattern':u'whitespace/semicolon', 'pattern_AND':u'Extra space before last semicolon'},
        missing_space_after_semicolon:{'pattern':u'whitespace/semicolon', 'pattern_AND':u'Missing space after'},
        missing_space_around_operator_egal:{'pattern':u'whitespace/operators', 'pattern_AND':u'Missing spaces around = '},
        extra_space_for_operator_add:{'pattern':u'whitespace/operators', 'pattern_AND':u'Extra space for operator  ++'},
        extra_space_for_operator_diff:{'pattern':u'whitespace/operators', 'pattern_AND':u'Extra space for operator !'},
        missing_space_around_operator_double_chevron:{'pattern':u'whitespace/operators', 'pattern_AND':u'Missing spaces around << '},
        missing_space_around_operator_simple_chevron:{'pattern':u'whitespace/operators', 'pattern_AND':u'Missing spaces around < '},
        missing_space_around_operator_diff_egal:{'pattern':u'whitespace/operators', 'pattern_AND':u'Missing spaces around !='},
        missing_space_around_operator_double_egal:{'pattern':u'whitespace/operators', 'pattern_AND':u'Missing spaces around =='},
        space_between_comments_and_code:{'pattern':u'whitespace/comments', 'pattern_AND':u'At least two spaces is best between code and comments'},
        space_between_comments_and_double_slash:{'pattern':u'whitespace/comments', 'pattern_AND':u'Should have a space between // and comment '},
        legal_copyright:{'pattern':u'legal/copyright'}, # Script à n'executer qu'une fois
        ifndef_header:{'pattern':u'build/header_guard', 'pattern_AND':u'#ifndef header guard has wrong style, please use'}, 
#        no_ifndef_header:{'pattern':u'build/header_guard', 'pattern_AND':u'No #ifndef header guard found'}, 
        endif_line:{'pattern':u'build/header_guard', 'pattern_AND':u'#endif line should be'},
        whitespace_end_of_line:{'pattern':u'whitespace/end_of_line', 'pattern_AND':u'Line ends in whitespace'},
        missing_space_after_comma:{'pattern':u'whitespace/comma'},
        namespace_should_be_terminated:{'pattern':u'readability/namespace', 'pattern_AND':u'Namespace should be terminated with'},
        anonymous_namespace_should_be_terminated:{'pattern':u'readability/namespace', 'pattern_AND':u'Anonymous namespace should be terminated with'},
		extra_space_after_opening_paranthesis:{'pattern':u'whitespace/parens', 'pattern_AND':u'Extra space after ('},
		extra_space_before_closing_paranthesis:{'pattern':u'whitespace/parens', 'pattern_AND':u'Extra space before )'},
		extra_space_before_opening_paranthesis_in_function_call:{'pattern':u'whitespace/parens', 'pattern':u'Extra space before ( in function call'},
		missing_space_before_opening_parenthesis_in_for:{'pattern':u'whitespace/parens', 'pattern_AND':u'Missing space before ( in for('},
		missing_space_before_opening_parenthesis_in_if:{'pattern':u'whitespace/parens', 'pattern_AND':u'Missing space before ( in if('},
		missing_space_before_opening_parenthesis_in_switch:{'pattern':u'whitespace/parens', 'pattern_AND':u'Missing space before ( in switch('},
		missing_space_before_opening_parenthesis_in_while:{'pattern':u'whitespace/parens', 'pattern_AND':u'Missing space before ( in while('},
		mismatching_spaces_inside_paranthesis:{'pattern':u'whitespace/parens', 'pattern_AND':u'Mismatching spaces inside ()'},
		missing_space_before_public_protected_private:{'pattern':u'whitespace/indent', 'pattern_AND':u' should be indented +1 space inside class'},
		
		wrong_number_of_spaces_while_indenting:{'pattern':u'whitespace/indent', 'pattern_AND':u'Weird number of spaces at line-start'},

		no_newline_at_the_end_of_file:{'pattern':u'whitespace/ending_newline'},

		order_of_storage_class_specifier:{'pattern':u'build/storage_class'},

		use_spaces_instead_of_tab:{'pattern':u'whitespace/tab'},

		no_semicolon_after_brace:{'pattern':u'readability/braces', 'pattern_AND':u'You don\'t need a ; after a }'},
#		position_of_else:{'pattern':u'readability/braces', 'pattern_AND':u'If an else has a brace on one side'},
#		else_should_be_previous_line:{'pattern':u'whitespace/newline', 'pattern_AND':u'An else should appear on the same line as the preceding'},
		position_of_opening_brace:{'pattern':u'whitespace/braces', 'pattern_AND':u'{ should almost always be at the end of the previous line'},
		missing_space_before_opening_brace:{'pattern':u'whitespace/braces', 'pattern_AND':u'Missing space before {'},
		missing_space_before_else:{'pattern':u'whitespace/braces', 'pattern_AND':u'Missing space before else'},
		
		make_const_reference:{'pattern':u'runtime/references', 'pattern_AND':u'Is this a non-const reference? If so, make const or use a pointer'},

		make_constructor_explicit:{'pattern':u'runtime/explicit'},

		cpp_file_should_include_h_file:{'pattern':u'build/include', 'pattern_AND':u'should include its header file'},
		_h_file_already_included:{'pattern':u'build/include', 'pattern_AND':u'already included at'},

		replace_short_by_int16:{'pattern':u'runtime/int', 'pattern_AND':u'Use int16/int64/etc, rather than the C type short'},
		replace_long_by_int64:{'pattern':u'runtime/int', 'pattern_AND':u'Use int16/int64/etc, rather than the C type long'},

		do_not_leave_blank_line_after_public_protected_private:{'pattern':u'whitespace/blank_line', 'pattern_AND':u'Do not leave a blank line after'},
		do_not_leave_blank_line_at_the_start_of_code_block:{'pattern':u'whitespace/blank_line', 'pattern_AND':u'Redundant blank line at the start of a code block should be deleted'},
		do_not_leave_blank_line_at_the_end_of_code_block:{'pattern':u'whitespace/blank_line', 'pattern_AND':u'Redundant blank line at the end of a code block should be deleted'},
		add_blank_line_before_public:{'pattern':u'whitespace/blank_line', 'pattern_AND':u'"public:" should be preceded by a blank line'},
		add_blank_line_before_protected:{'pattern':u'whitespace/blank_line', 'pattern_AND':u'"protected:" should be preceded by a blank line'},
		add_blank_line_before_private:{'pattern':u'whitespace/blank_line', 'pattern_AND':u'"private:" should be preceded by a blank line'},

		add_include_what_you_use:{'pattern':u'build/include_what_you_use'},

}

HOOKS_DELETING_OR_ADDING_LINES = [no_newline_at_the_end_of_file, position_of_opening_brace, _h_file_already_included, 
add_blank_line_before_public, add_blank_line_before_protected, add_blank_line_before_private, do_not_leave_blank_line_after_public_protected_private, do_not_leave_blank_line_at_the_start_of_code_block, do_not_leave_blank_line_at_the_end_of_code_block, legal_copyright, add_include_what_you_use]
HOOKS_PARSING_THE_WHOLE_FILE = [cpp_file_should_include_h_file, do_not_leave_blank_line_after_public_protected_private,
 do_not_leave_blank_line_at_the_start_of_code_block, do_not_leave_blank_line_at_the_end_of_code_block, legal_copyright, ifndef_header, add_include_what_you_use] #Hook à éxecuter avant les autres hooks 

def replace_line_no(path, nol, oldline, newline):
	"""
	Remplace, la ligne No nol du fichier path (relatif à SRCROOTDIR) par la chaîne newline (un unicode)
	oldline sert uniquement pour vérifier que tout est ok
	"""
	print "replace_line_no : ", oldline, " by ", newline
	# récupérer le contenu du fichier
	fic = open(get_src_path(path), "r")
	liste = fic.readlines()
	fic.close()
	# test de corruption
	if liste[nol-1].decode(ENCODING) != oldline :
		raise Exception(u"Le fichier source %s semble être corrompu" %path)
	# remplacement de la ligne
	liste[nol-1] = newline.encode(ENCODING)
	# recréation du fichier corrigé
	fic = open(get_src_path(path), "w")	
	fic.writelines(liste)
	fic.close()	

def replace_fic_copyright(ficpath, warning=None):
	"""
	Remplace le fichier sans la ligne copyright par le fichier avec la ligne copyright
	"""
	
	fic = open(get_src_path(ficpath), "r")
	liste=fic_readlines(fic)
	fic.close()
	new_liste = []
	if liste[0].find(u'/ Copyright (C) 2014-20xx CEA/DEN, EDF R&D\n\n')== -1:			
		newliste=[u'// Copyright (C) 2014-20xx CEA/DEN, EDF R&D\n\n']+liste	
		newliste=fic_writelines(newliste)
		fic = open(get_src_path(ficpath), "w")
		fic.writelines(newliste)
		fic.close()


def replace_fic_ifndef(ficpath, ficline, warning, mode):
	"""
	Remplace dans le fichier, toutes les erreurs sur le style des #define et #ifndef à partir du warning
	"""
        fic = open(get_src_path(ficpath),'r')
        liste = fic_readlines(fic)
        new_liste = []

	if mode == "no_guard_found":
		m = re.match("(.*)No #ifndef header guard found, suggested CPP variable is: (.+)  (.+)", warning)
	else:
	        m = re.match("(.*)ifndef header guard has wrong style, please use: (.+)  (.+)", warning)

        if(m):
                print 'group_0', m.group(0)
                print 'group_1', m.group(1)
                print 'group_2', m.group(2)

                # Header à changer
		header_guard=m.group(2)

                # Recherche de la ligne à modifier
		i = 0
		trouve = False
                for ligne in liste:
			i += 1
			if i == ficline:
				if ligne.find(u'#ifndef') >= 0:
					new_liste.append("#ifndef "+header_guard+"\n")
					trouve = True
				else:
					print "ERROR : Pattern #ifndef not found in line ", ficline
					return
			else:
				if trouve == True:
					if ligne.find(u'#define') >= 0:
						new_liste.append("#define "+header_guard+"\n")
						trouve = False
					else:
						print "WARNING : Pattern #define not found in the line following the pattern #ifndef, we abandon."
						return
				else:
					new_liste.append(ligne)
        else:
                print "ERROR : Pattern not found : \"#ifndef header guard has wrong style, please use\""

        newliste=fic_writelines(new_liste)
        fichier = open(get_src_path(ficpath), "w")
        fichier.writelines(newliste)
			

def get_hook(warning, fichier):
	"""
	Fait le matching entre un warning et le hook permettant de le traiter.
	Les critères pour sélectionner un ou l'autre des hook sont consignés dans un dict associé (dans HOOKS_DICT) à chaque hook disponible.
	Le format de chaque dict de critère est le suivant :
	{
		"pattern": <motif principal devant se trouver dans le texte du warning>,
		"pattern_AND": <motif secondaire devant se trouver également dans le texte du warning>,
		"pattern_NOT": <motif secondaire ne devant pas se trouver dans le texte du warning>,
		"ext": <extension du fichier considéré>	
	}
	Seul le premier hook trouvé est renvoyé.
	Les arguments sont les suivants :
	* warning : le texte unicode du warning
	* fichier : le chemin vers le fichier concerné
	"""
	def test_it(condition, data, warning, fichier):
		if condition in ["pattern", "pattern_AND"] :
			if data in warning:
				return True
		elif condition in ["PATTERN_NOT"]:
			if data not in warning:
				return True
		elif condition in ["ext"]:
			if data not in os.path.splitext(fichier)[1]:
				return True
		return False
	
	for key, val in HOOKS_DICT.items(): # boucle sur les hooks existants
		test = None
		for condition, data in val.items():
			if test is False:
				# si une condition n'est pas réalisée => c'est pas le bon hook
				continue
			test = test_it(condition, data, warning, fichier)
		if test is True:
			return key
	return
	
def main_routine(cpplint_output_file):
	logfile = open(cpplint_output_file,"r")
	generateur_w = create_warning_generator(logfile)
	last_ficpath_with_modified_nb_lines = ""
	for ficpath, ficline, warning in generateur_w:
		oldline = get_line_no(ficpath, ficline)
		if ficline > 0:
			previous_oldline = get_line_no(ficpath, ficline-1)
		else:
			previous_oldline = None
		print "===\nNew warning:"
		print "\tFile = "+ficpath
		print "\tlast_ficpath_with_modified_nb_lines = "+last_ficpath_with_modified_nb_lines
		print "\tNo ligne =" , ficline
		if VERBOSE_FLAG == "True":
			print "\tWarning ='"+warning+"'"
			print "\tOld text of current line = '"+oldline+"'"
			if previous_oldline:
				print "\tOld text of previous line = '"+previous_oldline+"'"
			if ficpath == last_ficpath_with_modified_nb_lines:
				print "File ", ficpath, "already modified. Waiting next cpplint run to process it."
				continue
		hook = get_hook(warning, ficpath)
		print "hook = ", hook
		if hook is None:
			print u"No hook found"
			continue

		if VERBOSE_FLAG == "True":
			print "\t=> Processing with hook", hook

		if hook in HOOKS_DELETING_OR_ADDING_LINES:
			last_ficpath_with_modified_nb_lines = ficpath
			
		if hook in HOOKS_PARSING_THE_WHOLE_FILE:
			hook(ficpath, ficline, warning)
			print "Done"
			continue
				
		# Cas particulier pour le hook make_const_reference : il faut faire un traitement sur le fichier .cpp associé au fichier .h concerné par le hook
		if hook in [make_const_reference]:
			if ficpath.find('.h') > -1:
				newline, previous_newline, huchk = hook(ficpath, oldline, previous_oldline, warning)
			else:
				if H_FILE_MAKE_CONST_REFERENCE_MODIFIED:
					continue
				else:
					newline, previous_newline, huchk = hook(ficpath, oldline, previous_oldline, warning)
		else:
			newline, previous_newline, huchk = hook(oldline, previous_oldline, warning)

		if VERBOSE_FLAG == "True":
			print "\tNew text = '"+newline+"'"
		if huchk:
			if previous_newline:
				print "Replace line : \n'%s'\n with line \n'%s'\n O/N ? :"%(previous_oldline, previous_newline)
				reponse = raw_input()
				if reponse in ['O', 'o']:
					replace_line_no(ficpath, ficline-1, previous_oldline, previous_newline)	
					print "Replacement done."		
				else :
					print "Replacement aborted."

				print "Replace line : \n'%s'\n with line \n'%s'\n O/N ? :"%(oldline, newline)
				reponse = raw_input()
				if reponse in ['O', 'o']:
					replace_line_no(ficpath, ficline, oldline, newline)	
				print "Replacement done."		
			else :
				print "Replacement aborted."


		else :
			if previous_newline:
				replace_line_no(ficpath, ficline-1, previous_oldline, previous_newline)
			replace_line_no(ficpath, ficline, oldline, newline)
		print "Done"
	logfile.close()
		
if __name__ == '__main__':
	H_FILE_MAKE_CONST_REFERENCE_MODIFIED = False

	parser = argparse.ArgumentParser()
	parser.add_argument("--cpplint_output_file", type=str, help="Fichier de rapport cpplint à lire en entrée")
	args = parser.parse_args()

	if not args.cpplint_output_file:
		parser.print_help()
	else:
		main_routine(args.cpplint_output_file)
