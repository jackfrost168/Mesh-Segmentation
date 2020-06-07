#include "stdafx.h"
#include "MathFunctions.h"
#include "Cluster.h"
#include <gsl/gsl_eigen.h>
#include <gsl/gsl_permutation.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <stdio.h>
#include <stdlib.h>
#include "Timing.h"


Cluster::Cluster()
{
	numCluster=0;
	maxsize=0;
	maxvar=0;
	cluOfV=NULL;
	colorOfClu=NULL;
	pcluster=NULL;
	ptree=NULL;
	neighborclusters=NULL;
	pnewmodel = NULL;
	maxangle = 1.04;
	weight = 1.0;
	verticesrecord = NULL;
	kdtree = NULL;
	approximationerror = 0.0;
	numsamplingvertices = 0;
	meancurvature = NULL;
	highcurvaturevertices = NULL;
	maxcurvature = -1e5;
	mincurvature = 1e5;
	averagecurvature = 0.0;
}

Cluster::~Cluster()
{
	if (meancurvature)
	{
		delete []meancurvature;
		meancurvature = NULL;
	}
	DeleteClusters();
}

void Cluster::K_Means(GLMmodel *meshmodel,float maxerror,float *normalofvertices,float w)
{
	int i;
	clock_t start,stop;
	double time;
	start=clock();	
	
	cluOfV=(int*) malloc ((meshmodel->numvertices+1) * sizeof(int));
	pcluster=new _cluster [numCluster+1];
	for (i=1;i<=numCluster;i++)
	{
		pcluster[i].index=i;
		pcluster[i].iscluster=1;
		pcluster[i].order=0;
		pcluster[i].pvertices=NULL;
		pcluster[i].variation=0;
		pcluster[i-1].nextcluster=&(pcluster[i]);
	}
	pcluster[numCluster].nextcluster=NULL;

	K_Random(meshmodel,normalofvertices);
	
	Clustering(meshmodel,normalofvertices,w);

	while(reCenter(meshmodel,normalofvertices)>maxerror)
	{
		Clustering(meshmodel,normalofvertices,w);
	}
	
	Color(numCluster);

	printf("\n%d",numCluster);

	stop=clock();
	time=((double)(stop-start))/CLK_TCK;
	printf("\nTime: %lf",time);
}

void Cluster::K_Random(GLMmodel *meshmodel,float *normalofvertices)
{
	int i,j,k,m,temp;
	int *intArray = (int*) malloc (numCluster*sizeof(int));

	for(i=0;i<numCluster;i++)
	{
		intArray[i]=0;
	}
	
	for(i=0; i<numCluster; i++)
	{
		temp= (rand() % (int)(meshmodel->numvertices))+1;
		
		for(j=0; j<i; j++)
			if(temp== intArray[j])
				temp++;
			else
				if(temp>intArray[j])
					;
				else
					{
						m=j;
						for(j=i;j>m;j--)
							intArray[j]= intArray[j-1];
						break;
					}
		m=j;
		intArray[m] =temp;
		for (k=0;k<3;k++)
		{
			pcluster[i+1].center[k]=meshmodel->vertices[3*temp+k];
			pcluster[i+1].normal[k]=normalofvertices[3*temp+k];
		}	
	}
}

void Cluster::Clustering(GLMmodel *meshmodel,float *normalofvertices,float w)
{
	int i,j,k,jmin;
	float x[3],xnormal[3],energy,minenergy=INFINITE;
	IndexList *pp;

	for (i=1;i<=numCluster;i++)
	{
		pcluster[i].order=0;
		deleteIndexList(pcluster[i].pvertices);
		pcluster[i].pvertices=NULL;
	}
	
	for(i=1;i<= (int) meshmodel->numvertices;i++)
	{
		minenergy=INFINITE;
		for (k=0;k<3;k++)
		{
			x[k]=meshmodel->vertices[3*i+k];
			xnormal[k]=normalofvertices[3*i+k];
		}
		for(j=1;j<=numCluster;j++)
		{
			energy=w*glmDist(x,pcluster[j].center)-(1-w)*Dot(pcluster[j].normal,xnormal);
			if(energy<minenergy)
			{
				jmin=j;
				minenergy=energy;
			}
		}
		cluOfV[i]=jmin;
		pcluster[jmin].order++;
		if (pcluster[jmin].pvertices==NULL)
		{
			pcluster[jmin].pvertices=new IndexList;
			pcluster[jmin].pvertices->index=i;
			pcluster[jmin].pvertices->next=NULL;
		} 
		else
		{
			for(pp=pcluster[jmin].pvertices;pp->next!=NULL;pp=pp->next)
				;
			pp->next=new IndexList;
			pp=pp->next;
			pp->index=i;
			pp->next=NULL;
		}		
	}
}

float Cluster::reCenter(GLMmodel *meshmodel,float *normalofvertices)
{
	int i,j;
	float oldcenter[3],temp,maxDiff=0,sum[3];
	IndexList *pp;
	
	for(i=1;i<=numCluster;i++)
	{
		for (j=0;j<3;j++)
		{
			oldcenter[j]=pcluster[i].center[j];
		}	
		for (j=0;j<3;j++)
		{
			sum[j]=0;
		}
		for (pp=pcluster[i].pvertices;pp!=NULL;pp=pp->next)
		{
			for (j=0;j<3;j++)
			{
				sum[j]+=meshmodel->vertices[3*(pp->index)+j];
			}				
		}
		for (j=0;j<3;j++)
		{
			pcluster[i].center[j]=sum[j]/pcluster[i].order;
		}
		temp=glmDist(oldcenter,pcluster[i].center);
		if (temp>maxDiff)
		{
			maxDiff=temp;
		}
		for (j=0;j<3;j++)
		{
			oldcenter[j]=pcluster[i].normal[j];
		}	
		for (j=0;j<3;j++)
		{
			sum[j]=0;
		}
		for (pp=pcluster[i].pvertices;pp!=NULL;pp=pp->next)
		{
			for (j=0;j<3;j++)
			{
				sum[j]+=normalofvertices[3*(pp->index)+j];
			}				
		}
		for (j=0;j<3;j++)
		{
			pcluster[i].normal[j]=sum[j]/pcluster[i].order;
		}
		temp=glmDist(oldcenter,pcluster[i].normal);
		if (temp>maxDiff)
		{
			maxDiff=temp;
		}
	}

	return maxDiff;
}

void Cluster::Color(int num)
{
	int i;

	colorOfClu = (float*) malloc (3 * (num+1)* sizeof(float));

	for(i=0;i<=3*num+2;i++)
	{
		colorOfClu[i]= ((rand()%100001)*255)/100000.0/250;
	}

	////Glass48
	//printf("index1:%f,%f,%f\n", colorOfClu[1 * 3], colorOfClu[1 * 3 + 1], colorOfClu[1 * 3 + 2]);
	//printf("index65:%f,%f,%f\n", colorOfClu[65 * 3], colorOfClu[65 * 3 + 1], colorOfClu[65 * 3 + 2]);
	//printf("index267:%f,%f,%f\n", colorOfClu[267 * 3], colorOfClu[267 * 3 + 1], colorOfClu[267 * 3 + 2]);
	//printf("index526:%f,%f,%f\n", colorOfClu[526 * 3], colorOfClu[526 * 3 + 1], colorOfClu[526 * 3 + 2]);

	//int leg1 = 48; int leg2 = 371; int leg3 = 1,leg4 = 308; //(2，3为镜面，1，4腿)

	////leg1
	//colorOfClu[leg1 * 3] = 0.024694;
	//colorOfClu[leg1 * 3 + 1] = 0.145962;
	//colorOfClu[leg1 * 3 + 2] = 0.067493;
	//////leg2
	////colorOfClu[leg2 * 3] = 0.211987;
	////colorOfClu[leg2 * 3 + 1] = 0.183957;
	////colorOfClu[leg2 * 3 + 2] = 0.328705;
	////leg2
	//colorOfClu[leg2 * 3] = 0.270300;
	//colorOfClu[leg2 * 3 + 1] = 0.195524;
	//colorOfClu[leg2 * 3 + 2] = 0.160385;
	////leg3
	//colorOfClu[leg3 * 3] = 0.270300;
	//colorOfClu[leg3 * 3 + 1] = 0.195524;
	//colorOfClu[leg3 * 3 + 2] = 0.160385;

	////leg4
	//colorOfClu[leg4 * 3] = 0.210528;
	//colorOfClu[leg4 * 3 + 1] = 0.265730;
	//colorOfClu[leg4 * 3 + 2] = 0.002407;

	//Cup30
	//printf("index1:%f,%f,%f\n", colorOfClu[1 * 3], colorOfClu[1 * 3 + 1], colorOfClu[1 * 3 + 2]);
	//printf("index690:%f,%f,%f\n", colorOfClu[690 * 3], colorOfClu[690 * 3 + 1], colorOfClu[690 * 3 + 2]);
	//printf("index233:%f,%f,%f\n", colorOfClu[233 * 3], colorOfClu[233 * 3 + 1], colorOfClu[233 * 3 + 2]);

	//int leg1 = 324; int leg2 = 37; int leg3 = 1; 

	////leg1
	//colorOfClu[leg1 * 3] = 0.098930;
	//colorOfClu[leg1 * 3 + 1] = 0.199543;
	//colorOfClu[leg1 * 3 + 2] = 0.261232;
	////leg2
	//colorOfClu[leg2 * 3] = 0.139363;
	//colorOfClu[leg2 * 3 + 1] = 0.280092;
	//colorOfClu[leg2 * 3 + 2] = 0.221983;
	////leg3
	//colorOfClu[leg3 * 3] = 0.007823;
	//colorOfClu[leg3 * 3 + 1] = 0.273911;
	//colorOfClu[leg3 * 3 + 2] = 0.290170;

	/////////////////////octopus281
	//printf("index74:%f,%f,%f\n", colorOfClu[74 * 3], colorOfClu[74 * 3 + 1], colorOfClu[74 * 3 + 2]);
	//printf("index111:%f,%f,%f\n", colorOfClu[111 * 3], colorOfClu[111 * 3 + 1], colorOfClu[111 * 3 + 2]);
	//printf("index672:%f,%f,%f\n", colorOfClu[672 * 3], colorOfClu[672 * 3 + 1], colorOfClu[672 * 3 + 2]);
	//printf("index258:%f,%f,%f\n", colorOfClu[258 * 3], colorOfClu[258 * 3 + 1], colorOfClu[258 * 3 + 2]);
	//printf("index636:%f,%f,%f\n", colorOfClu[636 * 3], colorOfClu[636 * 3 + 1], colorOfClu[636 * 3 + 2]);
	//printf("index61:%f,%f,%f\n", colorOfClu[61 * 3], colorOfClu[61 * 3 + 1], colorOfClu[61 * 3 + 2]);
	//printf("index1:%f,%f,%f\n", colorOfClu[1 * 3], colorOfClu[1 * 3 + 1], colorOfClu[1 * 3 + 2]);
	//printf("index318:%f,%f,%f\n", colorOfClu[318 * 3], colorOfClu[318 * 3 + 1], colorOfClu[318 * 3 + 2]);
	//printf("index467:%f,%f,%f\n", colorOfClu[467 * 3], colorOfClu[467 * 3 + 1], colorOfClu[467 * 3 + 2]);

	//int head = 113; 
	//int leg1 = 1; int leg2 = 290; int leg3 = 241; int leg4 = 568;
	//int leg5 = 597; int leg6 = 415; int leg7 = 103; int leg8 = 59;
	////head
	//colorOfClu[head*3] = 0.034088;
	//colorOfClu[head*3+1] = 0.249441;
	//colorOfClu[head*3+2] = 0.101633;
	////leg1
	//colorOfClu[leg1 * 3] = 0.322473;
	//colorOfClu[leg1 * 3 + 1] = 0.114464;
	//colorOfClu[leg1 * 3 + 2] = 0.233162;
	////leg2
	//colorOfClu[leg2 * 3] = 0.240149;
	//colorOfClu[leg2 * 3 + 1] = 0.305811;
	//colorOfClu[leg2 * 3 + 2] = 0.233968;
	////leg3
	//colorOfClu[leg3 * 3] = 0.194861;
	//colorOfClu[leg3 * 3 + 1] = 0.144667;
	//colorOfClu[leg3 * 3 + 2] = 0.120207;
	////leg4
	//colorOfClu[leg4 * 3] = 0.290455;
	//colorOfClu[leg4 * 3 + 1] = 0.223064;
	//colorOfClu[leg4 * 3 + 2] = 0.063454;
	////leg5
	//colorOfClu[leg5 * 3] = 0.200267;
	//colorOfClu[leg5 * 3 + 1] = 0.107661;
	//colorOfClu[leg5 * 3 + 2] = 0.083803;
	////leg6
	//colorOfClu[leg6 * 3] = 0.203929;
	//colorOfClu[leg6 * 3 + 1] = 0.164577;
	//colorOfClu[leg6 * 3 + 2] = 0.275808;
	////leg7
	//colorOfClu[leg7 * 3] = 0.191525;
	//colorOfClu[leg7 * 3 + 1] = 0.020318;
	//colorOfClu[leg7 * 3 + 2] = 0.216893;
	////leg8
	//colorOfClu[leg8 * 3] = 0.063974;
	//colorOfClu[leg8 * 3 + 1] = 0.050266;
	//colorOfClu[leg8 * 3 + 2] = 0.321269;

	///////////////////Armadillo281
	//printf("index219:%f,%f,%f\n", colorOfClu[219 * 3], colorOfClu[219 * 3 + 1], colorOfClu[219 * 3 + 2]);
	//printf("index745:%f,%f,%f\n", colorOfClu[745 * 3], colorOfClu[745 * 3 + 1], colorOfClu[745 * 3 + 2]);
	//printf("index983:%f,%f,%f\n", colorOfClu[983 * 3], colorOfClu[983 * 3 + 1], colorOfClu[983 * 3 + 2]);
	//printf("index504:%f,%f,%f\n", colorOfClu[504 * 3], colorOfClu[504 * 3 + 1], colorOfClu[504 * 3 + 2]);
	//printf("index242:%f,%f,%f\n", colorOfClu[242 * 3], colorOfClu[242 * 3 + 1], colorOfClu[242 * 3 + 2]);
	//printf("index1:%f,%f,%f\n", colorOfClu[1 * 3], colorOfClu[1 * 3 + 1], colorOfClu[1 * 3 + 2]);

	//int body = 219; int head = 745; 
	//int lefthand = 983; int righthand = 504;
	//int leftleg = 242; int rightleg = 1;
	////body
	//colorOfClu[body*3] = 0.230174;
	//colorOfClu[body*3+1] = 0.277768;
	//colorOfClu[body*3+2] = 0.291638;
	////head
	//colorOfClu[head*3] = 0.112781;
	//colorOfClu[head*3+1] = 0.336721;
	//colorOfClu[head*3+2] = 0.274023;
	////lefthand
	//colorOfClu[lefthand * 3] = 0.146982;
	//colorOfClu[lefthand * 3 + 1] = 0.215473;
	//colorOfClu[lefthand * 3 + 2] = 0.235783;
	////righthand
	//colorOfClu[righthand * 3] = 0.164200;
	//colorOfClu[righthand * 3 + 1] = 0.326074;
	//colorOfClu[righthand * 3 + 2] = 0.194096;
	////leftleg
	//colorOfClu[leftleg * 3] = 0.165801;
	//colorOfClu[leftleg * 3 + 1] = 0.192005;
	//colorOfClu[leftleg * 3 + 2] = 0.076429;
	////rightleg
	//colorOfClu[rightleg * 3] = 0.075674;
	//colorOfClu[rightleg * 3 + 1] = 0.241159;
	//colorOfClu[rightleg * 3 + 2] = 0.280429;

	///////////////////Ant
	/*printf("index1:%f,%f,%f\n", colorOfClu[1 * 3], colorOfClu[1 * 3 + 1], colorOfClu[1 * 3 + 2]);
	printf("index629:%f,%f,%f\n", colorOfClu[629 * 3], colorOfClu[629 * 3 + 1], colorOfClu[629 * 3 + 2]);
	printf("index193:%f,%f,%f\n", colorOfClu[193 * 3], colorOfClu[193 * 3 + 1], colorOfClu[193 * 3 + 2]);
	printf("index957:%f,%f,%f\n", colorOfClu[957 * 3], colorOfClu[957 * 3 + 1], colorOfClu[957 * 3 + 2]);
	printf("index599:%f,%f,%f\n", colorOfClu[599 * 3], colorOfClu[599 * 3 + 1], colorOfClu[599 * 3 + 2]);
	printf("index31:%f,%f,%f\n", colorOfClu[31 * 3], colorOfClu[31 * 3 + 1], colorOfClu[31 * 3 + 2]);
	printf("index35:%f,%f,%f\n", colorOfClu[35 * 3], colorOfClu[35 * 3 + 1], colorOfClu[35 * 3 + 2]);
	printf("index1032:%f,%f,%f\n", colorOfClu[1032 * 3], colorOfClu[1032 * 3 + 1], colorOfClu[1032 * 3 + 2]);
	printf("index360:%f,%f,%f\n", colorOfClu[360 * 3], colorOfClu[360 * 3 + 1], colorOfClu[360 * 3 + 2]);
	printf("index392:%f,%f,%f\n", colorOfClu[392 * 3], colorOfClu[392 * 3 + 1], colorOfClu[392 * 3 + 2]);
	printf("index652:%f,%f,%f\n", colorOfClu[652 * 3], colorOfClu[652 * 3 + 1], colorOfClu[652 * 3 + 2]);*/
	
	/*int body = 41; int head = 51; int tail = 560;
	int leftbrow = 248; int rightbrow = 283;
	int frontleftleg = 7; int frontrightleg = 90;
	int middleleftleg = 476; int middlerightleg = 352;
	int backleftleg = 436; int backrightleg = 316;*/
	////body
	//colorOfClu[body*3] = 0.230398;
	//colorOfClu[body*3+1] = 0.112455;
	//colorOfClu[body*3+2] = 0.081671;
	////head
	//colorOfClu[head*3] = 0.261049;
	//colorOfClu[head*3+1] = 0.332928;
	//colorOfClu[head*3+2] = 0.012434;
	////tail
	//colorOfClu[tail * 3] = 0.074205;
	//colorOfClu[tail * 3 + 1] = 0.286151;
	//colorOfClu[tail * 3 + 2] = 0.121462;
	////leftbrow
	//colorOfClu[leftbrow * 3] = 0.009119;
	//colorOfClu[leftbrow * 3 + 1] = 0.074103;
	//colorOfClu[leftbrow * 3 + 2] = 0.118351;
	////rightbrow
	//colorOfClu[rightbrow * 3] = 0.222431;
	//colorOfClu[rightbrow * 3 + 1] = 0.234641;
	//colorOfClu[rightbrow * 3 + 2] = 0.260090;
	////frontleftleg
	//colorOfClu[frontleftleg * 3] = 0.078948;
	//colorOfClu[frontleftleg * 3 + 1] = 0.221901;
	//colorOfClu[frontleftleg * 3 + 2] = 0.149348;
	////frontrightleg
	//colorOfClu[frontrightleg * 3] = 0.021349;
	//colorOfClu[frontrightleg * 3 + 1] = 0.198390;
	//colorOfClu[frontrightleg * 3 + 2] = 0.259865;
	////middleleftleg
	//colorOfClu[middleleftleg * 3] = 0.19557;
	//colorOfClu[middleleftleg * 3 + 1] = 0.226492;
	//colorOfClu[middleleftleg * 3 + 2] = 0.025808;
	////middlerightleg
	//colorOfClu[middlerightleg * 3] = 0.073165;
	//colorOfClu[middlerightleg * 3 + 1] = 0.044176;
	//colorOfClu[middlerightleg * 3 + 2] = 0.035302;
	////backleftleg
	//colorOfClu[backleftleg * 3] = 0.092351;
	//colorOfClu[backleftleg * 3 + 1] = 0.173043;
	//colorOfClu[backleftleg * 3 + 2] = 0.331592;
	////backrightleg
	//colorOfClu[backrightleg * 3] = 0.048266;
	//colorOfClu[backrightleg * 3 + 1] = 0.319138;
	//colorOfClu[backrightleg * 3 + 2] = 0.202490;

	///////////////////Teddy
	//int body = 1; int head = 410;
	//int lefthand = 476; int righthand = 605;
	//int leftleg = 101; int rightleg = 280;
	//int leftear = 0; int rightear = 0;
	////body
	//colorOfClu[body*3] = 0.232693;
	//colorOfClu[body*3+1] = 0.097043;
	//colorOfClu[body*3+2] = 0.145952;
	////head
	//colorOfClu[head*3] = 0.308254;
	//colorOfClu[head*3+1] = 0.040433;
	//colorOfClu[head*3+2] = 0.222166;
	////lefthand
	//colorOfClu[lefthand * 3] = 0.151847;
	//colorOfClu[lefthand * 3 + 1] = 0.201970;
	//colorOfClu[lefthand * 3 + 2] = 0.233070;
	////righthand
	//colorOfClu[righthand * 3] = 0.187915;
	//colorOfClu[righthand * 3 + 1] = 0.090678;
	//colorOfClu[righthand * 3 + 2] = 0.170513;
	////leftleg
	//colorOfClu[leftleg * 3] = 0.270300;
	//colorOfClu[leftleg * 3 + 1] = 0.195524;
	//colorOfClu[leftleg * 3 + 2] = 0.160385;
	////rightleg
	//colorOfClu[rightleg * 3] = 0.016473;
	//colorOfClu[rightleg * 3 + 1] = 0.026612;
	//colorOfClu[rightleg * 3 + 2] = 0.112853;
	////leftear
	//colorOfClu[leftear * 3] = 0.253042;
	//colorOfClu[leftear * 3 + 1] = 0.138302;
	//colorOfClu[leftear * 3 + 2] = 0.093391;
	////rightear
	//colorOfClu[rightear * 3] = 0.257693;
	//colorOfClu[rightear * 3 + 1] = 0.016555;
	//colorOfClu[rightear * 3 + 2] = 0.070533;
	/////////////////////////////////////////////////////



}

