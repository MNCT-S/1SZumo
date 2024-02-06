import subprocess
import time

from watchdog.events import PatternMatchingEventHandler
from watchdog.observers import Observer

class InputDeviceDetector(PatternMatchingEventHandler):
    def __init__(self, patterns):
        super().__init__(patterns=patterns)
    
    def on_created(self, event):
        print('detected to connect an input device. [%s]' % event.src_path)
        cmd = ['python', '/home/pi/python/BLEjoy.py']
        self.proc = subprocess.Popen(cmd)
    
    def on_deleted(self, event):
        print('detected to disconnect an input device. [%s]' % event.src_path)
        self.proc.kill()

def main():
    input_dev_handler = InputDeviceDetector(['/dev/input/js*'])
    observer = Observer()
    observer.schedule(input_dev_handler, '/dev/input')
    observer.start()

    try:
        while True:
            time.sleep(10)
    except KeyboardInterrupt:
        pass
    
    observer.stop()
    observer.join()

if __name__ == '__main__':
    main()
