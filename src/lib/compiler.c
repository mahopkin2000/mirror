#include "mirror.h"
#include "compiler.h"
#include "memory.h"
#include "interp.h"
#include "tree.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "config.h"
#include "dict.h"
#include "linklst.h"
#include <sys/stat.h>

#define DEFAULT_BYTECODE_SIZE 5000
#define MM_NAMESPACE_DICT_SIZE  100

extern FILE* yyin;     
extern mmAbstractNode *program;  
extern long lineno;
extern char* sourceFile;
extern mmLinkedList *instVars,*classVars;
extern void setup_parse_buffer(char* buff);
extern void free_parse_buffer();

typedef struct mm_source_unit {
    int _lineno;
    long _line_start_pos;
    long _sourcePos;
} MM_SourceUnit;

typedef struct mmByte_code {
    int _size;
    int _numbytes;
    MM_SourceUnit** _sa;
    mmByte _bytes[1];
} mmByteCode;

typedef struct mm_scope_info {
    mmLinkedList* _localVars;
    int _scopeIdx;
    struct mm_scope_info* _parentInfo;
} MM_ScopeInfo;

typedef enum {
    forLoop,
    whileLoop
} MM_LoopType;

typedef struct mm_loop_info {
    MM_LoopType _loopType;
    int _loopStart;
    int _forLoopExprStart;
    int _loopEnd;
} MM_LoopInfo;

typedef enum {
    LocalVar,
    InstanceVar,
    ClassInstanceVar
} MM_VariableType;

typedef enum {
    InstanceSide,
    ClassSide
} MM_MethodScope;

/* static utility functions */
static mmObjectPtr buildBlock(mmAbstractNode* csn,mmObjectPtr parentBlock,
                     mmObjectPtr cl,MM_MethodScope scope);
static mmByteCode* newByteCode();
static mmByte getByteCode(mmByteCode* bc,int idx);
static void addByteCode(mmByteCode* bc,mmByte b);
static void freeByteCode(mmByteCode* bc);  
static void buildExpression(mmAbstractNode* e,mmLinkedList* literals,
                            mmByteCode* bc,mmObjectPtr cl,MM_ScopeInfo* si,
                            MM_MethodScope scope,mmObjectPtr block);
static MM_ScopeInfo* newScopeInfo(
                            mmLinkedList* localVars,
                            int scopeIdx,
                            MM_ScopeInfo* parentInfo);
static void freeScopeInfo(MM_ScopeInfo* bi);
static int indexOfLocalVar(char* varname,MM_ScopeInfo* startScope,MM_ScopeInfo** result);
static void buildStatements(mmAbstractNode* sl,mmObjectPtr cl,MM_ScopeInfo* si,
                            mmLinkedList* literals,mmByteCode* bc,mmObjectPtr block,
                            mmLinkedList* scopelist,int* scopeIdx,MM_LoopInfo* li,
                            MM_MethodScope scope);
static MM_LoopInfo* newLoopInfo(MM_LoopType loopType,int loopStart,int forLoopExprStart,int loopEnd);
static void freeLoopInfo(MM_LoopInfo* li);
static mmObjectPtr importClass(mmAbstractNode* import,mmObjectPtr dict);
static MM_SourceUnit** newSourceUnitArray();
static char* findClassFile(char* cn);
static char* baseClassname(char* cn);


void mm_print_ll(mmLinkedList* ll) {
    mmLinkedListNode* node=ll->_head;
    int item=0;
    while (node) {
        debug("Linked list item: %d\n",item++);
        mm_print_tree(node->_data,0);
        node = node->_next;
    }
}

int mm_index_of_ll_node_for_string(mmLinkedList* ll,char* s) {
    mmLinkedListNode* node=ll->_head;
    mmLinkedListNode* rv=NULL;
    int idx=0;

    while (node) {
        if (    (node->_data) &&
                (strcmp(node->_data,s)==0)  ) {
            return idx;
        }
        node=node->_next;
        idx++;
    }

    return -1;
}

static char* fixup_string(char* str) {
    char* newstr=NULL;
    int i,j;
    int escnt=0,newlen;
    char* rv;

    newstr=(char*)malloc(strlen(str)-1);
    for (i=1,j=0;i<strlen(str)-1;i++,j++) {
        newstr[j]=str[i];    
    }
    newstr[j]='\0';
    newlen=strlen(newstr);
    for (i=0;i<newlen;i++) {
        if (newstr[i]=='\\') {
            switch (newstr[i+1]) {
            case 'n':
                newstr[i]='\r';
                newstr[++i]='\n';
                continue;
            case 'r':
                newstr[i]='\r';
                break;
            case 't':
                newstr[i]='\t';
                break;
				case '"':
					 newstr[i]='"';
					 break;
            case '\\':
                newstr[i]='\\';
                break;
            default:
                compile_error("Invalid escape sequence: '%c%c'\n",'\\',newstr[i+1]);
            }
            newstr[++i]='\0';
            escnt++;
        }
    }
    if (escnt) {
        rv=(char*)malloc(newlen-escnt+1);
        for (i=0,j=0;i<newlen;i++) {
            if (newstr[i]!='\0') {
                rv[j]=newstr[i];
                j++;
            }
        }
        rv[newlen-escnt]='\0';
        free(newstr);
    } else {
        rv=newstr;
    }
    return rv;
}

static int addOrFindLiteral(mmLinkedList* literals,mmAbstractNode* ln,mmObjectPtr o) {
    mmLinkedListNode* node=literals->_head;
    int idx=0;
    mmObjectPtr lit;
    char* str=NULL;
    int i,j;

    if (ln) {
        if (ln->_type == mmStringNode) {
            str=fixup_string(ln->_payload._sVal);
        } else if ( (ln->_type != mmIntegerNode) && 
                    (ln->_type != mmFloatNode) &&
                    (ln->_type != mmCharacterNode)  ) {
            str=strdup(ln->_payload._sVal);
        }

        while (node) {
            lit=(mmObjectPtr)node->_data;
            if (    (ln->_type==mmIntegerNode) &&
                    (lit->_class==mm_integer_class) &&
                    (mm_get_integer(lit)==ln->_payload._iVal)   ) {
                break;    
            } else if ( (ln->_type==mmFloatNode) &&
                        (lit->_class==mm_float_class) &&
                        (mm_get_float(lit)==ln->_payload._dVal)   ) {
                break; 
            } else if ( (ln->_type==mmCharacterNode) &&
                        (lit->_class==mm_character_class) &&
                        (mm_get_character(lit)==(ln->_payload._sVal)[1])    ) {
                break;
            } else if ( (lit->_class==mm_string_class) &&
                        (str) &&
                        (strcmp(mm_get_string(lit),str)==0) ) {
                break;
            }
            idx++;
            node=node->_next;
        }
    
        if (idx==literals->_size) {
            if (ln->_type==mmIntegerNode) {
                mm_add_ll_node(literals,(void*)mm_new_integer_from_string(ln->_payload._sVal));
                debug("ADDED Integer literal: %s\n",ln->_payload._sVal);
            } else if (ln->_type==mmFloatNode) {
                mm_add_ll_node(literals,(void*)mm_new_float_from_string(ln->_payload._sVal));
                debug("ADDED Float literal: %s\n",ln->_payload._sVal);
            } else if (ln->_type==mmCharacterNode) {
                mm_add_ll_node(literals,(void*)mm_new_character((ln->_payload._sVal)[1]));
                debug("ADDED Character literal: %c\n",(ln->_payload._sVal)[1]);
            } else {
                mm_add_ll_node(literals,(void*)mm_new_string(str));
                free(str);
                debug("ADDED String literal: %s\n",ln->_payload._sVal);
            }
        }
    } else {
        while (node) {
            lit=(mmObjectPtr)node->_data;
            if (lit==0) {
                break;
            }
            idx++;
            node=node->_next;
        }
        if (idx==literals->_size) {
            mm_add_ll_node(literals,(void*)o);
        }
    }

    return idx;
}

static int index_of_inst_var(mmObjectPtr array,char* varname) {
    int i;
    for (i=0;i<array->_size;i++) {
        if (!strcmp(mm_get_string(mm_get_slot(array,i)),varname)) {
            return i;
        }
    }
    return -1;
}


static int index_of_variable(MM_MethodScope scope,mmObjectPtr cl,MM_ScopeInfo* localVars,
                                char* varname,MM_VariableType* vartype,
                                MM_ScopeInfo** result) {
    int varIdx=-1;
    varIdx=indexOfLocalVar(varname,localVars,result);
    if (varIdx >= 0) {
        *vartype=LocalVar;
        return varIdx;
    }

    if (scope==InstanceSide) {
        varIdx=index_of_inst_var(mm_get_slot(cl,MM_CLASSDESC_INSTVARS),varname);
        if (varIdx >= 0) {
            *vartype=InstanceVar;
            return varIdx;
        }
    } 

    varIdx=index_of_inst_var(mm_get_slot(cl->_class,MM_CLASSDESC_INSTVARS),varname);
    if (varIdx >= 0) {
        *vartype=ClassInstanceVar;
        return varIdx;
    }

    return -1;
}

