.PHONY: all
LATEX=pdflatex
ARCHIVOS=informe.tex informe_ej1.tex informe_ej2.tex informe_ej3.tex dibujo_ej3_ej1.tex ejemplo_ej3.tex
all: informe.pdf

informe.pdf: $(ARCHIVOS)
	@echo "Compiling informe in latex..."
#se hace 2 veces el latex para que cargue bien la TOC y redirije todo a loginforme para no imprimir 23098230982309823 lineas en el makefile.
	$(LATEX) informe.tex > loginforme.log
	$(LATEX) informe.tex > loginforme.log

clean: 
	@echo "Cleaning informe"	
	- rm -f *.log
	- rm -f *.toc
	- rm -f *.aux
	- rm -f *.out
	- rm -f *.pdf
	- rm -f *.dvi
	- rm -f *~
