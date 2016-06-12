namespace Core;

class Process : Object {
instance:
    attribute context,state,next,previous,exceptionContext;
    method getContext() {
        return context;
    }
    method setContext(aContext) {
        context := aContext;
    }
    method getState() {
        return state;
    }
    method getNext() {
        return next;
    }
    method getPrevious() {
        return previous;
    }
    method getExceptionContext() {
        return exceptionContext;
    }
    primitive method start() {
        self.primitiveFailed();
    }
    primitive method suspend() {
        self.primitiveFailed();
    }
    primitive method resume() {
        self.primitiveFailed();
    }
    primitive method terminate() {
        self.primitiveFailed();
    }
}
