GNUPLOT_FILES = $(wildcard *.plt)
# create the target file list by substituting the extensions of the plt files
FICHIERS_PNG = $(patsubst %.plt,%.png,  $(GNUPLOT_FILES))

all: $(FICHIERS_PNG)

%.eps: %.plt
        @ echo "compillation of "$<
        @gnuplot $<

%.png: %.eps
        @echo "conversion in png format"
        @convert -density 300 $< $*.png 
        @echo "end"
