#include "interp.h"
#include "memory.h"
#include "stdio.h"
#include "primitive.h"
#include "dict.h"
#include "compiler.h"

/* utility function prototypes */
static int dispatch(mmObjectPtr gate_context);
static mmObjectPtr find_method(mmObjectPtr cl,mmObjectPtr selector,mmObjectPtr* found_class);
static int cycle();
static mmObjectPtr lookup_method(mmObjectPtr rcvr,mmObjectPtr selector,int sendToSuper);
static void check_process_switch();
static void free_locals(mmObjectPtr context);
static mmObjectPtr overload_selector(mmObjectPtr selector,int numargs);
static int interp_bootstrapped=0;
static mmObjectPtr release_selector=0;
static mmObjectPtr release_args=0;

void mm_init_interpreter() {
    int i,j;
    mmObjectPtr main_process=mm_nil;
    mmObjectPtr class_vars=mm_nil;
    main_process = mm_new_object(mm_process_class,MM_PROCESS_SIZE);
    mm_store_slot(main_process,MM_PROCESS_STATE,mm_new_integer(MM_PROCESS_RUNNING));
    mm_store_slot(main_process,MM_PROCESS_CONTEXT,mm_nil);
    mm_store_slot(main_process,MM_PROCESS_PREVIOUS,main_process);
    mm_store_slot(main_process,MM_PROCESS_NEXT,main_process);
    mm_store_slot(mm_processor_class,MM_PROCESSOR_PROCESS_LIST_HEAD_CV,
            main_process);
    mm_store_slot(mm_processor_class,MM_PROCESSOR_PROCESS_LIST_END_CV,
            main_process);
    mm_store_slot(mm_processor_class,MM_PROCESSOR_ACTIVE_PROCESS_CV,
            main_process);
	 interp_bootstrapped=1;
	 release_selector=mm_new_string("release");
	 release_args=mm_new_array(1);
}

mmObjectPtr mm_new_block(mmObjectPtr parent) {
    mmObjectPtr block = 
        mm_new_object(mm_block_class,MM_BLOCK_SIZE);
    mm_store_slot(block,MM_BLOCK_PARENT,parent);
    mm_store_slot(block,MM_BLOCK_START_SCOPE,mm_new_integer(0));
    return block;
}


mmObjectPtr mm_new_block_context(mmObjectPtr block,mmObjectPtr parent,mmObjectPtr rcvr) {
    mmObjectPtr scopeTemplArray=mm_nil;
    mmObjectPtr scopeTemplArrayEntry=mm_nil;
    mmObjectPtr scopeArray=mm_nil;
    int idx,scopeArraySize;
    mmObjectPtr context =
        mm_new_object(mm_block_context_class,MM_BLOCK_CONTEXT_SIZE);
    mm_store_slot(context,MM_BLOCK_CONTEXT_BLOCK,block);
    mm_store_slot(context,MM_BLOCK_CONTEXT_PARENT,parent);
    mm_store_slot(context,MM_BLOCK_CONTEXT_RCVR,rcvr);
    mm_store_slot(context,MM_BLOCK_CONTEXT_IP,mm_new_integer(0));
    mm_store_slot(context,MM_BLOCK_CONTEXT_SP,mm_new_integer(-1));
    mm_store_slot(context,MM_BLOCK_CONTEXT_STACK,mm_new_array(MM_MAX_STACK_SIZE));
    scopeTemplArray=mm_get_slot(block,MM_BLOCK_NESTED_SCOPES_TEMPLATE);
    scopeArraySize=scopeTemplArray->_size;
    scopeArray=mm_new_array(scopeArraySize);
    for (idx=0;idx<scopeArraySize;idx++) {
        scopeTemplArrayEntry=mm_get_slot(scopeTemplArray,idx);
        mm_store_slot(scopeArray,idx,mm_new_array(scopeTemplArrayEntry->_size-1));
    }
    mm_store_slot(context,MM_BLOCK_CONTEXT_NESTED_SCOPES,scopeArray);
    mm_store_slot(context,MM_BLOCK_CONTEXT_SCOPE_LEVEL,mm_new_integer(0));
    return context;
}

