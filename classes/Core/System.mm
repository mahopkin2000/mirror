namespace Core;

import Collection::Dictionary;
import Collection::Vector;

class System : Object {
class:
        attribute systemDictionary;
		  attribute releaseMutex;
		  attribute objectCollection;
		  attribute releaseProcess;
        method getSystemDictionary() {
                return systemDictionary;
        }
        method startSystem(aClass,aMethod,aArgArray) {
                self.initializeClasses(systemDictionary);
					 releaseMutex := Semaphore.forMutualExclusion();
					 objectCollection := Vector.new();
					 self.startReleaseProcess();
                return aClass.perform(aMethod,[aArgArray]);
        }
        method initializeClasses(aDictionary) {
                var keys := aDictionary.keys();
                var i;
                var v;
                var key;
                for (i:=0;i<keys.size();i:=i+1) {
                        key := keys.at(i);
                        v := aDictionary.at(key);
                        if (v.isInstanceOf(Dictionary)) {
                                self.initializeClasses(v);
                        } else {
                                v.initialize();
										  if (v.respondsTo("release")) {
												v.setFlags(v.getFlags() | 1);
											}
                        }
                }
        }
		  method addReleaseCandidate(object) {
//			OS.stdout().nextPutAll("adding release candidate\n");
 		  	releaseMutex.wait();
		   objectCollection.add(object);	
			releaseMutex.signal();
		  }
		  method startReleaseProcess() {
			releaseProcess:= {
				var delay := Delay.forSeconds(2);
				while (true) {
 		  			releaseMutex.wait();
					for (var obj,var i:= 0;i<objectCollection.size();i:=i+1) {
						obj := objectCollection.at(i);
						obj.perform("release",[]);
						objectCollection.put(i,nil);
						//var wr := WeakReference.on(obj);
						//obj := nil;
						//self.nilAndFree(wr);
					}
					objectCollection.clear();
					releaseMutex.signal();
					delay.wait();
				}
			}.fork([]);
		  }
		primitive method nilAndFree(object) {
			self.primitiveFailed();
		}
                
        
}
