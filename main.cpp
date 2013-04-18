#include <iostream>
#include <string>
#include <iomanip>

#include <vector>
#include <fstream>

#include <opencv2/imgproc/imgproc.hpp>  // Gaussian Blur
#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/highgui/highgui.hpp>  // OpenCV window I/O
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
//Test and logs v2

#define RANK_SIZE 10
#define QNT_DATASETS 2

using namespace std;
using namespace cv;

vector <vector <vector < unsigned short> > > datasetRaw[QNT_DATASETS];
vector <vector <vector < unsigned short> > > datasetNewPlane[QNT_DATASETS];
unsigned short **datasetWHShrink[QNT_DATASETS];
vector<Mat> datasetSlices[2];

FILE* inFile;
ofstream outFile;
vector<int> sliceAndDistance[3][3];
vector<int> rankSumm[3];


Scalar getPSNR ( const Mat& I1, const Mat& I2);
Scalar getMSSIM( const Mat& I1, const Mat& I2);
Scalar getSURF( const Mat& I1, const Mat& I2);

struct sliceRank
{
	int sliceNumber;
	float value;
};
	
void onTrackbar( int val, void* )
{
	imshow( "Dataset1", datasetSlices[0][val]);
	imshow( "Dataset2", datasetSlices[1][val]);
}

bool mySort (sliceRank i,sliceRank j) 
{ 
	return (i.value>j.value); 
}
bool mySort2 (sliceRank i,sliceRank j) 
{ 
	return (i.value<j.value); 
}

void ordenaRank(int slices, vector<sliceRank> &srr,int algorithm)
{
	if(algorithm!=3)
	{
		sort(srr.begin(),srr.end(),mySort);
	}
	else
	{
		sort(srr.begin(),srr.end(),mySort2);
	}
}