static mmByteCode* newByteCode() {
    int i;
    mmByteCode* bc=(mmByteCode*)malloc(sizeof(mmByteCode)+DEFAULT_BYTECODE_SIZE);
    bc->_numbytes=0;
    bc->_size=DEFAULT_BYTECODE_SIZE;
    bc->_sa=(MM_SourceUnit**)malloc(sizeof(MM_SourceUnit*) * DEFAULT_BYTECODE_SIZE);
    for (i=0;i<DEFAULT_BYTECODE_SIZE;i++) {
        bc->_sa[i]=NULL;
    }
    return bc;
}

static mmByte getByteCode(mmByteCode* bc,int idx) {
    return bc->_bytes[idx];
}

static void addByteCode(mmByteCode* bc,mmByte b) {
    if (bc->_numbytes < bc->_size) {
        bc->_bytes[bc->_numbytes++]=b;
    } else {
        /* need to add some logic here to grow the byte array */
    }
}

static void printByteCode(mmByteCode* bc) {
    int i;
    fprintf(stderr,"Dumping byte code array - size=%d,capacity=%d:\n",bc->_numbytes,bc->_size);
    for (i=0;i<bc->_numbytes;i++) {
        fprintf(stderr,"%d ",bc->_bytes[i]);
    }
    fprintf(stderr,"\n");
}

static void storeByteCode(mmByteCode* bc,int idx,mmByte b) {
    bc->_bytes[idx]=b;
}

static void freeByteCode(mmByteCode* bc) {
    free(bc);
}

static int getByteIndex(mmByteCode* bc) {
    return bc->_numbytes;
}

static MM_ScopeInfo* newScopeInfo(
                            mmLinkedList* localVars,
                            int scopeIdx,
                            MM_ScopeInfo* parentInfo) {
    MM_ScopeInfo* rv=(MM_ScopeInfo*)malloc(sizeof(MM_ScopeInfo));
    rv->_localVars=localVars;
    rv->_scopeIdx=scopeIdx;
    rv->_parentInfo=parentInfo;
    return rv;
}                    

static int indexOfLocalVar(char* varname,MM_ScopeInfo* startScope,MM_ScopeInfo** result) {
    int localVarIdx=-1;
    mmLinkedList* localVars=startScope->_localVars;

    localVarIdx=mm_index_of_ll_node_for_string(localVars,varname);
    if (localVarIdx >= 0) {
        *result=startScope;
        return localVarIdx;
    } else if (startScope->_parentInfo) {
        return indexOfLocalVar(varname,startScope->_parentInfo,result); 
    } else {
        *result=NULL;
        return -1;
    }
}

static void freeScopeInfo(MM_ScopeInfo* bi) {
    free(bi);
}

static MM_LoopInfo* newLoopInfo(MM_LoopType loopType,int loopStart,int forLoopExprStart,int loopEnd) {
    MM_LoopInfo* rv = (MM_LoopInfo*)malloc(sizeof(MM_LoopInfo));
    rv->_loopType=loopType;
    rv->_loopStart=loopStart;
    rv->_forLoopExprStart=forLoopExprStart;
    rv->_loopEnd=loopEnd;
    return rv;
}

static void freeLoopInfo(MM_LoopInfo* li) {
    free(li);
}

static mmObjectPtr lookupGlobalIdent(char* ident) {
    /*  if this identifier is a qualified identifier (i.e. has :: namespace delimiters
        within it, need to reference the system dictionary hierarchy to attempt to locate
        the identifier.  Otherwise, assume this identifier has been imported into the 
        global namespace of this class and attempt to reference it in the class pool
        dictionary.  Return nil if the identifier does not exist */
    mmObjectPtr rv=mm_nil;
    char *tok,*la;
    char *buff=strdup(ident);
    mmObjectPtr d=mm_nil;

    /* seed dictionary and string token */
    tok=strtok(buff,"::");
    la=strtok(NULL,"::");
    d=mm_sys_dict;
    while (la) {
        d=mm_dict_at(d,tok);
        if (d==mm_nil) {
            break;
        }
        tok=la;
        la=strtok(NULL,"::");
    }
    if (d != mm_nil) {
        rv=mm_dict_at(d,tok);
    }
    free(buff);
    return rv;
}

static mmObjectPtr resolveGlobalIdent(mmAbstractNode* identNode,
		mmObjectPtr cl,char* namespace) {
    /*  if this identifier is a qualified identifier (i.e. has :: namespace delimiters
        within it, need to reference the system dictionary hierarchy to attempt to locate
        the identifier.  Otherwise, assume this identifier has been imported into the 
        global namespace of this class and attempt to reference it in the class pool
        dictionary.  Return nil if the identifier does not exist */
    mmObjectPtr pooldict=mm_nil;
    mmObjectPtr rv=mm_nil;
    char* ident=identNode->_payload._sVal;
    char buff[256];
    
    debug("resolveGlobalIdent: namespace is %s\n",namespace);
    if (    (strchr(ident,':')) ||
            (cl==mm_nil) ) {
        rv=mm_load_class(ident,identNode,mm_nil);
    } else {
        // check to see if the identifier refers to a class within
        // the provided namespace.  If so, arrange for that class to be
        // loaded
        if ((rv=mm_dict_at(cl,ident))==mm_nil) {
            bzero(buff,sizeof(buff));
            if (namespace) {
                strcat(buff,namespace);
                strcat(buff,"::");
            }
            strcat(buff,ident);
            debug("resolveGlobalIdent: buff is %s\n",buff);
            rv=mm_load_class(buff,identNode,mm_nil);
        }
    }
    return rv;
}

static MM_SourceUnit** newSourceUnitArray() {
    int i=0;
    MM_SourceUnit** rv=(MM_SourceUnit**)malloc(sizeof(MM_SourceUnit*) * DEFAULT_BYTECODE_SIZE);
    for (i=0;i<DEFAULT_BYTECODE_SIZE;i++) {
        rv[i]=NULL;
    }
    return rv;
}

static void addSourceUnit(mmByteCode* bc,mmAbstractNode* n) {
    MM_SourceUnit** sa=bc->_sa;
    MM_SourceUnit* su=(MM_SourceUnit*)malloc(sizeof(MM_SourceUnit));

    su->_sourcePos=n->_filepos;
    su->_lineno=n->_lineno;
    su->_line_start_pos=n->_line_start_pos;
    sa[bc->_numbytes]=su;
}

static mmObjectPtr dictForNamespace(char* ns) {
    mmObjectPtr dict=mm_nil;
    char* namespace=strdup(ns);
    char* nsp=NULL;
    mmObjectPtr nd=mm_nil;
    mmObjectPtr d=mm_sys_dict;

    nsp=strtok(namespace,"::");
    while (nsp) {
        if ((nd=mm_dict_at(d,nsp))==mm_nil) {
            nd=mm_new_dictionary(MM_NAMESPACE_DICT_SIZE);
            mm_dict_put(d,nsp,nd);
        }
        d=nd;
        nsp=strtok(NULL,"::");
    }
    free(namespace);

    return d;
}

static void alloc_class_slots(mmObjectPtr cl,int num_inst_vars) {
    int size;
    int i;

    /* if we have any class instance variables, allocate space for them
       on the class object */
    if (num_inst_vars) {
        size = MM_CLASS_SIZE+num_inst_vars;
    } else {
        size = MM_CLASS_SIZE;
    }
    cl->_slots=mm_alloc(sizeof(mmObjectPtr)*size);
    cl->_size = size;
    for (i=0;i<size;i++) {
        cl->_slots[i]=mm_nil;
    }
}

static void add_inst_vars(mmLinkedList* inst_attr_list,
                          mmObjectPtr nc_inst_vars,
                          int inst_attr_start_index,
                          mmLinkedList* cl_attr_list,
                          mmObjectPtr mc_inst_vars,
                          int cl_attr_start_index) {
    mmLinkedListNode* node=NULL;
    mmAbstractNode* an=NULL;

    node=inst_attr_list->_head;
    while (node) {
        an=node->_data;
        mm_store_slot(nc_inst_vars,inst_attr_start_index++,mm_new_string(an->_payload._sVal));
        node=node->_next;
    }

    node=cl_attr_list->_head;
    while (node) {
        an=node->_data;
        mm_store_slot(mc_inst_vars,cl_attr_start_index++,mm_new_string(an->_payload._sVal));
        node=node->_next;
    }
}

/*
 * main compilation routine
 */
