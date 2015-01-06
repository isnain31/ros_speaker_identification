#!/bin/bash

#DATADIR="/home/is9/alize/data/PCC/PCCdata16kHz/train/reverberated"
DATADIR="traindata"
FEATUREDIR="feature"
BGDATADIR="background"
IMPOSTORDIR="impostor"

DATADIRS=`ls -l $DATADIR | egrep '^d' | awk '{print $9}' ` 


rm -rf ${FEATUREDIR}/*
rm -rf ndx/*
rm -rf lst/*
rm _ndx.train
rm impz.ndx

echo "background training"

RAWFILES=` ls -l $BGDATADIR | awk '{print $9}' `

for WAVFILE in $RAWFILES
	do
		sfbcep -F WAVE -p 24 $BGDATADIR/${WAVFILE} ${FEATUREDIR}/${WAVFILE}.mfcc
		EnergyDetector --config cfg/EnergyDetector.cfg --inputFeatureFilename  ${WAVFILE} 
		NormFeat --config cfg/NormFeat.cfg --inputFeatureFilename  ${WAVFILE} 
		echo "feature/"${WAVFILE} >> lst/p.lst
	done

TrainWorld --config cfg/TrainWorld.cfg


echo "extracting features, detecting voiced and unvoiced frame and normalizing them to cancel background noise this may take some time"

for DIR in $DATADIRS
do

	#mkdir ${FEATUREDIR}/${DIR}
	RAWFILES=` ls -l $DATADIR/$DIR | awk '{print $9}' `

	var=$DIR	
        	
	echo "data for "$DIR
	for WAVFILE in $RAWFILES
	do
		sfbcep -F WAVE -p 24 $DATADIR/$DIR/${WAVFILE} ${FEATUREDIR}/${DIR}_${WAVFILE}.mfcc
		EnergyDetector --config cfg/EnergyDetector.cfg --inputFeatureFilename  ${DIR}_${WAVFILE} 
		NormFeat --config cfg/NormFeat.cfg --inputFeatureFilename  ${DIR}_${WAVFILE} 
		var=$var" "${DIR}_${WAVFILE}"  "
                
	done
	speaker=$DIR" "
	speakerlist=$speakerlist$speaker
	echo $var >> ndx/ndx
done

echo $speakerlist >> _ndx.train
echo "proessing of data done training begins"

#cp feature/id1_* tt/id1
#ls feature/*.wav > p.txt
#TrainWorld --config cfg/TrainWorld.cfg
TrainTarget --config cfg/TrainTarget.cfg


echo "impostor training"


ndxtrainz=$(<_ndx.train)
RAWFILES=` ls -l $IMPOSTORDIR | awk '{print $9}' `

for WAVFILE in $RAWFILES
	do
		rm ${FEATUREDIR}/${WAVFILE}.mfcc
		rm ${FEATUREDIR}/${WAVFILE}.lbl
		rm ${FEATUREDIR}/${WAVFILE}.norm.prm
 	        sfbcep -F WAVE -p 24 $IMPOSTORDIR/${WAVFILE} ${FEATUREDIR}/${WAVFILE}.mfcc
		EnergyDetector --config cfg/EnergyDetector.cfg --inputFeatureFilename  ${WAVFILE} 
		NormFeat --config cfg/NormFeat.cfg --inputFeatureFilename  ${WAVFILE} 
		echo ${WAVFILE}" "${ndxtrainz}>> impz.ndx
	done



TrainTarget --config cfg/TrainImposter.cfg

ComputeTest --config cfg/ComputeImpzTest.cfg

