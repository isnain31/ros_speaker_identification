#include "ros/ros.h"
#include "std_msgs/String.h"
#include "liatools.h"
#include "alize.h"
#include "NormFeat.h"
#include "EnergyDetector.h"
#include "ComputeTest.h"
 #include <iostream>
#include <stdio.h>
#include <dirent.h>
#include <fstream>
 #include <sstream>
#include <string>

void runEnergyDetector(int argc, char *argv[]);
void runNormFeat(int argc, char *argv[]);
void runComputeTest(int argc, char *argv[]);
void clear(const char *dirname);
void clearAll();
void generateNdx(string wavFilane);
string baseDirectory;


void identifireCallback(const std_msgs::String::ConstPtr& msg)
{

  string mfcc_commad,normConfig,testConfig,energyConfig,featureFilesPath,labelFilesPath,mixtureFilesPath,ndxFilename,outputFilename; 
   
  featureFilesPath=baseDirectory+"/data/prm/";

  char * chFeatureFilesPath = new char[featureFilesPath.length() + 1];
  strcpy(chFeatureFilesPath,featureFilesPath.c_str());	

  labelFilesPath=baseDirectory+"/data/lbl/";

  char * chLabelFilesPath = new char[labelFilesPath.length() + 1];
  strcpy(chLabelFilesPath,labelFilesPath.c_str());	

  mixtureFilesPath=baseDirectory+"/gmm/"; 

  char * chMixtureFilesPath = new char[mixtureFilesPath.length() + 1];
  strcpy(chMixtureFilesPath,mixtureFilesPath.c_str());	

  energyConfig=baseDirectory+"/cfg/EnergyDetector.cfg";
  char * chEnergyConfig= new char[energyConfig.length() + 1];
  strcpy(chEnergyConfig,energyConfig.c_str());	

  normConfig=baseDirectory+"/cfg/NormFeat.cfg";
  char * chNormConfig= new char[normConfig.length() + 1];
  strcpy(chNormConfig,normConfig.c_str());	

  testConfig=baseDirectory+"/cfg/ComputeTest.cfg";
  char * chTestConfig= new char[testConfig.length() + 1];
  strcpy(chTestConfig,testConfig.c_str());
  	
  ndxFilename=baseDirectory+"/ndx.train";
  char * chNdxFilename= new char[ndxFilename.length() + 1];
  strcpy(chNdxFilename,ndxFilename.c_str());

  outputFilename=baseDirectory+"/res/"+msg->data+".res";
  char * chOutputFilename= new char[outputFilename.length() + 1];
  strcpy(chOutputFilename,outputFilename.c_str());

  generateNdx(msg->data);

  char * chInputFeatureFilename=new char[msg->data.length() + 1];
  strcpy(chInputFeatureFilename,msg->data.c_str());

  mfcc_commad="sfbcep -F WAVE -p 24 -F WAVE  "+baseDirectory+"/data/"+msg->data+" "+baseDirectory+"/data/prm/"+msg->data+".mfcc";	
  system(mfcc_commad.c_str());

  char* EnergyDetect[] = {"EnergyDetector", "--config", chEnergyConfig,"--inputFeatureFilename",chInputFeatureFilename,"--featureFilesPath",chFeatureFilesPath,"--labelFilesPath",chLabelFilesPath};
  runEnergyDetector(sizeof( EnergyDetect ) / sizeof( EnergyDetect[ 0 ]),EnergyDetect);

  char* normalize[] ={"NormFeat", "--config", chNormConfig,"--inputFeatureFilename",chInputFeatureFilename,"--featureFilesPath",chFeatureFilesPath,"--labelFilesPath",  chLabelFilesPath};
  runNormFeat(sizeof( normalize ) / sizeof( normalize[ 0 ]),normalize);

  char* computeTest[] ={"ComputeTest", "--config", chTestConfig,"--mixtureFilesPath",chMixtureFilesPath,"--featureFilesPath",chFeatureFilesPath,"--labelFilesPath",  chLabelFilesPath,"--ndxFilename",chNdxFilename,"--outputFilename",chOutputFilename};
  runComputeTest( sizeof( computeTest ) / sizeof( computeTest[ 0 ] ) ,computeTest);
  
}