static void free_locals(mmObjectPtr context) {
    mmObjectPtr nestedScopes=mm_get_slot(context,MM_BLOCK_CONTEXT_NESTED_SCOPES);
    mmObjectPtr scopeArray=mm_nil;
    int i,j;
    for (i=0;i<nestedScopes->_size;i++) {
        scopeArray=mm_get_slot(nestedScopes,i);
        for (j=0;j<scopeArray->_size;j++) {
            mm_store_slot(scopeArray,j,mm_nil);
        }
    }
}

mmObjectPtr lookup_method(mmObjectPtr rcvr,mmObjectPtr selector,int sendToSuper) {
    mmObjectPtr method=mm_nil;
    mmObjectPtr startClass=mm_nil;
  
    if (sendToSuper) {
			find_method(rcvr->_class,selector,&startClass);
			if (startClass!=mm_nil) {
        		startClass=mm_get_slot(startClass,MM_BEHAVIOR_SUPERCLASS);
				debug("Sending %s to super: start class is %s\n",mm_get_string(selector),mm_get_class_name(startClass));
			} else {
					  return mm_nil;
			}
    } else {
        startClass=rcvr->_class;
    }

    method = find_method(startClass,selector,0);   
    return method;
}

static mmObjectPtr find_method(mmObjectPtr cl,mmObjectPtr selector,mmObjectPtr* found_class) {
    mmObjectPtr md=mm_nil;
    mmObjectPtr method=mm_nil;
    char* cn;

    if (cl == mm_nil) {
        return mm_nil;
    } else {
        md = mm_get_slot(cl,MM_BEHAVIOR_METHOD_DICT);
        method = mm_dict_at(md,mm_get_string(selector));
        if (method == mm_nil) {
            return find_method(mm_get_slot(cl,MM_BEHAVIOR_SUPERCLASS),selector,found_class);
        } else {
				if (found_class) {
						  *found_class=cl;
				}
				debug("resolved method %s to class %s\n",mm_get_string(selector),mm_get_class_name(cl));
            return method;
        }
    }
}

mmObjectPtr pop(mmObjectPtr context) {
    register mmObjectPtr rv = mm_nil;
    register int sp = mm_get_integer(mm_get_slot(context,MM_BLOCK_CONTEXT_SP));
    if (sp >= 0) {
        rv = mm_get_slot(mm_get_slot(context,MM_BLOCK_CONTEXT_STACK),sp);
        mm_store_slot(mm_get_slot(context,MM_BLOCK_CONTEXT_STACK),sp,mm_nil);
        mm_store_slot(context,MM_BLOCK_CONTEXT_SP,mm_new_integer(--sp));
    } else {
        error("Stack underflow error\n");
        exit(-1);
    }
    return rv;
}

mmObjectPtr push(mmObjectPtr context,mmObjectPtr o) {
    register int sp = mm_get_integer(mm_get_slot(context,MM_BLOCK_CONTEXT_SP));
    if (sp < MM_MAX_STACK_SIZE) {
        mm_store_slot(mm_get_slot(context,MM_BLOCK_CONTEXT_STACK),++sp,o);
        mm_store_slot(context,MM_BLOCK_CONTEXT_SP,mm_new_integer(sp));
    } else {
        error("Stack overflow error\n");
        exit(-1);
    }
}

mmObjectPtr top(mmObjectPtr context) {
    register mmObjectPtr rv = mm_nil;
    register int sp = mm_get_integer(mm_get_slot(context,MM_BLOCK_CONTEXT_SP));
    if (sp >= 0) {
        rv = mm_get_slot(mm_get_slot(context,MM_BLOCK_CONTEXT_STACK),sp);
    } else {
        error("Stack underflow error\n");
        exit(-1);
    }
    return rv;
}

int isEmpty(mmObjectPtr context) {
    return (mm_get_integer(mm_get_slot(context,MM_BLOCK_CONTEXT_SP)) < 0);
}

mmObjectPtr mm_new_method(mmObjectPtr selector,mmObjectPtr topBlock) {
    mmObjectPtr method = mm_new_object(mm_method_class,MM_METHOD_SIZE);
    mm_store_slot(method,MM_METHOD_NAME,selector);
    mm_store_slot(method,MM_METHOD_BLOCK,topBlock);
    return method;
}

