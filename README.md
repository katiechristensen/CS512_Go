# Go Translation of a C Implementation of the K-Means Clustering Algorithm

### CSCI 512 - Design and Implementation of Computer Programming Languages

Katie Christensen, Winter 2023

This program will implement an optimal solution to the k-means clustering algorithm.

The program understands two command line parameters.

'''Usage:
[-k value] where value is the number of clusters to generate
[-h] print usage message
Defaults: k = 5'''

Enter kmeans to run the C version and generate test output. To run the Go program feed it a sorted distribution through standard in with various values for n using a command like: 

kmeans -k n < testm > testmout-n

where n is a number indicating the number of clusters requested and m is a number indicating which test input file was used.