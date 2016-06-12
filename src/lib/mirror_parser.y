/*

 mirror.y
 
 Grammar for the Mirror programming language

*/

%{
#include "tree.h" 
#include "string.h"
#include "stdio.h"
#include "linklst.h"
#include "mirror.h"

#define MM_CLASS_SIDE      0
#define MM_INSTANCE_SIDE   1

mmAbstractNode *program,*tn,*tn1; 
mmLinkedList *instVars,*classVars;
extern long lineno;
extern char* yytext;
char* sourceFile;
extern char linebuf[1000];
unsigned short scope=MM_INSTANCE_SIDE;
extern long last_message_send_pos;
extern long last_newline_pos;

int yyerror(char* s)
{
        compile_error("%s: %s at line %d: token is [%s]\n%s\n",
                      sourceFile,s,lineno,yytext,linebuf);
}     
 
%}

/* definition of YYSTYPE */
%union{
  char		cval;
  double	fval;
  long		ival;
  char		*sval;
  mmAbstractNode*	node;
}

/* single definite characters */     
%token CLASS ATTRIBUTE METHOD INSTANCE_SIDE CLASS_SIDE
%token RETURN VAR ASSIGNMENT IF ELSE IMPORT
%token PRIMITIVE MESSAGE_SEND NIL TRUE FALSE SELF SUPER
%token ELSIF WHILE FOR INDEXED BYTE BREAK CONTINUE
%token NAMESPACE THIS_CONTEXT

/* larger lexical items */
%token <sval> BINARY_MESSAGE IDENT STRING CHAR
%token <ival> INTEGER
%token <fval> FLOAT

%type <node> program classdef class_content class_qualifier class_element 
        method attribute parm_list attribute_list identifier number expression
        assignment_expression binary_message_expression message_send_expression
        message_send variable_decl_expression statement_list
        statement expression_statement declaration_statement compound_statement
        return_statement if_else_statement super_class class_import_list 
        class_import string message_chain special_literal
        elsif_clause elsif_list while_statement for_statement class_modifier
        variable_decl character array_literal primary_expression expression_list
        continue_statement break_statement for_initializer for_initializer_list
        for_initializer_statement block method_modifier namespace_decl 
        message_send_op
        

%left ASSIGNMENT
%left BINARY_MESSAGE '-' '|'
%left BINARY_MESSAGE_EXPR
%left MESSAGE_SEND_EXPR
%left MESSAGE_SEND
%nonassoc UMINUS
%nonassoc BLOCK_PARM
%nonassoc IDENT FLOAT INTEGER

%%
program:        namespace_decl class_import_list classdef {
                        program = mm_new_node(mmProgramNode,$1,$2);
                        mm_add_child(program,$3);
                        $$ = program;
                }
         |      block {
                        program = $1;
                        $$ = $1;      
         }
;

namespace_decl: /* empty */     {
                                $$=NULL;
                        instVars=mm_new_ll();
                        classVars=mm_new_ll();
                                }
        |       NAMESPACE IDENT ';' {
                        tn=mm_new_node(mmNamespaceDeclNode,NULL,NULL);
                        mm_set_string_value(tn,$2);
                        instVars=mm_new_ll();
                        classVars=mm_new_ll();
                        $$=tn;
                }
        |       error ';' {
           $$=NULL;
        }
                              
;  

class_import:   IMPORT IDENT ';' {
                                tn = mm_new_node(mmImportNode,NULL,NULL);
                                mm_set_string_value(tn,$2);
                                $$ = tn;
                                }
        |       error ';' {
           $$=NULL;
        }
;

class_import_list:              {
                                $$ = mm_new_node(mmImportListNode,NULL,NULL);
                                }      
        |       class_import_list class_import  {
                                mm_add_child($1,$2);
                                $$=$1;
                                }
;

