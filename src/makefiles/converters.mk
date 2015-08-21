MOLAUTO   = /home/champ/bin/molauto
MOLSCRIPT = /home/champ/bin/molscript
RENDER    = /usr/local/bin/render

#molauto 1hry.pdb |molscript -r|render -bg white -size 300x300 -png 1hry.pdb.png
%.pdb.png:	%.pdb
	$(MOLAUTO) $< | $(MOLSCRIPT) -r | $(RENDER) -bg white -size 300x300 -png >$@

%.pnm:	%.png
	pngtopnm < $< > $@

#%.svg:	%.pnm
#	potrace --svg < $< > $@

#jpegtopnm < capital-r/photographed.jpg | ppmtopgm | pgmnorm > tmp.pgm
#convert -monochrome tmp.pgm tmp.png
