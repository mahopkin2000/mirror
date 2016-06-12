#ifndef _MM_INTERPRETER_H_
#define _MM_INTERPRETER_H_

#include "memory.h"

/* constants */
#define MM_MAX_STACK_SIZE   20

/* bytecode definitions */
#define MM_BC_JUMP_FORWARD              100
#define MM_BC_JUMP_BACKWARD             101
#define MM_BC_JUMP_FORWARD_ON_TRUE      102
#define MM_BC_JUMP_BACKWARD_ON_TRUE     103
#define MM_BC_JUMP_FORWARD_ON_FALSE     104
#define MM_BC_JUMP_BACKWARD_ON_FALSE    105
#define MM_BC_PUSH_SELF         112
#define MM_BC_PUSH_TRUE         113
#define MM_BC_PUSH_FALSE        114
#define MM_BC_PUSH_NIL          115
#define MM_BC_PUSH_THIS_CONTEXT 116
#define MM_BC_RETURN            124
#define MM_BC_MSG_SEND          132
#define MM_BC_MSG_SEND_SUPER    133
#define MM_BC_POP               135
#define MM_BC_INST_VAR_FETCH    139
#define MM_BC_LITERAL_FETCH     140 
#define MM_BC_INST_VAR_STORE    141
#define MM_BC_ARRAY_LITERAL     142
#define MM_BC_BLOCK_CLOSURE     143
#define MM_BC_LOCAL_VAR_FETCH   145
#define MM_BC_LOCAL_VAR_STORE   146
#define MM_BC_CL_INST_VAR_STORE 147
#define MM_BC_CL_INST_VAR_FETCH 148
#define MM_BC_SET_SCOPE_LEVEL   150

/* block definition */
#define MM_BLOCK_PARENT                 0
#define MM_BLOCK_LITERALS               1
#define MM_BLOCK_BYTECODE               2
#define MM_BLOCK_NESTED_SCOPES_TEMPLATE  3
#define MM_BLOCK_START_SCOPE            4
#define MM_BLOCK_SOURCE_ARRAY           5
#define MM_BLOCK_SIZE                   6

/* block context definition */
#define MM_BLOCK_CONTEXT_BLOCK          0
#define MM_BLOCK_CONTEXT_PARENT         1
#define MM_BLOCK_CONTEXT_STACK          2
#define MM_BLOCK_CONTEXT_IP             3
#define MM_BLOCK_CONTEXT_SP             4
#define MM_BLOCK_CONTEXT_RCVR           5
#define MM_BLOCK_CONTEXT_NESTED_SCOPES  6 
#define MM_BLOCK_CONTEXT_SCOPE_LEVEL    7
#define MM_BLOCK_CONTEXT_SIZE           8

/* method definition */
#define MM_METHOD_BLOCK                 0
#define MM_NATIVE_METHOD_ADDRESS        1
#define MM_METHOD_NAME                  2
#define MM_METHOD_NUMARGS               3
#define MM_METHOD_SOURCE_FILE		4
#define MM_METHOD_SOURCE_CODE		5
#define MM_METHOD_SIZE                  6

/* process definition */
#define MM_PROCESS_CONTEXT              0
#define MM_PROCESS_STATE                1
#define MM_PROCESS_NEXT                 2
#define MM_PROCESS_PREVIOUS             3
#define MM_PROCESS_SIZE                 4

/* processor definition */
#define MM_PROCESSOR_PROCESS_LIST_HEAD_CV  (0+MM_CLASS_SIZE)
#define MM_PROCESSOR_PROCESS_LIST_END_CV   (1+MM_CLASS_SIZE)
#define MM_PROCESSOR_ACTIVE_PROCESS_CV     (2+MM_CLASS_SIZE)

/* process state enums */
#define MM_PROCESS_SUSPENDED            0
#define MM_PROCESS_RUNNING              1
#define MM_PROCESS_READY                2
#define MM_PROCESS_BLOCKED              3
#define MM_PROCESS_TERMINATED           4

// msgSend return constants
#define MM_MSG_SEND_PRIM_RETURN_OK              0
#define MM_MSG_SEND_CONTEXT                     2

/* helper macros */
#define getActiveProcess() \
    (   mm_get_slot(mm_processor_class, \
        MM_PROCESSOR_ACTIVE_PROCESS_CV)   )

#define getProcessHead() \
    (   mm_get_slot(mm_processor_class, \
                   MM_PROCESSOR_PROCESS_LIST_HEAD_CV)    )

#define getProcessEnd() \
    (   mm_get_slot(mm_processor_class, \
                            MM_PROCESSOR_PROCESS_LIST_END_CV)   )
                            

/* interface */
#ifdef __cplusplus
extern "C" {
#endif
extern void mm_interpret();
extern void mm_interpret_until(mmObjectPtr gate_context);
extern mmObjectPtr mm_new_block(mmObjectPtr parent);
extern mmObjectPtr mm_new_method(mmObjectPtr selector,mmObjectPtr topBlock);
extern void mm_init_interpreter();
extern mmObjectPtr mm_schedule_process(mmObjectPtr context);
extern void mm_prepare_block(mmObjectPtr block,mmObjectPtr args);
extern int mm_msg_send(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr selector,
                       mmObjectPtr args,int sendToSuper,mmObjectPtr* rv);
extern void mm_copy_scope(mmObjectPtr src,mmObjectPtr dest);
mmObjectPtr mm_new_block_context(mmObjectPtr block,mmObjectPtr parent,mmObjectPtr rcvr);
extern void mm_raise(mmObjectPtr ex_class,char* message);
extern mmObjectPtr mm_sync_msg_send(mmObjectPtr rcvr,char* sel,mmObjectPtr args);
extern mmObjectPtr mm_send_release(mmObjectPtr rcvr);
#ifdef __cplusplus
}
#endif

#endif