classdef:      class_modifier CLASS IDENT '{' class_content '}'     {
                                if ($1) {
                                        tn=mm_new_node(mmClassNode,$1,$5);
                                } else {
                                        tn = mm_new_node(mmClassNode,$5,NULL);
                                }
                                mm_set_string_value(tn,$3);
                                $$ = tn;
                                }
        |      class_modifier CLASS IDENT ':' super_class '{' class_content '}'     {
                                if ($1) {
                                        tn=mm_new_node(mmClassNode,$1,$5);
                                        mm_add_child(tn,$7);
                                } else {
                                        tn = mm_new_node(mmClassNode,$5,$7);
                                }
                                mm_set_string_value(tn,$3);
                                $$ = tn;
                                }
;

class_modifier:         /* empty */        { $$=NULL; }         
                |       BYTE    {
                                tn=mm_new_node(mmClassModifierNode,NULL,NULL);
                                mm_set_string_value(tn,yytext);
                                $$=tn;
                                }
                |       INDEXED {
                                tn=mm_new_node(mmClassModifierNode,NULL,NULL);
                                mm_set_string_value(tn,yytext);
                                $$=tn;
                                }
;

super_class:    identifier {
                        $$ = $1;
                        }
;                        

class_content:  {
                        $$ = mm_new_node(mmClassContent,NULL,NULL);
                }
        |       class_content class_element     {
                        mm_add_child($1,$2);
                        $$ = $1;
                }
;

class_element:  class_qualifier
        |       method
        |       attribute       {
                        $$ = $1;
                }
;

class_qualifier:        INSTANCE_SIDE   {
                                scope=MM_INSTANCE_SIDE;
                                $$ = mm_new_node(mmInstanceNode,NULL,NULL);
                        }
   
        |               CLASS_SIDE      {
                                scope=MM_CLASS_SIDE;
                                $$ = mm_new_node(mmClassNode,NULL,NULL);
                        }
;

method_modifier:        /* empty */     {
                                $$=NULL;
                        }
        |               PRIMITIVE       {
                                $$ = mm_new_node(mmPrimitiveNode,NULL,NULL);
                        }
;

method:         method_modifier METHOD IDENT '(' ')' compound_statement {
                        tn = mm_new_node(mmMethodNode,NULL,NULL);
                        if ($1 != NULL) {
                                mm_add_child(tn,$1);
                        }
                        mm_add_child(tn,$6);
                        mm_set_string_value(tn,$3);
                        $$ = tn;
                }
        |       method_modifier METHOD IDENT '(' parm_list ')' compound_statement {
                        tn = mm_new_node(mmMethodNode,NULL,NULL);
                        if ($1 != NULL) {
                                mm_add_child(tn,$1);
                        }
                        mm_add_child(tn,$5);
                        mm_add_child(tn,$7);
                        mm_set_string_value(tn,$3);
                        $$ = tn;
                }
        |       method_modifier METHOD BINARY_MESSAGE IDENT compound_statement {
                        tn = mm_new_node(mmBinaryParmNode,NULL,NULL);
                        mm_set_string_value(tn,$4);
                        tn1 = mm_new_node(mmBinaryNode,NULL,NULL);
                        if ($1 != NULL) {
                                mm_add_child(tn1,$1);
                        }
                        mm_add_child(tn1,tn);
                        mm_add_child(tn1,$5);
                        mm_set_string_value(tn1,$3);
                        $$ = tn1;
                }
        |       method_modifier METHOD '-' IDENT compound_statement {
                        tn = mm_new_node(mmBinaryParmNode,NULL,NULL);
                        mm_set_string_value(tn,$4);
                        tn1 = mm_new_node(mmBinaryNode,NULL,NULL);
                        if ($1 != NULL) {
                                mm_add_child(tn1,$1);
                        }
                        mm_add_child(tn1,tn);
                        mm_add_child(tn1,$5);
                        mm_set_string_value(tn1,"-");
                        $$ = tn1;
                }
        |       method_modifier METHOD '|' IDENT compound_statement {
                        tn = mm_new_node(mmBinaryParmNode,NULL,NULL);
                        mm_set_string_value(tn,$4);
                        tn1 = mm_new_node(mmBinaryNode,NULL,NULL);
                        if ($1 != NULL) {
                                mm_add_child(tn1,$1);
                        }
                        mm_add_child(tn1,tn);
                        mm_add_child(tn1,$5);
                        mm_set_string_value(tn1,"|");
                        $$ = tn1;
                }
        |         error '}'      {
           $$=NULL;
        }
