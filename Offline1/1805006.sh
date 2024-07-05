#!/bin/bash

#check if fullmarks given
if (( $# == 2 ))
then
	fullmarks=$1
	maxid=$2
elif (( $# == 1 )) 
then
	fullmarks=$1
	maxid=5
elif (( $# == 0 )) 
then
	fullmarks=100
	maxid=5
else
	exit 1
fi


#extract ids
for (( i=1 ; i <= maxid; i++ ))
do
	ids[$i]="180512$i"
	(( marks[i] = fullmarks ))
done


#Give Marks
cd ./Submissions
for i in ${!ids[@]}
do

	if [[ $( find . -type d -name "${ids[$i]}" | wc -l ) != 1 ]]
	then 
		(( marks[i] = 0 ))
	elif [[ $( find "./${ids[$i]}" -name "${ids[$i]}.sh" | wc -l ) != 1 ]]
	then 
		(( marks[i] = 0 ))

	else

		m=$( grep '^<\|^>' <(diff -w <( bash "./${ids[$i]}/${ids[$i]}.sh" ) "../AcceptedOutput.txt") | wc -l )
		(( marks[i]-= (5*m) ))
		if(( marks[i] < 0 )); then
			(( marks[i] = 0 ))
		fi
	fi
	(( i++ ))
done
cd ..

#check plagiarism
cd ./Submissions
for (( i=1 ; i<=maxid ; i++ ))
do
	for (( j=i+1 ; j<=maxid ; j++ ))
	do
		if diff -w "./${ids[$i]}/${ids[$i]}.sh" "./${ids[$j]}/${ids[$j]}.sh" &>/dev/null
		then
			(( marks[i] = -fullmarks ))
			(( marks[j] = -fullmarks ))
		fi			
	done
done
cd ..

echo "student_id,score" > output.csv
for i in ${!ids[@]}
do 
	echo "${ids[$i]},${marks[$i]}" >> output.csv  
	((i++))
done