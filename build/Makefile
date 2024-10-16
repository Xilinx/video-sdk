
html:
	rm -rf ../v$(VER)
	make -C ../sources/v$(VER) html
	mv ../sources/v$(VER)/_build/html ../v$(VER)
	rm -rf ../sources/v$(VER)/_build
pub:
	git push origin
sandbox:
	rsync -a --delete ../sources/v$(VER)/ ../sandbox/video-sdk/sources/v$(VER)/
	rsync -a --delete ../v$(VER)/ ../sandbox/video-sdk/v$(VER)/