;

parm_list:      identifier        {
                        $$ = mm_new_node(mmParmListNode,$1,NULL);
                }
        |       parm_list ',' identifier  {
                        mm_add_child($1,$3);
                        $$ = $1;
                }
;

attribute:      ATTRIBUTE attribute_list ';'    {
                        $$ = mm_new_node(mmAttributeNode,$2,NULL); 
                        }
;

attribute_list: identifier    {
                        $$ = mm_new_node(mmAttributeListNode,$1,NULL);
                        mm_add_ll_node(((scope==MM_INSTANCE_SIDE) ? instVars : classVars),$1);
                }
        |       attribute_list ',' identifier    {
                        mm_add_child($1,$3);
                        mm_add_ll_node(((scope==MM_INSTANCE_SIDE) ? instVars : classVars),$3);
                        $$ = $1;
                }
;

identifier:     IDENT        {
                        tn = mm_new_node(mmIdentifierNode,NULL,NULL);
                        mm_set_string_value(tn,$1);
                        $$ = tn;
                }
;

string:         STRING  {
                        tn = mm_new_node(mmStringNode,NULL,NULL);
                        mm_set_string_value(tn,$1);
                        $$ = tn;
                        }
;

number:         FLOAT   {
                                tn = mm_new_node(mmFloatNode,NULL,NULL);
                                mm_set_string_value(tn,yytext);
                                $$ = tn;
                        } 
        |       INTEGER {
                                tn = mm_new_node(mmIntegerNode,NULL,NULL);
                                mm_set_string_value(tn,yytext);
                                $$ = tn;
                        }
;

special_literal:        NIL     {
                                tn = mm_new_node(mmSpecialLiteralNode,NULL,NULL);
                                mm_set_string_value(tn,yytext);
                                $$ = tn;
                                }
        |               TRUE    {
                                tn = mm_new_node(mmSpecialLiteralNode,NULL,NULL);
                                mm_set_string_value(tn,yytext);
                                $$ = tn;
                                }
        |               FALSE    {
                                tn = mm_new_node(mmSpecialLiteralNode,NULL,NULL);
                                mm_set_string_value(tn,yytext);
                                $$ = tn;
                                }
        |               SUPER    {
                                tn = mm_new_node(mmSpecialLiteralNode,NULL,NULL);
                                mm_set_string_value(tn,yytext);
                                $$ = tn;
                                }
        |               SELF    {
                                tn = mm_new_node(mmSpecialLiteralNode,NULL,NULL);
                                mm_set_string_value(tn,yytext);
                                $$ = tn;
                                }
        |               THIS_CONTEXT    {
                                tn = mm_new_node(mmSpecialLiteralNode,NULL,NULL);
                                mm_set_string_value(tn,yytext);
                                $$ = tn;
                                }
;

character:         CHAR  {
                        tn = mm_new_node(mmCharacterNode,NULL,NULL);
                        mm_set_string_value(tn,$1);
                        $$ = tn;
                        }
;

array_literal:          '[' ']' {
                                $$=mm_new_node(mmArrayNode,NULL,NULL);
                        }
        |               '[' expression_list ']' {
                                $$=mm_new_node(mmArrayNode,$2,NULL);
                        }
;

block:                  '{' parm_list '|' statement_list '}' %prec BLOCK_PARM {
                                $$=mm_new_node(mmBlockNode,$2,$4);
                        }
                |       '{' statement_list '}' {
                                $$=mm_new_node(mmBlockNode,$2,NULL);
                        }
