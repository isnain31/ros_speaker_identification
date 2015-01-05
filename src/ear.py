#!/usr/bin/env python
# license removed for brevity
import rospy
from std_msgs.msg import String
import pyaudio
import wave
import time
import sys

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
    while not rospy.is_shutdown():
    #while(1):
	ts=int(time.time())
	file_name=str(ts)+".wav"

	p = pyaudio.PyAudio()

	stream = p.open(format=FORMAT,
		        channels=CHANNELS,
		        rate=RATE,
		        input=True,
		        frames_per_buffer=CHUNK) #buffer
	print("* recording")
	frames = []

	for i in range(0, int(RATE / CHUNK * RECORD_SECONDS)):
	    data = stream.read(CHUNK)
	    frames.append(data) # 2 bytes(16 bits) per channel

	print("* done recording")

	stream.stop_stream()
	stream.close()
	p.terminate()

        wf = wave.open(WAVE_OUTPUT_FILENAME+file_name, 'wb')
	wf.setnchannels(CHANNELS)
	wf.setsampwidth(p.get_sample_size(FORMAT))
	wf.setframerate(RATE)
	wf.writeframes(b''.join(frames))
	wf.close()

	#rospy.loginfo(hello_str)
        pub.publish(file_name)
        rate.sleep()

if __name__ == '__main__':
    try:
        ear(sys.argv)
    except rospy.ROSInterruptException:
        pass