mmObjectPtr mm_compile_class(mmAbstractNode* pt,char* fn,mmObjectPtr proto_class)
{
    mmAbstractNode *namespace_node=NULL,*import_list_node=NULL,*class_node=NULL;
    mmObjectPtr nc=mm_nil;
    mmObjectPtr mc=mm_nil;
    mmObjectPtr d=mm_nil;
    char *cn=NULL,*namespace="";
    int hasPrimitives=0;
    mmObjectPtr pool_dict=mm_nil;
    mmAbstractNode *cc,*ce;
    MM_MethodScope mode = InstanceSide;
    mmLinkedList* instAttrList = mm_new_ll();
    mmLinkedList* clAttrList = mm_new_ll();
    mmLinkedList* instMethodList = mm_new_ll();
    mmLinkedList* clMethodList = mm_new_ll();
    mmAbstractNode *n1,*n2,*n3,*n4;
    mmAbstractNode* class_content_node=NULL;
    mmAbstractNode* class_modifier_node=NULL;
    mmAbstractNode* super_class_node=NULL;
    mmObjectPtr ncInstVars=mm_nil;
    int idx;
    mmLinkedListNode* node=NULL;
    mmAbstractNode* an=NULL;
    mmObjectPtr ncMethodDict=mm_nil;
    mmObjectPtr selector=mm_nil;
    mmObjectPtr method=mm_nil;
    mmObjectPtr block=mm_nil;
    mmObjectPtr mcInstVars=mm_nil;
    mmObjectPtr mcMethodDict=mm_nil;
    mmObjectPtr superclass=mm_nil;
    mmObjectPtr clClass=mm_nil;
    int clFormat=0,mcFormat=0;
    mmAbstractNode *parmlist,*parm,*primmod;
    int numargs=0;
    mmAbstractNode* import_node=NULL;
    char* import_base_cn;
    int inst_vars_size=0,cl_inst_vars_size=0;
    mmObjectPtr anc_inst_vars=mm_nil,anc_cl_inst_vars=mm_nil;
    int inst_vars_idx=0,cl_inst_vars_idx=0;

    /* assume class will be created in the global namespace */
    d=mm_sys_dict;
    pool_dict=mm_new_dictionary(20);

    /* check if a namespace has been declared for this class */
    namespace_node=mm_find_child_of_type(pt,mmNamespaceDeclNode);
    if (namespace_node) {
        namespace=mm_get_string_value(namespace_node);
        d=dictForNamespace(namespace);
    }

    /* now process the actual class definition */
    class_node=mm_find_child_of_type(pt,mmClassNode);
    cn=mm_get_string_value(class_node);
    mc=mm_new_object(mm_metaclass_class,MM_METACLASS_SIZE);
    mm_inc_ref(mc);
    if (proto_class==mm_nil) {
        nc=mm_new_object(mc,0);
    } else {
        proto_class->_class=mc;
        nc=proto_class;
        debug("using proto class for class %s\n",cn);
        nc->_size=0;
    }
    mm_store_slot(mc,MM_METACLASS_THISCLASS,nc);
    mm_dict_put(d,cn,nc);
    debug("added class %s to system dictionary\n",cn);

    /* check for modifier */
    class_modifier_node=mm_find_child_of_type(class_node,mmClassModifierNode);
    if (class_modifier_node) {
        if (strcmp(class_modifier_node->_payload._sVal,"byte")==0) {
            clFormat |= 0x01;    
        } else if (strcmp(class_modifier_node->_payload._sVal,"indexed")==0) {
            clFormat |= 0x02;
        }
    }

    class_content_node=mm_find_child_of_type(class_node,mmClassContent);
    ce = class_content_node->_children;
    while (ce) {
        switch (ce->_type) {
        case mmInstanceNode:
            mode = InstanceSide;
            break;
        case mmClassNode:
            mode = ClassSide;
            break;
        case mmMethodNode:
        case mmBinaryNode:
            mm_add_ll_node(((mode==InstanceSide) ? instMethodList : clMethodList),ce);
            break;
        case mmAttributeNode:
            n1 = ce->_children;
            n2 = n1->_children;
            while (n2) {
                mm_add_ll_node(((mode==InstanceSide) ? instAttrList : clAttrList),n2);
                n2 = n2->_nextNode;
            }
            break;
        default:
            break;
        }
        ce = ce->_nextNode;
    }


    /* resolve super class */
    super_class_node=mm_find_child_of_type(class_node,mmIdentifierNode);
    if (!super_class_node) {
        ncInstVars=mm_new_array(instAttrList->_size);
        mcInstVars=mm_new_array(clAttrList->_size);
        add_inst_vars(instAttrList,ncInstVars,0,clAttrList,mcInstVars,0);
        clFormat |= instAttrList->_size<<2;
        mcFormat |= clAttrList->_size<<2;
        alloc_class_slots(nc,clAttrList->_size);
        mm_store_slot(mc,MM_CLASSDESC_INSTVARS,mcInstVars);
        mm_store_slot(nc,MM_CLASSDESC_INSTVARS,ncInstVars);  
    }

    /* check if we have any imports to process.  Make sure to import the superclass
       first before other imports */
    import_list_node=mm_find_child_of_type(pt,mmImportListNode);
    if (super_class_node) {
        import_node = import_list_node->_children;
        while (import_node) {
            import_base_cn=baseClassname(import_node->_payload._sVal);
            if (strcmp(super_class_node->_payload._sVal,import_base_cn)==0) {
                importClass(import_node,pool_dict);
                break;
            }
            import_node=import_node->_nextNode;
        }
        superclass=resolveGlobalIdent(super_class_node,pool_dict,namespace);
        if (superclass==mm_nil) {
            compile_error("%s: error at line %d: superclass %s not a valid global identifier.\n",
              super_class_node->_sourceFile,super_class_node->_lineno,mm_get_string_value(super_class_node));
            return mm_nil;
        }

        /* build instance and class variable arrays */
        inst_vars_size=instAttrList->_size;
        cl_inst_vars_size=clAttrList->_size;

        anc_cl_inst_vars=mm_get_slot(superclass->_class,MM_CLASSDESC_INSTVARS);
        anc_inst_vars=mm_get_slot(superclass,MM_CLASSDESC_INSTVARS);
        inst_vars_size+=anc_inst_vars->_size;
        cl_inst_vars_size+=anc_cl_inst_vars->_size;

        ncInstVars=mm_new_array(inst_vars_size);
        mcInstVars=mm_new_array(cl_inst_vars_size);
        clFormat |= inst_vars_size<<2;
        mcFormat |= cl_inst_vars_size<<2;
        alloc_class_slots(nc,cl_inst_vars_size);

        for (inst_vars_idx=0;inst_vars_idx<anc_inst_vars->_size;inst_vars_idx++) {
            mm_store_slot(ncInstVars,inst_vars_idx,
                          mm_get_slot(anc_inst_vars,inst_vars_idx));
        }
        for (cl_inst_vars_idx=0;cl_inst_vars_idx<anc_cl_inst_vars->_size;cl_inst_vars_idx++) {
            mm_store_slot(mcInstVars,cl_inst_vars_idx,
                          mm_get_slot(anc_cl_inst_vars,cl_inst_vars_idx));
        }
        add_inst_vars(instAttrList,ncInstVars,inst_vars_idx,clAttrList,mcInstVars,cl_inst_vars_idx);
    }

    /* import all classes now */
    import_node = import_list_node->_children;
    while (import_node) {
        importClass(import_node,pool_dict);
        import_node=import_node->_nextNode;
    }


    /* build the new Metaclass object */
    mm_store_slot(mc,MM_CLASSDESC_INSTVARS,mcInstVars);
    mm_store_slot(mc,MM_BEHAVIOR_METHOD_DICT,mm_new_dictionary(clMethodList->_size*2));
    mm_store_slot(mc,MM_METACLASS_THISCLASS,nc);
    mm_store_slot(mc,MM_BEHAVIOR_FORMAT,mm_new_integer(mcFormat));

    /* build new Class object */
    mm_store_slot(nc,MM_CLASSDESC_INSTVARS,ncInstVars);
    mm_store_slot(nc,MM_CLASS_NAME,mm_new_string(cn));
    mm_store_slot(nc,MM_BEHAVIOR_METHOD_DICT,mm_new_dictionary(instMethodList->_size*2));
    mm_store_slot(nc,MM_BEHAVIOR_FORMAT,mm_new_integer(clFormat));
    mm_store_slot(nc,MM_CLASS_POOL,pool_dict);
    mm_store_slot(nc,MM_CLASS_NAMESPACE,mm_new_string(namespace));
    mm_dict_put(pool_dict,cn,nc);
    if (superclass != mm_nil) {
        mm_store_slot(nc,MM_BEHAVIOR_SUPERCLASS,superclass);
        mm_store_slot(mc,MM_BEHAVIOR_SUPERCLASS,superclass->_class);
    }

    /* add the methods to the class */
    ncMethodDict=mm_get_slot(nc,MM_BEHAVIOR_METHOD_DICT);
    node=instMethodList->_head;
    while (node) {
        parmlist=NULL;
        primmod=NULL;
        an=node->_data;
        numargs=0;
        if (an->_children->_type==mmPrimitiveNode) {
            primmod=an->_children;
            if (primmod->_nextNode->_type==mmParmListNode) {
                parmlist=primmod->_nextNode;
            }
            hasPrimitives=1;
        } else if (an->_children->_type==mmParmListNode) {
            parmlist=an->_children;
        }
        if (parmlist) {
            parm=parmlist->_children;
            while (parm) {
                numargs++;
                parm=parm->_nextNode;
            }
        } else if (an->_type==mmBinaryNode) {
            numargs=1;
        }
        if (	(an->_type==mmBinaryNode)  ||
					(numargs==0)	) {
        	  selector=mm_new_string(an->_payload._sVal);
			} else {
				char mname[128];
				sprintf(mname,"%s_%d",an->_payload._sVal,numargs);
        	  	selector=mm_new_string(mname);
			}
        debug("Adding method: %s\n",mm_get_string(selector));
        method=mm_new_method(selector,mm_nil);
        block=buildBlock(an,method,nc,InstanceSide);
        mm_store_slot(method,MM_METHOD_BLOCK,block);
        mm_store_slot(method,MM_METHOD_NUMARGS,mm_new_integer(numargs));
        mm_store_slot(method,MM_METHOD_SOURCE_FILE,mm_new_string(class_node->_sourceFile));
        mm_store_slot(block,MM_BLOCK_PARENT,method);
        mm_dict_put(ncMethodDict,mm_get_string(selector),method);
        node=node->_next;
    }
    mm_dict_print(ncMethodDict);

    /* add methods to metaclass */
    mcMethodDict=mm_get_slot(mc,MM_BEHAVIOR_METHOD_DICT);
    node=clMethodList->_head;
    while (node) {
        parmlist=NULL;
        primmod=NULL;
        an=node->_data;
        numargs=0;
        if (an->_children->_type==mmPrimitiveNode) {
            primmod=an->_children;
            if (primmod->_nextNode->_type==mmParmListNode) {
                parmlist=primmod->_nextNode;
            }
            hasPrimitives=1;
        } else if (an->_children->_type==mmParmListNode) {
            parmlist=an->_children;
        }
        if (parmlist) {
            parm=parmlist->_children;
            while (parm) {
                numargs++;
                parm=parm->_nextNode;
            }
        } else if (an->_type==mmBinaryNode) {
            numargs=1;
        }
        if (	(an->_type==mmBinaryNode) ||
					(numargs==0)	) {
        	  selector=mm_new_string(an->_payload._sVal);
			} else {
				char mname[128];
				sprintf(mname,"%s_%d",an->_payload._sVal,numargs);
        	  	selector=mm_new_string(mname);
			}
        method=mm_new_method(selector,mm_nil);
        block=buildBlock(an,method,nc,ClassSide);
        mm_store_slot(method,MM_METHOD_BLOCK,block);
        mm_store_slot(method,MM_METHOD_NUMARGS,mm_new_integer(numargs));
        mm_store_slot(method,MM_METHOD_SOURCE_FILE,mm_new_string(class_node->_sourceFile));
        mm_store_slot(block,MM_BLOCK_PARENT,method);
        if (primmod) {
            mm_store_slot(method,MM_NATIVE_METHOD_ADDRESS,mm_new_byte_array(mm_bytearray_class,4));
        }
        mm_dict_put(mcMethodDict,mm_get_string(selector),method);
        debug("added method to metaclass %s - selector is %s\n",
                cn,an->_payload._sVal);
        node=node->_next;
    }
    mm_dict_print(mcMethodDict);

    debug("finished compiling class %s\n",cn);
    
    /* add class to system dictionary */
    if (hasPrimitives) {
        mm_init_primitives(namespace,pt->_sourceFile,nc,fn);
    }

    return nc;
}

