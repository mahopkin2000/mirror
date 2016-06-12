%{
#include "tree.h"
#include "mirror_parser.h"
#include <string.h>

extern YYSTYPE yylval;
long lineno = 1;
char linebuf[1000];
long lineStartPos;
long last_message_send_pos;
long last_newline_pos;
YY_BUFFER_STATE old_buff;

extern long getInputPos() {
    long begin=0;
    int offset=0;
    if (yyin) {
    begin  = ftell(yyin)-YY_CURRENT_BUFFER->yy_n_chars;
    offset = yy_c_buf_p-YY_CURRENT_BUFFER->yy_ch_buf;
    }
    return begin+offset;
}

void setup_parse_buffer(char* buff) {
    YY_BUFFER_STATE state;
    old_buff=YY_CURRENT_BUFFER;
    state=yy_scan_string(buff);
    yy_switch_to_buffer(state);
}

void free_parse_buffer() {
   yy_delete_buffer(YY_CURRENT_BUFFER);
   yy_switch_to_buffer(old_buff);
}

%}

%option noyywrap

%x comment

MM_BINARY_MESSAGE  [\+\*\/\<\>\%\@=&\^]|(==)|(<=)|(>=)|(&&)|(\|\|)|(<<)|(>>)|(!=)
MM_FLOAT   [0-9]*\.[0-9]+L?
MM_INTEGER [0-9]+L?
MM_STRING       \"(\\.|[^\\"])*\"
MM_CHAR         \'.\'|\'\\.\'|\'\\[0-7]+\'
MM_IDENT        ([a-zA-Z_][a-zA-Z0-9_]*::)*([a-zA-Z_][a-zA-Z0-9_]*)

%%
\/\/.*  {}

"/*"         BEGIN(comment);

<comment>[^*\n]*        /* eat anything that's not a '*' */
<comment>"*"+[^/\n]*   /* eat up '*'s not followed by '/'s */
<comment>\n             ++lineno;
<comment>"*"+"/"        BEGIN(INITIAL);



"import" {
        return IMPORT;
        }
        
"class" {
        return CLASS;
        }
        
"attribute" {
        return ATTRIBUTE;
}

"method" {
        return METHOD;
}

"instance:" {
        return INSTANCE_SIDE;
}

"class:" {
        return CLASS_SIDE;
}

"return" {
        return RETURN;
}

"var" {
        return VAR;
}

"if" {
        return IF;
}

"elsif" {
        return ELSIF;
}

"else" {
        return ELSE;
}

"while" {
        return WHILE;
}

"for" {
        return FOR;
}

":=" {
        return ASSIGNMENT;
}

"primitive" {
        return PRIMITIVE;
}

"\." {
        last_message_send_pos=getInputPos();
        return MESSAGE_SEND;
}

"nil" {
        return NIL;
}

"true" {
        return TRUE;
}

"false" {
        return FALSE;
}

"self" {
        return SELF;
}

"super" {
        return SUPER;
}

"thisContext" {
        return THIS_CONTEXT;
}

"byte" {
        return BYTE;
}

"indexed" {
        return INDEXED;
}

"break" {
        return BREAK;
}

"continue" {
        return CONTINUE;
}

"namespace" {
        return NAMESPACE;
}

{MM_INTEGER} {
        yylval.ival = atoi(yytext);
        return INTEGER;
}

{MM_BINARY_MESSAGE} {
        yylval.sval = strdup(yytext);
        last_message_send_pos=getInputPos();
        return BINARY_MESSAGE;
}

{MM_IDENT} {
        yylval.sval = strdup(yytext);
        return IDENT;
}

{MM_FLOAT} {
        yylval.fval = atof(yytext);
        return FLOAT;
}


{MM_STRING} {
        yylval.sval = strdup(yytext);
        return STRING;
}

{MM_CHAR} {
        yylval.sval = strdup(yytext);
        return CHAR;
}

[\t\r ] ;

\n.*      {
        strcpy(linebuf,yytext+1);
        lineno++;
        yyless(1);
        last_newline_pos=getInputPos();

}

.       {
        return yytext[0];
}



