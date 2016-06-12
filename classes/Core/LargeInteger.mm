namespace Core;

byte class LargeInteger : LargeNumber {
instance:
        primitive method printString() {
                self.primitiveFailed();
        }
        method @ aInteger {
                return Point.fromCoordinates(self,aInteger);
        }
        method asInteger() {
                return self;
        }
class:
}
