namespace Core;

import Collection::LinkedList;

class Delay : Object {
instance:
    attribute milliseconds;
    method setMilliseconds(aMillisecondCount) {
        milliseconds := aMillisecondCount;
    }
    method wait() {
        var tod := TimeStamp.getTimeOfDay();
        var sec := tod.at(0);
        var ms := tod.at(1);
        sec := sec + ((ms+milliseconds)/1000);
        ms := (ms+milliseconds)%1000;
        var d := [ [sec,ms],milliseconds,
                    Semaphore.new()];
        self.getClass().enqueueDelay(d);
        d.at(2).wait();
    }
class:
    attribute delayProcess;
    attribute delayList;
    attribute delayListMutex;
    method initialize() {
        delayListMutex := Semaphore.forMutualExclusion();
        delayList := LinkedList.new();
        delayProcess := {
                while (true) {
                    self.checkDelays();
                }
        }.fork([]);
    }
    method forMilliseconds(aMillisecondCount) {
        var d := self.new();
        d.setMilliseconds(aMillisecondCount);
        return d;
    }
    method forSeconds(aSecondCount) {
        return self.forMilliseconds(aSecondCount * 1000);
    }
    method enqueueDelay(aDelay) {
        delayListMutex.critical({delayList.add(aDelay);});
    }
    method checkDelays() {
        var cms := TimeStamp.getTimeOfDay();
        var sec := cms.at(0);
        var ms := cms.at(1);
        var bl := {
           for (var i:=0;i<delayList.size();i:=i+1) {
              var e:=delayList.at(i);
              if (    (sec >= e.at(0).at(0)) &&
                      (ms >= e.at(0).at(1))   ) {
                 e.at(2).signal();
              }
           }
        };
        delayListMutex.critical(bl);
    }
}
