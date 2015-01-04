#include "ros/ros.h"
#include "std_msgs/String.h"
#include "liatools.h"
#include "alize.h"
#include "NormFeat.h"
#include "EnergyDetector.h"
#include "ComputeTest.h"
#include "ComputeNorm.h"
#include "Scoring.h"
 #include <iostream>
#include <stdio.h>
#include <dirent.h>
#include <fstream>
 #include <sstream>
#include <string>

void runEnergyDetector(int argc, char *argv[]);
void runNormFeat(int argc, char *argv[]);
void runComputeTest(int argc, char *argv[]);
void runComputeNorm(int argc, char *argv[]);
alize::String runScoring(int argc, char *argv[]);
void clear(const char *dirname);
void clearAll();
void generateNdx(string wavFilane);
string baseDirectory;
//ros::NodeHandle n;
//ros::Publisher chatter_pub = n.advertise<std_msgs::String>("chatter", 1000);

void identifireCallback(ros::Publisher &speaker_pub,const std_msgs::String::ConstPtr& msg)
{

  string mfcc_commad,normConfig,testConfig,energyConfig,featureFilesPath,labelFilesPath,mixtureFilesPath,ndxFilename,outputFilename; 
  string ndxImpFilename,outputImpFilename,tnormConfig,outputFileBaseName,decisionFileName,tnormfile;	
   
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

  ndxImpFilename=baseDirectory+"/ndx.imp.train";
  char * chNdxImpFilename= new char[ndxImpFilename.length() + 1];
  strcpy(chNdxImpFilename,ndxImpFilename.c_str());

  outputFilename=baseDirectory+"/res/"+msg->data+".res";
  char * chOutputFilename= new char[outputFilename.length() + 1];
  strcpy(chOutputFilename,outputFilename.c_str());

  outputImpFilename=baseDirectory+"/res/"+msg->data+".imp.res";
  char * chOutputImpFilename= new char[outputImpFilename.length() + 1];
  strcpy(chOutputImpFilename,outputImpFilename.c_str());

  tnormConfig=baseDirectory+"/cfg/ComputeNorm_tnorm.cfg";
  char * chTnormConfig= new char[tnormConfig.length() + 1];
  strcpy(chTnormConfig,tnormConfig.c_str());

  outputFileBaseName=baseDirectory+"/res/"+msg->data+".final.res";
  char * chOutputFileBaseName=new char[outputFileBaseName.length() + 1];
  strcpy(chOutputFileBaseName,outputFileBaseName.c_str());

  tnormfile=baseDirectory+"/res/"+msg->data+".final.res.tnorm";	
  char * chTnormfile=new char[tnormfile.length() + 1];
  strcpy(chTnormfile,tnormfile.c_str());

  decisionFileName=baseDirectory+"/res/"+msg->data+".socre.nist";
  char * chCecisionFileName=new char[decisionFileName.length() + 1];
  strcpy(chCecisionFileName,decisionFileName.c_str());


  generateNdx(msg->data);

  char * chInputFeatureFilename=new char[msg->data.length() + 1];
  strcpy(chInputFeatureFilename,msg->data.c_str());

  mfcc_commad="sfbcep -F WAVE -p 24 -F WAVE  "+baseDirectory+"/capture/"+msg->data+" "+baseDirectory+"/data/prm/"+msg->data+".mfcc";	
  system(mfcc_commad.c_str());

  char* EnergyDetect[] = {"EnergyDetector", "--config", chEnergyConfig,"--inputFeatureFilename",chInputFeatureFilename,"--featureFilesPath",chFeatureFilesPath,"--labelFilesPath",chLabelFilesPath};
  runEnergyDetector(sizeof( EnergyDetect ) / sizeof( EnergyDetect[ 0 ]),EnergyDetect);

  char* normalize[] ={"NormFeat", "--config", chNormConfig,"--inputFeatureFilename",chInputFeatureFilename,"--featureFilesPath",chFeatureFilesPath,"--labelFilesPath",  chLabelFilesPath};
  runNormFeat(sizeof( normalize ) / sizeof( normalize[ 0 ]),normalize);

  char* computeTest[] ={"ComputeTest", "--config", chTestConfig,"--mixtureFilesPath",chMixtureFilesPath,"--featureFilesPath",chFeatureFilesPath,"--labelFilesPath",  chLabelFilesPath,"--ndxFilename",chNdxFilename,"--outputFilename",chOutputFilename};
  runComputeTest( sizeof( computeTest ) / sizeof( computeTest[ 0 ] ) ,computeTest);

  char* computeImpTest[] ={"ComputeTest", "--config", chTestConfig,"--mixtureFilesPath",chMixtureFilesPath,"--featureFilesPath",chFeatureFilesPath,"--labelFilesPath",  chLabelFilesPath,"--ndxFilename",chNdxImpFilename,"--outputFilename",chOutputImpFilename};
  runComputeTest( sizeof( computeImpTest ) / sizeof( computeImpTest[ 0 ] ) ,computeImpTest);	
  
  char* computeNorm[] ={"ComputeNorm", "--config", chTnormConfig,"--testNistFile",chOutputFilename,"--tnormNistFile",chOutputImpFilename,"--outputFileBaseName",chOutputFileBaseName};
  runComputeNorm( sizeof( computeNorm ) / sizeof( computeNorm[ 0 ] ) ,computeNorm);	

  char* scoring[] ={"Scoring", "--mode", "NIST","--inputFile",chTnormfile,"--outputFile",chCecisionFileName,"--threshold","2","--segTypeTest", "1side", "--trainTypeTest", "1side", "--adaptationMode", "n"};

  alize::String speaker=runScoring( sizeof( scoring ) / sizeof( scoring[ 0 ] ) ,scoring);
  //std::cout<< speaker; 	

  //ros::NodeHandle n;
  //ros::Publisher speaker_pub = node_handle.advertise<std_msgs::String>("speaker", 1000);
  

  //msgs.data=speaker.c_str();	
  
   std_msgs::String msgs;
   std::stringstream ss;
   ss << speaker.c_str();
   msgs.data = ss.str();	
   ROS_INFO("%s",speaker.c_str());
   speaker_pub.publish(msgs);
   //ros::spinOnce();
   

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
  ros::Publisher pub = n.advertise<std_msgs::String>("/speaker", 1000);
  ros::Subscriber sub = n.subscribe<std_msgs::String>("/audio_file", 1000,  boost::bind(&identifireCallback, boost::ref(pub), _1) );
  ros::spin();


  

return 0;
}