void Cluster::Hierar(GLMmodel *meshmodel, IndexList **neighborvertices,int numcluster,float *normalofvertices,float w)
{
	int i,j;
	float energy,minenergy=0;
	_cluster *pi,*pimin,*pjmin,*temp;
	IndexList *pnc,*pil,*pindexlist;
	clock_t start,stop;
	double time;
	start=clock();
	
	numCluster=meshmodel->numvertices;
	pcluster=(_cluster*) malloc ((numCluster+1) * sizeof(_cluster));
	cluOfV=(int*) malloc ((meshmodel->numvertices+1) * sizeof(int));
	
	neighborclusters= new IndexList* [meshmodel->numvertices+1];
	for(i=1;i<=(int)meshmodel->numvertices;i++)
	{
		neighborclusters[i]=new IndexList;
		pil=neighborclusters[i];
		pil->index=neighborvertices[i]->index;
		for(pindexlist=neighborvertices[i]->next;pindexlist!=NULL;pindexlist=pindexlist->next)
		{
			pil->next=new IndexList;
			pil=pil->next;
			pil->index=pindexlist->index;
		}
		pil->next=NULL;
	}
	
	for(i=1;i<=numCluster;i++)
	{
		pcluster[i].index=i;
		pcluster[i].order=1;
		pcluster[i].iscluster=1;
		pcluster[i].variation=0;
		pcluster[i].center[0]=meshmodel->vertices[3*i];
		pcluster[i].center[1]=meshmodel->vertices[3*i+1];
		pcluster[i].center[2]=meshmodel->vertices[3*i+2];
		pcluster[i].normal[0]=normalofvertices[3*i];
		pcluster[i].normal[1]=normalofvertices[3*i+1];
		pcluster[i].normal[2]=normalofvertices[3*i+2];
		pcluster[i-1].nextcluster=&(pcluster[i]);
		pcluster[i].pvertices= new IndexList;
		pcluster[i].pvertices->index=i;
		pcluster[i].pvertices->next=NULL;
		cluOfV[i]=i;
	}
	pcluster[numCluster].nextcluster=NULL;

	while(numCluster>numcluster)
	{
		minenergy=INFINITE;
		for(pi=(pcluster+1);pi!=NULL;pi=pi->nextcluster)
		{
			if (pi->iscluster==1)
			{
				for(pnc=neighborclusters[pi->index];pnc!=NULL;pnc=pnc->next)
				{
					if((pnc->index>pi->index)&&(pcluster[pnc->index].iscluster==1))
					{
						energy=w*glmDist(pi->center,pcluster[pnc->index].center)-(1-w)*Dot(pi->normal,pcluster[pnc->index].normal);
						if(energy<minenergy)
						{
							pimin=pi;
							pjmin=pcluster+pnc->index;
							minenergy=energy;
						}
					}
				}
			}
		}
		Merge(pimin,pjmin,meshmodel,normalofvertices);
	}

	maxsize=meshmodel->numvertices/numCluster;
	Distribute(meshmodel, normalofvertices);

	temp=pcluster;
	pcluster= new _cluster [numCluster+1];
	for (i=1,pi=temp+1;pi!=NULL;pi=pi->nextcluster)
	{	
		if (pi->iscluster==1)
		{
			pcluster[i].index=i;
			for (j=0;j<3;j++)
			{
				pcluster[i].center[j]=pi->center[j];
				pcluster[i].normal[j]=pi->normal[j];
			}
			pcluster[i].iscluster=1;
			pcluster[i-1].nextcluster=&(pcluster[i]);
			pcluster[i].order=pi->order;
			pcluster[i].pvertices=pi->pvertices;
			for (pil=pcluster[i].pvertices;pil!=NULL;pil=pil->next)
			{
				cluOfV[pil->index]=i;
			}
			i++;
		}
	}
	pcluster[numCluster].nextcluster=NULL;
	delete[] temp;

	for (i=1;i<=(int)meshmodel->numvertices;i++)
	{
		deleteIndexList(neighborclusters[i]);
		neighborclusters[i] = NULL;
	}
	delete[] neighborclusters;
	neighborclusters=NULL;

	Color(numCluster);

	printf("\n%d",numCluster);
	stop=clock();
	time=((double)(stop-start))/CLK_TCK;
	printf("\nTime: %lf",time);
}

void Cluster::Merge(_cluster *pi, _cluster *pj, GLMmodel *meshmodel,float *normalofvertices)
{

	IndexList *ppoint,*pnc,*pncj;
	float sum[3]={0,0,0};
	int j;
	
	for(ppoint=pj->pvertices;ppoint!=NULL;ppoint=ppoint->next)
	{
		cluOfV[ppoint->index]=pi->index;
	}
	for(ppoint=pi->pvertices;ppoint->next!=NULL;ppoint=ppoint->next)
		;
	ppoint->next=pj->pvertices;
	pj->pvertices=NULL;
	pi->order+=pj->order;
	pj->iscluster=0;
	for(ppoint=pi->pvertices;ppoint!=NULL;ppoint=ppoint->next)
	{
		sum[0]+=meshmodel->vertices[3 * (ppoint->index)];
		sum[1]+=meshmodel->vertices[3 * (ppoint->index)+1];
		sum[2]+=meshmodel->vertices[3 * (ppoint->index)+2];
	}
	pi->center[0]=sum[0] / pi->order;
	pi->center[1]=sum[1] / pi->order;
	pi->center[2]=sum[2] / pi->order;
	for (j=0;j<3;j++)
	{
		sum[j]=0;
	}
	for(ppoint=pi->pvertices;ppoint!=NULL;ppoint=ppoint->next)
	{
		sum[0]+=normalofvertices[3 * (ppoint->index)];
		sum[1]+=normalofvertices[3 * (ppoint->index)+1];
		sum[2]+=normalofvertices[3 * (ppoint->index)+2];
	}
	pi->normal[0]=sum[0] / pi->order;
	pi->normal[1]=sum[1] / pi->order;
	pi->normal[2]=sum[2] / pi->order;
	
	for(pnc=neighborclusters[pj->index];pnc!=NULL;pnc=pnc->next)
	{
		if(pnc->index!=pi->index)
			insertneighbor(pi->index,&(neighborclusters[pnc->index]));
		deleteneighbor(&(neighborclusters[pnc->index]),pj->index);
	}
	deleteneighbor(&neighborclusters[pj->index],pi->index);
	if(neighborclusters[pi->index]==NULL)
	{
		neighborclusters[pi->index]=neighborclusters[pj->index];
	}
	else
	{
		for(pncj=neighborclusters[pj->index];pncj!=NULL;pncj=pncj->next)
		{
			insertneighbor(pncj->index,&(neighborclusters[pi->index]));
		}
	}

	numCluster--;
}

void Cluster::insertneighbor(int index, IndexList **clusteri)
{
	int  isIn=0;
	IndexList *pi=NULL, *pj=*clusteri;
	if(*clusteri==NULL)
	{
		(*clusteri)=new IndexList;
		(*clusteri)->index=index;
		(*clusteri)->next=NULL;
	}
	else
	{
		while(pj!=NULL)
		{
			if(pj->index==index)
			{
				return;
			}
			pi=pj;
			pj=pj->next;
		}
		pj=new IndexList;
		pj->index=index;
		pj->next=NULL;
		pi->next=pj;
	}
}

