#ifndef _MM_TREE_H_
#define _MM_TREE_H_

#include "linklst.h"

typedef enum {
    mmClassNode,
    mmProgramNode,
    mmClassContent,
    mmInstanceNode,
    mmMethodNode,
    mmBinaryParmNode,
    mmBinaryNode,
    mmParmListNode,
    mmAttributeNode,
    mmAttributeListNode,
    mmIdentifierNode,
    mmFloatNode,
    mmIntegerNode,
    mmAssignmentNode,
    mmBinaryMessageNode,
    mmBinaryMessageSendNode,
    mmMessageSendExprNode,
    mmMessageSendNode,
    mmVariableNode,
    mmVariableDeclNode,
    mmStatementListNode,
    mmExpressionStmtNode,
    mmDeclStmtNode,
    mmCompoundStmtNode,
    mmReturnNode,
    mmIfStmtNode,
    mmImportNode,
    mmImportListNode,
    mmStringNode,
    mmMessageChainNode,
    mmSpecialLiteralNode,
    mmElsifNode,
    mmElsifListNode,
    mmWhileStmtNode,
    mmForStmtNode,
    mmClassModifierNode,
    mmVariableInitNode,
    mmCharacterNode,
    mmArrayNode,
    mmExpressionListNode,
    mmContinueStmtNode,
    mmBreakStmtNode,
    mmForInitializerListNode,
    mmBlockNode,
    mmPrimitiveNode,
    mmNamespaceDeclNode,
    mmMessageSendOpNode
} mmNodeType;

typedef enum {
    mmStringType,
    mmIntegerType,
    mmDoubleType
} mmNodeValueType;

extern char* nodeTypes[];

/* parse tree data structures */
typedef struct mm_abstract_node {
    mmNodeType _type;
    int _childCount;
    mmNodeValueType _valueType;
    union {
        int _iVal;
        double _dVal;
        char* _sVal;
    } _payload;
    struct mm_abstract_node* _children;
    struct mm_abstract_node* _nextNode;
    struct mm_abstract_node* _lastNode;
    char* _sourceFile;
    int _lineno;
    long _filepos;
    long _line_start_pos;
} mmAbstractNode;

/* parse tree interface */
extern mmAbstractNode* mm_new_node(int type,mmAbstractNode* c1,mmAbstractNode* c2);
extern void mm_add_child(mmAbstractNode* parent,mmAbstractNode* child);
extern void mm_print_tree(mmAbstractNode* root,int indentLevel);
extern void mm_set_string_value(mmAbstractNode* n,char* value);
extern void mm_set_integer_value(mmAbstractNode* n,int value);
extern void mm_set_double_value(mmAbstractNode* n,double value);
extern char* mm_get_string_value(mmAbstractNode* n);
extern int mm_get_integer_value(mmAbstractNode* n);
extern double mm_get_double_value(mmAbstractNode* n);
extern mmAbstractNode* mm_find_child_of_type(mmAbstractNode* parent,mmNodeType type);
extern mmLinkedList* mm_find_children_of_type(mmAbstractNode* parent,mmNodeType type);
      
      
#endif
