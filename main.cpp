#include <iostream>
#include <fstream>
#include <algorithm>
#include <stack>
#include <map>
#include <deque>
#include <cstring>
#include <string>
#include "./Enviroment/Enviroment.hpp"
#include "json/jsonsaver.hpp"
using namespace std;

//-------------------------------/  GRAMMER  /--------------------------------/

/*
main		-> beginEnd
beginEnd	-> 'begin' rest0 'end'
rest0		-> decls block | block
block		-> '{' decls stmts '}'
decls		-> rest1
rest1		-> decl rest1 | none
decl		-> type id
stmts		-> rest2
rest2		-> stmt rest2 | none
stmt		-> block | factor ';'
factor		-> id
*/

//---------------------------/  GLOBAL VARIABLES  /---------------------------/

const int TOKENSIZE = 8; // Buffer size
enum TokenType			 // Token types
{
	BEGIN,		 // 'begin'
	END,		 // 'end'
	KEYWORD,	 // int, char, float, bool
	PUNCTUATION, // {, }, ;,
	IDENTIFIERS, // variables
	ENDOFFILE,
};
struct Token // Token structure
{
	TokenType id;
	string lexeme;
};
deque<Token> TOKENS; // storing tokens
Token cToken;		 // Current Token

char punctuationMarks[] = {'{', '}', ';', '+', '-'};
size_t arraySize = sizeof(punctuationMarks) / sizeof(char);

ifstream File;
int blockNumber, errorCode;
int lineNumber = 1;

Env *ptrTop; // pointer to the topest enviroment(symbol table)
deque<Env *> symbolTable;

//------------------------/  FUNCTION DECLARATIONS  /------------------------/

void detectError(int errorCode);
bool isCharNum(char ch);
string toLowercase(const string &input);
bool isCharInArray(char target, const char *charArray, size_t arraySize);
void tokenLoader();
Token topToken();
void popToken();
void beginEnd();
void rest0();
void block();
void decls();
void rest1();
void decl();
void stmts();
void rest2();
void stmt();
void factor();

//----------------------------------------------------------------------------/

int main(int argc, char *argv[])
{
	// Check if there are at least two arguments (including the program name)
	if (argc < 2)
	{
		cerr << "Error\nUsage: " << argv[0] << " source_code_location.magic" << endl;
		exit(EXIT_FAILURE); // Return an error code
	}

	// Checks if file format is magic or not
	string filePath = argv[1];
	if (filePath.substr(filePath.length() - 6) != ".magic")
	{
		cerr << "Error\nfile format should be .magic: " << endl;
		exit(EXIT_FAILURE); // Return an error code
	}

	std::ofstream file("data.json", std::ios::trunc);
	File.open(filePath);
	// main	-> beginEnd
	beginEnd();
	File.close();

	return 0;
}