/* When building a class we need to process a set of imports which will load
 * other classes into its namespace where they can be accessed by typing just
 * the unqualified name of the class in code.  Otherwise classes must be refernced
 * as a fully qualified name, such as Net::FTP or something similar.
 * A class can represent objects of one of three basic formats, which are 
 * regular object (default), indexed (as in an array), or byte (as in a string or * byte array).  This is stored in the format instance variable of the class.
 * 
 */

static mmObjectPtr build_scope_template(mmLinkedList* scope_list) {
    mmObjectPtr scope_array=mm_nil;  
    mmLinkedListNode* se_node=NULL;
    mmLinkedListNode* lv_node=NULL;
    MM_ScopeInfo* scope_entry=NULL;
    mmObjectPtr si_array=mm_nil;
    int si_idx;
    int se_idx;


    scope_array=mm_new_array(scope_list->_size);
    se_node=scope_list->_head;
    se_idx=0;
    while (se_node) {
        scope_entry=(MM_ScopeInfo*)se_node->_data;
        if (scope_entry) {
            si_array=mm_new_array(scope_entry->_localVars->_size+1);
            mm_store_slot(si_array,0,
                (scope_entry->_parentInfo) ? 
                          mm_new_integer(scope_entry->_parentInfo->_scopeIdx) : mm_nil);
            si_idx=1;
            lv_node=scope_entry->_localVars->_head;
            while (lv_node) {
                mm_store_slot(si_array,si_idx++,
                              mm_new_string(lv_node->_data));
                lv_node=lv_node->_next;
            }
        } else {
            si_array=mm_new_array(1);
            mm_store_slot(si_array,0,mm_nil);
        }
        mm_store_slot(scope_array,se_idx++,si_array);
        se_node=se_node->_next;
    }
    return scope_array;
}

static mmObjectPtr buildBlock(
                mmAbstractNode* csn,
                mmObjectPtr parentBlock,
                mmObjectPtr cl,
                MM_MethodScope scope) {
    mmObjectPtr block=mm_nil;

    mmAbstractNode *n1,*n2,*n3,*n4;
    mmLinkedList* localvars=NULL;
    mmLinkedList* blocklist=mm_new_ll();
    mmByteCode* bc=newByteCode();
    mmLinkedList* literals=mm_new_ll();
    mmObjectPtr bytearray=mm_nil;
    mmObjectPtr litarray=mm_nil;
    int idx;
    mmLinkedListNode* node; 
    MM_ScopeInfo* startScope=newScopeInfo(mm_new_ll(),0,NULL);
    mmLinkedList* scopelist=mm_new_ll();
    mmObjectPtr scopeArray=mm_nil;
    mmObjectPtr scopeArrayEntry=mm_nil;
    MM_ScopeInfo* scopeEntry=NULL;
    mmLinkedListNode* seNode=NULL;
    int scopeIdx=0;
    mmObjectPtr sourceUnitArray=mm_nil;
    MM_SourceUnit** sua=NULL;
    MM_SourceUnit* su=NULL;
    mmObjectPtr suObj=mm_nil;
    int si_idx;
    mmObjectPtr si_array=mm_nil;

    /* build the Block object */
    block=mm_new_block(parentBlock);
    localvars=startScope->_localVars;
    mm_add_ll_node(scopelist,startScope);


    /* get the child statements of this compound statement node */
    n1=csn->_children;
    if (n1->_type==mmPrimitiveNode) {
        n1=n1->_nextNode;
    }

    /*  check if this is a top level block (block associated with a method).
        if so, process the parameter list and include parameters as local
        variables */ 
    if (n1->_type==mmParmListNode) {
        n2=n1->_children;
        while (n2) {
            mm_add_ll_node(localvars,mm_get_string_value(n2));
            n2=n2->_nextNode;
        }
        n1=n1->_nextNode;
    } else if (n1->_type==mmBinaryParmNode) {
        mm_add_ll_node(localvars,mm_get_string_value(n1));
        n1=n1->_nextNode;
    }

    /*  n1 is a CompoundStmtNode. Now grab the StatementList and compile the individual
        statements  */
    n2=n1->_children;
    if (n2) {
        buildStatements(n2,cl,startScope,literals,bc,block,scopelist,&scopeIdx,NULL,scope);
    }

    /*  turn the scopelist into an Array of Integer objects.  Each Integer represents the
        size of a nested array to create when building the context */
    scopeArray=build_scope_template(scopelist);
    mm_store_slot(block,MM_BLOCK_NESTED_SCOPES_TEMPLATE,scopeArray);

    /* dump out generated bytecodes for debugging purposes */
    if (mm_verbose) {
        printByteCode(bc);
    }
    
    /* build the byte and literal array objects */
    bytearray=mm_new_byte_array(mm_bytearray_class,bc->_numbytes);
    sourceUnitArray=mm_new_array(bc->_numbytes);
    sua=bc->_sa;

    for (idx=0;idx<bc->_numbytes;idx++) {
        ((mmByteArray*)bytearray)->_bytes[idx]=bc->_bytes[idx];
        su=sua[idx];
        if (su) {
            suObj=mm_new_array(3);
            mm_store_slot(suObj,0,mm_new_integer(su->_sourcePos));
            mm_store_slot(suObj,1,mm_new_integer(su->_lineno));
            mm_store_slot(suObj,2,mm_new_integer(su->_line_start_pos));
            mm_store_slot(sourceUnitArray,idx,suObj);  
        }
    }
    mm_store_slot(block,MM_BLOCK_SOURCE_ARRAY,sourceUnitArray);
    mm_store_slot(block,MM_BLOCK_BYTECODE,bytearray);
    litarray=mm_new_array(literals->_size);
    node=literals->_head;
    idx=0;
    while (node) {
        mm_store_slot(litarray,idx,(mmObjectPtr)node->_data);
        node=node->_next;
        idx++;
    }
    mm_store_slot(block,MM_BLOCK_LITERALS,litarray);

    return block;

}