; 

primary_expression:     identifier
        |               string
        |               number
        |               '-' number %prec UMINUS {
                            char buff[256];
                            bzero(buff,sizeof(buff));
                            strcat(buff,"-");
                            strcat(buff,mm_get_string_value($2));
                            tn=mm_new_node($2->_type,NULL,NULL);
                            mm_set_string_value(tn,buff);
                            $$=tn;
                        }
        |               character
        |               array_literal
        |               block
        |               special_literal {
                                $$=$1;
                        }
        |               '(' expression ')' {
                                $$=$2;
                        }
;

expression:     primary_expression
        |       message_send_expression
        |       binary_message_expression 
        |       assignment_expression {
                        $$ = $1;
                }
;

expression_list:        expression {
                                $$=mm_new_node(mmExpressionListNode,$1,NULL);
                        }
        |               expression_list ',' expression {
                                mm_add_child($1,$3);
                                $$=$1;
                        }
;

assignment_expression:     identifier ASSIGNMENT expression     {
                        $$ = mm_new_node(
                                mmAssignmentNode,$1,$3);
                        }
; 



binary_message_expression:      expression BINARY_MESSAGE expression %prec BINARY_MESSAGE_EXPR {
                        tn = mm_new_node(mmBinaryMessageSendNode,$1,$3);
                        tn->_filepos=last_message_send_pos;
                        mm_set_string_value(tn,$2);
                        $$ = tn;
                        }
        |               expression '-' expression %prec BINARY_MESSAGE_EXPR {
                                tn = mm_new_node(mmBinaryMessageSendNode,$1,$3);
                                 tn->_filepos=last_message_send_pos;
                                mm_set_string_value(tn,"-");
                                $$ = tn;
                        }
        |               expression '|' expression %prec BINARY_MESSAGE_EXPR {
                                tn = mm_new_node(mmBinaryMessageSendNode,$1,$3);
                                 tn->_filepos=last_message_send_pos;
                                mm_set_string_value(tn,"|");
                                $$ = tn;
                        }
                        
;

message_send_expression:        expression message_chain %prec MESSAGE_SEND_EXPR    {
                                $$ = mm_new_node(mmMessageSendExprNode,$1,$2);
                                }
;

message_chain:  message_send    {
                                $$ = mm_new_node(mmMessageChainNode,$1,NULL);
                                } 
        |       message_chain message_send         {
                                mm_add_child($1,$2);
                                $$=$1;
                                }
;   

message_send_op:  MESSAGE_SEND   {
                           $$=mm_new_node(mmMessageSendOpNode,NULL,NULL);
                        }
;

message_send:   message_send_op IDENT '(' ')'        {
                        tn = mm_new_node(mmMessageSendNode,$1,NULL);
                        mm_set_string_value(tn,$2);
                        $$ = tn;
                        
                }
        |       message_send_op IDENT '(' expression_list ')'      {
                        tn = mm_new_node(mmMessageSendNode,$1,$4);
                        mm_set_string_value(tn,$2);
                        $$ = tn;
                }
;

variable_decl:                  IDENT {
                                        tn=mm_new_node(mmVariableNode,NULL,NULL);
                                        mm_set_string_value(tn,$1);
                                        $$=tn;
                                }
        |                       identifier ASSIGNMENT expression {
                                        tn=mm_new_node(mmVariableInitNode,NULL,NULL);
                                        mm_add_child(tn,mm_new_node(mmAssignmentNode,$1,$3));
                                        mm_set_string_value(tn,mm_get_string_value($1));
                                        $$=tn;
                                }
; 

variable_decl_expression:       VAR variable_decl    {
                                        $$ = mm_new_node(mmVariableDeclNode,$2,NULL);
                                }
        |                       variable_decl_expression ',' variable_decl   {
                                        mm_add_child($1,$3);
                                        $$ = $1;
                                }
;

