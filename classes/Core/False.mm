namespace Core;

class False : Boolean {
        method || aBoolean {
                return aBoolean;
        }

        method or(aBlock) {
            return aBlock.value([]);
        }
        
        method && aBoolean {
                return self;
        }

        method and(aBlock) {
                return self;
        }
        
        method not() {
                return true;
        }    
        method printString() {
                return "false";
        }          
}
