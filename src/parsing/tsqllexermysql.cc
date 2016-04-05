#include "MySQLGuiLexer.hpp"
#include "parsing/tsqllexer.h"

namespace SQLLexer
{

using namespace Antlr3GuiImpl;

class mySQLGuiLexer : public Lexer
{
public:
    mySQLGuiLexer(const QString &statement, const QString &name);
    virtual ~mySQLGuiLexer();
    virtual QString firstWord();
    virtual QString wordAt(const Position &);
    virtual token_const_iterator findStartToken(token_const_iterator const &);    
    virtual token_const_iterator findEndToken(token_const_iterator const &);
    virtual void setStatement(const char *s, unsigned len = -1);
    virtual void setStatement(const QString &s);

	typedef MySQLGuiLexerTraits::CommonTokenType CommonTokenType;
protected:
    virtual int size() const;
    virtual const Token& LA(int pos) const;

private:
    void init();
    void clean();
    QByteArray QBAinput;
    QByteArray QBAname;

    Antlr3GuiImpl::MySQLGuiLexerTraits::InputStreamType*    input;
    Antlr3GuiImpl::MySQLGuiLexer *lxr;
    Antlr3GuiImpl::MySQLGuiLexerTraits::TokenStreamType* tstream;

    unsigned lastLine, lastColumn, lastIndex;