static mmObjectPtr buildBlockClosure(mmAbstractNode* bcn,mmObjectPtr cl,
                                   MM_ScopeInfo* si,MM_MethodScope scope,
                                     mmObjectPtr parent) {

    mmObjectPtr block=mm_nil;

    mmAbstractNode *n1,*n2,*n3,*n4;
    mmLinkedList* localvars=NULL;
    mmLinkedList* blocklist=mm_new_ll();
    mmByteCode* bc=newByteCode();
    mmLinkedList* literals=mm_new_ll();
    mmObjectPtr bytearray=mm_nil;
    mmObjectPtr litarray=mm_nil;
    int idx;
    mmLinkedListNode* node; 
    MM_ScopeInfo* startScope=newScopeInfo(mm_new_ll(),si->_scopeIdx+1,si);
    mmLinkedList* scopelist=mm_new_ll();
    mmObjectPtr scopeArray=mm_nil;
    mmObjectPtr scopeArrayEntry=mm_nil;
    MM_ScopeInfo* scopeEntry=NULL;
    mmLinkedListNode* seNode=NULL;
    int scopeIdx=si->_scopeIdx+1;
    mmObjectPtr sourceUnitArray=mm_nil;
    MM_SourceUnit** sua=NULL;
    MM_SourceUnit* su=NULL;
    mmObjectPtr suObj=mm_nil;

    /* build a subset of the scopelist relevant to this closure */
    for (idx=0;idx<=si->_scopeIdx;idx++) {
        mm_add_ll_node(scopelist,NULL);
    }
    scopeEntry=si;
    while (scopeEntry!=NULL) {
        mm_store_ll_node(scopelist,scopeEntry->_scopeIdx,scopeEntry);
        scopeEntry=scopeEntry->_parentInfo;
    }

    /* build the Block object */
    block=mm_new_block(parent);
    mm_store_slot(block,MM_BLOCK_START_SCOPE,mm_new_integer(startScope->_scopeIdx));
    localvars=startScope->_localVars;
    mm_add_ll_node(scopelist,startScope);


    /* get the child statements of this block node */
    n1=bcn->_children;

    /*  check if this is a top level block (block associated with a method).
        if so, process the parameter list and include parameters as local
        variables */ 
    if (n1->_type==mmParmListNode) {
        n2=n1->_children;
        while (n2) {
            mm_add_ll_node(localvars,mm_get_string_value(n2));
            n2=n2->_nextNode;
        }
        n1=n1->_nextNode;
    }
    /*  compile the individual statements  */
    buildStatements(n1,cl,startScope,literals,bc,block,scopelist,&scopeIdx,NULL,scope);

    /*  turn the scopelist into an Array of Integer objects.  Each Integer represents the
        size of a nested array to create when building the context */
    scopeArray=build_scope_template(scopelist);
    mm_store_slot(block,MM_BLOCK_NESTED_SCOPES_TEMPLATE,scopeArray);

    /* dump out generated bytecodes for debugging purposes */
    if (mm_verbose) {
        printByteCode(bc);
    }

    /* build the byte and literal array objects */
    if (bc->_bytes[bc->_numbytes-1]==MM_BC_POP) {
        --bc->_numbytes;
    }
    bytearray=mm_new_byte_array(mm_bytearray_class,bc->_numbytes);
    sourceUnitArray=mm_new_array(bc->_numbytes);
    sua=bc->_sa;
    for (idx=0;idx<bc->_numbytes;idx++) {
        ((mmByteArray*)bytearray)->_bytes[idx]=bc->_bytes[idx];
        su=sua[idx];
        if (su) {
            suObj=mm_new_array(3);
            mm_store_slot(suObj,0,mm_new_integer(su->_sourcePos));
            mm_store_slot(suObj,1,mm_new_integer(su->_lineno));
            mm_store_slot(suObj,2,mm_new_integer(su->_line_start_pos));
            mm_store_slot(sourceUnitArray,idx,suObj);  
        }
    }
    mm_store_slot(block,MM_BLOCK_SOURCE_ARRAY,sourceUnitArray);
    mm_store_slot(block,MM_BLOCK_BYTECODE,bytearray);
    litarray=mm_new_array(literals->_size);
    node=literals->_head;
    idx=0;
    while (node) {
        mm_store_slot(litarray,idx,(mmObjectPtr)node->_data);
        node=node->_next;
        idx++;
    }
    mm_store_slot(block,MM_BLOCK_LITERALS,litarray);

    return block;
}