void Cluster::deleteneighbor(IndexList **clusteri, int index)
{

	IndexList *pi=NULL, *pj=*clusteri;
	if(*clusteri!=NULL)
	{
		if((*clusteri)->index==index)
		{
			*clusteri=(*clusteri)->next;
		}
		else
			while(pj!=NULL)
			{
				if(pj->index==index)
				{
					pi->next=pj->next;
					pj=pj->next;
				}
				else
				{
					pi=pj;
					pj=pj->next;
				}
			}
	}
}

void Cluster::AdvancedHierar(GLMmodel *meshmodel,float *normalofvertices,IndexList **verticesvindices,
		                      int *numofneighboringvertices, IndexList **verticestindices)
{
	int i;
	clock_t start,stop;
	double time;
	start=clock();

	cluOfV= new int [meshmodel->numvertices+1];
	ptree=new _node;
	
	ptree->order=meshmodel->numvertices;
	ptree->vertices= new int  [ptree->order+1];
	for(i=0;i<=ptree->order;i++)
	{
		ptree->vertices[i]=i;
	}
	ptree->pleft=NULL;
	ptree->pright=NULL;
	numCluster=1;

	if (meancurvature!=NULL)
	{
		delete[] meancurvature;
	}
	ComputeMeanCurvature(meshmodel,verticesvindices,numofneighboringvertices,verticestindices);	

	Split(ptree, meshmodel,normalofvertices);

	pcluster=new _cluster[numCluster+1];

	numCluster=0;
	Tree2Cluster(ptree);
	pcluster[numCluster].nextcluster=NULL;

	Color(numCluster);

	printf("\n%d",numCluster);
	stop=clock();
	time=((double)(stop-start))/CLK_TCK;
	printf("\nTime: %lf",time);
}


void Cluster::Split(_node *pnode, GLMmodel *meshmodel,float *normalofvertices)
{
	int i,j,k,flag;
	float *differencematrix,side,angle,normal[3],curvature=0;
	double variation,sum,planenor[3],x[3],comatrix[9];

	flag=0;
	for(j=0;j<3;j++)
	{
		pnode->center[j]=0;
		pnode->normal[j]=0;
	}

	for(i=1;i<=pnode->order;i++)
	{
		for(j=0;j<3;j++)
		{
			pnode->center[j]+=meshmodel->vertices[3*(pnode->vertices[i])+j];
			pnode->normal[j]+=normalofvertices[3*(pnode->vertices[i])+j];
		}
		curvature+=100*(meancurvature[pnode->vertices[i]]);
	}
	for(i=0;i<3;i++)
	{
		pnode->center[i]/=pnode->order;
		pnode->normal[i]/=pnode->order;
	}
	Normalize(pnode->normal);
	curvature/=pnode->order;
	differencematrix= new float [3*(pnode->order+1)];

	for(i=1;i<=pnode->order;i++)
	{
		for(j=0;j<3;j++)
		{
			differencematrix[3*i+j]=10*(meshmodel->vertices[3*(pnode->vertices[i])+j]-pnode->center[j]);
			normal[j]=normalofvertices[3*(pnode->vertices[i])+j];
		}
		angle=acos(Dot(normal,pnode->normal));
		if (angle>=maxangle)
		{
			flag=1;
		}
	}
	
	for(i=0;i<3;i++)
	{
		for(j=0;j<3;j++)
		{
			sum=0;
			for(k=1;k<= pnode->order;k++)
				sum += differencematrix[3*k+i]*differencematrix[3*k+j];
			comatrix[3*i+j]=sum;
		}
	}
	
	gsl_matrix_view m = gsl_matrix_view_array(comatrix, 3, 3);
	gsl_vector *eval = gsl_vector_alloc (3);
	gsl_matrix *evec = gsl_matrix_alloc (3, 3);
	gsl_eigen_symmv_workspace * w = gsl_eigen_symmv_alloc (3);
    gsl_eigen_symmv (&m.matrix, eval, evec, w);
	gsl_eigen_symmv_free (w);
	gsl_eigen_symmv_sort (eval, evec,GSL_EIGEN_SORT_ABS_DESC);

	sum=0;
	for(i=0;i<3;i++)
		sum += gsl_vector_get(eval, i);
	if (sum==0)
	{
		variation=0;	
	}
	else
		variation=gsl_vector_get(eval, 2)/sum;

	pnode->variation=variation;

	if((pnode->order<=maxsize)&&(pnode->variation<=maxvar)&&(flag==0)&&(curvature<=maxcurvature))
		return;

	if (pnode->order==1)
	{
		return;
	}

	pnode->pleft = new _node;
	pnode->pleft->order=0;
	pnode->pleft->pleft=NULL;
	pnode->pleft->pright=NULL;
	pnode->pleft->vertices= new int [pnode->order+1];
	pnode->pright = new _node;
	pnode->pright->order=0;
	pnode->pright->pleft=NULL;
	pnode->pright->pright=NULL;
	pnode->pright->vertices= new int [pnode->order+1];

	for(i=0;i<3;i++)
	{
		planenor[i]= gsl_matrix_get(evec, i, 0);
	}

	for(i=1;i<=pnode->order;i++)
	{
		for(j=0;j<3;j++)
			x[j]=differencematrix[3*i+j];
		side=Dot(planenor, x);
		if(side<0)
		{
			pnode->pleft->order++;
			pnode->pleft->vertices[pnode->pleft->order]=pnode->vertices[i];
		}
		else
		{
			pnode->pright->order++;
			pnode->pright->vertices[pnode->pright->order]=pnode->vertices[i];
		}
	}

	delete[] differencematrix;
	differencematrix=NULL;
	gsl_vector_free(eval);
	gsl_matrix_free(evec);
	
	numCluster++;

	Split(pnode->pleft, meshmodel,normalofvertices);
	Split(pnode->pright, meshmodel,normalofvertices);
}

void Cluster::Tree2Cluster(_node *pnode)
{
	
	int i;
	IndexList *pp;
	
	if(pnode->pleft!=NULL)
		Tree2Cluster(pnode->pleft);
	if(pnode->pright!=NULL)
		Tree2Cluster(pnode->pright);
	if(pnode->pleft==NULL&&pnode->pright==NULL)
		{
			numCluster++;
			for (i=0;i<3;i++)
			{
				pcluster[numCluster].center[i]=pnode->center[i];
				pcluster[numCluster].normal[i]=pnode->normal[i];
			}
			pcluster[numCluster].index=numCluster;
			pcluster[numCluster].iscluster=1;
			pcluster[numCluster-1].nextcluster=&(pcluster[numCluster]);
			pcluster[numCluster].order=pnode->order;
			pcluster[numCluster].variation=pnode->variation;
			pcluster[numCluster].pvertices=new IndexList;
			pp=pcluster[numCluster].pvertices;
			pp->index=pnode->vertices[1];
			pp->next=NULL;
			cluOfV[(pnode->vertices[1])]=numCluster;
			for(i=2;i<=pnode->order;i++)
			{
				cluOfV[pnode->vertices[i]]=numCluster;
				pp->next=new IndexList;
				pp=pp->next;
				pp->index=pnode->vertices[i];
				pp->next=NULL;
			}
		}
}

void Cluster::Regiongrow(GLMmodel *meshmodel,IndexList **neighborvertices,float *normalofvertices,
						 int *numofneighboringvertices,IndexList **verticestindices)
{
	int i,seed,complete,k=0,j;
	IndexList *pindexlist,*pil;
	_cluster *temp,*pi;
	clock_t start,stop;
	double time;
	start=clock();

	numCluster=0;
	pcluster=(_cluster*) malloc ((meshmodel->numvertices+1) * sizeof(_cluster));
	cluOfV=(int*) malloc ((meshmodel->numvertices+1) * sizeof(int));

	neighborclusters= new IndexList* [meshmodel->numvertices+1];
	for(i=1;i<=(int)meshmodel->numvertices;i++)
	{
		neighborclusters[i]=new IndexList;
		pil=neighborclusters[i];
		pil->index=neighborvertices[i]->index;
		for(pindexlist=neighborvertices[i]->next;pindexlist!=NULL;pindexlist=pindexlist->next)
		{
			pil->next=new IndexList;
			pil=pil->next;
			pil->index=pindexlist->index;
		}
		pil->next=NULL;
	}

	for(i=1;i<=(int)meshmodel->numvertices;i++)
	{
		pcluster[i].index=i;
		pcluster[i].order=1;
		pcluster[i].iscluster=0;
		pcluster[i].variation=0;
		for (j=0;j<3;j++)
		{
			pcluster[i].center[j]=meshmodel->vertices[3*i+j];
			pcluster[i].normal[j]=normalofvertices[3*i+j];
		}
		pcluster[i-1].nextcluster=&(pcluster[i]);
		pcluster[i].pvertices= new IndexList;
		pcluster[i].pvertices->index=i;
		pcluster[i].pvertices->next=NULL;
	}
	pcluster[meshmodel->numvertices].nextcluster=NULL;
	
	for(i=1;i<=(int)meshmodel->numvertices;i++)
		cluOfV[i]=-1;

	if (meancurvature!=NULL)
	{
		delete[] meancurvature;
	}
	ComputeMeanCurvature(meshmodel,neighborvertices,numofneighboringvertices,verticestindices);	

	while(1)
	{
		complete=0;
		for(seed=1;seed<=(int)meshmodel->numvertices;seed++)
		{
			if(cluOfV[seed]==-1)
			{
				numCluster++;
				cluOfV[seed]=seed;
				pcluster[seed].iscluster=1;
				Growfrom(pcluster+seed, meshmodel,neighborvertices,normalofvertices);
				complete=1;
			}
		}
		if(complete==0)
			break;
	}
	
	Distribute(meshmodel, normalofvertices);

	temp=pcluster;
	pcluster= new _cluster [numCluster+1];
	for (i=1,pi=temp+1;pi!=NULL;pi=pi->nextcluster)
	{	
		if (pi->iscluster==1)
		{
			pcluster[i].index=i;
			for (j=0;j<3;j++)
			{
				pcluster[i].center[j]=pi->center[j];
			}
			pcluster[i].iscluster=1;
			pcluster[i-1].nextcluster=&(pcluster[i]);
			pcluster[i].order=pi->order;
			pcluster[i].pvertices=pi->pvertices;
			for (pil=pcluster[i].pvertices;pil!=NULL;pil=pil->next)
			{
				cluOfV[pil->index]=i;
			}
			i++;
		}
	}
	pcluster[numCluster].nextcluster=NULL;
	delete[] temp;

	for (i=1;i<=(int)meshmodel->numvertices;i++)
	{
		deleteIndexList(neighborclusters[i]);
		neighborclusters[i] = NULL;
	}
	delete[] neighborclusters;
	neighborclusters=NULL;

	Color(numCluster);

	printf("\n%d",numCluster);
	stop=clock();
	time=((double)(stop-start))/CLK_TCK;
	printf("\nTime: %lf",time);
}

void Cluster::Growfrom(_cluster *pi, GLMmodel *meshmodel,IndexList **neighborvertices,float *normalofvertices)
{
	IndexList *pnc,*pncj;
	float energy,minenergy, *differencematrix,sum[3]={0,0,0},x[3],normal[3],curvature;
	_cluster *pjmin;
	IndexList *ppoint,*pp;
	int i,j,k,seed,flag;
	double variation, sumv, comatrix[9],angle;

	flag=0;
	for(pp=pi->pvertices;pp!=NULL;pp=pp->next)
	{
		while(1)
		{
			differencematrix= new float [3*(pi->order+1)];
			for(i=1,ppoint=pi->pvertices;i<=pi->order;i++,ppoint=ppoint->next)
			{
				for(j=0;j<3;j++)
				{
					differencematrix[3*i+j]=10*(meshmodel->vertices[3*(ppoint->index)+j]-pi->center[j]);
					normal[j]=normalofvertices[3*(ppoint->index)+j];
				}
				angle=acos(Dot(normal,pi->normal));
				if (angle>=maxangle)
				{
					flag=1;
				}
			}
			for(i=0;i<3;i++)
			{
				for(j=0;j<3;j++)
				{
					sumv=0;
					for(k=1;k<= pi->order;k++)
						sumv += differencematrix[3*k+i]*differencematrix[3*k+j];
					comatrix[3*i+j]=sumv;
				}
			}
			delete[] differencematrix;
			differencematrix=NULL;
			gsl_matrix_view m = gsl_matrix_view_array(comatrix, 3, 3);
			gsl_vector *eval = gsl_vector_alloc (3);
			gsl_matrix *evec = gsl_matrix_alloc (3, 3);
			gsl_eigen_symmv_workspace * w = gsl_eigen_symmv_alloc (3);
			gsl_eigen_symmv (&m.matrix, eval, evec, w);
			gsl_eigen_symmv_free (w);
			gsl_eigen_symmv_sort (eval, evec, GSL_EIGEN_SORT_ABS_DESC);
			sumv=0;
			for(i=0;i<3;i++)
				sumv += gsl_vector_get(eval, i);
			if (sumv==0)
			{
				variation=0;
			}
			else
				variation=gsl_vector_get(eval, 2)/sumv;
			gsl_vector_free(eval);
			gsl_matrix_free(evec);
			pi->variation=variation;

			if(variation>=maxvar)
				return;
			
			if(pi->order>=maxsize)
				return;

			if (flag==1)
			{
				return;
			}

			curvature=0;
			for (ppoint=pi->pvertices;ppoint!=NULL;ppoint=ppoint->next)
			{
				curvature+=100*(meancurvature[ppoint->index]);
			}
			curvature/=pi->order;
			if (curvature>=maxcurvature)
			{
				return;
			}

			seed=pp->index;
			for(i=0;i<3;i++)
			{
				x[i]=meshmodel->vertices[3*seed+i];
				normal[i]=normalofvertices[3*seed+i];
			}
			minenergy=INFINITE;
			pjmin=NULL;
			for(pnc=neighborvertices[seed];pnc!=NULL;pnc=pnc->next)
			{
				if(cluOfV[pnc->index]==-1)
				{
					energy=0.8*glmDist(x, pcluster[pnc->index].center)-0.2*Dot(normal,pcluster[pnc->index].normal);
					if(energy<minenergy)
					{
						pjmin=pcluster+(pnc->index);
						minenergy=energy;
					}
				}				
			}
			if(pjmin==NULL)
				break;
			
			cluOfV[pjmin->index]=pi->index;
			for(ppoint=pi->pvertices;ppoint->next!=NULL;ppoint=ppoint->next)
				;
			ppoint->next=pjmin->pvertices;
			pjmin->pvertices=NULL;
			pi->order++;
			for(i=0;i<3;i++)
			{
				pi->center[i]=0;
				pi->normal[i]=0;
			}
			for(ppoint=pi->pvertices;ppoint!=NULL;ppoint=ppoint->next)
			{
				for (i=0;i<3;i++)
				{
					pi->center[i]+=meshmodel->vertices[3 * (ppoint->index)+i];
					pi->normal[i]+=normalofvertices[3 * (ppoint->index)+i];
				}
			}
			for (i=0;i<3;i++)
			{
				pi->center[i]/=pi->order;
				pi->normal[i]/=pi->order;
			}
			Normalize(pi->normal);
			
			for(pnc=neighborclusters[pjmin->index];pnc!=NULL;pnc=pnc->next)
			{
				if(pnc->index!=pi->index)
					insertneighbor(pi->index,&(neighborclusters[pnc->index]));
				deleteneighbor(&(neighborclusters[pnc->index]),pjmin->index);
			}
			deleteneighbor(&neighborclusters[pjmin->index],pi->index);
			if(neighborclusters[pi->index]==NULL)
			{
				neighborclusters[pi->index]=neighborclusters[pjmin->index];
			}
			else
			{
				for(pncj=neighborclusters[pjmin->index];pncj!=NULL;pncj=pncj->next)
				{
					insertneighbor(pncj->index,&(neighborclusters[pi->index]));
				}
			}
		}
	}
}

