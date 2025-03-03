%top{
    #include "parser.tab.hh"
    #define YY_DECL yy::parser::symbol_type yylex()
    #include "Node.h"
    int lexical_errors = 0;
}
%option yylineno noyywrap nounput batch noinput stack 
%%

 /* Keywords */
"public"                {if(USE_LEX_ONLY) {printf("PUBLIC ");} else {return yy::parser::make_PUBLIC(yytext);}}
"class"                 {if(USE_LEX_ONLY) {printf("CLASS ");} else {return yy::parser::make_CLASS(yytext);}}
"static"                {if(USE_LEX_ONLY) {printf("STATIC ");} else {return yy::parser::make_STATIC(yytext);}}
"void"                  {if(USE_LEX_ONLY) {printf("VOID ");} else {return yy::parser::make_VOID(yytext);}}
"main"                  {if(USE_LEX_ONLY) {printf("MAIN ");} else {return yy::parser::make_MAIN(yytext);}}
"String"                {if(USE_LEX_ONLY) {printf("STRING ");} else {return yy::parser::make_STRING(yytext);}}
"return"                {if(USE_LEX_ONLY) {printf("RETURN ");} else {return yy::parser::make_RETURN(yytext);}}
"int"                   {if(USE_LEX_ONLY) {printf("INT_TYPE ");} else {return yy::parser::make_INT_TYPE(yytext);}}
"boolean"               {if(USE_LEX_ONLY) {printf("BOOLEAN ");} else {return yy::parser::make_BOOLEAN(yytext);}}
"if"                    {if(USE_LEX_ONLY) {printf("IF ");} else {return yy::parser::make_IF(yytext);}}
"else"                  {if(USE_LEX_ONLY) {printf("ELSE ");} else {return yy::parser::make_ELSE(yytext);}}
"while"                 {if(USE_LEX_ONLY) {printf("WHILE ");} else {return yy::parser::make_WHILE(yytext);}}
"System.out.println"    {if(USE_LEX_ONLY) {printf("PRINTLN ");} else {return yy::parser::make_PRINTLN(yytext);}}
"length"                {if(USE_LEX_ONLY) {printf("LENGTH ");} else {return yy::parser::make_LENGTH(yytext);}}
"true"                  {if(USE_LEX_ONLY) {printf("TRUE ");} else {return yy::parser::make_TRUE(yytext);}}
"false"                 {if(USE_LEX_ONLY) {printf("FALSE ");} else {return yy::parser::make_FALSE(yytext);}}
"this"                  {if(USE_LEX_ONLY) {printf("THIS ");} else {return yy::parser::make_THIS(yytext);}}
"new"                   {if(USE_LEX_ONLY) {printf("NEW ");} else {return yy::parser::make_NEW(yytext);}}
"extends"               {if(USE_LEX_ONLY) {printf("EXTENDS ");} else {return yy::parser::make_EXTENDS(yytext);}}

 /* Operators */
"+"                     {if(USE_LEX_ONLY) {printf("PLUS ");} else {return yy::parser::make_PLUS(yytext);}}
"-"                     {if(USE_LEX_ONLY) {printf("MINUS ");} else {return yy::parser::make_MINUS(yytext);}}
"*"                     {if(USE_LEX_ONLY) {printf("MULT ");} else {return yy::parser::make_MULT(yytext);}}
"&&"                    {if(USE_LEX_ONLY) {printf("AND ");} else {return yy::parser::make_AND(yytext);}}
"||"                    {if(USE_LEX_ONLY) {printf("OR ");} else {return yy::parser::make_OR(yytext);}}
"<"                     {if(USE_LEX_ONLY) {printf("LT ");} else {return yy::parser::make_LT(yytext);}}
">"                     {if(USE_LEX_ONLY) {printf("GT ");} else {return yy::parser::make_GT(yytext);}}
"=="                    {if(USE_LEX_ONLY) {printf("EQ ");} else {return yy::parser::make_EQ(yytext);}}
"="                     {if(USE_LEX_ONLY) {printf("ASSIGN ");} else {return yy::parser::make_ASSIGN(yytext);}}
"!"                     {if(USE_LEX_ONLY) {printf("NOT ");} else {return yy::parser::make_NOT(yytext);}}
"."                     {if(USE_LEX_ONLY) {printf("DOT ");} else {return yy::parser::make_DOT(yytext);}}

 /* Delimiters */
"("                     {if(USE_LEX_ONLY) {printf("LPAREN ");} else {return yy::parser::make_LPAREN(yytext);}}
")"                     {if(USE_LEX_ONLY) {printf("RPAREN ");} else {return yy::parser::make_RPAREN(yytext);}}
"{"                     {if(USE_LEX_ONLY) {printf("LBRACE ");} else {return yy::parser::make_LBRACE(yytext);}}
"}"                     {if(USE_LEX_ONLY) {printf("RBRACE ");} else {return yy::parser::make_RBRACE(yytext);}}
"["                     {if(USE_LEX_ONLY) {printf("LBRACKET ");} else {return yy::parser::make_LBRACKET(yytext);}}
"]"                     {if(USE_LEX_ONLY) {printf("RBRACKET ");} else {return yy::parser::make_RBRACKET(yytext);}}
";"                     {if(USE_LEX_ONLY) {printf("SEMICOLON ");} else {return yy::parser::make_SEMICOLON(yytext);}}
","                     {if(USE_LEX_ONLY) {printf("COMMA ");} else {return yy::parser::make_COMMA(yytext);}}

 /* Error handling for special characters - must come BEFORE identifier rule */
[\"\$\%\@]             { if(!lexical_errors) fprintf(stderr, "Lexical errors found! See the logs below: \n"); 
                        fprintf(stderr,"\t@error at line %d. Character %s is not recognized\n", yylineno, yytext); 
                        lexical_errors = 1;}

 /* Literals and Identifiers */
[0-9]+                  {if(USE_LEX_ONLY) {printf("INTEGER_LITERAL ");} else {return yy::parser::make_INTEGER_LITERAL(yytext);}}
[a-zA-Z_][a-zA-Z0-9_]*  {if(USE_LEX_ONLY) {printf("IDENTIFIER ");} else {return yy::parser::make_IDENTIFIER(yytext);}}

 /* Whitespace and Comments */
[ \t\n\r]+             { /* Skip whitespace */ }
"//"[^\n]*             { /* Skip single-line comments */ }

 /* Error handling */
.                      { if(!lexical_errors) fprintf(stderr, "Lexical errors found! See the logs below: \n"); 
                        fprintf(stderr,"\t@error at line %d. Character %s is not recognized\n", yylineno, yytext); 
                        lexical_errors = 1;}

<<EOF>>                {return yy::parser::make_END();}
%%