void generateNdx(string wavFilane){
  string ndxFilesPath,wavFilane_,impFilesPath,impFilesPath_;
  ndxFilesPath=baseDirectory+"/_ndx.train"; 	

  std::ifstream ifs(ndxFilesPath.c_str());
  std::string content,output_content,imp_content;

  content.assign( (std::istreambuf_iterator<char>(ifs) ),
                (std::istreambuf_iterator<char>()    ) );
  //std::cout << content;	
  ifs.close();

  impFilesPath_=baseDirectory+"/imp.train"; 
  std::ifstream ifp(impFilesPath_.c_str());
  

  imp_content.assign( (std::istreambuf_iterator<char>(ifp) ),
                (std::istreambuf_iterator<char>()    ) );
 
  ifp.close();

  
  ndxFilesPath=baseDirectory+"/ndx.train"; 	
  wavFilane_=wavFilane+" ";
  content.insert(0,wavFilane_.c_str());
  ofstream myfile;
  myfile.open (ndxFilesPath.c_str());
  myfile << content;
  myfile.close();	

  impFilesPath=baseDirectory+"/ndx.imp.train";
  imp_content.insert(0,wavFilane_.c_str());
  ofstream impNdxTrain;
  impNdxTrain.open (impFilesPath.c_str());
  impNdxTrain << imp_content;
  impNdxTrain.close();

}

