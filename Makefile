
SHELL := /bin/bash
.PHONY: all tidy dist

all:
	@echo "No 'all' target exists for this project."

tidy:
	@echo " TIDY"
	@for dir in $$(find firmware/* -type d); do \
	 make -s -C $${dir} clean; done
	@for dir in $$(find software/* -type d); do \
	 make -s -C $${dir} clean; done
	@find * -type f | awk -F '/' '$$NF~/^\._/' | xargs rm -vf

dist: tidy
	@echo " DIST"
	@pushd .. >/dev/null; \
	 isodate=$$(date +%Y%m%d); \
	 ftar="ppm-$${isodate}.tar"; \
	 ftgz="ppm-$${isodate}.tar.gz"; \
	 rm -f $${ftar}; \
	 if [ -e $${ftgz} ]; then \
	   echo -n "The file $${ftgz} exists. overwrite? (y/n) "; \
	   read resp; \
	   if [ "$${resp}" == "y" ]; then \
	     rm -f $${ftgz}; \
	   else \
	     exit; \
	   fi \
	 fi; \
	 tar cf $${ftar} ppm/ && gzip -9 $${ftar}; \
	 popd >/dev/null