static void buildStatements(mmAbstractNode* sl,mmObjectPtr cl,MM_ScopeInfo* si,
                            mmLinkedList* literals,mmByteCode* bc,mmObjectPtr block,
                            mmLinkedList* scopelist,int* scopeIdx,MM_LoopInfo* li,
                            MM_MethodScope scope) {
    mmAbstractNode* sn=NULL;
    mmAbstractNode *n1,*n2,*n3,*n4;
    mmLinkedList* localVars=si->_localVars;
    MM_ScopeInfo* nsi=NULL;
    int currByteIndex,byteOffset,jumpBlocks,jumpBytes,jumpExtra;
    mmByte jumpByteCode;
    int whileExprByteIndex,whileStartBlockIndex;
    int forExprByteIndex,forStartBlockIndex,forPostExprStartIndex;
    int startIfByteIndex,endIfByteIndex,idx;
    MM_LoopInfo* nli=NULL;
    mmAbstractNode *fi,*fie,*fidecl,*ese;
    int prev_scope_idx=0;

    if (sl) {
        sn=sl->_children;
    }
    while (sn) {
        switch (sn->_type) {
            case mmDeclStmtNode:
                n1=sn->_children;
                n1=n1->_children;
                while (n1) {
                    if (mm_index_of_ll_node_for_string(localVars,mm_get_string_value(n1)) > -1) {
                        compile_error("%s: error at line %d: duplicate declaration for local variable %s\n",
                          n1->_sourceFile,n1->_lineno,mm_get_string_value(n1));
                        continue;
                    }
                    mm_add_ll_node(localVars,mm_get_string_value(n1));
                    if (n1->_type==mmVariableInitNode) {
                        //addSourceUnit(bc,1,n1);
                        buildExpression(n1->_children,literals,bc,cl,si,scope,block);
                        addByteCode(bc,MM_BC_POP);
                    }
                    n1=n1->_nextNode;
                }
                break;
            case mmExpressionStmtNode:
                //addSourceUnit(bc,1,sn);
                if (sn->_children) {
                    buildExpression(sn->_children,literals,bc,cl,si,scope,block);
                    if (sn->_children) {
                        addByteCode(bc,MM_BC_POP);
                    }
                }
                break;
            case mmReturnNode:
                buildExpression(sn->_children,literals,bc,cl,si,scope,block);
                addSourceUnit(bc,sn);
                addByteCode(bc,MM_BC_RETURN);
                break;
            case mmIfStmtNode:
                n1 = sn->_children;
                //addSourceUnit(bc,1,n1);
                /* generate code to evaluate the conditional expression */
                buildExpression(n1,literals,bc,cl,si,scope,block);

                /* create a new scope info context for local block variables */
                prev_scope_idx=*scopeIdx;
                nsi=newScopeInfo(mm_new_ll(),++(*scopeIdx),si);
                /* add this scope to the list of scopes for this block */
                mm_add_ll_node(scopelist,nsi);

                /* get current index of byte code array for use in calculating jump */
                currByteIndex=getByteIndex(bc);
                startIfByteIndex=getByteIndex(bc);
                addByteCode(bc,MM_BC_JUMP_FORWARD_ON_FALSE);
                addByteCode(bc,0);
                addByteCode(bc,0);

                /* now recursively call buildStatements to compile this statement block */
                n1=n1->_nextNode;
                addByteCode(bc,MM_BC_SET_SCOPE_LEVEL);
                addByteCode(bc,*scopeIdx);
                buildStatements(n1->_children,cl,nsi,literals,bc,block,scopelist,scopeIdx,li,scope);
                addByteCode(bc,MM_BC_SET_SCOPE_LEVEL);
                addByteCode(bc,prev_scope_idx);
                
                if (n1->_nextNode) {
                    addByteCode(bc,MM_BC_JUMP_FORWARD);
                    addByteCode(bc,0);
                    addByteCode(bc,0);
                }

                /*  now calculate offset to jump in the case where the boolean expression
                    is false */
                byteOffset=getByteIndex(bc) - currByteIndex - 3;
                debug("byteOffset is %d\n",byteOffset);
                storeByteCode(bc,currByteIndex+1,byteOffset/256);
                storeByteCode(bc,currByteIndex+2,byteOffset%256);
                 
                n1=n1->_nextNode;
                /* if an elsif group is present, process it */
                if (    (n1) &&
                        (n1->_type==mmElsifListNode)  ) {
                    n2=n1->_children;
                    while (n2) {
                        n3=n2->_children;
                        //addSourceUnit(bc,1,n3);
                        buildExpression(n3,literals,bc,cl,si,scope,block);
                        prev_scope_idx=*scopeIdx;
                        nsi=newScopeInfo(mm_new_ll(),++(*scopeIdx),si);
                        mm_add_ll_node(scopelist,nsi);
                        currByteIndex=getByteIndex(bc);
                        addByteCode(bc,MM_BC_JUMP_FORWARD_ON_FALSE);
                        addByteCode(bc,0);
                        addByteCode(bc,0);
                        n3=n3->_nextNode;
                        addByteCode(bc,MM_BC_SET_SCOPE_LEVEL);
                        addByteCode(bc,*scopeIdx);
                        buildStatements(n3->_children,cl,nsi,literals,bc,block,scopelist,scopeIdx,li,scope);
                        addByteCode(bc,MM_BC_SET_SCOPE_LEVEL);
                        addByteCode(bc,prev_scope_idx);
                        addByteCode(bc,MM_BC_JUMP_FORWARD);
                        addByteCode(bc,0);
                        addByteCode(bc,0);
                        byteOffset=getByteIndex(bc) - currByteIndex - 3;
                        debug("byteOffset is %d\n",byteOffset);
                        storeByteCode(bc,currByteIndex+1,byteOffset/256);
                        storeByteCode(bc,currByteIndex+2,byteOffset%256);
                        n2=n2->_nextNode;
                    }
                    n1=n1->_nextNode;
                }

                /* if an else clause is present, process it as well */
                if (n1)  {
                    prev_scope_idx=*scopeIdx;
                    nsi=newScopeInfo(mm_new_ll(),++(*scopeIdx),si);
                    mm_add_ll_node(scopelist,nsi);
                    addByteCode(bc,MM_BC_SET_SCOPE_LEVEL);
                    addByteCode(bc,*scopeIdx);
                    buildStatements(n1->_children,
                        cl,nsi,literals,bc,block,scopelist,scopeIdx,li,scope);
                    addByteCode(bc,MM_BC_SET_SCOPE_LEVEL);
                    addByteCode(bc,prev_scope_idx);
                }
                endIfByteIndex=getByteIndex(bc);

                /*  resolve unconditional jump instructions to jump to the end of
                    this if statement */
                for (idx=startIfByteIndex;idx<endIfByteIndex;idx++) {
                    if ( (getByteCode(bc,idx)==MM_BC_JUMP_FORWARD) &&
                         (getByteCode(bc,idx+1)==0) &&
                         (getByteCode(bc,idx+2)==0) ) {
                        byteOffset=endIfByteIndex - idx - 3;
                        debug("byteOffset is %d\n",byteOffset);
                        storeByteCode(bc,++idx,byteOffset/256);
                        storeByteCode(bc,++idx,byteOffset%256);
                    }
                }
                break;
            case mmWhileStmtNode:
                n1 = sn->_children;
                //addSourceUnit(bc,1,n1);
                /* get current index of byte code array for use in calculating jump */
                whileExprByteIndex=getByteIndex(bc);
                /* generate code to evaluate the conditional expression */
                buildExpression(n1,literals,bc,cl,si,scope,block);
                whileStartBlockIndex=getByteIndex(bc);
                addByteCode(bc,MM_BC_JUMP_FORWARD_ON_FALSE);
                addByteCode(bc,0);
                addByteCode(bc,0);
                /* create a new scope info context for local block variables */
                prev_scope_idx=*scopeIdx;
                nsi=newScopeInfo(mm_new_ll(),++(*scopeIdx),si);
                mm_add_ll_node(scopelist,nsi);
                nli=newLoopInfo(whileLoop,whileExprByteIndex,0,0);
                addByteCode(bc,MM_BC_SET_SCOPE_LEVEL);
                addByteCode(bc,*scopeIdx);
                buildStatements(n1->_nextNode->_children,cl,nsi,literals,bc,block,
                                scopelist,scopeIdx,nli,scope);
                addByteCode(bc,MM_BC_SET_SCOPE_LEVEL);
                addByteCode(bc,prev_scope_idx);
                byteOffset=getByteIndex(bc) - whileExprByteIndex + 3;
                debug("byteOffset is %d\n",byteOffset);
                addByteCode(bc,MM_BC_JUMP_BACKWARD);
                addByteCode(bc,byteOffset/256);
                addByteCode(bc,byteOffset%256);
                byteOffset=getByteIndex(bc) - whileStartBlockIndex - 3;
                debug("byteOffset is %d\n",byteOffset);
                storeByteCode(bc,whileStartBlockIndex+1,byteOffset/256);
                storeByteCode(bc,whileStartBlockIndex+2,byteOffset%256);
                /* resolve break statements, if any */
                for (idx=whileStartBlockIndex;idx<getByteIndex(bc);idx++) {
                    if (getByteCode(bc,idx)==255) {
                        byteOffset=getByteIndex(bc) - idx - 3;
                        storeByteCode(bc,idx,MM_BC_JUMP_FORWARD);
                        storeByteCode(bc,++idx,byteOffset/256);
                        storeByteCode(bc,++idx,byteOffset%256);
                    }
                }
                break;
            case mmForStmtNode:
                n1=sn->_children;   /* expression statement */
                n2=n1->_nextNode;   /* expression statement */
                //addSourceUnit(bc,1,n2);
                n3=n2->_nextNode;   /* optional expression - if not there this will be
                                        a CompoundStmtNode */
                if (n3->_type != mmCompoundStmtNode) {
                    n4=n3->_nextNode;
                } else {
                    n3=NULL;
                    n4=n3;
                }
                prev_scope_idx=*scopeIdx;
                nsi=newScopeInfo(mm_new_ll(),++(*scopeIdx),si);
                addByteCode(bc,MM_BC_SET_SCOPE_LEVEL);
                addByteCode(bc,*scopeIdx);
                mm_add_ll_node(scopelist,nsi);
                if (n1->_children) { 
                    fi=n1->_children;
                    while (fi) {
                        if (fi->_type==mmVariableDeclNode) {
                            fidecl=fi->_children;
                            mm_add_ll_node(nsi->_localVars,mm_get_string_value(fidecl));
                            if (fidecl->_type==mmVariableInitNode) {
                                buildExpression(fidecl->_children,literals,bc,cl,nsi,scope,block);
                                addByteCode(bc,MM_BC_POP);
                            }
                        } else {
                            buildExpression(fi,literals,bc,cl,nsi,scope,block);
                            addByteCode(bc,MM_BC_POP);
                        }
                        fi=fi->_nextNode;
                    }
                }
                forExprByteIndex=getByteIndex(bc);
                if (n2->_children) {
                    buildExpression(n2->_children,literals,bc,cl,nsi,scope,block);
                } else {
                    addByteCode(bc,MM_BC_PUSH_TRUE);
                }
                forStartBlockIndex=getByteIndex(bc);
                addByteCode(bc,MM_BC_JUMP_FORWARD_ON_FALSE);
                addByteCode(bc,0);
                addByteCode(bc,0);
                /* create a new scope info context for local block variables */
                if (n4->_children) {
                    nli=newLoopInfo(forLoop,forExprByteIndex,0,0);
                    buildStatements(n4->_children,cl,nsi,literals,bc,block,
                                    scopelist,scopeIdx,nli,scope);
                    nli->_forLoopExprStart=getByteIndex(bc);
                }
                forPostExprStartIndex=getByteIndex(bc);
                if (n3) {
                    ese=n3->_children;
                    while (ese) {
                        buildExpression(ese,literals,bc,cl,nsi,scope,block);
                        addByteCode(bc,MM_BC_POP);
                        ese=ese->_nextNode;
                    }
                }
                byteOffset=getByteIndex(bc) - forExprByteIndex + 3;
                debug("byteOffset is %d\n",byteOffset);
                addByteCode(bc,MM_BC_JUMP_BACKWARD);
                addByteCode(bc,byteOffset/256);
                addByteCode(bc,byteOffset%256);
                byteOffset=getByteIndex(bc) - forStartBlockIndex - 3;
                debug("byteOffset is %d\n",byteOffset);
                storeByteCode(bc,forStartBlockIndex+1,byteOffset/256);
                storeByteCode(bc,forStartBlockIndex+2,byteOffset%256);
                /* resolve break and continue statements, if any */
                for (idx=forStartBlockIndex;idx<getByteIndex(bc);idx++) {
                    if (getByteCode(bc,idx)==255) {
                        byteOffset=getByteIndex(bc) - idx - 3;
                        storeByteCode(bc,idx,MM_BC_JUMP_FORWARD);
                        storeByteCode(bc,++idx,byteOffset/256);
                        storeByteCode(bc,++idx,byteOffset%256);
                    }
                    
                    if (getByteCode(bc,idx)==254) {
                        byteOffset=forPostExprStartIndex - idx - 3;
                        storeByteCode(bc,idx,MM_BC_JUMP_FORWARD);
                        storeByteCode(bc,++idx,byteOffset/256);
                        storeByteCode(bc,++idx,byteOffset%256);
                    }
                }
                addByteCode(bc,MM_BC_SET_SCOPE_LEVEL);
                addByteCode(bc,prev_scope_idx);
                break;
            case mmContinueStmtNode:
                if (li==NULL) {
                    compile_error("%s: error at line %d: continue statement only valid in a loop\n",
                          sn->_sourceFile,sn->_lineno);
                }
                //addSourceUnit(bc,1,sn);
                if (li->_loopType==whileLoop) {
                    byteOffset=getByteIndex(bc) - li->_loopStart+3;
                    debug("byteOffset is %d\n",byteOffset);
                    addByteCode(bc,MM_BC_JUMP_BACKWARD);
                    addByteCode(bc,byteOffset/256);
                    addByteCode(bc,byteOffset%256);
                } else {
                    addByteCode(bc,254);
                    addByteCode(bc,0);
                    addByteCode(bc,0);
                }
                break;
            case mmBreakStmtNode:
                if (li==NULL) {
                    compile_error("%s: error at line %d: break statement only valid in a loop\n",
                      sn->_sourceFile,sn->_lineno);
                }
                //addSourceUnit(bc,1,sn);
                addByteCode(bc,255);
                addByteCode(bc,0);
                addByteCode(bc,0);
                break;
            default:
                error("Unsupported statement type: %s\n",nodeTypes[sn->_type]);
                exit(-1);
        }
        /* and on to the next statement */
        sn=sn->_nextNode;
    }

}