void mm_copy_scope(mmObjectPtr src,mmObjectPtr dest) {
    mmObjectPtr srcosa=mm_nil;
    mmObjectPtr srcisa=mm_nil;
    mmObjectPtr destosa=mm_nil;
    mmObjectPtr destisa=mm_nil;
    int idx,idx2;
    int destosaSize,destisaSize;
    int numSharedScopes;
    mmObjectPtr block=mm_nil;

    srcosa=mm_get_slot(src,MM_BLOCK_CONTEXT_NESTED_SCOPES);
    destosa=mm_get_slot(dest,MM_BLOCK_CONTEXT_NESTED_SCOPES);
    destosaSize=destosa->_size;
    block=mm_get_slot(dest,MM_BLOCK_CONTEXT_BLOCK);
    numSharedScopes=mm_get_integer(mm_get_slot(block,MM_BLOCK_START_SCOPE));
    for (idx=0;idx<numSharedScopes;idx++) {
        srcisa=mm_get_slot(srcosa,idx);
        mm_store_slot(destosa,idx,srcisa);
    }
}

mmObjectPtr overload_selector(mmObjectPtr selector,int numargs) {
		  mmObjectPtr new_selector=selector;
		  if (numargs > 0) {
					 char mname[128];
					 sprintf(mname,"%s_%d",mm_get_string(selector),numargs);
					 new_selector=mm_new_string(mname);
		  }
		  return new_selector;
}

void mm_interpret() {
    int done=0;
    while (!done) {
        done=cycle();
    }
}

void mm_interpret_until(mmObjectPtr gate_context) {
	int done=0;
	while (!done) {
   	check_process_switch();
		done=dispatch(gate_context);
	}
}
	

int cycle() {
    check_process_switch();
    return dispatch(mm_nil);
}

