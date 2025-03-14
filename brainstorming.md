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
got a basic scipt working to parse a given dbc file and print all the signals to console]

given a can id and can interface, i can loop through message ids to find the right message (might be faster to preproccess a map rather than iterating everytime)
then using the signal.decode() and signal.RawtoPhys() i can convert the payload to human readable format

need to figure out how to parse the log file and then i can implement the above ^


## Spyder

## Cloud
