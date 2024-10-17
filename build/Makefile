
html:
	rm -rf ../v$(VER)
	make -C ../sources/v$(VER) html
	mv ../sources/v$(VER)/_build/html ../v$(VER)
	rm -rf ../sources/v$(VER)/_build
pub:
	git push origin
sandbox:
	$(eval BRANCH := $(shell git rev-parse --abbrev-ref HEAD))
	cd ../sandbox/video-sdk/ && git checkout $(BRANCH) --
	rsync -a --delete --exclude  .git --exclude sandbox --exclude .cproject --exclude .externalToolBuilders --exclude  .project --exclude  .pydevproject ../ ../sandbox/video-sdk/

