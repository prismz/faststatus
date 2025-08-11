all:
	cc *.c elements/*.c -lm -lX11 -g -Wall -Wextra -pedantic -o faststatus

run:all
	./faststatus

install:all
	cp -f faststatus /usr/local/bin/

	# below 3 lines required for spotify data in bar by default - edit faststatus-spotify.sh
	cp -f scripts/faststatus-spotify.sh /usr/local/bin/
	cp -f scripts/scrolling_metadata /usr/local/bin/      # must be started separately (with dwm)
	cp -f scripts/progbar /usr/local/bin                  # progress bar for song (required by spotify plugin by default)

	cp -f scripts/pulseaudio-volume /usr/local/bin/       # volume script
	#cp elements/faststatus-tidal.sh /usr/local/bin/