void calculaSimilaridade(int planeOrientation, int algorithm, int slices)
{
	int summ[10];

	for(int j=0;j<10;j++)
		summ[j]=-1;

//	for(int i=0;i<4;i++)
//	{
		//sliceAndDistance.resize(4);
		//rankSumm[i].resize(
	//}
	Scalar mssimV,msurfV,mpsnrV;
	sliceRank sr_raw;
	vector<sliceRank> sr,sr_ranked,sr_ranked2;

	for(int i=0;i<slices;i++)
	{
		for(int k=0;k<slices;k++)
		{	
			switch(algorithm)
			{
			case 0:
				{
					mpsnrV=getPSNR(datasetSlices[0][i],datasetSlices[1][k]); //compara dataset1xdataset2 con PSNR	
					if(mpsnrV.val[0]==0) 
						sr_raw.value=100;
					else
						sr_raw.value = mpsnrV.val[0];
					sr_raw.sliceNumber=k;
					sr.push_back(sr_raw);
					break;
				}
			case 1:
				{
					mpsnrV=getPSNR(datasetSlices[0][i],datasetSlices[1][k]); //compara dataset1xdataset2 con PSNR	
					if(mpsnrV.val[0]==0) 
						sr_raw.value=100;
					else
						sr_raw.value = mpsnrV.val[0];
					sr_raw.sliceNumber=k;
					sr.push_back(sr_raw);
					break;
				}

			case 2:
				{
					mssimV=getMSSIM(datasetSlices[0][i],datasetSlices[1][k]);//compara dataset1xdataset2 com SSIM
					sr_raw.value = mssimV.val[0];
					sr_raw.sliceNumber=k;
					sr.push_back(sr_raw);
					break;
				}
			case 3:
				{
					msurfV=getSURF(datasetSlices[0][i],datasetSlices[1][k]); //compara dataset1xdataset2 com SURF
					sr_raw.value = msurfV.val[0];
					sr_raw.sliceNumber=k;
					sr.push_back(sr_raw);
					break;
				}

			default:
				break;
			}


		}
		sr_ranked.clear();
		sr_ranked=sr;
		sr.clear();
		
		ordenaRank(slices,sr_ranked,algorithm);
		//sr_ranked.resize(RANK_SIZE);

		if(algorithm==0) //se o algoritmo � o 0 faz o 2step verification com o SSIM
		{
			for(int k=0; k<RANK_SIZE;k++)
			{
				mssimV=getMSSIM(datasetSlices[0][i],datasetSlices[1][sr_ranked[k].sliceNumber]);//compara dataset1xdataset2 com SSIM
				sr_raw.value = mssimV.val[0];
				sr_raw.sliceNumber=sr_ranked[k].sliceNumber;			
				sr_ranked2.push_back(sr_raw);
			}

			ordenaRank(RANK_SIZE,sr_ranked2,algorithm);
			
			//printf("%d\n",i);
			//for(int k=0; k<RANK_SIZE;k++)
			//{
				//printf("<R1> sN:%d, v:%f - <R2> sN:%d, v:%f\n",sr_ranked[k].sliceNumber,sr_ranked[k].value,sr_ranked2[k].sliceNumber,sr_ranked2[k].value);
				//outFile<<sr_ranked[k].sliceNumber<<";"<<sr_ranked2[k].sliceNumber<<endl;
			//}
			//outFile << i<<endl; 
			for (int k=0; k<RANK_SIZE; k++)
			{
				int srr2=sr_ranked2[k].sliceNumber-i;
				if(srr2<0) srr2=srr2*-1;
				if(k==0)
				{
					sliceAndDistance[planeOrientation][algorithm].push_back(i); sliceAndDistance[planeOrientation][algorithm].push_back(srr2);
				} 
				//outFile<<i<<";"<<srr2<<endl;
				if(sr_ranked2[k].sliceNumber==i) //verifica se o SSIM encontrou o slice na mesma posi�ao do dataset analizado  
				{
					if(summ[k]==-1) summ[k]*=-1;
					else			summ[k]++;
					break;
				}

			}

			sr_ranked2.clear();
		}
		else // se o algoritmo != 0 monta ranking comum
		{	
			//printf("%d\n",i);
			for (int k=0; k<RANK_SIZE; k++)
			{
				//printf("<%d> sN:%d, v:%f\n",k,sr_ranked[k].sliceNumber,sr_ranked[k].value);

				int srr1=sr_ranked[k].sliceNumber-i;
				if(srr1<0) srr1=srr1*-1; 
				if(k==0)
				{
					sliceAndDistance[planeOrientation][algorithm].push_back(i); sliceAndDistance[planeOrientation][algorithm].push_back(srr1);
				} 
					//outFile<< i<<";"<<srr1<<endl;							
				if(sr_ranked[k].sliceNumber==i) //verifica se encontrou o slice na mesma posi�ao do dataset analizado  
				{
					if(summ[k]==-1) summ[k]*=-1;
					else			summ[k]++;	//incrementa summ para exibir qntos slices foram exatamente encontrados na mesma posi�ao do dataset analizado
					break;				
				}
			}			
		}
	}
	for(int i=0;i<slices;i+=2)
		cout << sliceAndDistance[planeOrientation][algorithm][i]<<" "<< sliceAndDistance[planeOrientation][algorithm][i+1];
	
	for(int i=0;i<RANK_SIZE;i++)
		rankSumm[algorithm].push_back(summ[i]);
//	int sumTotal=0;
//	for(int ll=0; ll < RANK_SIZE; ll++)
//	{
//		outFile <<endl<< summ[ll];
//		//cout <<endl<< summ[ll];
//		sumTotal+=summ[ll];
//	}
//	//printf("\n%d\n",sumTotal);
//	outFile << endl<<sumTotal<<endl;

	//return sr_ranked;
}



int loadDatasets(char** l_datasets,short unsigned int l_width, short unsigned int l_height, short unsigned int l_sliceRange[])
{

	// allocate memory for the 3d dataset
	for(int k=0; k<QNT_DATASETS;k++)
	{
		datasetRaw[k].resize(l_sliceRange[1]);
		for(int i=0;i<l_sliceRange[1];i++)
		{
			datasetRaw[k][i].resize(l_width);
			for(int j=0;j<l_width;j++)
			{
				datasetRaw[k][i][j].resize(l_height);
			}
		}
	}

	for(int k=0; k<QNT_DATASETS;k++)
	{
		if(!(inFile = fopen( l_datasets[k], "rb" ) ))
		{
			printf("Problems when tried to open the dataset %d\n",k);
			return -1;
		}
		for(int i=0; i<l_sliceRange[1];i++)
		{
			for(int j=0;j<l_width;j++)
			{
				for(int l=0;l<l_height;l++)
				{
					unsigned short value;
					fread( &value, 1, sizeof(unsigned short), inFile );
					datasetRaw[k][i][j][l] = value;
				}
			}
		}
		fclose(inFile);
	}

	return 1;
}