void detectError(int errorNumber)
{
	/*
		errorCode 0 => no error
		errorCode 1 => Error in line
		errorCode 2 => ND
		errorCode 3 => end of file
	*/
	switch (errorNumber)
	{
	case 0:
		errorCode = 0;
		return;
	case 1:
	{
		errorCode = 1;
		cout << "ERROR in line " << lineNumber << endl;
		exit(EXIT_FAILURE);
	}
	case 2:
	{
		errorCode = 2;
		cout << "ND" << lineNumber << "; ";
		return;
	}
	case 3:
	{
		if (File.eof())
		{
			errorCode = 3;
			return;
		}
		else
		{
			errorCode = 0;
			return;
		};
	}
	}
}
bool isCharNum(char ch)
{
	// checks if the character is not a punctuation mark
	return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch == '_') || (ch >= '0' && ch <= '9');
}
string toLowercase(const string &input)
{
	// returns the lowercase of the input string
	string result = input;
	transform(result.begin(), result.end(), result.begin(), [](unsigned char c)
			  { return tolower(c); });
	return result;
}
bool isCharInArray(char target, const char *charArray, size_t arraySize)
{
	for (size_t i = 0; i < arraySize; ++i)
	{
		if (charArray[i] == target)
		{
			return true; // Character found in the array
		}
	}
	return false; // Character not found in the array
}
void tokenLoader()
{
	// fills the deque with tokens
	if ((!TOKENS.empty()) && (TOKENS.back().id == ENDOFFILE)) // detecting if file has finished
	{
		return;
	}
	while (TOKENS.size() <= 1)
	{
		// checking if file has finished (double check in order to be sure 'while' wont crash)
		detectError(3);
		if (3 == errorCode)
		{
			Token token;
			token.id = ENDOFFILE;
			token.lexeme = "EOF";
			TOKENS.push_back(token);
			return;
		}

		char head = File.get(); // gets a char from the file
								// checking type of each char and saving token in deque
		if ('\n' == head)
		{

			lineNumber++;
			break;
		}

		else if ('\t' == head)
		{
			break;
		}
		else if (' ' == head)
		{
			break;
		}
		else if (isCharInArray(head, punctuationMarks, arraySize))
		{
			Token token;
			token.id = PUNCTUATION;
			token.lexeme = head;
			TOKENS.push_back(token);
			break;
		}
		else if ('/' == head) // detecting the comments
		{
			head = File.get();
			switch (head) // checking if comment is // or /* */
			{
			case '/': // comment type is //
			{
				do // skips everything until \n
				{
					head = File.get();
					if ('\n' == head)
					{
						lineNumber++;
						break;
					}
				} while ('\n' != head);

				break;
			}
			case '*': // coment type is /*
			caseStar:
			{
				do // skips everything until *
				{
					head = File.get();
					if ('\n' == head)
					{
						lineNumber++;
					}
				} while ('*' != head);
				head = File.get(); // gets following character of *
				switch (head)
				{
				case '/': // if following char of * is / then gets out
					break;
				default: // if following char is not / then repeates this process
					goto caseStar;
				}
			}
			default: // comment may lead to end of file
				detectError(3);
				break;
			}
			break;
		}
		else
		{ // if head was not a punctuation or (newline, space, tab)

			if (-1 == static_cast<int>(head)) // char before end of file wich has asci code of -1
			{
				Token token;
				token.id = ENDOFFILE;
				token.lexeme = "EOF";
				TOKENS.push_back(token);
				return;
			}

			if (isdigit(head)) // checks if token is not starting with integer
			{
				detectError(1);
				return;
			}
			else // saving tokens based on the TOKENSIZE
			{
				Token token;
				token.lexeme = "";
				token.lexeme += head;
				for (int i = 1; i < TOKENSIZE; i++)
				{
					head = File.get();
					if (isCharNum(head))
					{
						if (TOKENSIZE - 1 == i) // checking if word does not surpass the TOKENSIZE
						{
							detectError(1);
							return;
						}
						else
						{
							token.lexeme += head;
						}
					}
					else // saving token then checking the following char of token
					// this process may result in at least one or two token in TOKENS
					{
						{ // saving the word as:
							if (("bool" == toLowercase(token.lexeme)) || ("int" == toLowercase(token.lexeme)) || ("float" == toLowercase(token.lexeme)) || ("char" == toLowercase(token.lexeme)))
							{
								// token is keyword
								token.lexeme = toLowercase(token.lexeme); // saving lower case
								token.id = KEYWORD;
							}
							else if ("begin" == toLowercase(token.lexeme))
							{
								// token is 'begin'
								token.lexeme = toLowercase(token.lexeme); // saving lower case
								token.id = BEGIN;
							}
							else if ("end" == toLowercase(token.lexeme))
							{
								// token is 'end'
								token.lexeme = toLowercase(token.lexeme); // saving lower case
								token.id = END;
							}
							else
							{
								// token is identifier
								token.id = IDENTIFIERS;
							}
							TOKENS.push_back(token);
						}

						{ // checking the next character of the file (following char of the last token)
							if ('\n' == head)
							{
								lineNumber++;
								break;
							}
							else if ('\t' == head)
							{
								break;
							}
							else if (' ' == head)
							{
								break;
							}
							else if (isCharInArray(head, punctuationMarks, arraySize))
							{
								Token token;
								token.id = PUNCTUATION;
								token.lexeme = head;
								TOKENS.push_back(token);
								break;
							}
							else if ('/' == head)
							{
								head = File.get();
								switch (head) // checking if comment is // or /* */
								{
								case '/':
								{
									do // skips everything until \n
									{
										head = File.get();
										if ('\n' == head)
										{
											lineNumber++;
											break;
										}
									} while ('\n' != head);

									break;
								}
								case '*':
								caseStar2:
								{
									do // skipps everything until *
									{
										head = File.get();
										if ('\n' == head)
										{
											lineNumber++;
										}
									} while ('*' != head);
									head = File.get(); // gets following character of *
									switch (head)
									{
									case '/': // if following char of * is / then gets out
										break;
									default: // if following char is not / then repeates this process
										goto caseStar2;
									}
								}
								default: // checking end of file
									detectError(3);
									break;
								}
								break;
							}
							else if (-1 == static_cast<int>(head)) // char befor end of file wich has asci code of -1
							{
								Token token;
								token.id = ENDOFFILE;
								token.lexeme = "EOF";
								TOKENS.push_back(token);
								return;
							}
						}
					}
				}
			}
		}
	}
}

