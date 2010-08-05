#!/bin/bash
export THUMBDIR=small
mkdir -p $THUMBDIR
export FILES="$(ls *.jpg *.JPG 2>/dev/null)"
for i in $FILES
do 
	worker exifautotran $i 
done
worker --join
for i in $FILES
do
	TO=$THUMBDIR/$i
	if [ "$i" -nt "$TO" ] || [ ! -f "$TO" ] 
	then
		echo $i
		worker convert -quality 70 -resize  15%x15% $i $TO
	fi
done
worker --join
