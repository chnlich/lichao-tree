.PHONY: all clean run pdf arxiv

all:
	$(MAKE) -C benchmark

clean:
	$(MAKE) -C benchmark clean
	rm -f doc/report.pdf

run:
	$(MAKE) -C benchmark run

pdf: doc/report.pdf

doc/report.pdf: doc/report.tex doc/references.bib
	cd doc && pdflatex -interaction=nonstopmode report.tex && bibtex report && pdflatex -interaction=nonstopmode report.tex && pdflatex -interaction=nonstopmode report.tex

arxiv: doc/report.bbl
	tar czf arxiv.tar.gz -C doc report.tex report.bbl references.bib

doc/report.bbl: doc/report.tex doc/references.bib
	cd doc && pdflatex -interaction=nonstopmode report.tex && bibtex report

watch-pdf:
	@echo "Watching doc/report.tex for changes..."
	@while true; do \
		inotifywait -e modify doc/report.tex 2>/dev/null && \
		echo "Rebuilding PDF..." && \
		$(MAKE) pdf; \
	done
