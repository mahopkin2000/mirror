#include "tree.h"
#include "stdio.h"
#include "string.h"

extern int lineno;
extern char* sourceFile;
extern FILE* yyin;
extern long getInputPos();
extern long last_newline_pos;


char* nodeTypes[] = {
    "mmClassNode",
    "mmProgramNode",
    "mmClassContent",
    "mmInstanceNode",
    "mmMethodNode",
    "mmBinaryParmNode",
    "mmBinaryNode",
    "mmParmListNode",
    "mmAttributeNode",
    "mmAttributeListNode",
    "mmIdentifierNode",
    "mmFloatNode",
    "mmIntegerNode",
    "mmAssignmentNode",
    "mmBinaryMessageNode",
    "mmBinaryMessageSendNode",
    "mmMessageSendExprNode",
    "mmMessageSendNode",
    "mmVariableNode",
    "mmVariableDeclNode",
    "mmStatementListNode",
    "mmExpressionStmtNode",
    "mmDeclStmtNode",
    "mmCompoundStmtNode",
    "mmReturnNode",
    "mmIfStmtNode",
    "mmImportNode",
    "mmImportListNode",
    "mmStringNode",
    "mmMessageChainNode",
    "mmSpecialLiteralNode",
    "mmElsifNode",
    "mmElsifListNode",
    "mmWhileStmtNode",
    "mmForStmtNode",
    "mmClassModifierNode",
    "mmVariableInitNode",
    "mmCharacterNode",
    "mmArrayNode",
    "mmExpressionListNode",
    "mmContinueStmtNode",
    "mmBreakStmtNode",
    "mmForInitializerListNode",
    "mmBlockNode",
    "mmPrimitiveNode",
    "mmNamespaceDeclNode",
    "mmMessageSendOpNode"
};

/* parse tree interface */
mmAbstractNode* mm_new_node(int type,mmAbstractNode* c1,mmAbstractNode* c2)
{
    mmAbstractNode* nn = (mmAbstractNode*)malloc(sizeof(mmAbstractNode));
    long pos;
    nn->_type = type;
    nn->_children = NULL;
    nn->_nextNode = NULL;
    nn->_lastNode = NULL;
    nn->_childCount = 0;
    nn->_valueType = 0;
    nn->_payload._sVal = NULL;
    nn->_lineno=lineno;
    nn->_sourceFile=sourceFile;
    nn->_filepos=getInputPos();
    nn->_line_start_pos=last_newline_pos;
    if (c1) {
        mm_add_child(nn,c1);
    }
    if (c2) {
        mm_add_child(nn,c2);
    }
    return nn;
}

void mm_add_child(mmAbstractNode* parent,mmAbstractNode* child)
{
    mmAbstractNode* ch = parent->_children;
    if (child) {
        if (ch) {
            ch->_lastNode->_nextNode = child;
        }
        else {
            parent->_children = child;
        }
        parent->_children->_lastNode = child;
        ++parent->_childCount;
    }
}

void mm_print_tree(mmAbstractNode* root,int indentLevel)
{
    mmAbstractNode* child;
    int idx;
    int childCount=0;
    char indent[50];
    char* strval;
    strcpy(indent,"");
    for (idx=0;idx < indentLevel;idx++) {
        strcat(indent," ");
    }
    switch (root->_valueType) {
        case mmStringType:           
            strval = (root->_payload._sVal) ? root->_payload._sVal : "";
            printf("%sType=%s,Payload=%s,Num. Children=%d\n",
                   indent,nodeTypes[root->_type],
                   strval,root->_childCount);
            break;
        case mmIntegerType:
            printf("%sType=%s,Payload=%d,Num. Children=%d\n",
                   indent,nodeTypes[root->_type],
                   root->_payload._sVal,root->_childCount);
            break;
        case mmDoubleType:
            printf("%sType=%s,Payload=%f,Num. Children=%d\n",
                   indent,nodeTypes[root->_type],
                   root->_payload._sVal,root->_childCount);
            break;
        default:
            fprintf(stderr,"Unsupported value type '%d': aborting...\n",
                    root->_valueType);
            exit(-1);
    }
    child = root->_children;
    while (child) {
        mm_print_tree(child,indentLevel+1);
        child = child->_nextNode;
    }
}      

void mm_set_string_value(mmAbstractNode* n,char* value)
{
    n->_payload._sVal = strdup(value);
    n->_valueType = mmStringType;
}

void mm_set_integer_value(mmAbstractNode* n,int value)
{
    n->_payload._iVal = value;
    n->_valueType = mmIntegerType;
}

void mm_set_double_value(mmAbstractNode* n,double value)
{
    n->_payload._dVal = value;
    n->_valueType = mmDoubleType;
}

char* mm_get_string_value(mmAbstractNode* n) {
    return n->_payload._sVal;
}

int mm_get_integer_value(mmAbstractNode* n) {
    return n->_payload._iVal;
}

double mm_get_double_value(mmAbstractNode* n) {
    return n->_payload._dVal;
}

mmAbstractNode* mm_find_child_of_type(mmAbstractNode* parent,mmNodeType type) {
    mmAbstractNode* n=parent->_children;
    mmAbstractNode* rv=NULL;
    while (n) {
        if (n->_type==type) {
            rv=n;
            break;
        }
        n=n->_nextNode;
    }
    return rv;
}

mmLinkedList* mm_find_children_of_type(mmAbstractNode* parent,mmNodeType type) {
    mmLinkedList* ll=mm_new_ll();
    mmAbstractNode* n=parent->_children;
    while (n) {
        if (n->_type==type) {
            mm_add_ll_node(ll,n);
        }
        n=n->_nextNode;
    }
    return ll;
}