statement_list:         statement       {
                                $$ = mm_new_node(mmStatementListNode,$1,NULL);
                        }
        |               statement_list statement        {
                                mm_add_child($1,$2);
                                $$ = $1;
                        }
;

statement:              expression_statement
        |               declaration_statement  
        |               return_statement
        |               if_else_statement        
        |               while_statement
        |               for_statement 
        |               continue_statement
        |               break_statement {
                                $$ = $1;
                        }
        |               error ';' {
                           $$=NULL;
                        }
;    

expression_statement:   ';'     {
                                $$ = mm_new_node(mmExpressionStmtNode,NULL,NULL);
                                }
        |               expression ';'  {
                                $$ = mm_new_node(mmExpressionStmtNode,$1,NULL);
                                }
;

declaration_statement:  variable_decl_expression ';'    {
                                $$ = mm_new_node(mmDeclStmtNode,$1,NULL);
                        }
;

compound_statement:     '{' '}' {
                                $$ = mm_new_node(mmCompoundStmtNode,NULL,NULL);
                        }
        |               '{' statement_list '}'  {
                                $$ = mm_new_node(mmCompoundStmtNode,$2,NULL);
                        }
;

return_statement:       RETURN expression ';'   {
                                $$ = mm_new_node(mmReturnNode,$2,NULL);
                        }
;


if_else_statement:      IF expression compound_statement {
                                $$ = mm_new_node(mmIfStmtNode,$2,$3);
                        }
        |               IF expression compound_statement ELSE compound_statement {
                                tn = mm_new_node(mmIfStmtNode,$2,$3);
                                mm_add_child(tn,$5);
                                $$ = tn;
                        }
        |               IF expression compound_statement elsif_list ELSE compound_statement {
                                tn = mm_new_node(mmIfStmtNode,$2,$3);
                                mm_add_child(tn,$4);
                                mm_add_child(tn,$6);
                                $$ = tn;
                        }
;

elsif_clause:           ELSIF expression compound_statement {
                                $$ = mm_new_node(mmElsifNode,$2,$3);
                        }
; 

elsif_list:             elsif_clause {
                                $$ = mm_new_node(mmElsifListNode,$1,NULL);
                        }
        |               elsif_list elsif_clause {
                                mm_add_child($1,$2);
                                $$ = $1;
                        }
; 

while_statement:        WHILE expression compound_statement {
                                $$ = mm_new_node(mmWhileStmtNode,$2,$3);
                        }
;

for_initializer:        expression {
                                $$=$1;
                        } 
        |               VAR variable_decl    {
                                $$ = mm_new_node(mmVariableDeclNode,$2,NULL);
                        }
;

for_initializer_list:   for_initializer {
                                $$=mm_new_node(mmForInitializerListNode,$1,NULL);
                        }
        |               for_initializer_list ',' for_initializer {
                                mm_add_child($1,$3);
                                $$=$1;
                        }
; 

for_initializer_statement:      ';' {
                                $$=mm_new_node(mmForInitializerListNode,NULL,NULL);
                                }
                        |       for_initializer_list ';' {
                                $$=$1;
                                }
;

for_statement:          FOR '(' for_initializer_statement expression_statement ')' compound_statement {
                                tn = mm_new_node(mmForStmtNode,$3,$4);
                                mm_add_child(tn,$6);
                                $$ = tn;
                        }
        |               FOR '(' for_initializer_statement expression_statement expression_list ')' compound_statement {
                                tn = mm_new_node(mmForStmtNode,$3,$4);
                                mm_add_child(tn,$5);
                                mm_add_child(tn,$7);
                                $$ = tn;
                        }
;

continue_statement:     CONTINUE ';' {
                                $$=mm_new_node(mmContinueStmtNode,NULL,NULL);
                        }
;

break_statement:        BREAK ';' {
                                $$=mm_new_node(mmBreakStmtNode,NULL,NULL);
                        }
;                    
  
%%