int changePlane(short unsigned int c_width, short unsigned int c_height, short unsigned int c_sliceRange[], short unsigned int c_planeOrientation)
{						
//	short unsigned int c_slices=c_sliceRange[1]-c_sliceRange[0];		  
	short unsigned int c_sliceF = c_sliceRange[1];
	if(c_planeOrientation!=0)
		swap(c_height,c_sliceF);

	for(int k=0; k<QNT_DATASETS;k++)
	{
		datasetNewPlane[k].resize(c_sliceF);
		for(int i=0;i<c_sliceF;i++)
		{
			datasetNewPlane[k][i].resize(c_width);
			for(int j=0;j<c_width;j++)
			{
				datasetNewPlane[k][i][j].resize(c_height);
			}
		}
	}

	for(int k=0; k<QNT_DATASETS;k++)
	{
		for(int i=0; i<c_sliceF;i++)
		{
			for(int j=0;j<c_width;j++)
			{
				for(int l=0;l<c_height;l++)
				{
					if(c_planeOrientation==0)
					{
						datasetNewPlane[k][i][j][l] = datasetRaw[k][i][j][l]; // XZ plane						

					}
					else if(c_planeOrientation==1)
					{
						if(k==1 && i>=8) //offset para a combinacao tardia x venosa e arterial
							datasetNewPlane[k][i-2][j][l] = datasetRaw[k][l][i][j]; // XZ plane
						else if(k==0)
							datasetNewPlane[k][i][j][l] = datasetRaw[k][l][i][j]; // XZ plane
					}
					else if(c_planeOrientation==2)
					{
						if(k==0 && i>=8) //offset para a combinacao tardia x venosa e arterial
							datasetNewPlane[k][i-2][j][l] = datasetRaw[k][l][j][i]; // YZ plane
						else if(k==1)
							datasetNewPlane[k][i][j][l] = datasetRaw[k][l][j][i]; // YZ plane
					}
				}
			}
		}
		datasetNewPlane[k].erase(datasetNewPlane[k].begin(),datasetNewPlane[k].begin()+c_sliceRange[0]);
		datasetNewPlane[k].resize(c_sliceRange[1]-c_sliceRange[0]);		
	}

	return 1;
}


void splitDatasets(short unsigned int s_width,short unsigned int s_height,short unsigned int s_sliceRange[],short unsigned int s_planeOrientation)
{		
	short unsigned int s_slices=s_sliceRange[1]-s_sliceRange[0];
	short unsigned int s_sliceF = s_sliceRange[1];
	if(s_planeOrientation!=0)
	{
		//swap(s_height,s_slices);
		//s_slices-=s_sliceRange[0];
		s_height=s_sliceF;
	}

	// split the dataset into image planes for easy data access
	datasetWHShrink[0] = (unsigned short**)malloc(s_slices * sizeof(unsigned short*));
	datasetWHShrink[1] = (unsigned short**)malloc(s_slices * sizeof(unsigned short*));

	for (int i=0; i < s_slices; i++)
	{
		datasetWHShrink[0][i] = (unsigned short*)malloc(sizeof(unsigned short) * (s_height*s_width));
		datasetWHShrink[1][i] = (unsigned short*)malloc(sizeof(unsigned short) * (s_height*s_width));
	}
	
	for( int k = 0; k < QNT_DATASETS; k++ )
	{
		for( int i = 0; i < s_slices; i++ )
		{
			for(int j=0; j<s_width;j++)
			{
				for(int l=0; l<s_height;l++)
				{
					if(s_planeOrientation==0)
						datasetWHShrink[k][i][l * s_width + j]=datasetRaw[k][i][l][j];	
					else
						datasetWHShrink[k][i][l * s_width + j]=datasetNewPlane[k][i][j][l];
				}
			}
		}
	}

	for( int k = 0; k < QNT_DATASETS; k++ )
	{
		for( int i = 0; i < s_slices; i++ )
		{
			Mat slice(s_height,s_width,CV_16UC1,datasetWHShrink[k][i]);
			Mat plane;
			slice.convertTo(plane,CV_8UC3);
			datasetSlices[k].push_back(plane);
		}
	}
}