static void buildExpression(mmAbstractNode* e,mmLinkedList* literals,
                            mmByteCode* bc,mmObjectPtr cl,MM_ScopeInfo* si,
                            MM_MethodScope scope,mmObjectPtr block) {
    mmAbstractNode *n1,*n2,*n3,*n4,*in;
    mmLinkedListNode *node;
    int instVarIdx,localVarIdx,litIdx,varIdx;
    int intObject;
    double floatObject;
    mmByte byte;
    int numargs;
    MM_VariableType vartype;
    MM_ScopeInfo* result=NULL;
    int arraySize;
    int sendToSuper=0;
    mmObjectPtr blockClosure=mm_nil;
    char* ident;
    mmObjectPtr go=mm_nil;
    char *nsp,*la;
    mmAbstractNode* msgSendOp;

    switch (e->_type) {
        case mmAssignmentNode:
            n1=e->_children;
            n2=n1->_nextNode;
            buildExpression(n2,literals,bc,cl,si,scope,block);
            /* search through local and instance variables for symbol */ 
            varIdx=index_of_variable(scope,cl,si,n1->_payload._sVal,&vartype,&result);

            if (varIdx==-1) {
                compile_error("%s: error at line %d: Undefined symbol: %s - maybe attempting to reference a "
                    "non-existant instance,class, or local variable?\n",
                    n1->_sourceFile,n1->_lineno,n1->_payload._sVal);
                break;
            }
            addSourceUnit(bc,e);
            if (vartype==LocalVar) {
                addByteCode(bc,MM_BC_LOCAL_VAR_STORE);
                addByteCode(bc,result->_scopeIdx);
                addByteCode(bc,varIdx);
            } else if (vartype==InstanceVar)  {
                addByteCode(bc,MM_BC_INST_VAR_STORE);
                addByteCode(bc,varIdx);
            } else if (vartype==ClassInstanceVar) {
                if (scope==InstanceSide) {
                    addByteCode(bc,MM_BC_CL_INST_VAR_STORE);
                    addByteCode(bc,varIdx+MM_CLASS_SIZE);
                } else {
                    addByteCode(bc,MM_BC_INST_VAR_STORE);
                    addByteCode(bc,varIdx+MM_CLASS_SIZE);
                }
            }
            break;
        case mmSpecialLiteralNode:
            if (    (strcmp(e->_payload._sVal,"self")==0) ||
                    (strcmp(e->_payload._sVal,"super")==0)  ) {
                addByteCode(bc,MM_BC_PUSH_SELF);
            } else if (strcmp(e->_payload._sVal,"nil")==0) {
                addByteCode(bc,MM_BC_PUSH_NIL);
            } else if (strcmp(e->_payload._sVal,"true")==0) {
                addByteCode(bc,MM_BC_PUSH_TRUE);
            } else if (strcmp(e->_payload._sVal,"false")==0) {
                addByteCode(bc,MM_BC_PUSH_FALSE);
            } else if (strcmp(e->_payload._sVal,"thisContext")==0) {
                addByteCode(bc,MM_BC_PUSH_THIS_CONTEXT);
            }
            break;
        case mmIdentifierNode:
            /*  attempt to resolve identifier.  an indentifier is resolved according to 
                the following precedence:
                
                1) local variable
                2) instance variable
                3) class instance variable (if method on instance side)
                4) global symbol (typically a class object)      
            */
            ident=strdup(e->_payload._sVal);
            varIdx=index_of_variable(scope,cl,si,ident,&vartype,&result);
            if (varIdx>=0) {
                if (vartype==LocalVar) {
                    addByteCode(bc,MM_BC_LOCAL_VAR_FETCH);
                    addByteCode(bc,result->_scopeIdx);
                    addByteCode(bc,varIdx);
                } else if (vartype==InstanceVar) {
                    addByteCode(bc,MM_BC_INST_VAR_FETCH);
                    addByteCode(bc,varIdx);
                } else if (vartype==ClassInstanceVar) {
                    if (scope==InstanceSide) {
                        addByteCode(bc,MM_BC_CL_INST_VAR_FETCH);
                        addByteCode(bc,varIdx+MM_CLASS_SIZE);
                    } else {
                        addByteCode(bc,MM_BC_INST_VAR_FETCH);
                        addByteCode(bc,varIdx+MM_CLASS_SIZE);
                    }
                }
            /*  otherwise this identifier is assumed to be a global identifier.  It may either
                be a fully qualified global identifier (i.e. Core::Object) or an abridged name,
                in which case it may be either at the top level namespace or have been imported 
                into the pool dictionary for this class (i.e. Object).  */  
            } else {
                go=resolveGlobalIdent(e,mm_get_slot(cl,MM_CLASS_POOL),
                        mm_get_string(mm_get_slot(cl,MM_CLASS_NAMESPACE)));
                if (go==mm_nil) {
                    compile_error("%s: error at line %d: Undefined global identifier: %s - maybe attempting to reference a "
                        "global identifier which has not been imported?\n",
                        e->_sourceFile,e->_lineno,ident);
                }
                litIdx=addOrFindLiteral(literals,NULL,go);
                addByteCode(bc,MM_BC_LITERAL_FETCH);        
                addByteCode(bc,litIdx);
            }
            //error("%s: error at line %d: Undefined symbol: %s - maybe attempting to reference a "
            //    "non-existant global,instance,class, or local variable?\n",
            //    e->_sourceFile,e->_lineno,e->_payload._sVal);
            free(ident);
            break;
        case mmIntegerNode:
        case mmFloatNode:
        case mmStringNode:
        case mmCharacterNode:
            litIdx=addOrFindLiteral(literals,e,mm_nil);
            addByteCode(bc,MM_BC_LITERAL_FETCH);        
            addByteCode(bc,litIdx);
            break;
        case mmArrayNode:
            arraySize=0;
            if (e->_children) {
                n1=e->_children;
                n2=n1->_children;
                while (n2) {
                    buildExpression(n2,literals,bc,cl,si,scope,block);
                    arraySize++;
                    n2=n2->_nextNode;
                }
            }
            addByteCode(bc,MM_BC_ARRAY_LITERAL);
            addByteCode(bc,arraySize); 
            break;
        case mmBlockNode:
            blockClosure=buildBlockClosure(e,cl,si,scope,mm_get_slot(block,MM_BLOCK_PARENT));
            mm_add_ll_node(literals,(void*)blockClosure);
            addByteCode(bc,MM_BC_BLOCK_CLOSURE);
            addByteCode(bc,literals->_size-1);
            break;
        case mmMessageSendExprNode:
            n1=e->_children;
            if (    (n1->_type==mmSpecialLiteralNode) &&
                    (strcmp("super",mm_get_string_value(n1))==0) ) {
                sendToSuper=1;
            }
            buildExpression(n1,literals,bc,cl,si,scope,block);
            
            
            /* the message send chain-can be multiple messages changed together */
            n2=n1->_nextNode;
            n2=n2->_children;

            /*  iterate through each message send on the chain, processing from
                left to right */
            while (n2) {
                /* check if any arguments provided for this message.  if so, process them */
                numargs=0;
                msgSendOp=n2->_children;
                n3=msgSendOp->_nextNode;

                if (n3) {
                    n3=n3->_children;
                    while (n3) {
                        buildExpression(n3,literals,bc,cl,si,scope,block);
                        n3=n3->_nextNode;
                        numargs++;
                    }
                }

                /*  lookup the selector for this message in the literal array.  if not there, add it */
                litIdx=addOrFindLiteral(literals,n2,mm_nil);
                addSourceUnit(bc,msgSendOp);
                if (sendToSuper) {
                    addByteCode(bc,MM_BC_MSG_SEND_SUPER);
                    sendToSuper=0;
                } else {
                    addByteCode(bc,MM_BC_MSG_SEND);
                }
                addByteCode(bc,numargs);
                addByteCode(bc,litIdx);
                n2=n2->_nextNode;
            }
            debug("finished compiling message send\n");
            break;
        case mmBinaryMessageSendNode:
            n1=e->_children;
            if (    (n1->_type==mmSpecialLiteralNode) &&
                    (strcmp("super",mm_get_string_value(n1))==0) ) {
                sendToSuper=1;
            }
            buildExpression(n1,literals,bc,cl,si,scope,block);
            buildExpression(n1->_nextNode,literals,bc,cl,si,scope,block);
            litIdx=addOrFindLiteral(literals,e,mm_nil);
            addSourceUnit(bc,e);
            if (sendToSuper) {
                addByteCode(bc,MM_BC_MSG_SEND_SUPER);
                sendToSuper=0;
            } else {
                addByteCode(bc,MM_BC_MSG_SEND);
            }
            addByteCode(bc,1);
            addByteCode(bc,litIdx);
            break;
        default:
            error("Unsupported expression type: %s\n",nodeTypes[e->_type]);
            exit(-1);
    }
}

