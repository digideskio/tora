grammar S2;

options {
	language=Cpp;
	//backtrack=true;
	//memoize=true;
	output=AST;
}

tokens {
    BLOCK = 'block';
}

scope GlobalOne {
	std::string globalFoo;
}

@lexer::includes 
{
#include "UserMiniTraits.hpp"
}
@lexer::namespace{ Antlr3Mini }

@parser::includes {
#include "UserMiniTraits.hpp"
#include "S2Lexer.hpp"
#include <string>
}
@parser::namespace{ Antlr3Mini }


start_rule
scope {
	std::string localFoo;
}
scope GlobalOne;
@init {
}
: if_statement;

if_statement
    : ifSt=IF LEFT_PAREN  /*cond=*/expression RIGHT_PAREN if_then if_else?
      //->  ^(IF_ST[$ifSt] $cond if_then if_else?)
	{
		$GlobalOne::globalFoo.append(".").append($ifSt->getText());
        $start_rule::localFoo.append(".").append($ifSt->getText());
	} 
    ;
if_then
    : th=LEFT_BRACE /*thexpr=*/script? RIGHT_BRACE //-> ^(BLOCK[$th] $thexpr?)
    | /*cs=*/script //-> ^(BLOCK $cs)
    ;

if_else
    : el=ELSE LEFT_BRACE elexpr=script? RIGHT_BRACE  //-> ^(BLOCK[$el] $elexpr?)
    | el=ELSE /*cs=*/script//-> ^(BLOCK[$el] $cs)
    ;

script
		: COMMAND SEMI 
		; 

expression
		: EXPRESSION
		;
		
COMMAND: 'COMMAND';

EXPRESSION: 'EXPRESSION';

SEMI: ';';

ELSE: 'ELSE';

IF: 'IF';

LEFT_PAREN: '(';

RIGHT_PAREN: ')';

LEFT_BRACE: '{';

RIGHT_BRACE: '}';