void clearAll(){

	string featureFilesPath,labelFilesPath,outputFilesPath,capturedFilesPath;
	featureFilesPath=baseDirectory+"/data/prm/";
	labelFilesPath=baseDirectory+"/data/lbl/";
	outputFilesPath=baseDirectory+"/res/";
	capturedFilesPath=baseDirectory+"/capture/";
	clear(capturedFilesPath.c_str());
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

alize::String runScoring(int argc, char *argv[]){
	alize::String currentSpeaker;
	ConfigChecker cc;
	cc.addStringParam("mode",true,true,"NIST,ETF,MDTM,leaveMaxOutTnorm");
	cc.addStringParam("inputFile",true,true,"fooIn.nist");
	cc.addStringParam("outputFile",true,true,"fooOut.nist");
	cc.addStringParam("adaptationMode",false,true,"u|n");
	cc.addStringParam("segTypeTest",false,true,"<10sec|...|1side>");
	cc.addStringParam("trainTypeTest",false,true,"<10sec|...|1side>");
	cc.addIntegerParam("threshold",false,true,"threshold on scores to accept or reject test");
	cc.addIntegerParam("hard",false,false,"in ETF accept one speaker per test (identification)");
	try {
		CmdLine cmdLine(argc, argv);
		if (cmdLine.displayVersionRequired()){cout <<"Version 2-beta"<<endl;} 
		if (cmdLine.displayHelpRequired()){
			cout << "************************************" << endl;
			cout << "********** Scoring.exe *************" << endl;
			cout << "************************************" << endl;
			cout << endl;
			cout << "Apply a decision on scores according to a thershold and Format test Files from ComputeTest into either NIST04 format, ETF format or MDTM format" << endl;
			cout <<endl<<cc.getParamList()<<endl;
		}
		else {
			Config tmp;
			cmdLine.copyIntoConfig(tmp);
			Config config;
			if (tmp.existsParam("config")) config.load(tmp.getParam("config"));
			cmdLine.copyIntoConfig(config);
			cc.check(config);
			config.setParam("minLLK","-200");
			currentSpeaker=getSpeaker(config);
		}  
	}
	catch (alize::Exception& e) {cout << e.toString() << endl << cc.getParamList() << endl;}
return currentSpeaker;
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


void runComputeNorm(int argc, char *argv[]){

  ConfigChecker cc;
  cc.addStringParam("testNistFile",true, true,"target_seg score file");
  cc.addStringParam("normType",true, true,"tnorm|znorm|ztnorm|tznorm, the normalization method (ztnorm is outputing both tnorm and ztnorm scores, tznorm both tznorm and znorm scores)");
  cc.addStringParam("tnormNistFile",false,true,"imp_seg score file, impostor scores used for tnorm and ztnorm");
  cc.addStringParam("znormNistFile",false,true,"target_imp score file, impostor scores used for znorm and ztnorm");
  cc.addStringParam("ztnormNistFile",false,true,"imp_imp score file, impostor scores used for ztnorm and tznorm");
  cc.addStringParam("outputFileBaseName",true, true,"the output file(s) basename");
  cc.addStringParam("znormFilesExtension",false, true,"znorm output file extension (default='.znorm'");
  cc.addStringParam("ztnormFilesExtension",false, true,"ztnorm output file extension (default='.znorm'");
  cc.addStringParam("tnormFilesExtension",false, true,"tnorm output file extension (default='.tnorm'");
  cc.addStringParam("tznormFilesExtension",false, true,"tznorm output file extension (default='.tznorm'");
  cc.addStringParam("cohortFilePath",false, true,"cohort files path, for selectTargetDependentCohortInFile");
  cc.addStringParam("cohortFileExt",false, true,"cohort files extension, for selectTargetDependentCohortInFile");
  cc.addIntegerParam("maxIdNb",false, true,"Max target speakers - use to fix the max number of znorm score distributions (default=1000)");
  cc.addIntegerParam("maxSegNb",false, true,"Max test segments - use to fix the max number of tnorm score distributions (default=1000)");
  cc.addIntegerParam("maxScoreDistribNb",false, true,"Max scores per distribution - use to fix the max number of score in a distribution (default=1000)");
  cc.addIntegerParam("fieldGender",false, true,"The field for gender in the nist file format (default=0)");
  cc.addIntegerParam("fieldName",false, true,"The field for gender in the nist file format (default=1)");
  cc.addIntegerParam("fieldDecision",false, true,"The field for gender in the nist file format (default=2)");
  cc.addIntegerParam("fieldSeg",false, true,"The field for gender in the nist file format (default=3)");
  cc.addIntegerParam("fieldLLR",false, true,"The field for gender in the nist file format (default=4)");
  cc.addStringParam("impostorIDList",false, true,"If the option is set, it limits the used impostor scores to the one of the given list");
  cc.addIntegerParam("meanMode",false,true,"Score distrib mean computation mode. 0 classical, 1 median (default 0)"); 
  cc.addFloatParam("percentH",false,true,"% of higest scores discarded (default 0)");  
  cc.addFloatParam("percentL",false,true,"% of lowest scores discarded (default 0)");  
  
  try {
      CmdLine cmdLine(argc, argv);
      if (cmdLine.displayHelpRequired()){
	cout << "************************************" << endl;
	cout << "********** ComputeNorm.exe *************" << endl;
	cout << "************************************" << endl;
	cout << endl;
	cout << "Apply Z-T-ZT or ZT Norm to a Score File" << endl<< "ztnorm includes tnorm"<<endl;
        cout <<endl<<cc.getParamList()<<endl;
        
      }
      if (cmdLine.displayVersionRequired()){
        cout <<"Version 3"<<endl;
      } 
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
      // Initialize the default values for the parameters TODO : add this option in the addParam functions...
 	  if(!config.existsParam("znormFilesExtension")) config.setParam("znormFilesExtension",".znorm");
 	  if(!config.existsParam("tnormFilesExtension")) config.setParam("tnormFilesExtension",".tnorm");
	  if(!config.existsParam("ztnormFilesExtension")) config.setParam("ztnormFilesExtension",".ztnorm");
	  if(!config.existsParam("tznormFilesExtension")) config.setParam("tznormFilesExtension",".tznorm");
	  if(!config.existsParam("maxIdNb")) config.setParam("maxIdNb","1000");
	  if(!config.existsParam("maxSegNb")) config.setParam("maxSegNb","1000");
	  if(!config.existsParam("maxScoreDistribNb")) config.setParam("maxScoreDistribNb","1000");
	  if(!config.existsParam("fieldGender")) config.setParam("fieldGender","0");
	  if(!config.existsParam("fieldName")) config.setParam("fieldName","1");
	  if(!config.existsParam("fieldDecision")) config.setParam("fieldDecision","2");	  	  	  	  	  	  	   	  
	  if(!config.existsParam("fieldSeg")) config.setParam("fieldSeg","3");	  	  	  	  	  	  	   	  
	  if(!config.existsParam("fieldLLR")) config.setParam("fieldLLR","4");	  
	  if(!config.existsParam("meanMode")) config.setParam("meanMode","0");
	  if(!config.existsParam("percentH")) config.setParam("percentH","0.0");
	  if(!config.existsParam("percentL")) config.setParam("percentL","0.0");
	  
	  
	  	  	  	  	  	  	   	  

      // start the prog!
      ComputeNorm(config);
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
