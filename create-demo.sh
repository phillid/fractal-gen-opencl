#!/bin/sh

temp_name="demo.tmp.pgm"

clean() {
	rm -v "$temp_name"
}

create_demo() {
	echo "::: Invoking fractal-gen through pnminvert for negative PGM image"
	./fractal-gen -p any -s "$1" -i "$2"| pnminvert > "$temp_name"
	echo "::: Invoking ImageMagick to convert to PNG"
	convert "$temp_name" "$3"
	echo "::: Success. Final image is $3"
}

hash pnminvert || exit
hash convert || exit
trap clean EXIT

create_demo 768 75 demo-low-iterat.png
create_demo 768 10000000 demo-high-iterat.png
