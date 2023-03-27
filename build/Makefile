
all: v1.0 v1.5 v2.0 v3.0
	
v%:
	rm -rf ../$@
	make -C ../sources/$@ html
	mv ../sources/$@/_build/html ../$@
	rm -rf ../sources/$@/_build

view:
	firefox ../index.html &
