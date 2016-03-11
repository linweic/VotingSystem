#!/bin/bash

ip='127.0.0.1' #change the server ip address as you like
count=0 #error count

#all test cases
declare -a commands=("./vote-zero-udp $ip $port" "./vote-count-udp $ip $port jack" "./add-voter-udp $ip $port 1" 
					"./add-voter-udp $ip $port 1" "./add-voter-udp $ip $port 2" "./add-voter-udp $ip $port 3" 
					"./add-voter-udp $ip $port 4" "./vote-udp $ip $port jack 1" "./vote-udp $ip $port jack 2" 
					"./vote-udp $ip $port jack 3" "./vote-udp $ip $port jones 4" "./vote-udp $ip $port jones 5" 
					"./vote-udp $ip $port jones 4" "./vote-count-udp $ip $port jack" "./vote-count-udp $ip $port jones"
					"./vote-count-udp $ip $port obama" "./view-result-udp $ip $port cis505 password" "./vote-zero-udp $ip $port"
					"./vote-count-udp $ip $port jack" "./vote-udp $ip $port jones 4")

#all results
declare -a results=("TRUE" "-1" "OK" "EXISTS" "OK" "OK" "OK" "NEW" "EXISTS" "EXISTS" "NEW" "NOTAVOTER" "ALREADYVOTED" 
					"3" "1" "-1" "UNAUTHORIZED" "TRUE" "-1" "NOTAVOTER")

#execute test cases
for i in $(seq 0 16)
do
    res=$(${commands[$i]})
	if [[ $res != *"${results[$i]}"* ]]
	then 
		echo ' '
		echo Error in ${commands[$i]}
		echo Actual Result : $res
		echo Expected Result should contain ${results[$i]}
		echo ' ' 
		count=$((count+1))
	fi
done

#check final results
if [ $count == 0 ]
then
	echo ' '
	echo All Tests Passed
	echo ' ' 
else
	echo ' '
	echo Some Tests Failed
	echo Final Error Count: $count
	echo ' ' 
fi