void Cluster::Distribute(GLMmodel *meshmodel,float *normalofvertices)
{
	_cluster *pi,*pjmin;
	IndexList *pp,*pnc,*ppoint;
	float energy,minenergy,sum[3],x[3],normal[3];
	int i;

	for(pi=pcluster+1;pi!=NULL;pi=pi->nextcluster)
	{
		if((pi->iscluster==1)&&pi->order<maxsize/3)
		{	
			minenergy=INFINITE;
			for(pnc=neighborclusters[pi->index];pnc!=NULL;pnc=pnc->next)
			{
				energy=glmDist(pi->center, pcluster[pnc->index].center);
				if(energy<minenergy)
				{
					pjmin=pcluster+pnc->index;
					minenergy=energy;
				}
				deleteneighbor(&(neighborclusters[pnc->index]),pi->index);
			}
			for(pp=pi->pvertices;pp!=NULL;pp=pp->next)
			{
				cluOfV[pp->index]=pjmin->index;
			}
			for(pp=pjmin->pvertices;pp->next!=NULL;pp=pp->next)
				;
			pp->next=pi->pvertices;
			pjmin->order+=pi->order;
			pi->iscluster=0;
			pi->pvertices=NULL;
			for(pnc=neighborclusters[pi->index];pnc!=NULL;pnc=pnc->next)
			{
				if(pnc->index!=pjmin->index)
				{
					insertneighbor(pjmin->index,&(neighborclusters[pnc->index]));
					insertneighbor(pnc->index,&(neighborclusters[pjmin->index]));
				}
			}
			neighborclusters[pi->index]=NULL;
            for(i=0;i<3;i++)
			{
				sum[i]=0;
			}
			for(pp=pjmin->pvertices;pp!=NULL;pp=pp->next)
			{
				sum[0]+=meshmodel->vertices[3 * (pp->index)];
				sum[1]+=meshmodel->vertices[3 * (pp->index)+1];
				sum[2]+=meshmodel->vertices[3 * (pp->index)+2];
			}
			pjmin->center[0]=sum[0] / pjmin->order;
			pjmin->center[1]=sum[1] / pjmin->order;
			pjmin->center[2]=sum[2] / pjmin->order;
			numCluster--;
		}
	}

	for(pi=pcluster+1;pi!=NULL;pi=pi->nextcluster)
	{
		if((pi->iscluster==1))
		{
			for(ppoint=pi->pvertices;ppoint!=NULL;ppoint=ppoint->next)
			{
				for(i=0;i<3;i++)
				{
					x[i]=meshmodel->vertices[3*(ppoint->index)+i];
					normal[i]=normalofvertices[3*(ppoint->index)+i];
				}
				energy=weight*glmDist(x, pi->center)-(1-weight)*Dot(normal,pi->normal);
				pjmin=pi;
				minenergy=energy;
			
				for(pnc=neighborclusters[pi->index];pnc!=NULL;pnc=pnc->next)
				{
					energy=weight*glmDist(x, pcluster[pnc->index].center)-(1-weight)*Dot(normal,pcluster[pnc->index].normal);
					if(energy<minenergy)
					{
						pjmin=pcluster+pnc->index;
						minenergy=energy;
					}
				}

				cluOfV[ppoint->index]=pjmin->index;
				if(pjmin!=pi)
				{
					if(pi->pvertices->index==ppoint->index)
						pi->pvertices=ppoint->next;
					else
					{
						for(pp=pi->pvertices;pp->next!=ppoint;pp=pp->next)
							;
						pp->next=ppoint->next;
					}
					pi->order--;
					if (pjmin->pvertices==NULL)
					{
						pjmin->pvertices=new IndexList;
						pjmin->pvertices->index=ppoint->index;
						pjmin->pvertices->next=NULL;
					}
					else
					{
						for(pp=pjmin->pvertices;pp->next!=NULL;pp=pp->next)
							;
						pp->next=new IndexList;
						pp=pp->next;
						pp->index=ppoint->index;
						pp->next=NULL;
					}
					(pjmin->order)++;
					for(i=0;i<3;i++)
						sum[i]=0;
					for(pp=pjmin->pvertices;pp!=NULL;pp=pp->next)
					{
						sum[0]+=meshmodel->vertices[3 * (pp->index)];
						sum[1]+=meshmodel->vertices[3 * (pp->index)+1];
						sum[2]+=meshmodel->vertices[3 * (pp->index)+2];
					}
					pjmin->center[0]=sum[0] / pjmin->order;
					pjmin->center[1]=sum[1] / pjmin->order;
					pjmin->center[2]=sum[2] / pjmin->order;
					for(i=0;i<3;i++)
						sum[i]=0;
					for(pp=pjmin->pvertices;pp!=NULL;pp=pp->next)
					{
						sum[0]+=normalofvertices[3 * (pp->index)];
						sum[1]+=normalofvertices[3 * (pp->index)+1];
						sum[2]+=normalofvertices[3 * (pp->index)+2];
					}
					pjmin->normal[0]=sum[0] / pjmin->order;
					pjmin->normal[1]=sum[1] / pjmin->order;
					pjmin->normal[2]=sum[2] / pjmin->order;
				}
			}
			if (pi->order==0)
			{
				pi->iscluster=0;
				for (pnc=neighborclusters[pi->index];pnc!=NULL;pnc=pnc->next)
				{
					deleteneighbor(&(neighborclusters[pnc->index]),pi->index);
				}
			}
		}
	}
}

void Cluster::deleteIndexList(IndexList *pp)
{
	if (pp==NULL)
	{
		return;
	}
	else
	{
		deleteIndexList(pp->next);
		delete pp;
	}
}

void Cluster::GetNeighbors(IndexList **neighborvertices,int numcluster)
{
	_cluster *pi;
	IndexList *pp,*ppoint;
	int i;

	neighborclusters= new IndexList* [numcluster+1];
	for (i=0;i<=numcluster;i++)
	{
		neighborclusters[i]=NULL;
	}
	for (pi=pcluster+1;pi!=NULL;pi=pi->nextcluster)
	{
		if (pi->iscluster==1)
		{
			for (ppoint=pi->pvertices;ppoint!=NULL;ppoint=ppoint->next)
			{
				for (pp=neighborvertices[ppoint->index];pp!=NULL;pp=pp->next)
				{
					if (cluOfV[pp->index]!=pi->index)
					{
						insertneighbor(cluOfV[pp->index],&(neighborclusters[pi->index]));
					}
				}
			}
		}
	}
}

void Cluster::GenerateNewModel(IndexList **neighborvertices)
{
	struct temptriangles 
	{
		int vindex[3];
		temptriangles *next;
	} *ptriangles,*phead;
	
	int i,j,k;
	_cluster *pc;
	IndexList *pp1,*pp2;
	float matrix[3][3],det;

	phead=new temptriangles;
	ptriangles=phead;
	ptriangles->next=NULL;

	if (pnewmodel)
	{
		glmDelete(pnewmodel);
		pnewmodel= NULL;
	}
    
	pnewmodel= new GLMmodel;
	
	pnewmodel->pathname = NULL;
    pnewmodel->mtllibname = NULL;

	pnewmodel->numvertices = 0;
    pnewmodel->vertices = NULL;

    pnewmodel->numnormals = 0;
    pnewmodel->normals = NULL;

    pnewmodel->numtexcoords = 0;
    pnewmodel->texcoords = NULL;

    pnewmodel->numfacetnorms = 0;
    pnewmodel->facetnorms = NULL;

    pnewmodel->numtriangles = 0;
    pnewmodel->triangles = NULL;

    pnewmodel->nummaterials = 0;
    pnewmodel->materials = NULL;

    pnewmodel->numgroups = 0;
    pnewmodel->groups = NULL;

	pnewmodel->numvertices=numCluster;
	pnewmodel->vertices= new float [3*(numCluster+1)];

	GetNeighbors(neighborvertices,numCluster);

	for (k=1,pc=pcluster+1;pc!=NULL;pc=pc->nextcluster,k++)
	{
		if (pc->iscluster==1)
		{
			for (j=0;j<3;j++)
			{
				pnewmodel->vertices[3*k+j]=pc->center[j];
			}

			for (pp1=neighborclusters[pc->index];pp1!=NULL;pp1=pp1->next)
			{
				if (pp1->index>pc->index)
				{
					for (pp2=neighborclusters[pp1->index];pp2!=NULL;pp2=pp2->next)
					{
						if (pp2->index>pp1->index&&(IsInList(pp2->index,neighborclusters[pc->index])))
						{
							for (i=0;i<3;i++)
							{
								matrix[0][i]=(pc->normal[i]+pcluster[pp1->index].normal[i]+pcluster[pp2->index].normal[i])/3;
								matrix[1][i]=pcluster[pp1->index].center[i]-pc->center[i];
								matrix[2][i]=pcluster[pp2->index].center[i]-pc->center[i];
							}
							ptriangles->next=new temptriangles;
							ptriangles=ptriangles->next;
							ptriangles->next=NULL;
							det=Det(matrix);
							if (det>=0)
							{
								ptriangles->vindex[0]=pc->index;
								ptriangles->vindex[1]=pp1->index;
								ptriangles->vindex[2]=pp2->index;
							}
							else
							{
								ptriangles->vindex[0]=pc->index;
								ptriangles->vindex[1]=pp2->index;
								ptriangles->vindex[2]=pp1->index;
							}
							pnewmodel->numtriangles++;
						}
					}
				}
			}
		}
	}

	pnewmodel->triangles= new GLMtriangle [pnewmodel->numtriangles];
	for (i=0,ptriangles=phead->next;ptriangles!=NULL;ptriangles=ptriangles->next,i++)
	{
		for (j=0;j<3;j++)
		{
			pnewmodel->triangles[i].vindices[j]=ptriangles->vindex[j];
		}
	}
}

bool Cluster::IsInList(int index, IndexList *pl)
{
	bool flag = false;

	while (pl)
	{
		if (pl->index == index)
		{
			flag = true;
			break;
		}
		pl = pl->next;
	}

	return flag;
}

void Cluster::GetNormals(float *normalofvertices)
{
	_cluster *pc;
	IndexList *pp;
	float sum[3];
	int i;

	for (pc=pcluster+1;pc!=NULL;pc=pc->nextcluster)
	{
		if (pc->iscluster==1)
		{
			for (i=0;i<3;i++)
			{
				sum[i]=0;
			}
			for (pp=pc->pvertices;pp!=NULL;pp=pp->next)
			{
				for (i=0;i<3;i++)
				{
					sum[i]+=normalofvertices[3*(pp->index)+i];
				}
			}
			for (i=0;i<3;i++)
			{
				pc->normal[i]=sum[i] / pc->order;
			}
		}
	}
}

void Cluster::DeleteClusters()
{
	int i;

	delete[] cluOfV;
 	cluOfV=NULL;
	delete[] colorOfClu;
 	colorOfClu=NULL;

	if (pcluster!=NULL)
	{
		for (i=1;i<=numCluster;i++)
		{
			deleteIndexList(pcluster[i].pvertices);
		}
		pcluster[i].pvertices=NULL;
		delete[] pcluster;
		pcluster=NULL;
	}

	if (neighborclusters!=NULL)
	{
		for (i=1;i<=numCluster;i++)
		{
			deleteIndexList(neighborclusters[i]);
			neighborclusters[i]=NULL;
		}
		delete[] neighborclusters;
		neighborclusters=NULL;
	}

	if (pnewmodel)
	{
		glmDelete(pnewmodel);
		pnewmodel=NULL;
	}
}