/* here's the main routine which performs the work of the interpreter */
int dispatch(mmObjectPtr gate_context)
{
    mmObjectPtr context=mm_get_slot(getActiveProcess(),MM_PROCESS_CONTEXT);
    mmObjectPtr block = mm_get_slot(context,MM_BLOCK_CONTEXT_BLOCK);
    int ic = mm_get_integer(mm_get_slot(context,MM_BLOCK_CONTEXT_IP));
    mmByteArray* bytearray = (mmByteArray*)mm_get_slot(block,MM_BLOCK_BYTECODE);
    int numBytes = bytearray->_numbytes;
    mmByte* ba = bytearray->_bytes;
    mmObjectPtr suArray=mm_get_slot(block,MM_BLOCK_SOURCE_ARRAY);
    int index;
    mmByte extra;
    int numargs;
    int litindex;
    mmObjectPtr selector=mm_nil;
    mmObjectPtr method=mm_nil;
    int idx;
    mmObjectPtr newrcvr=mm_nil;
    mmObjectPtr clName=mm_nil,cl=mm_nil;
    mmObjectPtr argarray=mm_nil;
    mmObjectPtr primreturn=mm_nil;
    mmObjectPtr boolVal=mm_nil;
    int jumpMult,jumpBytes;
    int scopeIdx,localIdx;
    mmObjectPtr localvars=mm_nil;
    mmObjectPtr parent=mm_nil;
    int loc,reqNumArgs;
    mmObjectPtr closureBlock,closureContext;
    mmObjectPtr exception=mm_nil;
    char buff[128];
    mmByte bc=ba[ic];
    mmObjectPtr rcvr = mm_get_slot(context,MM_BLOCK_CONTEXT_RCVR);
    int mm_msg_sendRC=-1;
    mmObjectPtr mm_msg_sendRV;
    mmObjectPtr o=mm_nil;
    int scope_level;

    block = mm_get_slot(context,MM_BLOCK_CONTEXT_BLOCK);
    if (    (numBytes > 0) && (ic<numBytes) ) {
    switch (bc) {
    case MM_BC_PUSH_SELF:
        push(context,mm_get_slot(context,MM_BLOCK_CONTEXT_RCVR));
        break;
    case MM_BC_PUSH_TRUE:
        push(context,mm_true);
        break;
    case MM_BC_PUSH_FALSE:
        push(context,mm_false);
        break;
    case MM_BC_PUSH_NIL:
        push(context,mm_nil);
        break;
    case MM_BC_PUSH_THIS_CONTEXT:
        push(context,context);
        break;
    case MM_BC_RETURN:
        parent = mm_get_slot(context,MM_BLOCK_CONTEXT_PARENT);
        if (parent != mm_nil) {
            push(parent,top(context));
            //freeLocals(context);
				mm_inc_ref(context);
            mm_store_slot(getActiveProcess(),MM_PROCESS_CONTEXT,parent);
				mm_dec_ref(context);
        } else {
            mm_store_slot(getActiveProcess(),MM_PROCESS_STATE,
                    mm_new_integer(MM_PROCESS_TERMINATED));
            if (getActiveProcess()==getProcessHead()) {
                return 1;
            }
        }
			if (parent==gate_context) {
				return 1;
		   } else {
        		return 0;
			}
        break;
    case MM_BC_MSG_SEND:
        numargs = ba[++(ic)];
        litindex=ba[++(ic)];
        selector=mm_get_slot(mm_get_slot(block,MM_BLOCK_LITERALS),litindex);
        argarray=mm_new_array(numargs);
        for (idx=numargs-1;idx>=0;idx--) {
            mm_store_slot(argarray,idx,top(context));
            pop(context);
        }
        newrcvr = top(context);
        mm_msg_sendRC=mm_msg_send(context,newrcvr,selector,argarray,0,&mm_msg_sendRV);
        pop(context);
        mm_free_object(argarray);
        if (mm_msg_sendRC==MM_MSG_SEND_PRIM_RETURN_OK) {
            push(context,mm_msg_sendRV);
        }
        break;
    case MM_BC_MSG_SEND_SUPER:
        numargs = ba[++(ic)];
        litindex=ba[++(ic)];
        selector=mm_get_slot(mm_get_slot(block,MM_BLOCK_LITERALS),litindex);
        argarray=mm_new_array(numargs);
        for (idx=numargs-1;idx>=0;idx--) {
            mm_store_slot(argarray,idx,top(context));
            pop(context);
        }
        newrcvr = top(context);
        mm_msg_sendRC=mm_msg_send(context,newrcvr,selector,argarray,1,&mm_msg_sendRV);
        pop(context);
        mm_free_object(argarray);
        if (mm_msg_sendRC==MM_MSG_SEND_PRIM_RETURN_OK) {
            push(context,mm_msg_sendRV);
        }
        break;
    case MM_BC_POP:
        pop(context);
        break;
    case MM_BC_INST_VAR_FETCH:
        extra = ba[++(ic)];
        push(context,mm_get_slot(rcvr,extra));
        break;
    case MM_BC_CL_INST_VAR_FETCH:
        extra = ba[++(ic)];
        push(context,mm_get_slot(rcvr->_class,extra));
        break;
    case MM_BC_LITERAL_FETCH:
        extra = ba[++(ic)];
        push(context,mm_get_slot(mm_get_slot(block,MM_BLOCK_LITERALS),extra));
        break;
    case MM_BC_INST_VAR_STORE:
        extra = ba[++(ic)];
        o = top(context);
        mm_store_slot(rcvr,extra,o);
        break;
    case MM_BC_CL_INST_VAR_STORE:
        extra = ba[++(ic)];
        o = top(context);
        mm_store_slot(rcvr->_class,extra,o);
        break;
    case MM_BC_ARRAY_LITERAL:
        extra = ba[++(ic)];
        o=mm_new_array(extra);
        for (idx=extra-1;idx>=0;idx--) {
            mm_store_slot(o,idx,top(context));
            pop(context);
        }
        push(context,o);
        break;
    case MM_BC_BLOCK_CLOSURE:
        litindex=ba[++(ic)];
        closureBlock=mm_get_slot(mm_get_slot(block,MM_BLOCK_LITERALS),litindex);
        closureContext = mm_new_block_context(closureBlock,mm_nil,rcvr);
        mm_copy_scope(context,closureContext);
        push(context,closureContext);
        break;
    case MM_BC_LOCAL_VAR_FETCH:
        scopeIdx = ba[++(ic)];
        localIdx = ba[++(ic)];
        localvars = mm_get_slot(mm_get_slot(context,MM_BLOCK_CONTEXT_NESTED_SCOPES),scopeIdx);
        push(context,mm_get_slot(localvars,localIdx));
        break;
    case MM_BC_LOCAL_VAR_STORE:
        scopeIdx = ba[++(ic)];
        localIdx = ba[++(ic)];
        localvars = mm_get_slot(mm_get_slot(context,MM_BLOCK_CONTEXT_NESTED_SCOPES),scopeIdx);
        mm_store_slot(localvars,localIdx,top(context));
	    break;
    case MM_BC_JUMP_FORWARD:
    case MM_BC_JUMP_BACKWARD:
        jumpMult=ba[++(ic)];
        jumpBytes=ba[++(ic)];
        jumpBytes=((jumpMult*256)+jumpBytes);
        jumpBytes=(bc==MM_BC_JUMP_FORWARD) ? jumpBytes : -jumpBytes;
        (ic)+=jumpBytes;
        break;
    case MM_BC_JUMP_FORWARD_ON_TRUE:
    case MM_BC_JUMP_BACKWARD_ON_TRUE:
        jumpMult=ba[++(ic)];
        jumpBytes=ba[++(ic)];
        boolVal=pop(context);
        if (boolVal==mm_true) {
            jumpBytes=((jumpMult*256)+jumpBytes);
            jumpBytes=(bc==MM_BC_JUMP_FORWARD_ON_TRUE) 
                ? jumpBytes : -jumpBytes;
            (ic)+=jumpBytes;
        }
        break;
    case MM_BC_JUMP_FORWARD_ON_FALSE:
    case MM_BC_JUMP_BACKWARD_ON_FALSE:
        jumpMult=ba[++(ic)];
        jumpBytes=ba[++(ic)];
        boolVal=pop(context);
        if (boolVal==mm_false) {
            jumpBytes=((jumpMult*256)+jumpBytes);
            jumpBytes=(bc==MM_BC_JUMP_FORWARD_ON_FALSE) 
                ? jumpBytes : -jumpBytes;
            (ic)+=jumpBytes;
        }
        break;
    case MM_BC_SET_SCOPE_LEVEL:
        scope_level=ba[++(ic)];
        mm_store_slot(context,MM_BLOCK_CONTEXT_SCOPE_LEVEL,mm_new_integer(scope_level));
        break;
    default:
        error("Unsupported bytecode: %d\n",bc);
    }
    }

    // check for out of bytes condition with no explicit return statement
    ++ic;
    if (    (ic>=numBytes) && 
            (mm_msg_sendRC != MM_MSG_SEND_CONTEXT)  ) {
        o=mm_get_slot(context,MM_BLOCK_CONTEXT_PARENT);
        if (o != mm_nil) {
            if (isEmpty(context)) {
                push(o,mm_get_slot(context,MM_BLOCK_CONTEXT_RCVR));
            } else {
                push(o,top(context));
            }
            //if (mm_get_slot(context,MM_BLOCK_CONTEXT_FLAGS) != mm_nil) {
            //    pop(o);
            //}
            //freeLocals(context);
				mm_inc_ref(context);
            mm_store_slot(getActiveProcess(),MM_PROCESS_CONTEXT,o);
				mm_dec_ref(context);
				if (o==gate_context) {
					return 1;
				}
        } else {
            if (isEmpty(context)) {
                push(context,mm_get_slot(context,MM_BLOCK_CONTEXT_RCVR));
            }
            mm_store_slot(getActiveProcess(),MM_PROCESS_STATE,
                    mm_new_integer(MM_PROCESS_TERMINATED));
            if (getActiveProcess()==getProcessHead()) {
                return 1;
            }
        }
    } else {
        mm_store_slot(context,MM_BLOCK_CONTEXT_IP,mm_new_integer(ic));
    }
    return 0;

}

