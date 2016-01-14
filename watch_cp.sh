#!/bin/bash

# The directory to watch
WATCH_PATH=$1
# The destination directory
REMOTE=$2
# A file containing a list of regexes for files to ignore
IGNORE_LIST="ignore_list"
inotifywait "$WATCH_PATH" -m -r -e CLOSE_WRITE |
	while read path action file; do
		echo "$file in $path was $action"
		if echo "$file" | grep -v -q -f "$IGNORE_LIST"
		then
			echo "Copying $file to $REMOTE$file..."
			# Actually copy the file
			cp $file $REMOTE$file
		fi
	done