void Cluster::GMM(GLMmodel *meshmodel,float *normalofvertices)
{
	int i,j,k,maxclusterindex;
	float *w,*o,**b,x[3],*prew,*preo,**preu,sum1[3],sum2,maxprobability,prob,error;
	IndexList *pp;
	clock_t start,stop;
	double time;
	start=clock();	

	cluOfV= new int [meshmodel->numvertices+1];
	pcluster=new _cluster [numCluster+1];
	w=new float [numCluster+1];
	o=new float [numCluster+1];
	b=new float* [numCluster+1];
	prew=new float [numCluster+1];
	preo=new float [numCluster+1];
	preu=new float* [numCluster+1];
	for (i=1;i<=numCluster;i++)
	{
		pcluster[i].index=i;
		pcluster[i].iscluster=1;
		pcluster[i].order=0;
		pcluster[i].pvertices=NULL;
		pcluster[i].variation=0;
		pcluster[i-1].nextcluster=&(pcluster[i]);
		w[i]=1.0/numCluster;
		o[i]=1;
		b[i]=new float [meshmodel->numvertices+1];
		preu[i]=new float [3];
	}
	pcluster[numCluster].nextcluster=NULL;

	K_Random(meshmodel,normalofvertices);

	do 
	{
		error=0;
		for (i=1;i<=numCluster;i++)
		{
			for (j=1;j<=(int)meshmodel->numvertices;j++)
			{
				x[0]=meshmodel->vertices[3*j];
				x[1]=meshmodel->vertices[3*j+1];
				x[2]=meshmodel->vertices[3*j+2];
				b[i][j]=PostProbability(i,x,w,o);
			}
		}
	
		for (j=1;j<=numCluster;j++)
		{
			preu[j]=pcluster[j].center;
			for (i=0;i<3;i++)
			{
				sum1[i]=0;
			}
			sum2=0;
			for (i=1;i<=(int)meshmodel->numvertices;i++)
			{
				for (k=0;k<3;k++)
				{
					sum1[k]+=b[j][i]*(meshmodel->vertices[3*i+k]);	
				}
				sum2+=b[j][i];
			}
			for (k=0;k<3;k++)
			{
				pcluster[j].center[k]=sum1[k]/sum2;
			}
	
			preo[j]=o[j];
			for (i=0;i<3;i++)
			{
				sum1[i]=0;
			}
			for (i=1;i<=(int)meshmodel->numvertices;i++)
			{
				for (k=0;k<3;k++)
				{
					x[k]=meshmodel->vertices[3*i+k];
				}
				sum1[0]+=b[j][i]*glmDistSquared(x,pcluster[j].center);
			}
			o[j]=sqrt(sum1[0]/3/sum2);

			prew[j]=w[j];
			w[j]=sum2/numCluster;
	
			error+=((w[j]-prew[j])*(w[j]-prew[j])+(o[j]-preo[j])*(o[j]-preo[j])+
			    glmDistSquared(pcluster[j].center,preu[j]));
	
		}
	printf("\n%f",error);	
	} while(error>1);

	
	for (i=1;i<=(int)meshmodel->numvertices;i++)
	{
		maxprobability=0;
		for (k=0;k<3;k++)
		{
			x[k]=meshmodel->vertices[3*i+k];
		}
		for (j=1;j<=numCluster;j++)
		{
			prob=Gaussian(x,pcluster[j].center,o[j]);
			if (prob>maxprobability)
			{
				maxprobability=prob;
				maxclusterindex=j;
			}
		}
		cluOfV[i]=maxclusterindex;
		pcluster[maxclusterindex].order++;
		if (pcluster[maxclusterindex].pvertices==NULL)
		{
			pcluster[maxclusterindex].pvertices=new IndexList;
			pcluster[maxclusterindex].pvertices->index=i;
			pcluster[maxclusterindex].pvertices->next=NULL;
		}
		else
		{
			for (pp=pcluster[maxclusterindex].pvertices;pp->next!=NULL;pp=pp->next)
				;
			pp->next=new IndexList;
			pp=pp->next;
			pp->index=i;
			pp->next=NULL;
		}
	}

	for(i=1;i<=numCluster;i++)
	{
		for (j=0;j<3;j++)
		{
			sum1[j]=0;
		}
		for (pp=pcluster[i].pvertices;pp!=NULL;pp=pp->next)
		{
			for (j=0;j<3;j++)
			{
				sum1[j]+=meshmodel->vertices[3*(pp->index)+j];
			}				
		}
		for (j=0;j<3;j++)
		{
			pcluster[i].center[j]=sum1[j]/pcluster[i].order;
		}
	}
	
	Color(numCluster);

	stop=clock();
	time=((double)(stop-start))/CLK_TCK;
	printf("\nTime: %lf",time);
}

float Cluster::Gaussian(float *x,float *u,float o)
{
	return (1/sqrt(8*PI*PI*PI)/o/o/o)*exp(-glmDistSquared(x,u)/2/o/o);
}

float Cluster::PostProbability(int j,float *x,float *w,float *o)
{
	int i;
	float sum=0;
	for (i=1;i<=numCluster;i++)
	{
		sum+=(w[i]*Gaussian(x,pcluster[i].center,o[i]));	
	}
	return w[j]*Gaussian(x,pcluster[j].center,o[j])/sum;	
}

/*void Cluster::MeanShift(GLMmodel *meshmodel,IndexList **neighborvertices,float radius,float maxerror)
{
	int i,j,flag,mini,*ismoved;
	float error,x[3],dist,mindist,sum[3];
	_cluster *pi,*temp;
	IndexList *pp,*ppoint,*pnc;
	clock_t start,stop;
	double time;
	start=clock();
	
	numCluster=0;
	pcluster=(_cluster*) malloc ((meshmodel->numvertices+1) * sizeof(_cluster));
	cluOfV=(int*) malloc ((meshmodel->numvertices+1) * sizeof(int));
	neighborclusters= new IndexList*;
	*neighborclusters= new IndexList;
	(*neighborclusters)->index=1;
	(*neighborclusters)->next=NULL;
	ismoved= new int [meshmodel->numvertices+1];
		
	for(i=1;i<=(int)meshmodel->numvertices;i++)
	{
		pcluster[i].index=i;
		pcluster[i].order=1;
		pcluster[i].iscluster=0;
		pcluster[i].variation=0;
		pcluster[i].center[0]=meshmodel->vertices[3*i];
		pcluster[i].center[1]=meshmodel->vertices[3*i+1];
		pcluster[i].center[2]=meshmodel->vertices[3*i+2];
		pcluster[i-1].nextcluster=&(pcluster[i]);
		pcluster[i].pvertices= new IndexList;
		pcluster[i].pvertices->index=i;
		pcluster[i].pvertices->next=NULL;
		cluOfV[i]=i;
		ismoved[i]=0;
	}
	pcluster[meshmodel->numvertices].nextcluster=NULL;

	for (pnc=(*neighborclusters);pnc!=NULL;pnc=pnc->next)
	{
		pi=&(pcluster[pnc->index]);
		do 
		{
			for (j=0;j<3;j++)
			{
				x[j]=pi->center[j];	
			}
			meanshift(pi->center,meshmodel);
			error=glmDist(x,pi->center);
//			printf("\n%d  %f",pi->index,error);
		} while(error>maxerror);
		ismoved[pi->index]=1;

		flag=0;
		mindist=INFINITE;
		for (pp=neighborvertices[pi->index];pp!=NULL;pp=pp->next)
		{
			insertneighbor(pp->index,neighborclusters);
			if (ismoved[pp->index]==1)
			{
				dist=glmDist(pcluster[(pp->index)].center,pi->center);
//				printf("\n%f",dist);
				if ((dist<radius)&&(dist<mindist))
				{
					mindist=dist;
					mini=pp->index;
					flag=1;
				}
			}
		}
		if (flag==0)
		{
			pi->iscluster=1;
			numCluster++;
			printf("\n%d",numCluster);
		}
		else
		{
			mini=cluOfV[mini];
			cluOfV[pi->index]=mini;
			for(ppoint=pcluster[mini].pvertices;ppoint->next!=NULL;ppoint=ppoint->next)
				;
			ppoint->next=pi->pvertices;
			pi->pvertices=NULL;
			pcluster[mini].order+=pi->order;
		}
	}

	temp=pcluster;
	pcluster= new _cluster [numCluster+1];
	for (i=1,pi=temp+1;pi!=NULL;pi=pi->nextcluster)
	{	
		if (pi->iscluster==1)
		{
			pcluster[i].index=i;
			for (j=0;j<3;j++)
			{
				pcluster[i].center[j]=pi->center[j];
			}
			pcluster[i].iscluster=1;
			pcluster[i-1].nextcluster=&(pcluster[i]);
			pcluster[i].order=pi->order;
			pcluster[i].pvertices=pi->pvertices;
			for (pp=pcluster[i].pvertices;pp!=NULL;pp=pp->next)
			{
				cluOfV[pp->index]=i;
			}
			i++;
		}
	}
	pcluster[numCluster].nextcluster=NULL;
	delete[] temp;
	temp=NULL;

	deleteIndexList(*neighborclusters);
	neighborclusters=NULL;

	for(i=1;i<=numCluster;i++)
	{
		for (j=0;j<3;j++)
		{
			sum[j]=0;
		}
		for (pp=pcluster[i].pvertices;pp!=NULL;pp=pp->next)
		{
			for (j=0;j<3;j++)
			{
				sum[j]+=meshmodel->vertices[3*(pp->index)+j];
			}				
		}
		for (j=0;j<3;j++)
		{
			pcluster[i].center[j]=sum[j]/pcluster[i].order;
		}
	}
	
	Color(numCluster);
	
	printf("\n%d",numCluster);
	stop=clock();
	time=((double)(stop-start))/CLK_TCK;
	printf("\nTime: %lf",time);
}*/

/*void Cluster::MeanShift(GLMmodel *meshmodel,IndexList **neighborvertices,float radius,float maxerror)
{
	int i,j,m,mini,*intArray,temp1;
	float error,x[3],dist,mindist,sum[3];
	_cluster *pi,*temp;
	IndexList *pp,*ppoint;
	clock_t start,stop;
	time_t t;
	double time1;
	start=clock();

	numCluster=50;
	
	pcluster=(_cluster*) malloc ((meshmodel->numvertices+1) * sizeof(_cluster));
	cluOfV=(int*) malloc ((meshmodel->numvertices+1) * sizeof(int));
			
	for(i=1;i<=(int)meshmodel->numvertices;i++)
	{
		pcluster[i].index=i;
		pcluster[i].order=1;
		pcluster[i].iscluster=0;
		pcluster[i].variation=0;
		pcluster[i].center[0]=meshmodel->vertices[3*i];
		pcluster[i].center[1]=meshmodel->vertices[3*i+1];
		pcluster[i].center[2]=meshmodel->vertices[3*i+2];
		pcluster[i-1].nextcluster=&(pcluster[i]);
		pcluster[i].pvertices= new IndexList;
		pcluster[i].pvertices->index=i;
		pcluster[i].pvertices->next=NULL;
		do 
		{
			for (j=0;j<3;j++)
			{
				x[j]=pcluster[i].center[j];	
			}
			meanshift(pcluster[i].center,meshmodel);
			error=glmDist(x,pcluster[i].center);
//			printf("\n%d  %f",i,error);
		} while(error>maxerror);
	}
	pcluster[meshmodel->numvertices].nextcluster=NULL;

	intArray= new int [numCluster];

	srand((unsigned)time(&t));
	
	for(i=0;i<numCluster;i++)
	{
		intArray[i]=0;
	}
	
	for(i=0; i<numCluster; i++)
	{
		temp1= (rand() % (int)(meshmodel->numvertices))+1;
		
		for(j=0; j<i; j++)
			if(temp1== intArray[j])
				temp1++;
			else
				if(temp1>intArray[j])
					;
				else
				{
					m=j;
					for(j=i;j>m;j--)
						intArray[j]= intArray[j-1];
					break;
				}
		m=j;
		intArray[m] =temp1;
		pcluster[temp1].iscluster=1;
	}
	for (i=1;i<=(int)meshmodel->numvertices;i++)
	{
		mindist=INFINITE;
		for (j=0;j<numCluster;j++)
		{
			dist=glmDist(pcluster[i].center,pcluster[intArray[j]].center);
			if (dist<mindist)
			{
				mindist=dist;
				mini=intArray[j];
			}
		}
		if (i!=mini)
		{
			cluOfV[i]=mini;
			for(ppoint=pcluster[mini].pvertices;ppoint->next!=NULL;ppoint=ppoint->next)
				;
			ppoint->next=pcluster[i].pvertices;
			pcluster[i].pvertices=NULL;
			pcluster[mini].order++;
		}
	}
			
	temp=pcluster;
	pcluster= new _cluster [numCluster+1];
	for (i=1,pi=temp+1;pi!=NULL;pi=pi->nextcluster)
	{	
		if (pi->iscluster==1)
		{
			pcluster[i].index=i;
			for (j=0;j<3;j++)
			{
				pcluster[i].center[j]=pi->center[j];
			}
			pcluster[i].iscluster=1;
			pcluster[i-1].nextcluster=&(pcluster[i]);
			pcluster[i].order=pi->order;
			pcluster[i].pvertices=pi->pvertices;
			for (pp=pcluster[i].pvertices;pp!=NULL;pp=pp->next)
			{
				cluOfV[pp->index]=i;
			}
			i++;
		}
	}
	pcluster[numCluster].nextcluster=NULL;
	delete[] temp;
	temp=NULL;

	for(i=1;i<=numCluster;i++)
	{
		for (j=0;j<3;j++)
		{
			sum[j]=0;
		}
		for (pp=pcluster[i].pvertices;pp!=NULL;pp=pp->next)
		{
			for (j=0;j<3;j++)
			{
				sum[j]+=meshmodel->vertices[3*(pp->index)+j];
			}				
		}
		for (j=0;j<3;j++)
		{
			pcluster[i].center[j]=sum[j]/pcluster[i].order;
		}
	}
	
	Color(numCluster);
	
	printf("\n%d",numCluster);
	stop=clock();
	time1=((double)(stop-start))/CLK_TCK;
	printf("\nTime: %lf",time1);
}*/

void Cluster::MeanShift(GLMmodel *meshmodel,IndexList **neighborvertices,float radius,float maxerror,float *normalofvertices)
{
	int i,j,k,mini,minj;
	float error,x[3],dist;
	_cluster *pi,*temp;
	IndexList *pp;
	clock_t start,stop;
	double time;
	start=clock();
	
	numCluster=meshmodel->numvertices;
	pcluster=(_cluster*) malloc ((meshmodel->numvertices+1) * sizeof(_cluster));
	cluOfV=(int*) malloc ((meshmodel->numvertices+1) * sizeof(int));
	
	for(i=1;i<=(int)meshmodel->numvertices;i++)
	{
		pcluster[i].index=i;
		pcluster[i].order=1;
		pcluster[i].iscluster=1;
		pcluster[i].variation=0;
		for (j=0;j<3;j++)
		{
			pcluster[i].center[j]=meshmodel->vertices[3*i+j];
			pcluster[i].normal[j]=normalofvertices[3*i+j];
		}
		pcluster[i-1].nextcluster=&(pcluster[i]);
		pcluster[i].pvertices= new IndexList;
		pcluster[i].pvertices->index=i;
		pcluster[i].pvertices->next=NULL;
		cluOfV[i]=i;
		do 
		{
			for (j=0;j<3;j++)
			{
				x[j]=pcluster[i].center[j];	
			}
			meanshift(&(pcluster[i]),meshmodel,neighborvertices,normalofvertices);
			error=glmDist(x,pcluster[i].center);
		} while(error>maxerror);
	}
	pcluster[meshmodel->numvertices].nextcluster=NULL;

	stop=clock();
	time=((double)(stop-start))/CLK_TCK;
	printf("\nTime spent in meanshift: %lf",time);

	for (i=1;i<=(int)meshmodel->numvertices;i++)
	{
		for (j=i+1;j<=(int)meshmodel->numvertices;j++)
		{
			if ((pcluster[j].iscluster==1))
			{
				dist=weight*glmDist(pcluster[i].center,pcluster[j].center)+(1-weight)*(1-Dot(pcluster[i].normal,pcluster[j].normal));
				if (dist<radius)
				{
					mini=cluOfV[i];
					minj=j;
					for (pp=pcluster[minj].pvertices;pp!=NULL;pp=pp->next)
					{
						cluOfV[pp->index]=mini;
					}
					for (pp=pcluster[mini].pvertices;pp->next!=NULL;pp=pp->next)
							;
					pp->next=pcluster[minj].pvertices;	
					pcluster[minj].pvertices=NULL;
					pcluster[mini].order+=pcluster[minj].order;
					pcluster[minj].iscluster=0;
					pcluster[minj].order=0;
					for (k=0;k<3;k++)
					{
						pcluster[mini].normal[k]=0;
					}
					for (pp=pcluster[mini].pvertices;pp!=NULL;pp=pp->next)
					{
						for (k=0;k<3;k++)
						{
							pcluster[mini].normal[k]+=normalofvertices[3*(pp->index)+k];
						}				
					}
					for (k=0;k<3;k++)
					{
						pcluster[mini].normal[k]/=pcluster[mini].order;
					}
					Normalize(pcluster[mini].normal);
					numCluster--;
				}
			}
		}	
	}

	temp=pcluster;
	pcluster= new _cluster [numCluster+1];
	for (i=1,pi=temp+1;pi!=NULL;pi=pi->nextcluster)
	{	
		if (pi->iscluster==1)
		{
			pcluster[i].index=i;
			for (j=0;j<3;j++)
			{
				pcluster[i].center[j]=pi->center[j];
				pcluster[i].normal[j]=pi->normal[j];
			}
			pcluster[i].iscluster=1;
			pcluster[i-1].nextcluster=&(pcluster[i]);
			pcluster[i].order=pi->order;
			pcluster[i].pvertices=pi->pvertices;
			for (pp=pcluster[i].pvertices;pp!=NULL;pp=pp->next)
			{
				cluOfV[pp->index]=i;
			}
			i++;
		}
	}
	pcluster[numCluster].nextcluster=NULL;
	delete[] temp;
	temp=NULL;
	
	for(i=1;i<=numCluster;i++)
	{
		for (j=0;j<3;j++)
		{
			pcluster[i].center[j]=0;
		}
		for (pp=pcluster[i].pvertices;pp!=NULL;pp=pp->next)
		{
			for (j=0;j<3;j++)
			{
				pcluster[i].center[j]+=meshmodel->vertices[3*(pp->index)+j];
			}				
		}
		for (j=0;j<3;j++)
		{
			pcluster[i].center[j]/=pcluster[i].order;
		}
	}
	Color(numCluster);
	
	printf("\n%d",numCluster);
	stop=clock();
	time=((double)(stop-start))/CLK_TCK;
	printf("\nTotal Time: %lf",time);
}

