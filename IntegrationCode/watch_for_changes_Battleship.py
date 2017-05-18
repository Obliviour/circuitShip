#!/usr/bin/python
# watch_for_changes_wav_runme.py  4/22/17 renamed as watch_for_changes_wav_runmeDbl.py because it repeats itself 2x
# watch_for_changes_wav_runme.py 5/8/17 eliminate the double-speak in the original version by moving it to "on-created" instead of on-modified (appeard to get modified 2x)
#	requires deleting the recorded file "recorded_test_UNKNOWN_sound_recorded.wav" each time so it can be created anew (see ResponsiveRecordingLoop.py & ResponsiveRecording.py)

import sys, os.path, time, logging, subprocess, os
from watchdog.observers import Observer
from watchdog.events import PatternMatchingEventHandler
from pathlib import Path

# Execution:  python watch_for_changes_wav_runme.py 

# modified version of callBashRUNME_recorded.py
# http://stackoverflow.com/questions/32313989/check-specific-file-has-been-modified-using-python-watchdog

#  can't define path here (related to white space in path) so define path in line 35
#  some parts of path are hard-coded, e.g. subdirectories of 02_KeywordSearch in corpus

print("watched_dir")
watched_dir=sys.argv[4]
print("%s" % watched_dir)
#print("line 12")



class MyEventHandler(PatternMatchingEventHandler):
    #def on_moved(self, event):
    #    super(MyEventHandler, self).on_moved(event)
    #    logging.info("File %s was just moved" % event.src_path)

    def on_created(self, event):
        super(MyEventHandler, self).on_created(event)
        #logging.info("File %s was just created" % event.src_path)
        #print ("on_created")
        #print( event.src_path)
        path02K = "C:/Users/kt57/Documents/BenYang/NaOMI/HTK/demoAutobots6Record/GT_KWS_Train_Test_Packages/02_KeywordSearch/" 
        os.chdir( path02K )
        #print("os.chdir")
        #print("%s" % path02K)
        errorpath=os.path.join(path02K,'error.mlf')
        #if os.path.exists("C:/Users/kt57/Documents/Ben Yang/NaOMI/HTK/demoAutobots3/GT_KWS_Train_Test_Packages/02_KeywordSearch/error.mlf"):    # works
        if os.path.exists(errorpath): 
            #print ("error.mlf exists ... removed it\n")
            #os.remove("C:/Users/kt57/Documents/Ben Yang/NaOMI/HTK/demoAutobots3/GT_KWS_Train_Test_Packages/02_KeywordSearch/error.mlf")
            os.remove(errorpath)
        #if os.path.exists("C:/Users/kt57/Documents/Ben Yang/NaOMI/HTK/demoAutobots3/GT_KWS_Train_Test_Packages/02_KeywordSearch/error.mlf_saved"):  # works
        errorpathS=os.path.join( path02K,'error.mlf_saved' )
        if os.path.exists(errorpathS): 
            #os.remove("C:/Users/kt57/Documents/Ben Yang/NaOMI/HTK/demoAutobots3/GT_KWS_Train_Test_Packages/02_KeywordSearch/error.mlf_saved")
            os.remove(errorpathS)
        #error_mlfsaved = Path("error.mlf_saved")
        #if error_mlfsaved.is_file():
            #os.remove("error.mlf_saved")
        path_done=os.path.join(path02K,'.done_step_01')
        #if os.path.exists("C:/Users/kt57/Documents/Ben Yang/NaOMI/HTK/demoAutobots3/GT_KWS_Train_Test_Packages/02_KeywordSearch/.done_step_01"):   #works
        if os.path.exists(path_done): 
            os.remove(path_done)
            #os.remove("C:/Users/kt57/Documents/Ben Yang/NaOMI/HTK/demoAutobots3/GT_KWS_Train_Test_Packages/02_KeywordSearch/.done_step_01")  #works
        #print ("RUNME_recorded.sh called")
        #ErrFile = open("error.mlf", 'r')
        #line = ErrFile.readlines()[6]
        #print (line)
        #ErrFile.close()
        #print (event.src_path)
        pathcorpus=event.src_path.split("02_KeywordSearch",1)[1]
        #print (pathcorpus[1:])   #ok
        pathlist=os.path.join(path02K,'list/testwave_recorded.lst')
        with open(pathlist, "w") as text_file:
            print (pathcorpus[1:], file=text_file )
        #print ("on_created call RUNME_recorded.sh") 	
        subprocess.call(['bash.exe', '.\RUNME_recorded.sh'])

    #def on_deleted(self, event):
    #    super(MyEventHandler, self).on_deleted(event)
    #    logging.info("File %s was just deleted" % event.src_path)

    #def on_modified(self, event):
        #super(MyEventHandler, self).on_modified(event)
        #turned off logging info ...not printing is faster
        #logging.info("File %s was just modified" % event.src_path)
        #print ("on_modified")

def main(file_path=None):
    logging.basicConfig(level=logging.INFO,
        format='%(asctime)s - %(message)s',
        datefmt='%Y-%m-%d %H:%M:%S')
    #watched_dir = os.path.split(file_path)[0]
    #print ("watched_dir")
    #print ("%s" % watched_dir)
    #print ("path")
    #print ("%s" % path)
    #print ("os.path"
    #print (os.path)
    #patterns = [file_path]
    patterns = ["*.wav"]
    print ("%s" % patterns)
    print ("open command prompt I:> r to set-up recording")
    event_handler = MyEventHandler(patterns=patterns)
    observer = Observer()
    observer.schedule(event_handler, watched_dir, recursive=False)   # True works but generates a trigger for any folder mods in subdirectories
    observer.start()
    print ("started")
    try:
        while True:
            time.sleep(1)
    except KeyboardInterrupt:
        observer.stop()
    observer.join()


if __name__ == "__main__":
    if len(sys.argv) > 1:
        path = sys.argv[1]
        main(file_path=path.strip())
    else:
        sys.exit(1)