int mm_msg_send(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr selector,mmObjectPtr args,int sendToSuper,mmObjectPtr* rv) {
    mmByteArray* pmptr;
    mmPrimFunc pmfptr=NULL;
    mmObjectPtr primreturn=0;
    int primrc=0;
    mmObjectPtr newcontext=mm_nil;
    mmObjectPtr localvars=mm_nil;
    mmObjectPtr method=mm_nil;
    int numargs=(args!=mm_nil) ? args->_size : 0;
    int idx;
    int reqNumArgs;
    mmObjectPtr dnaArgs=mm_nil;
    int rc;
    int arg_idx;
	 char* mname=mm_get_string(selector);
	 int free_selector=0;

	 if (	(numargs > 0) && 
		 	(	(isalpha(mname[0]))|| (mname[0]=='_')	)	) {
		selector=overload_selector(selector,numargs);
		free_selector=1;
   }
    debug("sending message %s\n",mm_get_string(selector));
    method = lookup_method(rcvr,selector,sendToSuper);
    if (method==mm_nil) {                   
        dnaArgs=mm_new_array(2);
        mm_store_slot(dnaArgs,0,selector);
        mm_store_slot(dnaArgs,1,mm_copy(args));
        rc=mm_msg_send(context,rcvr,mm_new_string("doesNotUnderstand"),
                dnaArgs,sendToSuper,rv);
        mm_free_object(dnaArgs);
        return rc;
    } else {
        /* verify that we were passed the correct number of arguments */
        reqNumArgs=mm_get_integer(mm_get_slot(method,MM_METHOD_NUMARGS));
        if (reqNumArgs != numargs) {
				char msg[512];
            if (rcvr->_class->_class==mm_metaclass_class) {
                sprintf(msg,"Expected %d arguments for method %s class::%s, got %d\n",
                        reqNumArgs,
                        mm_get_string(mm_get_slot(mm_get_slot(rcvr->_class,MM_METACLASS_THISCLASS),MM_CLASS_NAME)),
                        mm_get_string(selector),numargs);
            } else {
                sprintf(msg,"Expected %d arguments for method %s::%s, got %d\n",
                        reqNumArgs,mm_get_string(mm_get_slot(rcvr->_class,MM_CLASS_NAME)),
                        mm_get_string(selector),numargs);
            }
				mm_raise(mm_exception_class,msg);
        }

        /* now we need to determine if this method will be responded to primitively or
           via the interpreter. This check is accomplished by testing the 
           MM_PRIMITIVE_NUMBER field and if it is not nil, invoking the referenced
           primitive.  Note that a new context is not created when a primitive is invoked,
           as opposed to a standard message send */
        pmptr=(mmByteArray*)mm_get_slot(method,MM_NATIVE_METHOD_ADDRESS);
        if ((mmObjectPtr)pmptr != mm_nil) {
            memcpy(&pmfptr,pmptr->_bytes,sizeof(pmfptr));
            if (!pmfptr) {
					 char msg[512];
                error("primitive function pointer is NULL for method %s\n",mm_get_string(selector));
					 sprintf(msg,"primitive function pointer is NULL for method %s\n",mm_get_string(selector));
					 mm_raise(mm_exception_class,msg);
            }
            primrc=pmfptr(context,rcvr,args,&primreturn);
            if (primrc==MM_PRIM_STD_RETURN) {
                if (primreturn==0) {
                    *rv=rcvr;
                } else {
                    *rv=primreturn;
                }
                return MM_MSG_SEND_PRIM_RETURN_OK;
            } else if (primrc==MM_PRIM_CONTROL) {
                return MM_MSG_SEND_CONTEXT;
            } else {
                debug("primitive failed...falling through to failure handler\n");
                /* fall through to primitive failure handler */
            }
        }

        /* regular message send */
        newcontext = mm_new_block_context(mm_get_slot(method,MM_METHOD_BLOCK),context,rcvr);
        if (numargs) {
            localvars=mm_get_slot(mm_get_slot(newcontext,MM_BLOCK_CONTEXT_NESTED_SCOPES),0);
            for (idx=0;idx < numargs;idx++) {
                mm_store_slot(localvars,idx,mm_get_slot(args,idx));
            }
        }
        mm_store_slot(getActiveProcess(),MM_PROCESS_CONTEXT,newcontext);
		  if (free_selector) {
				mm_free_object(selector);
			}
        return MM_MSG_SEND_CONTEXT;
    }
}