void Cluster::meanshift(_cluster *pi,GLMmodel *meshmodel,IndexList **neighborvertices,float *normalofvertices)
{
	int j;
	float sum[3]={0,0,0},y[3],sum1=0,normal[3],dist,energy,maxdist=0,angle,maxangle=0;
	IndexList *pp;
		
	for (pp=neighborvertices[pi->index];pp!=NULL;pp=pp->next)
	{
		for (j=0;j<3;j++)
		{
			y[j]=meshmodel->vertices[3*(pp->index)+j];
			normal[j]=normalofvertices[3*(pp->index)+j];
		}
		dist=glmDistSquared(y,pi->center);
		if (dist>maxdist)
		{
			maxdist=dist;
		}
		angle=1-Dot(pi->normal,normal);
		if (angle>maxangle)
		{
			maxangle=angle;
		}
	}
	for (pp=neighborvertices[pi->index];pp!=NULL;pp=pp->next)
	{
		for (j=0;j<3;j++)
		{
			y[j]=meshmodel->vertices[3*(pp->index)+j];
			normal[j]=normalofvertices[3*(pp->index)+j];
		}
		dist=glmDistSquared(y,pi->center);
		angle=1-Dot(pi->normal,normal);
		energy=weight*dist/maxdist+(1-weight)*angle/maxangle;	
		for (j=0;j<3;j++)
		{
			sum[j]+=(exp(-energy)*y[j]);
		}
		sum1+=(exp(-energy));
				
		for (j=0;j<3;j++)
		{
			sum[j] += exp(-dist/(2*maxdist))*exp(-angle*angle/(2*maxangle*maxangle))*y[j];
		}
		sum1 += exp(-dist/(2*maxdist))*exp(-angle*angle/(2*maxangle*maxangle));
	}
	for (j=0;j<3;j++)
	{
		pi->center[j]=sum[j]/sum1;
	}
}

void Cluster::ReconstructMesh(GLMmodel *originalmesh,GLMmodel *deformedmesh,
							  IndexList *handles,IndexList *editregion,IndexList *boundary)
{
	int i,j,k,*isedit;
	float weight,x[3],sum[3],weightsum,dist,*vertices;	
	double ***rotationmatrix;
	IndexList *pp;

	vertices= new float [3*((int)originalmesh->numvertices+1)];
	isedit= new int [(int)originalmesh->numvertices+1];

	for (i=1;i<=(int)originalmesh->numvertices;i++)
	{
		isedit[i]=0;
	}

	rotationmatrix=new double ** [numCluster+1];
	ConstructRotationMatrix(deformedmesh,rotationmatrix);

	while (handles!=NULL)
	{
		i=handles->index;
		isedit[i]=1;
		weightsum=0;
		for (j=0;j<3;j++)
		{
			x[j]=originalmesh->vertices[3*i+j]-pcluster[cluOfV[i]].center[j];
			vertices[3*i+j]=0.0;
		}
		dist=Length(x)+0.00001;
		weight=1/dist;
		weightsum+=weight;
		for (j=0;j<3;j++)
		{
			for (k=0;k<3;k++)
			{
				vertices[3*i+j]+=((rotationmatrix[cluOfV[i]][j][k])*(x[k]));
			}
			vertices[3*i+j]+=deformedmesh->vertices[3*(cluOfV[i])+j];
			vertices[3*i+j]*=weight;
		}
		for (pp=neighborclusters[cluOfV[i]];pp!=NULL;pp=pp->next)
		{
			for (j=0;j<3;j++)
			{
				x[j]=originalmesh->vertices[3*i+j]-pcluster[pp->index].center[j];
				sum[j]=0;
			}
			dist=Length(x)+0.00001;
			weight=1/dist;			
			weightsum+=weight;
			for (j=0;j<3;j++)
			{
				for (k=0;k<3;k++)
				{
					sum[j]+=((rotationmatrix[pp->index][j][k])*(x[k]));
				}
				sum[j]=(sum[j]+deformedmesh->vertices[3*(pp->index)+j])*weight;
			}
			for (j=0;j<3;j++)
			{
				vertices[3*i+j]+=sum[j];
			}
		}
		for (j=0;j<3;j++)
		{
			vertices[3*i+j]/=weightsum;
		}
		handles=handles->next;
	}

	while (editregion!=NULL)
	{
		i=editregion->index;
		isedit[i]=1;
		weightsum=0;
		for (j=0;j<3;j++)
		{
			x[j]=originalmesh->vertices[3*i+j]-pcluster[cluOfV[i]].center[j];
			vertices[3*i+j]=0.0;
		}
		dist=Length(x)+0.00001;
		weight=1/dist;
		weightsum+=weight;
		for (j=0;j<3;j++)
		{
			for (k=0;k<3;k++)
			{
				vertices[3*i+j]+=((rotationmatrix[cluOfV[i]][j][k])*(x[k]));
			}
			vertices[3*i+j]+=deformedmesh->vertices[3*(cluOfV[i])+j];
			vertices[3*i+j]*=weight;
		}
		for (pp=neighborclusters[cluOfV[i]];pp!=NULL;pp=pp->next)
		{
			for (j=0;j<3;j++)
			{
				x[j]=originalmesh->vertices[3*i+j]-pcluster[pp->index].center[j];
				sum[j]=0;
			}
			dist=Length(x)+0.00001;
			weight=1/dist;			
			weightsum+=weight;
			for (j=0;j<3;j++)
			{
				for (k=0;k<3;k++)
				{
					sum[j]+=((rotationmatrix[pp->index][j][k])*(x[k]));
				}
				sum[j]=(sum[j]+deformedmesh->vertices[3*(pp->index)+j])*weight;
			}
			for (j=0;j<3;j++)
			{
				vertices[3*i+j]+=sum[j];
			}
		}
		for (j=0;j<3;j++)
		{
			vertices[3*i+j]/=weightsum;
		}
		editregion=editregion->next;
	}

	while (boundary!=NULL)
	{
		i=boundary->index;
		isedit[i]=1;
		weightsum=0;
		for (j=0;j<3;j++)
		{
			x[j]=originalmesh->vertices[3*i+j]-pcluster[cluOfV[i]].center[j];
			vertices[3*i+j]=0.0;
		}
		dist=Length(x)+0.00001;
		weight=1/dist;
		weightsum+=weight;
		for (j=0;j<3;j++)
		{
			for (k=0;k<3;k++)
			{
				vertices[3*i+j]+=((rotationmatrix[cluOfV[i]][j][k])*(x[k]));
			}
			vertices[3*i+j]+=deformedmesh->vertices[3*(cluOfV[i])+j];
			vertices[3*i+j]*=weight;
		}
		for (pp=neighborclusters[cluOfV[i]];pp!=NULL;pp=pp->next)
		{
			for (j=0;j<3;j++)
			{
				x[j]=originalmesh->vertices[3*i+j]-pcluster[pp->index].center[j];
				sum[j]=0;
			}
			dist=Length(x)+0.00001;
			weight=1/dist;			
			weightsum+=weight;
			for (j=0;j<3;j++)
			{
				for (k=0;k<3;k++)
				{
					sum[j]+=((rotationmatrix[pp->index][j][k])*(x[k]));
				}
				sum[j]=(sum[j]+deformedmesh->vertices[3*(pp->index)+j])*weight;
			}
			for (j=0;j<3;j++)
			{
				vertices[3*i+j]+=sum[j];
			}
		}
		for (j=0;j<3;j++)
		{
			vertices[3*i+j]/=weightsum;
		}
		boundary=boundary->next;
	}

	for (i=1;i<=(int)originalmesh->numvertices;i++)
	{
		if (isedit[i]==1)
		{
			for (j=0;j<3;j++)
			{
				originalmesh->vertices[3*i+j] = vertices[3*i+j];
			}
		}
	}

	Centroid(originalmesh);
}

void Cluster::ConstructRotationMatrix(GLMmodel *meshmodel,double ***rotationmatrix)
{
	double **a;
	float *normalofdeformedmesh,*normalofsimplifiedmesh;
	int i,j;
	a=new double* [numCluster+1];
	normalofdeformedmesh=new float [3*(numCluster+1)];
	normalofsimplifiedmesh=new float [3*(numCluster+1)];
	for (i=1;i<=numCluster;i++)
	{
		a[i]=new double [9];
	}

	ConstructVertexNormals(meshmodel,normalofdeformedmesh);
	ConstructVertexNormals(pnewmodel,normalofsimplifiedmesh);

	ConstructDeformMatrix(meshmodel,normalofdeformedmesh,normalofsimplifiedmesh,a);
	for (i=1;i<=numCluster;i++)
	{
		rotationmatrix[i]=new double* [3];
		for (j=0;j<3;j++)
		{
			rotationmatrix[i][j]=new double [3];
		}
		ComputeRotationMatrixPDSVD(a[i],3,rotationmatrix[i]);
	}
}

float Cluster::ComputeTriangleArea(float *point1, float *point2, float *point3)
{
	float area;

	float crossproduct[3];
	CrossProd(point1, point2, point3, crossproduct);

	area = 0.5 * Length(crossproduct);

	return area;
}

void Cluster::ConstructVertexNormals(GLMmodel *meshmodel,float *normalofvertices)
{
	glmFacetNormals(meshmodel);
	
	if (normalofvertices)
	{
		delete []normalofvertices;
		normalofvertices = NULL;
	}
	
	if (!normalofvertices)
	{
		normalofvertices = new float [3*((int)meshmodel->numvertices+1)];
	}
	
	IndexList *p1;
	float facetnormssum[3], weightsum, weightfacet;	
	int indexf;
	int indexv1, indexv2, indexv3;
	
	for (int i = 1; i <= (int)meshmodel->numvertices; i++)
	{		
		p1 = neighborclusters[i];
		weightsum = 0.0;
		facetnormssum[0] = 0.0;
		facetnormssum[1] = 0.0;
		facetnormssum[2] = 0.0;
		
		while (p1)
		{
			indexf = meshmodel->triangles[p1->index].findex;
			
			indexv1 = meshmodel->triangles[p1->index].vindices[0];
			indexv2 = meshmodel->triangles[p1->index].vindices[1];
			indexv3 = meshmodel->triangles[p1->index].vindices[2];
			
			weightfacet = ComputeTriangleArea(meshmodel->vertices+3*indexv1,
											  meshmodel->vertices+3*indexv2,
											  meshmodel->vertices+3*indexv3);
			weightsum += weightfacet;
			
			facetnormssum[0] += weightfacet * meshmodel->facetnorms[3*indexf];
			facetnormssum[1] += weightfacet * meshmodel->facetnorms[3*indexf+1];
			facetnormssum[2] += weightfacet * meshmodel->facetnorms[3*indexf+2];
			
			p1 = p1->next;
		}
		
		facetnormssum[0] /= weightsum;
		facetnormssum[1] /= weightsum;
		facetnormssum[2] /= weightsum;
		Normalize(facetnormssum);
		
		normalofvertices[3*i] = facetnormssum[0];
		normalofvertices[3*i+1] = facetnormssum[1];
		normalofvertices[3*i+2] = facetnormssum[2];		
	}
}

void Cluster::ConstructDeformMatrix(GLMmodel *meshmodel,float *normalofdeformedmesh,float *normalofsimplifiedmesh,double **a)
{
	float p1[3],p2[3],sum1[3][3],sum2[3][3],length1,length2;
	int i,j,k,num,sign;
	IndexList *pp;
	gsl_matrix *coefficientmatrix;
	gsl_permutation *permutation; 
	gsl_vector *b,*x;
	
	coefficientmatrix=gsl_matrix_alloc(3,3);
	permutation=gsl_permutation_alloc(3);
	b=gsl_vector_alloc(3);
	x=gsl_vector_alloc(3);
	
	for (i=1;i<=numCluster;i++)
	{
		for (k=0;k<3;k++)
		{
			for (j=0;j<3;j++)
			{
				sum1[k][j]=0;
				sum2[k][j]=0;
			}
		}
		length1=0;
		length2=0;
		for (num=0,pp=neighborclusters[i];pp!=NULL;pp=pp->next,num++)
		{
			for (k=0;k<3;k++)
			{
				p1[k]=pcluster[pp->index].center[k]-pcluster[i].center[k];
				p2[k]=meshmodel->vertices[3*(pp->index)+k]-meshmodel->vertices[3*i+k];	
			}
			length1+=Length(p1);
			length2+=Length(p2);
			for (k=0;k<3;k++)
			{
				for (j=0;j<3;j++)
				{
					sum1[k][j]+=((p2[k])*(p1[j]));
					sum2[k][j]+=((p1[k])*(p1[j]));
				}
			}
		}
		length1/=num;
		length2/=num;
		for (j=0;j<3;j++)
		{
			p1[j]=normalofsimplifiedmesh[3*i+j];
			p2[j]=normalofdeformedmesh[3*i+j];
		}
		Normalize(p1);
		Multiply(p1,length1);
		Normalize(p2);
		Multiply(p2,length2);
		for (k=0;k<3;k++)
		{
			for (j=0;j<3;j++)
			{
				sum1[k][j]+=((p2[k])*(p1[j]));
				sum2[k][j]+=((p1[k])*(p1[j]));
			}
		}
		for (k=0;k<3;k++)
		{
			for (j=0;j<3;j++)
			{
				gsl_matrix_set(coefficientmatrix,k,j,sum2[j][k]);
			}
		}
		gsl_linalg_LU_decomp(coefficientmatrix,permutation,&sign);
		for (k=0;k<3;k++)
		{
			for (j=0;j<3;j++)
			{
				gsl_vector_set(b,j,sum1[k][j]);
			}
			gsl_linalg_LU_solve(coefficientmatrix,permutation,b,x);
			for (j=0;j<3;j++)
			{
				a[i][3*k+j]=gsl_vector_get(x,j);		
			}
		}
	}
	gsl_matrix_free(coefficientmatrix);
	gsl_vector_free(b);
	gsl_vector_free(x);
	gsl_permutation_free(permutation);
}

