 
#!/bin/bash

valgrind --tool=massif --time-unit=B  ./bin/sfmlExample

###use massif-visualizer to view output file.
