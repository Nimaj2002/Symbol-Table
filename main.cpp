#include <iostream>
#include <fstream>
#include <algorithm>
#include <stack>
#include <map>
#include <deque>
#include <cstring>
#include <string>
#include "./simboltable/simboltable.h"
using namespace std;

// ------------------------------------
const int TOKENSIZE = 8;
// Token types
enum TokenType
{
	BEGIN,
	END,
	KEYWORD,	 // int, char, float, bool
	PUNCTUATION, // {, }, ;,
	IDENTIFIERS, // variables
	ENDOFFILE,
};
// Token structure
struct Token
{
	TokenType id;
	string lexeme;
};
// storing tokens
deque<Token> TOKENS;
Token cToken;

ifstream File;
int blockNumber, errorCode;
int lineNumber = 1;
Env top;
Env *ptrTop = &top;
// ------------------------------------

void detectError(int errorCode);
bool isCharNum(char ch);
void tokenLoader();
void beginEnd();
Token topToken();
void popToken();
void block();
void decls();
void decl();
void stmts();
void stmt();
void factor();
void rest1();
void rest2();

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
	File.open(filePath);
	beginEnd();
	// while (cToken.id != ENDOFFILE)
	// {
	// 	cToken = topToken();
	// 	cout << cToken.id << " -- " << cToken.lexeme << endl;
	// 	popToken();
	// }
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
	return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch == '_') || (ch >= '0' && ch <= '9');
}
void tokenLoader()
{
	// detecting if file has finished
	if ((!TOKENS.empty()) && (TOKENS.back().id == ENDOFFILE))
	{
		return;
	}
	while (TOKENS.size() <= 1)
	{
		// checking if file has finished (double check in order to be sure while wont crash)
		detectError(3);
		if (3 == errorCode)
		{
			Token token;
			token.id = ENDOFFILE;
			token.lexeme = "EOF";
			TOKENS.push_back(token);
			return;
		}

		char head = File.get();
		switch (head)
		{
		case '\n':
			lineNumber++;
			break;
		case '\t':
			break;
		case ' ':
			break;
		case '{':
		{
			Token token;
			token.id = PUNCTUATION;
			token.lexeme = "{";
			TOKENS.push_back(token);
			break;
		}
		case '}':
		{
			Token token;
			token.id = PUNCTUATION;
			token.lexeme = "}";
			TOKENS.push_back(token);
			break;
		}
		case ';':
		{
			Token token;
			token.id = PUNCTUATION;
			token.lexeme = ";";
			TOKENS.push_back(token);
			break;
		}
		case '-':
		{
			Token token;
			token.id = PUNCTUATION;
			token.lexeme = "-";
			TOKENS.push_back(token);
			break;
		}
		case '+':
		{
			Token token;
			token.id = PUNCTUATION;
			token.lexeme = "+";
			TOKENS.push_back(token);
			break;
		}
		case '/':
		{
			head = File.get();
			switch (head) // checking if comment is // or /* */
			{
			case '/': // comment type is //
			{
				do // skipps everything until \n
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
					goto caseStar;
				}
			}
			default: // comment may lead to end of file
				detectError(3);
				break;
			}
			break;
		}
		default: // if head was not a punctuation or (newline, space, tab)

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
				cout << "3" << endl;
				detectError(1);
				return;
			}
			else
			{
				Token token;
				token.lexeme = "";
				token.lexeme += tolower(head);
				for (int i = 1; i < TOKENSIZE; i++)
				{
					head = File.get();
					if (isCharNum(head))
					{
						if (TOKENSIZE - 1 == i) // checking if words does not surpass the TOKENSIZE
						{
							cout << "4" << endl;
							detectError(1);
							return;
						}
						else
						{
							token.lexeme += tolower(head);
						}
					}
					else // saving token then checking the following char of token
					// this process may result in at least one or two token in TOKENS
					{
						{ // saving the word as:
							if (("bool" == token.lexeme) || ("int" == token.lexeme) || ("float" == token.lexeme) || ("char" == token.lexeme))
							{
								token.id = KEYWORD;
							}
							else if ("begin" == token.lexeme)
							{
								token.id = BEGIN;
							}
							else if ("end" == token.lexeme)
							{
								token.id = END;
							}
							else
							{
								token.id = IDENTIFIERS; // variables
							}
							TOKENS.push_back(token);
						}

						{ // checking the type of head
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
							else if (';' == head)
							{
								Token token;
								token.id = PUNCTUATION;
								token.lexeme = ";";
								TOKENS.push_back(token);
								break;
							}
							else if ('{' == head)
							{
								Token token;
								token.id = PUNCTUATION;
								token.lexeme = "{";
								TOKENS.push_back(token);
								break;
							}
							else if ('}' == head)
							{
								Token token;
								token.id = PUNCTUATION;
								token.lexeme = "}";
								TOKENS.push_back(token);
								break;
							}
							else if ('-' == head)
							{
								Token token;
								token.id = PUNCTUATION;
								token.lexeme = "-";
								TOKENS.push_back(token);
								break;
							}
							else if ('-' == head)
							{
								Token token;
								token.id = PUNCTUATION;
								token.lexeme = "+";
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
									do // skipps everything until \n
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
								default:
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
	cToken = topToken();
	if (BEGIN != cToken.id)
	{
		cout << "6" << endl;
		detectError(1);
	}
	else
	{
		cout << "begin ";
		popToken();
		// cout << "enterblock" << endl;
		block();
	}
	cToken = topToken(); // finding end
	if (END != cToken.id)
	{
		cout << "7" << endl;
		detectError(1);
	}
	else
	{
		cout << "end" << endl;
		popToken();
	}
	return;
}

void block()
{
	cToken = topToken();
	if (PUNCTUATION != cToken.id && "{" != cToken.lexeme)
	{
		cout << "8" << endl;
		detectError(1);
	}
	else
	{
		cout << "{ ";
		popToken();
		blockNumber++;

		Env *saved = ptrTop;
		Env top(blockNumber, saved);
		ptrTop = &top;

		cToken = topToken();
		if (KEYWORD == cToken.id)
		{
			// cout << "enterdecls" << endl;
			decls();
		}
		cToken = topToken();
		if ((IDENTIFIERS == cToken.id) || ("{" == cToken.lexeme) || (";" == cToken.lexeme))
		{
			// cout << "enterstmts" << endl;
			stmts();
		}

		cToken = topToken();
		if ((PUNCTUATION == cToken.id))
		{

			if ("}" == cToken.lexeme)
			{
				popToken();
				blockNumber--;
				ptrTop = saved;
				cout << "} ";
				return;
			}
			else
			{
				cout << "9" << endl;
				detectError(1);
			}
		}
	}
	return;
}

void decls()
{
	rest1();
	return;
}
void rest1()
{
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
	cToken = topToken();
	if (KEYWORD == cToken.id)
	{
		if ("bool" == cToken.lexeme)
		{
			popToken();
			cToken = topToken();

			if (ptrTop->isInCurrentTop(cToken.lexeme)) // checking if there exists another id with other type
			{
				cout << "10" << endl;
				detectError(1);
			}

			ptrTop->put(cToken.lexeme, BOOL);
			popToken();
			cToken = topToken();
			if (";" != cToken.lexeme) // handeling ; after declaration
			{
				cout << "11" << endl;
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

			if (ptrTop->isInCurrentTop(cToken.lexeme)) // checking if there exists another id with other type
			{
				cout << "12" << endl;
				detectError(1);
			}

			ptrTop->put(cToken.lexeme, INT);
			popToken();
			cToken = topToken();
			if (";" != cToken.lexeme) // handeling ; after declaration
			{
				cout << "13" << endl;
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

			if (ptrTop->isInCurrentTop(cToken.lexeme)) // checking if there exists another id with other type
			{
				cout << "14" << endl;
				detectError(1);
			}

			ptrTop->put(cToken.lexeme, FLOAT);
			popToken();
			cToken = topToken();
			if (";" != cToken.lexeme) // handeling ; after declaration
			{
				cout << "15" << endl;
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

			if (ptrTop->isInCurrentTop(cToken.lexeme)) // checking if there exists another id with other type
			{
				cout << "16" << endl;
				detectError(1);
			}

			ptrTop->put(cToken.lexeme, CHAR);
			popToken();
			cToken = topToken();
			if (";" != cToken.lexeme) // handeling ; after declaration
			{
				cout << "17" << endl;
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
	rest2();
	return;
}

void rest2()
{
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
	cToken = topToken();
	if ("{" == cToken.lexeme)
	{
		block();
		return;
	}
	else if (";" == cToken.lexeme)
	{
		cout << "; ";
		popToken();
		return;
	}
	else if (IDENTIFIERS == cToken.id)
	{
		factor();
		return;
	}
}

void factor()
{
	cToken = topToken();
	popToken();
	if (";" == topToken().lexeme)
	{
		Symbol sym = ptrTop->get(cToken.lexeme);
		switch (sym)
		{
		case BOOL:
			cout << cToken.lexeme << ":"
				 << "bool"
				 << "; ";
			break;
		case FLOAT:
			cout << cToken.lexeme << ":"
				 << "float"
				 << "; ";
			break;
		case INT:
			cout << cToken.lexeme << ":"
				 << "int"
				 << "; ";
			break;
		case CHAR:
			cout << cToken.lexeme << ":"
				 << "char"
				 << "; ";
			break;
		case null:
			cout << cToken.lexeme << ":";
			detectError(2);
			break;
		}
	}
	else if (";" != topToken().lexeme)
	{
		cout << "18" << endl;
		detectError(1);
	}
	else
	{
		popToken();
	}

	return;
}