void Cluster::ComputeRotationMatrixPDSVD(double *LSMmatrix, int dimension,double **rotationmatrix)
{
	SingularValueDecomposition(LSMmatrix, rotationmatrix, 10e-7);
/*
	gsl_matrix_view m = gsl_matrix_view_array (LSMmatrix, dimension, dimension);
	gsl_matrix *v = gsl_matrix_alloc (dimension, dimension);
	gsl_vector *s = gsl_vector_alloc (dimension);

	gsl_linalg_SV_decomp_jacobi (&m.matrix, v, s);

	if (Det(&m.matrix)< 0.0)
	{
		int index;

		if (gsl_vector_get (s, 0) < gsl_vector_get (s, 2))
		{
			index = 0;
		}
		else
		{
			index = 2;
		}

		for (int i = 0; i < 3; i++)
		{
			gsl_matrix_set (v, i, index, -gsl_matrix_get (v, i, index));
		}
	}

	for (int i = 0; i < dimension; i++)
	{
		for (int j = 0; j < dimension; j++)
		{
			rotationmatrix[i][j] = 0.0;

			for (int k = 0; k < dimension; k++)
			{
				rotationmatrix[i][j] += gsl_matrix_get (&m.matrix, i, k)
					                   *gsl_matrix_get (v, j, k);
			}
		}
	}

	gsl_vector_free (s);
	gsl_matrix_free (v);*/
}

void Cluster::ConstructLocalFrame(GLMmodel *meshmodel,float *normalofvertices,IndexList **neighborvertices,float **localframe)
{
	int i,j;
	float x[3],y[3],z[3];
	IndexList *pp;

	for (i=1;i<=(int)meshmodel->numvertices;i++)
	{
		pp=neighborvertices[i];
		for (j=0;j<3;j++)
		{
			z[j]=normalofvertices[3*i+j];
			x[j]=meshmodel->vertices[3*(pp->index)+j]-meshmodel->vertices[3*i+j];	
		}
		CrossProd(z,x,y);
		Normalize(y);
		CrossProd(y,z,x);
		for (j=0;j<3;j++)
		{
			localframe[i][3*j]=x[j];
			localframe[i][3*j+1]=y[j];
			localframe[i][3*j+2]=z[j];
		}
	}
}

void Cluster::ReconstructMesh(GLMmodel *originalmesh,GLMmodel *simplifiedmesh,GLMmodel *deformedmesh,
							  IndexList *handles,IndexList *editregion,IndexList *boundary)
{
	float time;
	timestamp begin, end;
	get_timestamp(begin);

	int i,j,k,*isedit;
	float *normalofsimplifiedmesh,*normalofdeformedmesh,**localframeSM,**localframeDM,
		  x[3],y[3],z[3],p[3],localcoordinates[3],sum[3],dist,weight,weightsum;
	IndexList *pp;

	normalofdeformedmesh=new float [3*(deformedmesh->numvertices+1)];
	normalofsimplifiedmesh=new float [3*(simplifiedmesh->numvertices+1)];
	localframeSM=new float* [simplifiedmesh->numvertices+1];
	localframeDM=new float* [deformedmesh->numvertices+1];
	isedit= new int [(int)originalmesh->numvertices+1];

	for (i=1;i<=(int)originalmesh->numvertices;i++)
	{
		isedit[i]=0;
	}

	for (i=1;i<=(int)simplifiedmesh->numvertices;i++)
	{
		localframeSM[i]=new float [9];
		localframeDM[i]=new float [9];
	}

	float *vertices= new float [3*((int)originalmesh->numvertices+1)];

	ConstructVertexNormals(pnewmodel,normalofsimplifiedmesh);
	ConstructVertexNormals(deformedmesh,normalofdeformedmesh);

	ConstructLocalFrame(pnewmodel,normalofsimplifiedmesh,neighborclusters,localframeSM);
	ConstructLocalFrame(deformedmesh,normalofdeformedmesh,neighborclusters,localframeDM);	

	while(handles!=NULL)
	{
		i=handles->index;
		isedit[i]=1;
		weightsum=0;
		for (j=0;j<3;j++)
		{
			p[j]=originalmesh->vertices[3*i+j]-simplifiedmesh->vertices[3*(cluOfV[i])+j];
			x[j]=localframeSM[(cluOfV[i])][3*j];
			y[j]=localframeSM[(cluOfV[i])][3*j+1];
			z[j]=localframeSM[(cluOfV[i])][3*j+2];
			vertices[3*i+j]=0.0;
		}
		dist=Length(p)+0.00001;
		weight=1/dist/dist;
		weightsum+=weight;
		localcoordinates[0]=Dot(p,x);
		localcoordinates[1]=Dot(p,y);
		localcoordinates[2]=Dot(p,z);
		for (j=0;j<3;j++)
		{
			for (k=0;k<3;k++)
			{
				vertices[3*i+j]+=(localframeDM[cluOfV[i]][3*j+k]*localcoordinates[k]);
			}
			vertices[3*i+j]+=deformedmesh->vertices[3*(cluOfV[i])+j];
			vertices[3*i+j]*=weight;
		}
		for (pp=neighborclusters[cluOfV[i]];pp!=NULL;pp=pp->next)
		{
			for (j=0;j<3;j++)
			{
				p[j]=originalmesh->vertices[3*i+j]-simplifiedmesh->vertices[3*(pp->index)+j];
				x[j]=localframeSM[(pp->index)][3*j];
				y[j]=localframeSM[(pp->index)][3*j+1];
				z[j]=localframeSM[(pp->index)][3*j+2];
				sum[j]=0;
			}
			dist=Length(p)+0.00001;
			weight=1/dist/dist;
			weightsum+=weight;
			localcoordinates[0]=Dot(p,x);
			localcoordinates[1]=Dot(p,y);
			localcoordinates[2]=Dot(p,z);
			for (j=0;j<3;j++)
			{
				for (k=0;k<3;k++)
				{
					sum[j]+=(localframeDM[pp->index][3*j+k]*localcoordinates[k]);
				}
				sum[j]=weight*(sum[j]+deformedmesh->vertices[3*(pp->index)+j]);
			}
			for (j=0;j<3;j++)
			{
				vertices[3*i+j]+=sum[j];
			}
		}
		for (j=0;j<3;j++)
		{
			vertices[3*i+j]/=weightsum;
		}
		handles=handles->next;
	}

	while(editregion!=NULL)
	{
		i=editregion->index;
		isedit[i]=1;
		weightsum=0;
		for (j=0;j<3;j++)
		{
			p[j]=originalmesh->vertices[3*i+j]-simplifiedmesh->vertices[3*(cluOfV[i])+j];
			x[j]=localframeSM[(cluOfV[i])][3*j];
			y[j]=localframeSM[(cluOfV[i])][3*j+1];
			z[j]=localframeSM[(cluOfV[i])][3*j+2];
			vertices[3*i+j]=0.0;
		}
		dist=Length(p)+0.00001;
		weight=1/dist/dist;
		weightsum+=weight;
		localcoordinates[0]=Dot(p,x);
		localcoordinates[1]=Dot(p,y);
		localcoordinates[2]=Dot(p,z);
		for (j=0;j<3;j++)
		{
			for (k=0;k<3;k++)
			{
				vertices[3*i+j]+=(localframeDM[cluOfV[i]][3*j+k]*localcoordinates[k]);
			}
			vertices[3*i+j]+=deformedmesh->vertices[3*(cluOfV[i])+j];
			vertices[3*i+j]*=weight;
		}
		for (pp=neighborclusters[cluOfV[i]];pp!=NULL;pp=pp->next)
		{
			for (j=0;j<3;j++)
			{
				p[j]=originalmesh->vertices[3*i+j]-simplifiedmesh->vertices[3*(pp->index)+j];
				x[j]=localframeSM[(pp->index)][3*j];
				y[j]=localframeSM[(pp->index)][3*j+1];
				z[j]=localframeSM[(pp->index)][3*j+2];
				sum[j]=0;
			}
			dist=Length(p)+0.00001;
			weight=1/dist/dist;
			weightsum+=weight;
			localcoordinates[0]=Dot(p,x);
			localcoordinates[1]=Dot(p,y);
			localcoordinates[2]=Dot(p,z);
			for (j=0;j<3;j++)
			{
				for (k=0;k<3;k++)
				{
					sum[j]+=(localframeDM[pp->index][3*j+k]*localcoordinates[k]);
				}
				sum[j]=weight*(sum[j]+deformedmesh->vertices[3*(pp->index)+j]);
			}
			for (j=0;j<3;j++)
			{
				vertices[3*i+j]+=sum[j];
			}
		}
		for (j=0;j<3;j++)
		{
			vertices[3*i+j]/=weightsum;
		}
		editregion=editregion->next;
	}

	while(boundary!=NULL)
	{
		i=boundary->index;
		isedit[i]=1;
		weightsum=0;
		for (j=0;j<3;j++)
		{
			p[j]=originalmesh->vertices[3*i+j]-simplifiedmesh->vertices[3*(cluOfV[i])+j];
			x[j]=localframeSM[(cluOfV[i])][3*j];
			y[j]=localframeSM[(cluOfV[i])][3*j+1];
			z[j]=localframeSM[(cluOfV[i])][3*j+2];
			vertices[3*i+j]=0.0;
		}
		dist=Length(p)+0.00001;
		weight=1/dist/dist;
		weightsum+=weight;
		localcoordinates[0]=Dot(p,x);
		localcoordinates[1]=Dot(p,y);
		localcoordinates[2]=Dot(p,z);
		for (j=0;j<3;j++)
		{
			for (k=0;k<3;k++)
			{
				vertices[3*i+j]+=(localframeDM[cluOfV[i]][3*j+k]*localcoordinates[k]);
			}
			vertices[3*i+j]+=deformedmesh->vertices[3*(cluOfV[i])+j];
			vertices[3*i+j]*=weight;
		}
		for (pp=neighborclusters[cluOfV[i]];pp!=NULL;pp=pp->next)
		{
			for (j=0;j<3;j++)
			{
				p[j]=originalmesh->vertices[3*i+j]-simplifiedmesh->vertices[3*(pp->index)+j];
				x[j]=localframeSM[(pp->index)][3*j];
				y[j]=localframeSM[(pp->index)][3*j+1];
				z[j]=localframeSM[(pp->index)][3*j+2];
				sum[j]=0;
			}
			dist=Length(p)+0.00001;
			weight=1/dist/dist;
			weightsum+=weight;
			localcoordinates[0]=Dot(p,x);
			localcoordinates[1]=Dot(p,y);
			localcoordinates[2]=Dot(p,z);
			for (j=0;j<3;j++)
			{
				for (k=0;k<3;k++)
				{
					sum[j]+=(localframeDM[pp->index][3*j+k]*localcoordinates[k]);
				}
				sum[j]=weight*(sum[j]+deformedmesh->vertices[3*(pp->index)+j]);
			}
			for (j=0;j<3;j++)
			{
				vertices[3*i+j]+=sum[j];
			}
		}
		for (j=0;j<3;j++)
		{
			vertices[3*i+j]/=weightsum;
		}
		boundary=boundary->next;
	}

	for (i=1;i<=(int)originalmesh->numvertices;i++)
	{
		if(isedit[i]==1)
		{
			for (j=0;j<3;j++)
			{
				originalmesh->vertices[3*i+j] = vertices[3*i+j];
			}
		}
	}

	Centroid(originalmesh);

	get_timestamp(end);
	time = fabs(end - begin);
	printf("\nInitialGuess-LC:time = %g", time);
}

void Cluster::Centroid(GLMmodel *meshmodel)
{
	int i,j;
	float sum[3];
	IndexList *pp;
	
	for(i=1;i<=numCluster;i++)
	{
		for (j=0;j<3;j++)
		{
			sum[j]=0;
		}
		for (pp=pcluster[i].pvertices;pp!=NULL;pp=pp->next)
		{
			for (j=0;j<3;j++)
			{
				sum[j]+=meshmodel->vertices[3*(pp->index)+j];
			}				
		}
		for (j=0;j<3;j++)
		{
			pcluster[i].center[j]=sum[j]/pcluster[i].order;
			pnewmodel->vertices[3*i+j] = sum[j]/pcluster[i].order;
		}
	}
}

void Cluster::Centroid(GLMmodel *meshmodel, GLMmodel *simplifiedMesh)
{
	int i,j;
	float sum[3];
	IndexList *pp;
	
	for(i=1;i<=numCluster;i++)
	{
		for (j=0;j<3;j++)
		{
			sum[j]=0;
		}
		for (pp=pcluster[i].pvertices;pp!=NULL;pp=pp->next)
		{
			for (j=0;j<3;j++)
			{
				sum[j]+=meshmodel->vertices[3*(pp->index)+j];
			}				
		}
		for (j=0;j<3;j++)
		{
			pcluster[i].center[j]=sum[j]/pcluster[i].order;
			pnewmodel->vertices[3*i+j] = sum[j]/pcluster[i].order;
			simplifiedMesh->vertices[3*i+j] = pnewmodel->vertices[3*i+j];
		}
	}
}