    mutable Token retvalLA;
};

mySQLGuiLexer::mySQLGuiLexer(const QString &statement, const QString &name)
    : Lexer(statement, name)
    , QBAinput(statement.toUtf8())
    , QBAname(name.toUtf8())
    , lastLine(1)
    , lastColumn(0)
    , lastIndex(0)
{
	init();
}

mySQLGuiLexer::~mySQLGuiLexer()
{
    clean();
}

void mySQLGuiLexer::init()
{
    input = new MySQLGuiLexerTraits::InputStreamType(
    		(const ANTLR_UINT8 *)QBAinput.data(),
			antlr3::ENC_UTF8,
    		QBAinput.size(), //strlen(data.c_str()),
    		(ANTLR_UINT8*)QBAname.data());

    input->setUcaseLA(true); // ignore case

    if (input == NULL)
    {
        // TODO throw here
        throw Exception();
        //exit(ANTLR_ERR_NOMEM);
    }

    // Our input stream is now open and all set to go, so we can create a new instance of our
    // lexer and set the lexer input to our input stream:
    //  (file | memory | ?) --> inputstream -> lexer --> tokenstream --> parser ( --> treeparser )?
    //
    lxr     = new MySQLGuiLexer(input);     // MySQLGuiLexer is generated by ANTLR
    //lxr->pLexer->input->charPositionInLine = 0; // fix off-by-one error for getCharPositionInLine for the 1st row

    if ( lxr == NULL )
    {
        // TODO throw here
        throw Exception();
        //exit(ANTLR_ERR_NOMEM);
    }

    tstream = new MySQLGuiLexerTraits::TokenStreamType(ANTLR_SIZE_HINT, lxr->get_tokSource());

    if (tstream == NULL)
    {
        // TODO throw here
        //_mState = P_ERROR;
        throw Exception();
        //exit(ANTLR_ERR_NOMEM);
    }
    //_mState = P_LEXER;
};

void mySQLGuiLexer::setStatement(const char *s, unsigned len)
{
	clean();
	QBAinput.clear();
	QBAinput.append(s, len);
	lastLine = 1;
	lastColumn = 0;
	lastIndex = 0;
	init();
}

void mySQLGuiLexer::setStatement(const QString &statement)
{
	clean();
	QBAinput.clear();
	QBAinput.append(statement.toUtf8());
	lastLine = 1;
	lastColumn = 0;
	lastIndex = 0;
	init();
}

void mySQLGuiLexer::clean()
{
	if( tstream)
		delete tstream;
	if( lxr)
		delete lxr;
	if( input)
		delete input;
	tstream = NULL;
	lxr = NULL;
	input = NULL;
}

int mySQLGuiLexer::size() const
{
	if(tstream)
		return tstream->getTokens()->size()+1;
	else
		return 0;
}

const Token& mySQLGuiLexer::LA(int pos) const
{
	if ( pos <= 0 || pos > size())
		throw Exception();

	if( pos == size())
	{
		// Requesting EOF_TOKEN
		if (pos == 1)
		{
			// The buffer is empty - Only EOF_TOKEN is "present"
			Token::TokenType type = Token::X_EOF;
			retvalLA = Token(Position(0, 0), 0, MySQLGuiLexer::EOF_TOKEN, type);
			retvalLA.setText("EOF");
			retvalLA.setBlockContext(NONE);
			return retvalLA;
		}

		CommonTokenType const* token = tstream->get(pos-2);
		if (!token)
			throw Exception();

		int line = token->get_line() - 1;
		int column = token->get_charPositionInLine();
		unsigned length = token->get_stopIndex() - token->get_startIndex() + 1;
		QString txt = QString::fromUtf8(token->getText().c_str());
		Token::TokenType type = Token::X_EOF;
		retvalLA = Token(Position(line, column+length+1), 0, MySQLGuiLexer::EOF_TOKEN, type);
		retvalLA.setText("EOF");
		retvalLA.setBlockContext(NONE);
		return retvalLA;
	}

	CommonTokenType const* token = tstream->get(pos-1);
	if(token)
	{
		// ANTLR3 starts with 1st while QScintilla starts with 0th
		int line = token->get_line() - 1;
		int column = token->get_charPositionInLine();
		unsigned length = token->get_stopIndex() - token->get_startIndex() + 1;
		int offset = token->get_startIndex();
		Token::TokenType type = Token::X_UNASSIGNED;

		switch(token->getType())
		{ 
		case MySQLGuiLexer::EOF_TOKEN: 
			type = Token::X_EOF;
			break;
		case MySQLGuiLexer::BUILDIN_FUNCTIONS:
			type = Token::L_BUILDIN;
			break;
		case MySQLGuiLexer::STRING_LITERAL:
		case MySQLGuiLexer::USER_VAR:
			type = Token::L_STRING;
			break;
		case MySQLGuiLexer::COMMENT_ML:
		case MySQLGuiLexer::COMMENT_ML_PART:
			type = Token::X_COMMENT_ML;
			break;
		case MySQLGuiLexer::COMMENT_ML_END:
			type = Token::X_COMMENT_ML_END;
			break;
		case MySQLGuiLexer::COMMENT_SL:
			type = Token::X_COMMENT;
			break;
		case MySQLGuiLexer::TOKEN_FAILURE:
			type = Token::X_FAILURE;
			break;
		case MySQLGuiLexer::MYSQL_RESERVED:
			type = Token::L_RESERVED; 
			break;
		case MySQLGuiLexer::NEWLINE:
		case MySQLGuiLexer::SPACE:
		case MySQLGuiLexer::WHITE:
			type = Token::X_WHITE; 
			break;
		case MySQLGuiLexer::BIND_VAR:
			type = Token::L_BIND_VAR;
			break;
		case MySQLGuiLexer::BIND_VAR_WITH_PARAMS:
			type = Token::L_BIND_VAR_WITH_PARAMS;
			break;			
		default:
			type = Token::X_UNASSIGNED;
			break;
		}		

	    retvalLA = Token(Position(line, column), length, token->getType(), type);
		retvalLA.setText(QString::fromUtf8(token->getText().c_str()));
	    return retvalLA;
	} else
		throw Exception();
}

QString mySQLGuiLexer::firstWord()
{
	MySQLGuiLexerTraits::CommonTokenType const* token = tstream->LT(1);
    if( token)
    {
		return QString((const char*)(token->getText().c_str()));
    }
    return QString();
}

QString mySQLGuiLexer::wordAt(const Position &pos)
{
	unsigned line = pos.getLine();
	unsigned column = pos.getLinePos();

    line++; // ANTLR3 starts with 1st while QScintilla starts with 0th

    QString retval;
    //ANTLR_UINT32 i, startIndex;
    //ANTLR_UINT32 size = tstream->getTokens()->size();
    //User::MyTraits::CommonTokenType const* token = NULL;

    //if(size == 0)
    //    return retval;

    //if( lastLine > line || (lastLine == line && lastColumn > column))
    //{
    //	User::MyTraits::CommonTokenType const* tokenZero = (pANTLR3_COMMON_TOKEN)lexerTokenVector->get(lexerTokenVector, 0);
    //	retval = QString::fromUtf8((const char*)(tokenZero->getText()->chars));
    //	startIndex = 1;
    //} else {
    //	User::MyTraits::CommonTokenType const* tokenZero = (pANTLR3_COMMON_TOKEN)lexerTokenVector->get(lexerTokenVector, lastIndex);
    //	retval = QString::fromUtf8((const char*)(tokenZero->getText()->chars));
    //	startIndex = lastIndex;
    //}

    //for (i = startIndex; i <= size; i++)
    //{
    //    token = (pANTLR3_COMMON_TOKEN)lexerTokenVector->get(lexerTokenVector, i);
    //    if( token == NULL)
    //    	break;

    //    if ( token->getChannel(token) != HIDDEN)
    //    {
    //        lastIndex  = i;
    //        lastLine   = token->getLine(token);
    //        lastColumn = token->getCharPositionInLine(token);
    //    	retval = QString::fromUtf8((const char*)(token->getText(token)->chars));
    //    }

    //    if( token->getLine(token) > line
    //    		|| ( token->getLine(token) == line && token->getCharPositionInLine(token) > column ))
    //    	break;	
    //}

    return retval;
}

Lexer::token_const_iterator mySQLGuiLexer::findStartToken( Lexer::token_const_iterator const &start)
{
	QSet<SQLLexer::Token::TokenType> INTRODUCERS = QSet<SQLLexer::Token::TokenType>()
		<< SQLLexer::Token::L_SELECT_INTRODUCER
		<< SQLLexer::Token::L_DML_INTRODUCER
		<< SQLLexer::Token::L_DDL_INTRODUCER
		<< SQLLexer::Token::L_PL_INTRODUCER
		<< SQLLexer::Token::L_OTHER_INTRODUCER
		<< SQLLexer::Token::L_LPAREN
		<< SQLLexer::Token::X_ONE_LINE
		;	
	token_const_iterator i(start);
	if(!INTRODUCERS.contains(i->getTokenType()))
	{
		i = i.consumeUntil(INTRODUCERS);
		i++;
	}
	return i;
}

Lexer::token_const_iterator mySQLGuiLexer::findEndToken( Lexer::token_const_iterator const &start)
{
	token_const_iterator i( start);
	switch( i->getTokenType())
	{
	case Token::L_SELECT_INTRODUCER:
	case Token::L_DML_INTRODUCER:
	case Token::L_DDL_INTRODUCER:
	case Token::L_PL_INTRODUCER:
	case Token::L_OTHER_INTRODUCER:
		while(true)
		{
			i++;
			if( i->getTokenType() == Token::X_EOF)
				break;
			if( i->getTokenType() == Token::X_UNASSIGNED && i->getOrigTokenType() == MySQLGuiLexer::SEMI)
				break;
		}
		break;
	default:
		throw new Exception();
	}

 	if( i == start) // If the statement contains only one token advance forward. (Never return the same token)
 		i++;
	return i;
}

};

Util::RegisterInFactory<SQLLexer::mySQLGuiLexer, LexerFactTwoParmSing> reMySQLLexStatement("mySQLGuiLexer");
