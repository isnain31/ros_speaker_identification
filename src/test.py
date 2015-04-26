#!/usr/bin/env python
'''
This file is part of speak_ident which is a ROS package for speaker identification
Spro 4.0, ALIZE toolkit, LIA_RAL, pyAudio are required to use this package.

This package uses partially modfied code of LIA_RAL's EnergyDetector.cpp
ComputeNorm.cpp TrainWorld.cpp NormFeat.cpp Scoring.cpp TrainWorld.cpp.
And uses the rest as it is.

speak_ident is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation, either version 3 of
the License, or any later version.

speak_ident is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU Lesser General Public License for more details.

This particular file was coded by taking a lot of useful guideline from pyAudio 
documentation and stackoverflow community

Copyright (C) 2014
Isnain Siddique [isnain.siddique@smail.inf.h-brs.de]
'''



import rospy
from std_msgs.msg import String
import pyaudio
import wave
import time
import sys
import glob
import os

CHUNK = 1024 
FORMAT = pyaudio.paInt16 #paInt8
CHANNELS = 2 
RATE = 44100 #sample rate
RECORD_SECONDS = 5




def ear(argv):
    if(len(argv)<2):
	print "base file location is not given. System will exit now"	
	sys.exit()
    pub = rospy.Publisher('audio_file', String, queue_size=10)
    rospy.init_node('ear', anonymous=True)
    rate = rospy.Rate(1) # 10hz
    WAVE_OUTPUT_FILENAME = argv[1]+"/capture/"
    print WAVE_OUTPUT_FILENAME
    allFiles=glob.glob(  WAVE_OUTPUT_FILENAME+"*.wav")	
    while not rospy.is_shutdown():
    #while(1):
	for files in allFiles:		
		pub.publish(os.path.basename(files))
  		rate.sleep()

if __name__ == '__main__':
    try:
        ear(sys.argv)
    except rospy.ROSInterruptException:
        pass
