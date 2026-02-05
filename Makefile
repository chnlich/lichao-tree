.PHONY: all clean run pdf

all:
	$(MAKE) -C benchmark

clean:
	$(MAKE) -C benchmark clean
	rm -f doc/report.pdf

run:
	$(MAKE) -C benchmark run

pdf: doc/report.pdf

doc/report.pdf: doc/report.tex
	cd doc && pdflatex -interaction=nonstopmode report.tex && pdflatex -interaction=nonstopmode report.tex

watch-pdf:
	@echo "Watching doc/report.tex for changes..."
	@while true; do \
		inotifywait -e modify doc/report.tex 2>/dev/null && \
		echo "Rebuilding PDF..." && \
		$(MAKE) pdf; \
	done