int main (int argc, char *argv[])
{
  using namespace std;
  using namespace alize;

  if(argc<2)	{
	std::cout<< "Please specify the file name";
	return 0;
  }	

  baseDirectory=(string)argv[1];

  clearAll();

  ros::init(argc, argv, "identifire");
  ros::NodeHandle n;
  ros::Subscriber sub = n.subscribe("/audio_file", 1000, identifireCallback);
  ros::spin();


  

return 0;
}


void generateNdx(string wavFilane){
  string ndxFilesPath,wavFilane_;
  ndxFilesPath=baseDirectory+"/_ndx.train"; 	

  std::ifstream ifs(ndxFilesPath.c_str());
  std::string content,output_content;

  content.assign( (std::istreambuf_iterator<char>(ifs) ),
                (std::istreambuf_iterator<char>()    ) );
  std::cout << content;	
  ifs.close();
  
  ndxFilesPath=baseDirectory+"/ndx.train"; 	
  wavFilane_=wavFilane+" ";
  content.insert(0,wavFilane_.c_str());
  ofstream myfile;
  myfile.open (ndxFilesPath.c_str());
  myfile << content;
  myfile.close();	
}

void clearAll(){

	string featureFilesPath,labelFilesPath,outputFilesPath;
	featureFilesPath=baseDirectory+"/data/prm/";
	labelFilesPath=baseDirectory+"/data/lbl/";
	outputFilesPath=baseDirectory+"/res/";
	clear(featureFilesPath.c_str());
	clear(labelFilesPath.c_str());
	clear(outputFilesPath.c_str());
}

void clear(const char* dirname){

    struct dirent *next_file;
    DIR *theFolder;

    char filepath[256];

    theFolder = opendir(dirname);

    while ( next_file = readdir(theFolder) )
    {
        // build the full path for each file in the folder
        sprintf(filepath, "%s/%s", dirname, next_file->d_name);
	//std::cout << filepath << std::endl;
        remove(filepath);
    }

}


void runNormFeat(int argc, char *argv[]){

ConfigChecker cc;
    cc.addStringParam("config", false, true, "default config filename");
    cc.addStringParam("mode", true, true, "<norm: mean, var, warp normalization | info: save stats of file | featmap: feature mapping | FA EigenChannel Compensation >");
    cc.addStringParam("inputFeatureFilename",true,true,"input feature - could be a simple feature file or a list of filename");
    cc.addStringParam("labelSelectedFrames",true,true,"Only the frames from segments with this label  will be used");
    cc.addBooleanParam("fileMode",false,true,"One normalisation for all the selected data, could be applied after segmental/window modes if both are selected");  
    cc.addBooleanParam("segmentalMode",false,true,"if set to true, one normalisation by segment is computed");  
    cc.addBooleanParam("windowMode",false,true,"if set to true, the normalisation is computed by windows");  
    cc.addFloatParam("windowDuration",false,true,"Length of the window (for window mode) in seconds (default=5)");
    cc.addFloatParam("windowComp",false,true,"compensation factor for window mode (default =1.0 - no compensation");
    cc.addBooleanParam("writeAllFeatures",false,true,"if set to true,values for all the input frames are outputed (default true)");  
    cc.addStringParam("featureServerMode",true,true,"FEATURE_WRITABLE to write normalized features");  
    cc.addStringParam("outputInfoFilename",false,true,"The complete output filename for the info file");
    cc.addBooleanParam("cmsOnly",false,true,"If cmsOnly is set, only remove means from features (default false)");
    cc.addBooleanParam("varOnly",false,true,"If cmsOnly is set, reduce only variance from features (default false)");
    cc.addBooleanParam("warp",false,true,"If warp is set, uses featureWarping (default false)");
    cc.addIntegerParam("warpBinCount",false,true,"If warp is set, this parameter fixes the number of bins in the data histo (default=40)");
    cc.addStringParam("warpGaussHistoFilename",false,true,"for warping, if this parameter is set, it loads the destination histo, default it creates it (N(0,1))");
    cc.addBooleanParam("warpAdd01Norm",false,true,"for warping, add a global N(0,1) normalisation after warping (default false)");  
    cc.addStringParam("externalStatsFilename",false,true,"filename containing external stats to apply for normalization, usually obtained with info mode (opt.)");
    cc.addFloatParam("frameLength",false,true,"length of a frame, by default 10ms");
  try
  {
    CmdLine cmdLine (argc, argv);
    if (cmdLine.displayHelpRequired ()){	// --help
      cout << "NormFeat" << endl;
      cout << "NormFeat.exe --config <foo.cfg> --inputFeatureFileName <foo.prm> --mode <norm|info|featMap>"<< endl;
      cout<<cc.getParamList()<<endl;
      }
    else if (cmdLine.displayVersionRequired ())	// --version
      cout << "Version 2.0" << endl;
    else{
      Config tmp;
      cmdLine.copyIntoConfig (tmp);
      Config config;
      if (tmp.existsParam("config")) config.load(tmp.getParam("config"));
      cmdLine.copyIntoConfig(config);
      cc.check(config);
      debug=config.getParam_debug();
      if (config.existsParam("verbose")) verbose=config.getParam("verbose").toBool();
      else verbose=false;
      if (verbose) verboseLevel=1;else verboseLevel=0;
      if (config.existsParam("verboseLevel"))verboseLevel=config.getParam("verboseLevel").toLong();
      if (verboseLevel>0) verbose=true;
      String mode=config.getParam("mode");
      if ( mode == "norm"){normFeat(config);}
      else if ( mode == "info"){infoFeat(config);}
      else if (mode == "featMap"){featMap(config);}
      else if (mode == "NAP"){normFeatNAP(config);}      
      else if (mode == "FA"){normFeatFA(config);}      
      else if (mode == "LFA"){normFeatLFA(config);}      
      else throw Exception("Error: Mode unknown!",__FILE__,__LINE__);
    }
  }
  catch (alize::Exception & e) {cout << e.toString () << endl << cc.getParamList() << endl;}
  #ifdef NDEBUG 
  cout<<"*** Objects created and destroyed **"<<Object::getCreationCounter()<<"-"<<Object::getDestructionCounter()<<endl;    
  #endif

}


