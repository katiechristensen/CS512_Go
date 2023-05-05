/* kmeans.c Provided by Dr. Meehan from Western Washington University
   
   The distribution fed to this program should be sorted
   It doesn't matter whether is is sorted ascending or descending

   The program does not check that the input is sorted.
   If you feed it unsorted data the "results" are meaningless

   ---

   This is an EXACT solution to an NP-hard problem
   Don't be scared. It will produce a solution very quickly
   for small data sets with a reasonable k. We are dealing with
   one dimensional data rather than the general kmeans problem
   with arbitrarily large dimensional vectors.
   In particular for application to a distributioon of  scores 
   in a class of typical size the results are instantaneous.
   I routinely use it with k = 10 on 40 point distributions 
*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>

#define BIG_double (INFINITY)
#define HYPHEN '-'
#define kdefault 5
#define km1default 4
#define InitialScoresArraySize 100  // we grow the scores array
                                    // in increments of 100 as needed

static double *scores; 	        // the distribution to be analyzed
                                // assumed to be sorted if it isn't
                                // results are meaningless 
static double bestAllk=BIG_double;

static int k=kdefault;
static int km1=km1default;

typedef struct 
{ 
  int     start;
  int     stop;
  double  mean;
  double  tightness;
} cluster;

// number of clusters we want
static cluster *clusters; // k cluster structs where the partitions are varied
static cluster *bestPartition;   // tightest overall partition known

double clustmean(int i)         // compute mean for cluster i
{ double  d=0.0;
  int clustcount=(clusters[i].stop - clusters[i].start) + 1;
  int j;
  for (j=clusters[i].start; j<=clusters[i].stop; j++) d+=scores[j];
  return d/clustcount;
} // end clustmean

void getstats()
{ int i,j;
  double tmpd,tmpd2;
  double tightnessAllk=0.0;
  for (i=0; i<k; i++)
    { tmpd=clustmean(i);
      clusters[i].mean=tmpd; 
      clusters[i].tightness=0.0;
      tmpd2=0.0;
      for (j=clusters[i].start; j<=clusters[i].stop; j++)
	{ tmpd2=scores[j]-tmpd;
	  clusters[i].tightness+=fabs(tmpd2);
	}
      tightnessAllk += clusters[i].tightness;
    };
  if ( tightnessAllk < bestAllk) 
    {bestAllk=tightnessAllk; 
      for (i=0; i<k; i++)
	{ bestPartition[i].start=clusters[i].start;         // 
	  bestPartition[i].stop=clusters[i].stop;
	  bestPartition[i].mean=clusters[i].mean;
	  bestPartition[i].tightness=clusters[i].tightness;  
	}
    }
} // end getstats

void findbest(int level)
{
  clusters[level].stop=clusters[level+1].start-1;
  if (level==0) getstats();
  else
    {
      for (clusters[level].start=level;				\
	   clusters[level].start<clusters[level+1].start;	\
	   ++clusters[level].start)
	findbest(level-1);
    }
}

void usage()
{
  printf("Usage: \n");
  printf("     [-k value] where value is the number of clusters to generate\n");
  printf("     [-h] print usage message \n");
  printf("Defaults: k = 5\n");
  exit(-1);
};

void kSizeError()
{
  printf("Error: K  value must be a positive integer > 1\n");
  usage();
};

void commandOptError()
{
  printf("Unrecognized option \n");
  usage();
};

int main (int argc, char *argv[])
{ int i=0;
  int c,tmpint,j,n,nm1,p,q,r,s,t,level,CurrentScoresArraySize;
  double tmpmean=0.0;
  double score;
  char optstring[]="hk:";
  CurrentScoresArraySize=InitialScoresArraySize;
  scores = (double *) malloc(sizeof(double)*InitialScoresArraySize);
  /* get the command line parameters to find out what k value we will
     be using
  */
  if (argc==1)
    printf("Defaulting to 5 clusters\n");
  else
    while ((c = getopt(argc,argv,optstring)) != -1)
      switch (c)
	{
	case 'k':
	  k = atoi(optarg);
	  km1=k-1;
	  break;
	case 'h':
	  usage();
	default:
	  commandOptError();
	};

  if (k <=1 ) kSizeError();
   
  clusters = (cluster *) malloc(sizeof(cluster)*k);
  bestPartition =(cluster *)malloc(sizeof(cluster)*k);
  while (scanf("%lf", &score) != EOF)
    {
      scores[i]=score;
      i++;
      if (i > CurrentScoresArraySize)
	{
	  printf("Number of points in the distribution is getting pretty large %d\n",i);
	  printf("This is an exact solution to an NP-hard algorithm\n");
	  printf("You may have to wait a while for results\n");
	  CurrentScoresArraySize+=100;
	  scores=realloc(scores,sizeof(double)*CurrentScoresArraySize);
	  if (scores==NULL)
	    {
	      printf("Bad news.\nrealloc failed trying to extend the scores array.\n Your data set must be insanely large???\n");
	      exit(-2);
	    }
	}
    }
  if (k > i )
    {
      printf("Number of clusters requested %d is > the number of data points %d\n",k,i);
      printf("Terminating. Get your head awake and try again.\n");
      exit(-3);
    }
  else
    printf("Number of points in the distribution is %d\n",i);
  n=i;
  nm1=n-1;

  //set initial cluster means and positions
  for (i=0; i< k; i++)
    {
      clusters[i].mean=scores[i];
      clusters[i].start=i;
      clusters[i].stop=i;
      clusters[i].tightness=0.0;
    }

  clusters[km1].stop=nm1;
  for (clusters[km1].start=km1; clusters[km1].start<n;++clusters[km1].start)
    {
      findbest(km1-1);
    }
 
  for (i=0; i<k ; i++)
    { printf("Partition %d = %d to %d scores=",i,bestPartition[i].start,bestPartition[i].stop);
      for (j=bestPartition[i].start;j<=bestPartition[i].stop;j++)
	{ printf("%2.2lf ",scores[j]);
	}
      printf("mean=%2.2lf ",bestPartition[i].mean);
      printf("tightness=%2.2lf\n",bestPartition[i].tightness/
	     (bestPartition[i].stop-bestPartition[i].start+1));
    }
} // end program