void mm_prepare_block(mmObjectPtr block,mmObjectPtr args) {
    mmObjectPtr localvars=mm_nil;
    int size=args->_size;
    int idx;
    mmObjectPtr bl=mm_get_slot(block,MM_BLOCK_CONTEXT_BLOCK);
    int startScope=mm_get_integer(mm_get_slot(bl,MM_BLOCK_START_SCOPE));
    mmObjectPtr cc=mm_nil;

    if (size) {
        localvars=mm_get_slot(mm_get_slot(block,MM_BLOCK_CONTEXT_NESTED_SCOPES),startScope);
        for (idx=0;idx < size;idx++) {
            mm_store_slot(localvars,idx,mm_get_slot(args,idx));
        }
    }  
}

mmObjectPtr mm_schedule_process(mmObjectPtr context) {
    mmObjectPtr process = mm_new_object(mm_process_class,MM_PROCESS_SIZE);

    mm_store_slot(process,MM_PROCESS_STATE,mm_new_integer(MM_PROCESS_READY));
    mm_store_slot(process,MM_PROCESS_CONTEXT,context);
    mm_store_slot(process,MM_PROCESS_PREVIOUS,getProcessEnd());
    mm_store_slot(process,MM_PROCESS_NEXT,getProcessHead());
    mm_store_slot(getProcessEnd(),MM_PROCESS_NEXT,process);
    mm_store_slot(mm_processor_class,
            MM_PROCESSOR_PROCESS_LIST_END_CV,process);

    return process;
}