void runComputeTest(int argc, char *argv[]){

ConfigChecker cc;
		cc.addStringParam("config", false, true, "default config filename");
		cc.addStringParam("ndxFilename",true,true, "NDX file listing all verification tests to achieve: first column: test File, all others: models");	
		cc.addStringParam("outputFilename",true,true, "output scores in this file: 'gender - test model - scores'");	
		cc.addStringParam("inputWorldFilename",true,true,"model repsresenting the H0 hypothesis to get the LLR");
		cc.addStringParam("labelSelectedFrames",true,true,"the segments with this label are used for training the worldmodel");		
		cc.addStringParam("computeLLKWithTopDistribs",true,true, "PARTIAL/COMPLETE: will compute LLK with topdistribs. COMPLETE: add world LLK to client LLK, PARTIAL: just keeps the topDistrib LLK");
		cc.addIntegerParam("topDistribsCount ",false,true,"Number of distrib to approximate complete LLK");
		cc.addStringParam("gender",true,true, "M/F: will output the gender in score file");	
		cc.addStringParam("fileLLR",false,true, "will output a score for the entire file (default true)");
		cc.addStringParam("segmentLLR",false,true, "will output a score for each speech segment (default false)");
		cc.addStringParam("windowLLR",false,true, "windowLLR: will output a score for each set of windowLLRSize frames. windowLLRDec gives the shift of the window (default false)");
		cc.addIntegerParam("windowLLRSize",false,true, "if windowLLR is set, gives the size of the window (default 30)");
		cc.addIntegerParam("windowLLRDec",false,true, "if windowLLR is set, gives the shift of the window (default windowLLRSize)");
		cc.addBooleanParam("byLabelModel",false,true, "if the parameter is present, we work with a model by client and by  cluster (default false)");
		cc.addBooleanParam("histoMode",false,true, "if the parameter is present, entropy of LR distrib is used as score (default false)");
		cc.addStringParam("channelCompensation",true,true,"if set to NAP or JFA, launch the corresponding channel compensation (default not set)");
	

		try {
		CmdLine cmdLine(argc, argv);
		if (cmdLine.displayHelpRequired()){
		cout << "************************************" << endl;
		cout << "********** ComputeTest.exe **********" << endl;
		cout << "************************************" << endl;
		cout << endl;
		cout << "LLR computation for an NDX (NIST format) File" << endl;
		cout << "" << endl;
		cout << cc.getParamList()<<endl;
		return;
		}
		if (cmdLine.displayVersionRequired()){cout <<"ComputeTest - for computing the LLR using nixt style ndx files"<<endl;} 
		Config tmp;
		cmdLine.copyIntoConfig(tmp);
		Config config;
		if (tmp.existsParam("config")) config.load(tmp.getParam("config"));
		cmdLine.copyIntoConfig(config);
		cc.check(config);
		debug=config.getParam_debug();
		if (config.existsParam("verbose"))verbose=config.getParam("verbose").toBool();else verbose=false;
		if (verbose) verboseLevel=1;else verboseLevel=0;
		if (config.existsParam("verboseLevel"))verboseLevel=config.getParam("verboseLevel").toLong();
		if (verboseLevel>0) verbose=true;
		ComputeTest(config);
	}
		catch (alize::Exception& e) {cout << e.toString() << endl << cc.getParamList()<< endl;}
}

