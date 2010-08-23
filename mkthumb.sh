#!/bin/bash
#	Copyright 2010 Benjamin Reh <ich@benjaminreh.de>
#
#	This file is part of 'Worker'.
#
#    'Worker' is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation version 3 of the License.
#
#    'Worker' is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with 'Worker'.  If not, see <http://www.gnu.org/licenses/>.

# This small script demonstrates the use of worker
# It is used in a directory with jpgs.
# First they are rotated with 'exifautotran' and then resized
# with imagick's convert in a seperate directory

# Name of the thumbnail directory
THUMBDIR=small
FILES="$(ls *.jpg *.JPG 2>/dev/null)"
QUALITY="-quality 70 -resize  15%x15%"

mkdir -p $THUMBDIR
#Round 1: rotate
for i in $FILES
do 
	worker exifautotran $i 
done
#wait for all workers to finisch (barrier)
worker --join

#Round 2: convert
for i in $FILES
do
	TO=$THUMBDIR/$i
	#only convert, if newer than target
	if [ "$i" -nt "$TO" ] || [ ! -f "$TO" ] 
	then
		echo $i
		worker convert $QUALITY $i $TO
	fi
done
#wait again
worker --join