int rankBuilder(char* dataset[], int width, int height,short unsigned int sliceRange[], int algorithm, int planeOrientation)
{

	int slices=sliceRange[1]-sliceRange[0];

	//monta nome do arquivo de log
	char nFile[200];
	stringstream output;
	output << dataset[0]<<"_"<<dataset[1]<<"_"<<slices<<"_combined";
	string sulfix = output.str();
	const char* ss = sulfix.c_str();
	//strcpy(nFile,"Logs/");
	//strcat(nFile,ss);
	strcpy(nFile,"a");
	strcat(nFile,".csv");
	outFile.open(nFile, ios::out);
	if(outFile.fail())
	{
		printf( "Could not open output file %s.\n", nFile );
		exit(-1);
	}
	//escreve cabe�alho do arquivo de log
	//outFile<<"Dataset1;"<<"Dataset2;"<<"width;"<<"height;"<<"slices;"<<"algorithm;"<<"planeOrientation"<<endl;
	outFile<<width<<"x"<<height<<"x"<<slices<<";["<<sliceRange[0]<<"-"<<sliceRange[1]<<"]; Plane:"<<planeOrientation<<endl;
	outFile<<"Plane:0;;;;;;;;;Plane:1;;;;;;;;;Plane:2"<<endl;
	outFile<<"PSNR+SSIM;;;PSNR;;;SSIM;;;PSNR+SSIM;;;PSNR;;;SSIM;;;PSNR+SSIM;;;PSNR;;;SSIM"<<endl;
	double timeTotal = (double)getTickCount();		

	printf("Loading datasets...  ");
	if(loadDatasets(dataset,width,height,sliceRange)==-1)
		return -1;		
	printf("DONE!\n");

	if(algorithm==4)//se algorithm == 3 os datasets s�o processados utilizando-se todas as t�cnicas 
	{
		for(int i=0;i<3;i++)
		{
			for(int j=0;j<3;j++)
			{
				printf("Changing plane...  ");
				if(changePlane(width,height,sliceRange,j)==-1)
					return -1;
				printf("DONE!\n");

				printf("Spliting datasets...  ");
				splitDatasets(width,height,sliceRange,j);
				printf("DONE!\n");	
				calculaSimilaridade(j,i,slices);		
			}
		}
	}
	else
	{
		printf("Changing plane...  ");
		if(changePlane(width,height,sliceRange,planeOrientation)==-1)
			return -1;
		printf("DONE!\n");

		printf("Spliting datasets...  ");
		splitDatasets(width,height,sliceRange,planeOrientation);
		printf("DONE!\n");		
	
		calculaSimilaridade(planeOrientation,algorithm,slices);
	}

	timeTotal = ((double)getTickCount() - timeTotal)/getTickFrequency();
	
	//build the csv formated

	for(int i=0;i<slices;i+=2)
	{
		for(int j=0;j<3;j++)
		{
			for(int k=0;k<3;k++)
			{
				cout<<sliceAndDistance[k][j][i]<<" "<<sliceAndDistance[k][j][i+1]<<endl;
				outFile<<sliceAndDistance[k][j][i]<<";"<<sliceAndDistance[k][j][i+1]<<";;";
			}
		}
		outFile<<endl;
	}


	outFile << endl<<"Total time: " << timeTotal <<endl;
	return 0;
}


int main(int argc, char *argv[])
{
	
	char* dataset[2];
	short unsigned int sliceRange[2]={0,0};
	dataset[0]=argv[1];
	dataset[1]=argv[2];
	short unsigned int width = atoi(argv[3]);
	short unsigned int height = atoi(argv[4]);
	sliceRange[0] = atoi(argv[5]);
	sliceRange[1] = atoi(argv[6]);
	short unsigned int algorithm = atoi(argv[7]);
	short unsigned int planeOrientation = atoi(argv[8]);

	short unsigned int slices = sliceRange[1]-sliceRange[0];

	printf("Building ranking...  ");	
	if(rankBuilder(dataset,width,height,sliceRange,algorithm,planeOrientation)==-1)
		return -1;

	printf("DONE!\n");

	namedWindow("Trackbar",0);
	if(planeOrientation==0)
		createTrackbar("TB","Trackbar",0,slices-1,onTrackbar);
	else
		createTrackbar("TB","Trackbar",0,slices-1,onTrackbar);
	onTrackbar(0,0);

	waitKey(0);
	destroyAllWindows();
    return 0;
}