void runEnergyDetector(int argc, char *argv[]){

ConfigChecker cc;
    cc.addStringParam("config", false, true, "default config filename");
    cc.addStringParam("inputFeatureFilename",true,true,"input feature - could be a simple feature file or a list of filename");
    cc.addStringParam("labelFilesPath",false,true,"path of input and output labels");
    cc.addStringParam("saveLabelFileExtension",true,true,"output labelFile extension");
    cc.addStringParam("labelOutputFrames",true,true,"output label tag");
    cc.addFloatParam("alpha",true,true,"threshold giving the percentage of data of the middle Gaussian to take into account");
    cc.addStringParam("segmentalMode",true,true,"file to have a file by file behaviour");

    // gestion du thresholdMode obligatoire
    cc.addStringParam("thresholdMode",true,true,"this parameter must be set to select 3 top gaussian. It's a default parameter");

  try
  {
    CmdLine cmdLine (argc, argv);
    if (cmdLine.displayHelpRequired ()){	// --help
	cout << "************************************" << endl;
	cout << "********** EnergyDetector.exe *********" << endl;
	cout << "************************************" << endl;
	cout << endl;
	cout << "Speech/Non Speech Detector." << endl;
	cout<<cc.getParamList()<<endl;
      }
    else if (cmdLine.displayVersionRequired ())
      cout << "Version 2beta" << endl;
    else{
      // copy parameters from command line into config
      Config tmp;
      cmdLine.copyIntoConfig (tmp);
      Config config;
      if (tmp.existsParam("config")) config.load(tmp.getParam("config"));
      cmdLine.copyIntoConfig(config);
      cc.check(config);
      debug=config.getParam_debug();
      if (config.existsParam("verbose")) verbose=config.getParam("verbose").toBool();
      else verbose=false;
      if (verbose) verboseLevel=1;else verboseLevel=0;
      if (config.existsParam("verboseLevel"))verboseLevel=config.getParam("verboseLevel").toLong();
      if (verboseLevel>0) verbose=true;
	      
	bool energy=true;
	if (config.existsParam("turnDetection")) energy=false;                 // choice between energy detection or GLR turn detection
	String inputFeatureFileName =config.getParam("inputFeatureFilename");  // input feature - could be a simple feature file or a list of filename
	String extOutput=".lbl";                                               // the extension of the output files    
	if (config.existsParam("saveLabelFileExtension")) extOutput=config.getParam("saveLabelFileExtension");   
	String pathOutput="./";                                                // the path of the output files    
	if (config.existsParam("labelFilesPath")) pathOutput=config.getParam("labelFilesPath");
	XLine inputFeatureFileNameList;                                        // The (feature) input filename list - list of files to process
	    try{                                            
	      XList inputFileNameXList(inputFeatureFileName,config);               // Read the filename list file if it is really a list
	      inputFeatureFileNameList=inputFileNameXList.getAllElements();        // And put the filename in a list if the file is a list of feature filenames
	    }
	    catch(FileNotFoundException& e){                                       // It was a simple feature file and not a filename list
	      inputFeatureFileNameList.addElement(inputFeatureFileName);           // add the filename in the list
	    }
      String *file;
      while ((file=inputFeatureFileNameList.getElement())!= NULL){         // Loop on each feature file
	String & fileName=*file;                                           // current input file basename 
	SegServer segServer;                                               // Create the segment server for dealing with selected/unselected segments
	if (energy){                                                       // The energy detector
	  SegCluster &outputSeg=energyDetector(config,segServer,fileName); 
	  outputLabelFile(outputSeg,pathOutput+fileName+extOutput,config); 
	}
      }                                                                    // end of file loop
    }
  }
  catch (alize::Exception & e) {cout << e.toString () << endl << cc.getParamList() << endl;}

}