mmLinkedList* mm_get_classpath() {
    static mmLinkedList* mmpath=NULL;
    char *path,*tok;

    if (!mmpath) {
        mmpath=mm_new_ll();
        //mm_add_ll_node(mmpath,strdup(MM_DEFAULT_CLASS_PATH));
    	if (mm_classPath) {
    		path = mm_classPath;
    	} else {
            	path=getenv("MM_PATH");
    	}
        if (path) {
            path=strdup(path);
            tok=strtok(path,MM_PATH_SEPARATOR);
            while (tok) {
                mm_add_ll_node(mmpath,tok);
                tok=strtok(NULL,MM_PATH_SEPARATOR);
            }
        }
    }
    return mmpath;
}

static char* findClassFile(char* cn) {
    char* buff=strdup(cn);
    char* tok;
    mmLinkedList* mmpath=NULL; 
    mmLinkedListNode* pathcomp=NULL;
    FILE* rv=NULL;
    char filename[512];
    char relname[512];
    struct stat stat_buf;


    /* set up the relative filename for this class */
    bzero(filename,sizeof(filename));
    bzero(relname,sizeof(relname));
    tok=strtok(buff,"::");
    while (tok) {
        strcat(relname,MM_DIRECTORY_SEPARATOR);
        strcat(relname,tok);
        tok=strtok(NULL,"::");
    }
    strcat(relname,".mm");

    /* now first check the current directory for a matching filename */
    strcat(filename,".");
    strcat(filename,relname);
    if (!stat(filename,&stat_buf)) {
        return strdup(filename);
    } else {
        /* otherwise check the components of MM_PATH in order */
        mmpath=mm_get_classpath();
        pathcomp=mmpath->_head;
        while (pathcomp) {
            bzero(filename,sizeof(filename));
            strcat(filename,(char*)pathcomp->_data);
            strcat(filename,relname);
            if (!stat(filename,&stat_buf)) {
                return strdup(filename);
            }
            pathcomp=pathcomp->_next;
        }
    }

    return NULL;
}


static char* baseClassname(char* cn) {
    char* basecn=NULL;

    basecn=strrchr(cn,':');
    if (basecn) {
        ++basecn;
    } else {
        basecn=cn;
    }
    return(basecn);
}

static mmObjectPtr importClass(mmAbstractNode* import,mmObjectPtr dict) {
    mmObjectPtr cl=mm_nil;
    char* key;
    char *cn=NULL;

    cn=import->_payload._sVal;
    debug("Importing class %s\n",cn);
    cl=mm_load_class(cn,import,mm_nil);
    key=baseClassname(cn);
    debug("Adding entry %s to pool dictionary\n",key);
    mm_dict_put(dict,key,cl);
    return(cl); 
}

mmObjectPtr mm_load_class(char* cn,mmAbstractNode* n,mmObjectPtr proto_class) {
    FILE* infile=NULL;
    char* filename;
    mmObjectPtr co=mm_nil;


    co=lookupGlobalIdent(cn);
    debug("checking if class %s, ptr %d needs loading\n",cn,co);
    if (    (co==mm_nil) || (co->_size < 0) ) {
        if (co->_size < 0) {
            proto_class=co;
        }
        debug("class %s needs loading\n",cn);
        filename=findClassFile(cn);
        if (    (!filename) && (n)  ) {
            error("%s: error at line %d: Could not locate class file for class %s\n",
                n->_sourceFile,n->_lineno,cn);
        } else if (!filename) {
            error("Could not locate class file for class %s\n",cn);
        }
        yyin=fopen(filename,"r");
        if (    (!yyin) && (n)    ) {                                     
            error("%s: error at line %d: Error opening class file %s for class %s\n",
                n->_sourceFile,n->_lineno,filename,cn);
        } else if (!yyin) {
            error("Error opening class file %s for class %s\n",filename,cn);
        }
        lineno=1;
        sourceFile=filename;
        yyparse();
        fclose(yyin);
        yyin=NULL;


        /* process the parse tree for this class */
        co=mm_compile_class(program,filename,proto_class);
        debug("class %s finished loading\n",cn);
        free(filename);
    } else {
        debug("Class %s has already been loaded.\n",cn);
    }
    return(co); 
}

MM_ScopeInfo* scope_info_from_context(mmObjectPtr context) {
    int scope_level=mm_get_integer(mm_get_slot(context,MM_BLOCK_CONTEXT_SCOPE_LEVEL));
    MM_ScopeInfo* si;
    mmObjectPtr scope_template_array;
    mmObjectPtr scope_template;
    mmLinkedList* scope_list=mm_new_ll();
    int i;
    mmLinkedListNode* node;

    scope_template_array=mm_get_slot(mm_get_slot(context,MM_BLOCK_CONTEXT_BLOCK),
                                     MM_BLOCK_NESTED_SCOPES_TEMPLATE);
    scope_template=mm_get_slot(scope_template_array,scope_level);
    while (scope_template != mm_nil) {
        si=newScopeInfo(mm_new_ll(),scope_level,NULL);
        mm_add_ll_node(scope_list,si);
        for (i=1;i<scope_template->_size;i++) {
            mm_add_ll_node(si->_localVars,mm_get_string(mm_get_slot(scope_template,i)));
        }
        if (mm_get_slot(scope_template,0)==mm_nil) {
            break;
        } else {
            scope_level=mm_get_integer(mm_get_slot(scope_template,0));
            scope_template = mm_get_slot(scope_template_array,mm_get_integer(mm_get_slot(scope_template,0)));
        }
    }
    node=scope_list->_head;
    while (node) {
        si=node->_data;
        si->_parentInfo=(node->_next) ? node->_next->_data : NULL;
        node=node->_next;
    }
    return scope_list->_head->_data;
}

mmObjectPtr mm_compile_anon_block(mmObjectPtr context,char* code) {
    mmObjectPtr block=mm_nil;
    mmObjectPtr rcvr=mm_nil;
    mmObjectPtr cl=mm_nil;
    MM_ScopeInfo* si;
    mmObjectPtr new_context=mm_nil;
    MM_MethodScope scope;
    mmObjectPtr method;

    rcvr=mm_get_slot(context,MM_BLOCK_CONTEXT_RCVR);
    if (rcvr->_class->_class==mm_metaclass_class) {
        scope=ClassSide;
        cl=rcvr;
    } else {
        scope=InstanceSide;
        cl=rcvr->_class;
    }

    // find method
    method=mm_get_slot(context,MM_BLOCK_CONTEXT_BLOCK);
    while (method->_class != mm_method_class) {
        method=mm_get_slot(method,MM_BLOCK_PARENT);
    }
    sourceFile=mm_get_string(mm_get_slot(method,MM_METHOD_SOURCE_FILE));

    lineno=1;
    setup_parse_buffer(code);
    yyparse();
    free_parse_buffer();
    si=scope_info_from_context(context);
    block=buildBlockClosure(program,cl,si,scope,
                                     mm_get_slot(context,MM_BLOCK_CONTEXT_BLOCK)); 
    new_context = mm_new_block_context(block,mm_nil,mm_get_slot(context,MM_BLOCK_CONTEXT_RCVR));
    mm_copy_scope(context,new_context);
    return new_context;
}

