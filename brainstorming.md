# Brainstorming

This file is used to document your thoughts, approaches and research conducted across all tasks in the Technical Assessment.

## Firmware
first setup dev enviroment
    - linting (clangd)
    - debugging 
then setup project enviroment
    - install all libraries (dbcppp & catch2)
    - make sure everything is linked properly
    - make sure output.txt is reflected locally
then start work on the actual tasks

read through the dbcppp examples to figure out how to extract messages from dbc files
got a basic scipt working to parse a given dbc file and print all the signals to console

given a can id and can interface, i can loop through message ids to find the right message (might be faster to preproccess a map rather than iterating everytime)
then using the signal.decode() and signal.RawtoPhys() i can convert the payload to human readable format

need to figure out how to parse the log file and then i can implement the above ^

i had two ideas in mind to parse the log file
1. split each line on a space, and then split the id#payload by a #
2. use regex to split each line
i figure the regex approach would be more robust and less LOC, but i dont know enough regex to write the pattern my self, so i used chatgpt to come up with the regex pattern.

got a basic log file parser done to extract timestamp, interface, id and payload.

plan:
- parse all 3 dbc files into dbc objects 
    - create a map between messageIds and message objects for each dbc file
- create a map that maps interfaces to dbc objects
- process each line of the log file
    - extract time,interface,id,payload
- obtain dbc object from interface
- obtain signal object from id 
- convert payload to human readable format
- bundle output into an object (probably useful when testing)
- write output to file 

notes on above plan:
- worked pre well
- had a minor issue where the progam would seg fault because i didnt handle the case where the payload was empty. glad i setup debugging, was able to resolve it quickly

future plan:
- refactor a bit
    - lots of nested loops/conditions
    - will probably need to resolve more objects to make the main function cleaner
- add tests

## Spyder

## Cloud
