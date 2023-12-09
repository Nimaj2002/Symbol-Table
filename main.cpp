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
	VARIABLES,	 // variables
	ENDOFFILE,
	BOOL,
	FLOAT,
	INT,
	CHAR
};
// Token structure
struct Token
{
	TokenType id;
	string lexeme;
};
// storing tokens
deque<Token> TOKENS;

ifstream File;
int blockNumber, errorCode;
int lineNumber = 1;
// ------------------------------------

void detectError(int errorCode);
bool isCharNum(char ch);
void tokenLoader();
Token getToken();

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

	while (3 != errorCode)
	{
		Token token = getToken();
		cout << token.id << " -- " << token.lexeme << endl;
	}

	cout << lineNumber << endl;

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
		cout << "ND " << lineNumber << endl;
		exit(EXIT_FAILURE);
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
								token.id = VARIABLES; // variables
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
Token getToken()
{
returnToken:
{
	if (!TOKENS.empty())
	{
		Token t = TOKENS.front();
		if (t.id != ENDOFFILE) // checks if the last token is endoffile
		{
			detectError(3);
			TOKENS.pop_front();
			return t;
		}
		else
		{
			return t;
		}
	}
	else
	{
		tokenLoader();
		goto returnToken;
	}
}
}