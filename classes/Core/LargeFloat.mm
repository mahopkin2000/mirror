namespace Core;

byte class LargeFloat : LargeNumber {
instance:
        primitive method printString() {
                self.primitiveFailed();
        }
        method asFloat() {
                return self;
        }
}