Token topToken()
{
	// returns the first token availible
returnToken:
{
	if (!TOKENS.empty())
	{
		Token t = TOKENS.front();
		return t;
	}
	else
	{
		tokenLoader();
		goto returnToken;
	}
}
}
void popToken()
{
	// delets the first availible token
tokenpoper:
{
	if (!TOKENS.empty())
	{
		Token t = TOKENS.front();
		if (t.id != ENDOFFILE) // checks if the last token is endoffile
		{
			detectError(3);
			TOKENS.pop_front();
			return;
		}
		else
		{
			return;
		}
	}
	else
	{
		tokenLoader();
		goto tokenpoper;
	}
}
}
void beginEnd()
{
	// beginEnd	-> 'begin' rest0 'end'
	cToken = topToken(); // finding 'begin'
	if (BEGIN != cToken.id)
	{
		detectError(1);
	}
	else
	{
		Env top; // semantic actions
		ptrTop = &top;
		cout << "begin ";
		popToken();
		rest0(); // rest0
	}
	cToken = topToken(); // finding 'end'
	if (END != cToken.id)
	{
		detectError(1);
	}
	else
	{
		cout << "end" << endl;
		popToken();
	}
	return;
}
void rest0()
{
	// rest0 -> decls block | block
	cToken = topToken();
	if (KEYWORD == cToken.id)
	{
		decls();
		block();
		return;
	}
	else
	{
		block();
		return;
	}
}
void block()
{
	// block -> '{' decls stmts '}'
	cToken = topToken();
	if (PUNCTUATION != cToken.id && "{" != cToken.lexeme)
	{
		detectError(1);
	}
	else
	{
		cout << "{ "; // '{'
		popToken();
		blockNumber++;

		Env *saved = ptrTop;
		Env top(blockNumber, saved);
		ptrTop = &top;

		cToken = topToken();
		while ("}" != cToken.lexeme)
		{
			if (KEYWORD == cToken.id)
			{
				decls();
			}
			cToken = topToken();
			if ((IDENTIFIERS == cToken.id) || ("{" == cToken.lexeme) || (";" == cToken.lexeme))
			{
				stmts();
			}
		}

		cToken = topToken();
		if ((PUNCTUATION == cToken.id))
		{

			if ("}" == cToken.lexeme) // '}'
			{
				popToken();
				// TODO save the data inside the Enviroment
				tableToJson(ptrTop);

				ptrTop = saved;
				tableToJson(ptrTop);
				cout << "} ";
				return;
			}
			else
			{
				detectError(1);
			}
		}
	}
	return;
}