void check_process_switch() {
    mmObjectPtr nextProcess=mm_nil;
    mmObjectPtr prevProcess=mm_nil;
    mmObjectPtr process=mm_get_slot(getActiveProcess(),MM_PROCESS_NEXT);
    while (1) {
        // if the next process in the chain is in a running state,
        // switch to it and we're done
        if (mm_get_integer(mm_get_slot(process,MM_PROCESS_STATE))==
                    MM_PROCESS_RUNNING) {
            mm_store_slot(mm_processor_class,
                    MM_PROCESSOR_ACTIVE_PROCESS_CV,process);
            break;
        }
        // if the next process in the chain is terminated, remove
        // it from the linked list
        if (mm_get_integer(mm_get_slot(process,MM_PROCESS_STATE))==
                    MM_PROCESS_TERMINATED) {
            nextProcess=mm_get_slot(process,MM_PROCESS_NEXT);
            prevProcess=mm_get_slot(process,MM_PROCESS_PREVIOUS);
            mm_store_slot(prevProcess,MM_PROCESS_NEXT,nextProcess);
            mm_store_slot(nextProcess,MM_PROCESS_PREVIOUS,prevProcess);
            process=nextProcess;
        } else {
            process=mm_get_slot(process,MM_PROCESS_NEXT);
        }
    }
}

void mm_raise(mmObjectPtr ex_class,char* message) {
		  mmObjectPtr args=mm_new_array(1);
		  mm_store_slot(args,0,mm_new_string(message));
		  mm_sync_msg_send(ex_class,"raise",args);
		  mm_free_object(args);
}

mmObjectPtr mm_sync_msg_send(mmObjectPtr rcvr,char* sel,mmObjectPtr args) {
	mmObjectPtr selector=mm_new_string(sel);
	mmObjectPtr process=getActiveProcess();
	mmObjectPtr context=mm_get_slot(process,MM_PROCESS_CONTEXT);
	mmObjectPtr rv=mm_nil;

	int rc=mm_msg_send(context,rcvr,selector,args,0,&rv);
	if (rc==MM_PRIM_STD_RETURN) {
		return rv;
	} else {
		mm_interpret_until(context);
		rv=top(context);
		mm_inc_ref(rv);
		pop(context);
		return rv;
	}
}

mmObjectPtr mm_send_release(mmObjectPtr rcvr) {
	mmObjectPtr method=mm_nil;
	mmObjectPtr rv=0;
	mmObjectPtr systemClass=mm_nil;
	if (interp_bootstrapped) {
		if (rcvr->_class->_flags & MM_IMPLEMENTS_RELEASE_FLAG) {
			method = lookup_method(rcvr,release_selector,0);
			if (method != mm_nil) {
				systemClass=mm_load_class("Core::System",NULL,mm_nil);
				release_args->_slots[0]=rcvr;
				rv=mm_sync_msg_send(systemClass,"addReleaseCandidate",release_args);
			}
		}
	}
	return rv;
}
