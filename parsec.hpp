/********************************************************
 * libparsec
 *
 * Description:     A univeral C++ parser combinator library 
 *                  inspirted by Parsec library of Haskell.
 * Author:          He Tao
 *
 *********************************************************/

class Parser;
class Value;
class ParseError;

class Parser
{
public:
    Parser();
    ~Parser();
};

class Value
{
public:
    Value();
    ~Value();
};

class ParseError
{
public:
    ParseError();
    ~ParseError();
};

Parser::Parser()
{

}

Parser::~Parser()
{

}

Value::Value()
{

}

Value::~Value()
{

}

ParseError::ParseError()
{

}

ParseError::~ParseError()
{
    
}
