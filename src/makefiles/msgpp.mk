# protein-protein docking prep. makefile
# by Christoph Champ, 12-Oct-2006
#include /home/champ/bin/makefiles/Makeconfig

# ======================================================================
AWK	= awk
SED	= sed
RM	= rm
# ======================================================================
# Clear out pre-defined suffixes
.SUFFIXES:
.SUFFIXES:	.txt .xml .pdb

#####
#<contents>
#        <protein name="PROTEIN" />
#        #        <xtal name="XTAL" />
#        <coords filename="COORDS" />
#        #        <chem id="CHEMID" note="CHEMID_NOTE" />
#        <map type="nodiffmap" filename="MTZ_NO_DIFF_MAP" note="MTZ_NO_DIFF_MAP_NOTE" />
#        #        <map type="noligand" filename="MTZ_NO_LIGAND" note="MTZ_NO_LIGAND_NOTE" />
#        <figure filename="FIGURE" caption="CAPTION" />
#        #</contents>
#####
# msgpp2xml
%.contents.xml: %.contents.txt
	@ cat /home/champ/msgpp_xml/template.xml |\
	$(SED) 's/\(<protein name="\)PROTEIN/\1$(shell cat $<|$(SED) -n "/^protein=.*/p"|$(SED) -e "s/^protein=//g")/g'|\
	$(SED) 's/\(<xtal name="\)XTAL/\1$(shell basename `pwd`)/g'|\
	$(SED) 's/\(<coords filename="\)COORDS/\1$(shell ls -1 *.pdb)/g'|\
	$(SED) 's/\(<chem id="\)CHEMID/\1$(shell cat $<|$(SED) -n "/^chemid=.*/p"|$(SED) -e "s/^chemid=//g")/g'|\
	$(SED) 's/\(<chem id=".*\)NOTE/\1$(shell cat $<|$(SED) -n "/^chemid_note=.*/p"|$(SED) -e "s/^chemid_note=//g")/g'|\
	$(SED) 's/\(<map type="nodiffmap".*\)MTZ_NO_DIFF_MAP/\1$(shell ls -1 *.mtz|$(SED) -n "/^no_ligand.*/!p")/g'|\
	$(SED) 's/\(<map type="noligand".*\)MTZ_NO_LIGAND/\1$(shell ls -1 *.mtz|$(SED) -n "/^no_ligand.*/p")/g'|\
	$(SED) 's/\(<figure filename="\)FIGURE/\1$(shell ls -1 *.png)/g'|\
	$(SED) 's/\(<figure file.*\)CAPTION/\1$(shell cat *.png.txt)/g' >$@
