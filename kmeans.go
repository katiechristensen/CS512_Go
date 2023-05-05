// Katie Christensen
// CSCI 512
// Program 2
// February 15, 2023

package main

import ( 
	"fmt"
	"os"
	"io"
	"strconv"
	"math"
	"strings"
)

var BIG_float64 = math.Inf(1)
var HYPHEN = "-"
var kdefault = 5
var km1default = 4
var InitialScoresArraySize = 100

var scores []float64
var bestAllk = BIG_float64
var k = kdefault
var km1 = km1default

type cluster struct {
	start int
	stop int
	mean float64
	tightness float64
}

var clusters []cluster
var bestPartition []cluster


func clustmean(i int) (float64) {
	var d float64
	var j, clustcount int 

	d = 0.0
	clustcount = (clusters[i].stop - clusters[i].start) + 1

	for j = clusters[i].start; j <= clusters[i].stop; j++ {
		d = d + scores[j]
	}

	return d/float64(clustcount)
}


func getstats() {
	var i, j int = 0,0
	var tmpd, tmpd2, tightnessAllk float64
	
	tightnessAllk = 0.0

	for i = 0; i < k; i++ {
		tmpd = clustmean(i)
		clusters[i].mean = tmpd
		clusters[i].tightness = 0.0
		tmpd2 = 0.0
		
		for j = clusters[i].start; j <= clusters[i].stop; j++ {
			tmpd2 = scores[j]-tmpd
			clusters[i].tightness += math.Abs(tmpd2)
		}
		
		tightnessAllk = tightnessAllk + clusters[i].tightness
	}

	if tightnessAllk < bestAllk {
		bestAllk = tightnessAllk
		
		for i = 0; i < k; i++ {
			bestPartition[i].start = clusters[i].start
	    	bestPartition[i].stop = clusters[i].stop
	    	bestPartition[i].mean = clusters[i].mean
	    	bestPartition[i].tightness = clusters[i].tightness 
		}
	}
}


func findbest(level int) {
	clusters[level].stop=clusters[level+1].start-1
	
	if level == 0 {
		getstats()
	} else {
		for clusters[level].start = level; clusters[level].start < clusters[level+1].start; clusters[level].start++ {
			findbest(level - 1)
		}
	}
}


func usage() {
	fmt.Println("Usage: ")
	fmt.Println("     [-k value] where value is the number of clusters to generate")
	fmt.Println("     [-h] print usage message ")
	fmt.Println("Defaults: k = 5")
	os.Exit(-1)
}


func kSizeError() {
	fmt.Println("Error: K  value must be a positive integer > 1")
	usage()
}


func commandOptError() {
	fmt.Println("Unrecognized option ")
	usage()
}


func main() {
	var i, index, optarg, j, n, nm1, CurrentScoresArraySize int = 0,0,0,0,0,0,0
	var c []string
	var score float64
	var flag string
	
	CurrentScoresArraySize = InitialScoresArraySize;
	
	scores = make([]float64, InitialScoresArraySize )

	if len(os.Args) == 1 {
		fmt.Println("Defaulting to 5 clusters")
	} else {
		for index = 1; index < len(os.Args); index++ {
			if strings.HasPrefix(os.Args[index], "-") && os.Args[index] != "--" && os.Args[index] != "-" {
				c = strings.Split(os.Args[index], "-")
				flag = c[1]
				switch flag {
				case "k":
					optarg, _ = strconv.Atoi(os.Args[index+1])
					k = optarg
					km1 = k - 1
					break
				case "h":
					usage()
				default:
					commandOptError()
				}
			} 
		}
	}

	if k <= 1 {
		kSizeError()
	}
	
	clusters = make([]cluster, k) 
	bestPartition = make([]cluster, k) 

	for {
		_, err := fmt.Scanf("%f", &score)
		
		if err == io.EOF {
			break
		} else {
			scores[i] = score
			i++
			if i > CurrentScoresArraySize {
				fmt.Printf("Number of points in the distribution is getting pretty large %d\n",i)
				fmt.Println("This is an exact solution to an NP-hard algorithm")
				fmt.Println("You may have to wait a while for results")
				
				CurrentScoresArraySize += 100
				
				scores = append(scores, float64(CurrentScoresArraySize)) 
				
				if scores == nil {
					fmt.Println("Bad news.\nrealloc failed trying to extend the scores array.\n Your data set must be insanely large???")
					os.Exit(-2)
				}
			}
		}
	}

	if k > i {
		fmt.Printf("Number of clusters requested %d is > the number of data points %d\n",k,i)
		fmt.Println("Terminating. Get your head awake and try again.")
		os.Exit(-3)
	} else {
		fmt.Printf("Number of points in the distribution is %d\n",i)
	}

	n = i
	nm1 = n - 1

	for i := 0; i < k; i++ {
		clusters[i].mean=scores[i]
      	clusters[i].start=i
      	clusters[i].stop=i
      	clusters[i].tightness=0.0
	}

	clusters[km1].stop = nm1
	
	for clusters[km1].start = km1; clusters[km1].start < n; clusters[km1].start++ {
		findbest(km1-1);
	}

	for i := 0; i < k; i++ {
		fmt.Printf("Partition %d = %d to %d scores=", i, bestPartition[i].start, bestPartition[i].stop)
		
		for j = bestPartition[i].start; j <= bestPartition[i].stop; j++ {
			fmt.Printf("%.2f ", scores[j])
		}

		fmt.Printf("mean=%.2f ", bestPartition[i].mean)
		fmt.Printf("tightness=%.2f\n", bestPartition[i].tightness / float64((bestPartition[i].stop - bestPartition[i].start + 1)))
	}
}