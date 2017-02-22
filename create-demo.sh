#!/bin/sh

final_name="demo.png"
temp_name="demo.tmp.pgm"

clean() {
	rm -v "$temp_name"
}

hash pnminvert || exit
hash convert || exit
trap clean exit

echo "::: Invoking fractal-gen through pnminvert for negative PGM image"

./fractal-gen -p any -s 768 -i 75 | pnminvert > "$temp_name"

echo "::: Invoking ImageMagick to convert to PNG"

convert "$temp_name" "$final_name"

echo "::: Success. Final image is $final_name"
