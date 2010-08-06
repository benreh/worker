#!/bin/bash
#	Copyright 2010 Benjamin Reh <ich@benjaminreh.de>
#
#	This file is part of 'Worker'.
#
#    'Worker' is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation version 3 of the License.
#
#    'Worker is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.


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