Scalar getPSNR(const Mat& i1, const Mat& i2)
{
    Mat s1;
    absdiff(i1, i2, s1);       // |i1 - i2|
    s1.convertTo(s1, CV_32F);  // cannot make a square on 8 bits
    s1 = s1.mul(s1);           // |i1 - i2|^2

    Scalar s = sum(s1);         // sum elements per channel

    double sse = s.val[0] + s.val[1] + s.val[2]; // sum channels

    if( sse <= 1e-10) // for small values return zero
        return 0;
    else
    {
        double  mse =sse /(double)(i1.channels() * i1.total());
        Scalar psnr;
		psnr.val[0]= 10.0*log10((255*255)/mse);
        return psnr;
    }
}
#define GB_R 10.5
#define GB_S 111
Scalar getMSSIM( const Mat& i1, const Mat& i2)
{
    const double C1 = 6.5025, C2 = 58.5225;
    /***************************** INITS **********************************/
    int d     = CV_32F;

    Mat I1, I2;
    i1.convertTo(I1, d);           // cannot calculate on one byte large values
    i2.convertTo(I2, d);

    Mat I2_2   = I2.mul(I2);        // I2^2
    Mat I1_2   = I1.mul(I1);        // I1^2
    Mat I1_I2  = I1.mul(I2);        // I1 * I2

    /*************************** END INITS **********************************/

    Mat mu1, mu2;   // PRELIMINARY COMPUTING
    GaussianBlur(I1, mu1, Size(GB_S, GB_S), GB_R);
    GaussianBlur(I2, mu2, Size(GB_S, GB_S), GB_R);

    Mat mu1_2   =   mu1.mul(mu1);
    Mat mu2_2   =   mu2.mul(mu2);
    Mat mu1_mu2 =   mu1.mul(mu2);

    Mat sigma1_2, sigma2_2, sigma12;

    GaussianBlur(I1_2, sigma1_2, Size(GB_S, GB_S), GB_R);
    sigma1_2 -= mu1_2;

    GaussianBlur(I2_2, sigma2_2, Size(GB_S, GB_S), GB_R);
    sigma2_2 -= mu2_2;

    GaussianBlur(I1_I2, sigma12, Size(GB_S, GB_S), GB_R);
    sigma12 -= mu1_mu2;

    ///////////////////////////////// FORMULA ////////////////////////////////
    Mat t1, t2, t3;

    t1 = 2 * mu1_mu2 + C1;
    t2 = 2 * sigma12 + C2;
    t3 = t1.mul(t2);              // t3 = ((2*mu1_mu2 + C1).*(2*sigma12 + C2))

    t1 = mu1_2 + mu2_2 + C1;
    t2 = sigma1_2 + sigma2_2 + C2;
    t1 = t1.mul(t2);               // t1 =((mu1_2 + mu2_2 + C1).*(sigma1_2 + sigma2_2 + C2))

    Mat ssim_map;
    divide(t3, t1, ssim_map);      // ssim_map =  t3./t1;

    Scalar mssim = mean( ssim_map ); // mssim = average of ssim map
	return mssim;
}

Scalar getSURF( const Mat& i1, const Mat& i2)
{

  //-- Step 1: Detect the keypoints using SURF Detector
  int minHessian = 5000;

  SurfFeatureDetector detector( minHessian );

  std::vector<KeyPoint> keypoints_1, keypoints_2;

  detector.detect( i1, keypoints_1 );
  detector.detect( i2, keypoints_2 );

  SurfDescriptorExtractor extractor; //Calculate descriptors (feature vectors)

  Mat descriptors_1, descriptors_2;

  extractor.compute( i1, keypoints_1, descriptors_1 );
  extractor.compute( i2, keypoints_2, descriptors_2 );

  
  FlannBasedMatcher matcher; //Matching descriptor vectors using FLANN matcher
  std::vector< DMatch > matches;
  matcher.match( descriptors_1, descriptors_2, matches );

  double max_dist = 0; double min_dist = 100;

  //Calculation of max and min distances between keypoints
  for( int i = 0; i < descriptors_1.rows; i++ )
  { double dist = matches[i].distance;
    if( dist < min_dist ) min_dist = dist;
    if( dist > max_dist ) max_dist = dist;
  }

  //-- Draw only "good" matches (i.e. whose distance is less than 2*min_dist )
  //-- PS.- radiusMatch can also be used here.
  std::vector< DMatch > good_matches;

  /*
  for( int i = 0; i < descriptors_1.rows; i++ )
  { if( matches[i].distance < 2*min_dist )
    { good_matches.push_back( matches[i]); }
  }

  //-- Draw only "good" matches
  Mat img_matches;

  drawMatches( i1, keypoints_1, i2, keypoints_2,
	  good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
               vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );

  //-- Show detected matches
  imshow( "Good Matches", img_matches );

  for( int i = 0; i < good_matches.size(); i++ )
  { printf( "-- Good Match [%d] Keypoint 1: %d  -- Keypoint 2: %d  \n", i, good_matches[i].queryIdx, good_matches[i].trainIdx ); }
   printf("-- Max dist : %f \n", max_dist );
  printf("-- Min dist : %f \n", min_dist );
*/

  Scalar v;
  v.val[0]=min_dist;
  v.val[1]=max_dist;
  return v;
}