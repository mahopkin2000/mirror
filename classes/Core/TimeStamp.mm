namespace Core;

class TimeStamp : Object {
instance:
    attribute secs,msecs,tmStructure;
    method getYear() {
        return tmStructure.at(0);
    }
    method getMonth() {
        return tmStructure.at(1);
    }
    method getDay() {
        return tmStructure.at(2);
    }
    method getHour() {
        return tmStructure.at(3);
    }
    method getMinutes() {
        return tmStructure.at(4);
    }
    method getSeconds() {
        return tmStructure.at(5);
    }
    method getDOW() {
        return tmStructure.at(6);
    }
    method getDOY() {
        return tmStructure.at(7);
    }
    method isDST() {
        return tmStructure.at(8);
    }
    method privSetTime(aTime) {
        secs := aTime.at(0);
        msecs := aTime.at(1);
        tmStructure := self.getClass().getTimeStructure(secs);
    }
    method printString() {
        return self.getYear().printString() + "-" +
                self.getMonth() + "-" +
                self.getDay() + "T" +
                self.getHour() + ":" +
                self.getMinutes() + ":" +
                self.getSeconds();
    }
class:
    primitive method getTimeOfDay() {
        self.primitiveFailed();
    }
    primitive method getTimeStructure(seconds) {
        self.primitiveFailed();
    }
    method now() {
        var t := super.new();
        t.privSetTime(self.getTimeOfDay());
        return t;
    }
    method new() {
        return self.now();
    }


}