void decls()
{
	// decls -> rest1
	rest1();
	return;
}
void rest1()
{
	// rest1 -> decl rest1 | none
	cToken = topToken();
	if (KEYWORD == cToken.id)
	{
		decl();
		rest1();
		return;
	}
	else
	{
		return;
	}
}
void decl()
{
	// decl -> type id
	cToken = topToken();
	if (KEYWORD == cToken.id)
	{
		if ("bool" == cToken.lexeme)
		{
			popToken(); // pops bool
			cToken = topToken();

			if (ptrTop->isInCurrentTop(toLowercase(cToken.lexeme))) // checks if there exists another id with another type
			{
				detectError(1);
			}
			ptrTop->put(toLowercase(cToken.lexeme), BOOL); // saving toke in the enviroment

			popToken();
			cToken = topToken();

			if (";" != cToken.lexeme) // handeling ; after declaration
			{
				detectError(1);
			}
			else
			{
				popToken();
				return;
			}
		}
		if ("int" == cToken.lexeme)
		{
			popToken();
			cToken = topToken();

			if (ptrTop->isInCurrentTop(toLowercase(cToken.lexeme))) // checking if there exists another id with other type
			{
				detectError(1);
			}
			ptrTop->put(toLowercase(cToken.lexeme), INT);

			popToken();
			cToken = topToken();

			if (";" != cToken.lexeme) // handeling ; after declaration
			{
				detectError(1);
			}
			else
			{
				popToken();
				return;
			}
		}
		if ("float" == cToken.lexeme)
		{
			popToken();
			cToken = topToken();

			if (ptrTop->isInCurrentTop(toLowercase(cToken.lexeme))) // checking if there exists another id with other type
			{
				detectError(1);
			}
			ptrTop->put(toLowercase(cToken.lexeme), FLOAT);

			popToken();
			cToken = topToken();

			if (";" != cToken.lexeme) // handeling ; after declaration
			{
				detectError(1);
			}
			else
			{
				popToken();
				return;
			}
		}
		if ("char" == cToken.lexeme)
		{
			popToken();
			cToken = topToken();

			if (ptrTop->isInCurrentTop(toLowercase(cToken.lexeme))) // checking if there exists another id with other type
			{
				detectError(1);
			}
			ptrTop->put(toLowercase(cToken.lexeme), CHAR);

			popToken();
			cToken = topToken();

			if (";" != cToken.lexeme) // handeling ; after declaration
			{
				detectError(1);
			}
			else
			{
				popToken();
				return;
			}
		}
	}
}

void stmts()
{
	// stmts -> rest2
	rest2();
	return;
}
void rest2()
{
	// rest2 -> stmt rest2 | none
	cToken = topToken();
	if ((IDENTIFIERS == cToken.id) || ("{" == cToken.lexeme) || (";" == cToken.lexeme))
	{
		stmt();
		rest2();
		return;
	}
	else
	{
		return;
	}
}
void stmt()
{
	// stmt	-> block | factor ';'
	cToken = topToken();
	if ("{" == cToken.lexeme) // block
	{
		block();
		return;
	}
	else if (";" == cToken.lexeme) // handeling alone ';' without any declaration
	{
		popToken();
		return;
	}
	else if (IDENTIFIERS == cToken.id) // factor
	{
		factor();
		return;
	}
}
void factor()
{
	// factor -> id
	cToken = topToken();
	popToken();

	if ((";" == topToken().lexeme) || ("+" == topToken().lexeme) || ("-" == topToken().lexeme)) // handeling +/-
	{
		Symbol sym = ptrTop->get(toLowercase(cToken.lexeme));
		switch (sym)
		{
		case BOOL:
			cout << cToken.lexeme << ":"
				 << "bool" << ptrTop->getBlockNumber(toLowercase(cToken.lexeme))
				 << "; ";
			break;
		case FLOAT:
			cout << cToken.lexeme << ":"
				 << "float" << ptrTop->getBlockNumber(toLowercase(cToken.lexeme))
				 << "; ";
			break;
		case INT:
			cout << cToken.lexeme << ":"
				 << "int" << ptrTop->getBlockNumber(toLowercase(cToken.lexeme))
				 << "; ";
			break;
		case CHAR:
			cout << cToken.lexeme << ":"
				 << "char" << ptrTop->getBlockNumber(toLowercase(cToken.lexeme))
				 << "; ";
			break;
		case null:
			cout << cToken.lexeme << ":";
			detectError(2);
			break;
		}
	}

	if (";" != topToken().lexeme)
	{
		detectError(1);
	}
	else
	{
		popToken();
	}

	return;
}