Step 1: Go into the directory speak_ident inside your downloaded package.

step 2: Put your training data in traindata directory, Background data in background directory and impostor data in impostor directory

step 3: List the impostor model name in imp.train and indicate which audio files they are using in the imp.ndx file

	example imp.train 
		imposter1 imposter2 imposter 3

	example imp.ndx
		imposter1 imposter1.wav
		imposter2 imposter2.wav
		imposter3 imposter3.wav

step 4: Open command prompt and browse to the directory and run 

			$ bash train.sh

step 5: After the training is done , run the node ear using the command :
	$ rosrun speak_ident ear.py /path/to/speak_ident 

step 6: run the other node by :
	$ rosrun speak_ident speak_ident_node /path/to/speak_ident <threshold> <tnorm/znorm>

last two parameters are optional default values of this two parameters are 2 and tnorm. In order to use 2 threshold and Z-Norm the following command will be good:

	$ rosrun speak_ident speak_ident_node /path/to/speak_ident 2 znorm

step 7: open another command prompt type rostopic echo /speaker

step 8: Testing platform does not need these directories : traindata,background and impostor 
