#release-script.sh - The script tests some of the edge cases for most methods except list-candidates, view-result and 
change-password since they either need shell interaction or manual verification. 

#Usage - 
place script in /proj2_ms1/part1/
start the udp server - 
./server-udp
execute  - 
port=port_no ./release_script1.sh
where port_no is the port number of the server

#Results - 
The script will display All Tests Passed, if no errors occurred.
Incase errors occurred then the test case, expected result and actual result will be displayed on the screen.
