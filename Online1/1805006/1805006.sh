.#!/bin/bash
cd "./movie_data"

while read file
do
	#echo "$file"
	cnt=0
	while read line
	do
		
		if (( cnt == 2 ))
		then
			#echo "$line"
			if [[ $( find . -type d -name "$line" | wc -l ) != 1 ]]
			then
				mkdir "$line"
			fi
			mv "$file" "./$line"
			((cnt++))
		fi
		if [[ "$line" = "" ]]
		then
			((cnt++))
			#echo "$cnt"
		fi
	done < <(cat "$file")
done< <(ls -1)