void Cluster::QuadricCenter(GLMmodel *meshmodel, GLMmodel *simplifiedMesh)
{
	int i,j,k,l,pointindex,clusterindex,normalindex,sign;
	double **quadricmatrix, **b,**tranQuadMatrix,**normQuadMatrix,**normalB;
	float normal[3],vertices[3];

	if (!(meshmodel->facetnorms))
	{
		glmFacetNormals(meshmodel);
	}

	quadricmatrix=new double* [numCluster+1];
	b=new double * [numCluster+1];
	tranQuadMatrix=new double* [numCluster+1];
	normQuadMatrix=new double* [numCluster+1];
	normalB=new double* [numCluster+1];
	for (i=1;i<=numCluster;i++)
	{
		quadricmatrix[i]=new double [9];
		tranQuadMatrix[i]=new double [9];
		normQuadMatrix[i]=new double [9];
		for (j=0;j<9;j++)
		{
			quadricmatrix[i][j]=0;
			tranQuadMatrix[i][j]=0;
			normQuadMatrix[i][j]=0;
		}
		b[i]=new double [3];
		normalB[i]=new double [3];
		for (j=0;j<3;j++)
		{
			b[i][j]=0;
			normalB[i][j]=0;
		}
	}

	double coef[9];
	
	for (i=0;i<(int)meshmodel->numtriangles;i++)
	{
		normalindex=meshmodel->triangles[i].findex;
		for (j=0;j<3;j++)
		{
			normal[j]=meshmodel->facetnorms[3*normalindex+j]+rand()/1000000.0; // 进行扰动，否则对于太规则的模型(例如Bar)，
			                                                                   // 构造的矩阵不可逆。
		}

		pointindex=meshmodel->triangles[i].vindices[0];		

		for (k=0;k<3;k++)
		{
			vertices[k]=meshmodel->vertices[3*pointindex+k];
		}

		coef[0] = normal[0]*normal[0];
		coef[1] = normal[0]*normal[1];
		coef[2] = normal[0]*normal[2];
		coef[3] = normal[1]*normal[1];
		coef[4] = normal[1]*normal[2];
		coef[5] = normal[2]*normal[2];
		coef[6] = normal[0]*(normal[0]*vertices[0]+normal[1]*vertices[1]+normal[2]*vertices[2]);
		coef[7] = normal[1]*(normal[0]*vertices[0]+normal[1]*vertices[1]+normal[2]*vertices[2]);
		coef[8] = normal[2]*(normal[0]*vertices[0]+normal[1]*vertices[1]+normal[2]*vertices[2]);

		for (j=0;j<3;j++)
		{
			pointindex=meshmodel->triangles[i].vindices[j];
			clusterindex=cluOfV[pointindex];
			
			quadricmatrix[clusterindex][0] += coef[0];
			quadricmatrix[clusterindex][1] += coef[1];
			quadricmatrix[clusterindex][2] += coef[2];
			quadricmatrix[clusterindex][3] += coef[1];
			quadricmatrix[clusterindex][4] += coef[3];
			quadricmatrix[clusterindex][5] += coef[4];
			quadricmatrix[clusterindex][6] += coef[2];
			quadricmatrix[clusterindex][7] += coef[4];
			quadricmatrix[clusterindex][8] += coef[5];
			b[clusterindex][0] += coef[6];
			b[clusterindex][1] += coef[7];
			b[clusterindex][2] += coef[8];
		}
	}

	for (i=1;i<=numCluster;i++)
	{
		for (j=0;j<3;j++)
		{
			for (k=0;k<3;k++)
			{
				tranQuadMatrix[i][3*j+k]=quadricmatrix[i][3*k+j];
			}
		}
		for (j=0;j<3;j++)
		{
			for (k=0;k<3;k++)
			{
				for (l=0;l<3;l++)
				{
					normQuadMatrix[i][3*j+k]+=tranQuadMatrix[i][3*j+l]*(quadricmatrix[i][3*l+k]);
				}
				normalB[i][j]+=tranQuadMatrix[i][3*j+k]*(b[i][k]);
			}	
		}
	}

	gsl_matrix *coefficientmatrix=gsl_matrix_alloc(3,3);
	gsl_permutation *permutation=gsl_permutation_alloc(3); 
	gsl_vector *v=gsl_vector_alloc(3),*x=gsl_vector_alloc(3);

	for (i=1;i<=numCluster;i++)
	{
		for (k=0;k<3;k++)
		{
			for (j=0;j<3;j++)
			{
				gsl_matrix_set(coefficientmatrix,k,j,normQuadMatrix[i][3*k+j]);
			}
		}
		gsl_linalg_LU_decomp(coefficientmatrix,permutation,&sign);
		for (j=0;j<3;j++)
		{
			gsl_vector_set(v,j,normalB[i][j]);
		}
		gsl_linalg_LU_solve(coefficientmatrix,permutation,v,x);
		for (j=0;j<3;j++)
		{
			pcluster[i].center[j]=gsl_vector_get(x,j);
			pnewmodel->vertices[3*i+j] = gsl_vector_get(x,j);
			simplifiedMesh->vertices[3*i+j] = gsl_vector_get(x,j);
		}
	}

	gsl_matrix_free(coefficientmatrix);
	gsl_vector_free(v);
	gsl_vector_free(x);
	gsl_permutation_free(permutation);

	for (i=1;i<=numCluster;i++)
	{
		delete[] quadricmatrix[i];
		delete[] b[i];
		delete[] tranQuadMatrix[i];
		delete[] normQuadMatrix[i];
		delete[] normalB[i];
	}
	delete[] quadricmatrix;
	delete[] b;
	delete[] tranQuadMatrix;
	delete[] normQuadMatrix;
	delete[] normalB;
}

void Cluster::QuadricCenter2(GLMmodel *meshmodel, GLMmodel *simplifiedMesh)
{
	int i,j,k,pointindex,clusterindex[3],normalindex,sign;
	double **quadricmatrix, **b;
	float normal[3],vertices[3];

	if (!(meshmodel->facetnorms))
	{
		glmFacetNormals(meshmodel);
	}

	quadricmatrix=new double* [numCluster+1];
	b=new double * [numCluster+1];
	for (i=1;i<=numCluster;i++)
	{
		quadricmatrix[i]=new double [9];
		for (j=0;j<9;j++)
		{
			quadricmatrix[i][j]=0.0;
		}
		b[i]=new double[3];
		for (j=0;j<3;j++)
		{
			b[i][j]=0.0;
		}
	}

// 	int *count, maxcount = 0, mincount = numCluster+1;
// 	count = new int [numCluster+1];
// 
// 	for (i=0;i<numCluster+1;i++)
// 	{
// 		count[i] = 0;
// 	}

	double coef[9];

	for (i=0;i<(int)meshmodel->numtriangles;i++)
	{
		normalindex=meshmodel->triangles[i].findex;
		for (k=0;k<3;k++)
		{
			normal[k]=meshmodel->facetnorms[3*normalindex+k]+rand()/1000000.0;
		}

		pointindex=meshmodel->triangles[i].vindices[0];
		for (k=0;k<3;k++)
		{
			vertices[k]=meshmodel->vertices[3*pointindex+k];
		}

		coef[0] = normal[0]*normal[0];
		coef[1] = normal[0]*normal[1];
		coef[2] = normal[0]*normal[2];
		coef[3] = normal[1]*normal[1];
		coef[4] = normal[1]*normal[2];
		coef[5] = normal[2]*normal[2];
		coef[6] = normal[0]*(normal[0]*vertices[0]+normal[1]*vertices[1]+normal[2]*vertices[2]);
		coef[7] = normal[1]*(normal[0]*vertices[0]+normal[1]*vertices[1]+normal[2]*vertices[2]);
		coef[8] = normal[2]*(normal[0]*vertices[0]+normal[1]*vertices[1]+normal[2]*vertices[2]);

		for (j=0;j<3;j++)
		{
			pointindex=meshmodel->triangles[i].vindices[j];
			clusterindex[j]=cluOfV[pointindex];

			if (j == 0 || (j == 1 && clusterindex[j] != clusterindex[j-1]) ||
			   (j == 2 && clusterindex[j] != clusterindex[j-1] && clusterindex[j] != clusterindex[j-2]))
			{
//				count[clusterindex[j]]++;

				quadricmatrix[clusterindex[j]][0] += coef[0];
				quadricmatrix[clusterindex[j]][1] += coef[1];
				quadricmatrix[clusterindex[j]][2] += coef[2];
				quadricmatrix[clusterindex[j]][3] += coef[1];
				quadricmatrix[clusterindex[j]][4] += coef[3];
				quadricmatrix[clusterindex[j]][5] += coef[4];
				quadricmatrix[clusterindex[j]][6] += coef[2];
				quadricmatrix[clusterindex[j]][7] += coef[4];
				quadricmatrix[clusterindex[j]][8] += coef[5];
				b[clusterindex[j]][0] += coef[6];
				b[clusterindex[j]][1] += coef[7];
				b[clusterindex[j]][2] += coef[8];
			}
		}
	}

// 	for (i=1;i<=numCluster;i++)
// 	{
// 		if (count[i] > maxcount)
// 		{
// 			maxcount = count[i];
// 		}
// 
// 		if (count[i] < mincount)
// 		{
// 			mincount = count[i];
// 		}
// 	}

//	printf("\nmaxcount = %d, mincount = %d", maxcount, mincount);
	
	gsl_matrix *coefficientmatrix=gsl_matrix_alloc(3,3);
	gsl_permutation *permutation=gsl_permutation_alloc(3); 
	gsl_vector *v=gsl_vector_alloc(3),*x=gsl_vector_alloc(3);

	for (i=1;i<=numCluster;i++)
	{
		for (k=0;k<3;k++)
		{
			for (j=0;j<3;j++)
			{
				gsl_matrix_set(coefficientmatrix,k,j,quadricmatrix[i][3*k+j]);
			}
		}
		gsl_linalg_LU_decomp(coefficientmatrix,permutation,&sign);
		for (j=0;j<3;j++)
		{
			gsl_vector_set(v,j,b[i][j]);
		}
		gsl_linalg_LU_solve(coefficientmatrix,permutation,v,x);
		for (j=0;j<3;j++)
		{
			pcluster[i].center[j]=gsl_vector_get(x,j);
			pnewmodel->vertices[3*i+j] = gsl_vector_get(x,j);
			simplifiedMesh->vertices[3*i+j] = gsl_vector_get(x,j);
		}
	}

	gsl_matrix_free(coefficientmatrix);
	gsl_vector_free(v);
	gsl_vector_free(x);
	gsl_permutation_free(permutation);

	for (i=1;i<=numCluster;i++)
	{
		delete[] quadricmatrix[i];
		delete[] b[i];
	}
	delete[] quadricmatrix;
	delete[] b;
}

void Cluster::SaveClusterInfo(char *filename,IndexList **neighborvertices)
{
	int i;
	IndexList *p1;

	if (pcluster==NULL)
		return;
	FILE *output;

	if (!(output=fopen(filename,"w")))
	{
		exit(0);
	}

	fprintf(output,"%d Clusters in Total\n\n",numCluster);
		
	for (i=1;i<=numCluster;i++)
	{
		fprintf(output,"cluster: %d\n",i);
		fprintf(output,"center: %f %f %f\n",pcluster[i].center[0],
											 pcluster[i].center[1],
											 pcluster[i].center[2]);
		fprintf(output,"normal: %f %f %f\n",pcluster[i].normal[0],
											 pcluster[i].normal[1],
										     pcluster[i].normal[2]);
		fprintf(output,"order: %d\nmembers: ",pcluster[i].order);
		for (p1=(pcluster[i].pvertices);p1!=NULL;p1=p1->next)
		{
			fprintf(output,"%d ",p1->index);
		}
		fprintf(output,"0\n");
		fprintf(output,"color: %f %f %f\n",colorOfClu[3*i],
										   colorOfClu[3*i+1],
										   colorOfClu[3*i+2]);
		if (neighborclusters==NULL)
		{
			GetNeighbors(neighborvertices,numCluster);
		}
		fprintf(output,"neighbors: ");
		for (p1=neighborclusters[i];p1!=NULL;p1=p1->next)
		{
			fprintf(output,"%d ",p1->index);
		}
		fprintf(output,"0\n");
		
		fprintf(output,"\n");
	}
	fclose(output);
}

void Cluster::ReadClusterInfo(char * filename,int numvertices)
{
	FILE *file=NULL;
	char buf[128];
	int i,index;
	IndexList *p1;

	if (pcluster)
	{
		DeleteClusters();
	}
	if (neighborclusters)
	{
		for (i=1;i<=sizeof(neighborclusters);i++)
		{
			deleteIndexList(neighborclusters[i]);
		}
	}
	if (cluOfV)
	{
		delete[] cluOfV;
	}

	if(!(file=fopen(filename,"r")))
	{
		exit(0);
	}

	fscanf(file,"%d",&numCluster);
	pcluster= new _cluster[numCluster+1];
	colorOfClu= new float [3*(numCluster+1)];
	neighborclusters=new IndexList* [numCluster+1];
	cluOfV=new int [numvertices+1];
	fgets(buf,sizeof(buf),file);

	while (fscanf(file,"%s",buf)!=EOF)
	{
		if (buf[0]=='#')
		{
			fgets(buf,sizeof(buf),file);
			continue;
		}
		if (_stricmp(buf,"cluster:")==0)
		{
			fscanf(file,"%d",&i);
			pcluster[i].index=i;
			pcluster[i].iscluster=1;
			pcluster[i-1].nextcluster=&(pcluster[i]);
			pcluster[i].order=0;
			pcluster[i].pvertices=NULL;
			pcluster[i].variation=0;
			continue;
		}
		if (_stricmp(buf,"center:")==0)
		{
			fscanf(file,"%f %f %f",&(pcluster[i].center[0]),&(pcluster[i].center[1]),&(pcluster[i].center[2]));
			continue;
		}
		if (_stricmp(buf,"normal:")==0)
		{
			fscanf(file,"%f %f %f",&(pcluster[i].normal[0]),&(pcluster[i].normal[1]),&(pcluster[i].normal[2]));
			continue;
		}
		if (_stricmp(buf,"order:")==0)
		{
			fscanf(file,"%d",&(pcluster[i].order));
			continue;
		}
		if (_stricmp(buf,"members:")==0)
		{
			fscanf(file,"%d",&index);
			p1=NULL;
			while (index!=0)
			{
				pcluster[i].pvertices=new IndexList;
				pcluster[i].pvertices->index=index;
				pcluster[i].pvertices->next=p1;
				p1=pcluster[i].pvertices;
				cluOfV[index]=i;
				fscanf(file,"%d",&index);
			}
			continue;
		}
		if (_stricmp(buf,"neighbors:")==0)
		{
			fscanf(file,"%d",&index);
			p1=NULL;
			while (index!=0)
			{
				neighborclusters[i]=new IndexList;
				neighborclusters[i]->index=index;
				neighborclusters[i]->next=p1;
				p1=neighborclusters[i];
				fscanf(file,"%d",&index);
			}
			continue;
		}
		if (_stricmp(buf,"color:")==0)
		{
			fscanf(file,"%f %f %f",&(colorOfClu[3*i]),&(colorOfClu[3*i+1]),&(colorOfClu[3*i+2]));
			continue;
		}
	}
}
