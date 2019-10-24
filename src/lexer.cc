#include "lexer.h"

#include <sstream>

namespace fel
{
    Lexer::Lexer(const File& a_file) : file(a_file)
    {
    }

    namespace
    {
        bool
        IsNewline(char c)
        {
            switch(c)
            {
            case '\r': case '\n':
                return true;
            default:
                return false;
            }
        }

        bool
        IsWhitespace(char c)
        {
            switch(c)
            {
            case ' ':
            case '\t':
            case '\r':
            case '\n':
                return true;

            default:
                return false;

            }
        }

        bool
        IsAlpha(char c)
        {
            if(c >='a' && c<='z') return true;
            if(c >='A' && c<='Z') return true;
            return c == '_';
        }

        bool
        IsNumeric(char c)
        {
            if(c >='0' && c<='9') return true;
            return false;
        }

        void
        EatWhitespace(FilePointer* file)
        {
            while(true)
            {
                if(IsWhitespace(file->Peek()))
                {
                    file->Read();
                }
                else if(file->Peek() == '/' && file->Peek(2) == '/')
                {
                    while(!IsNewline(file->Peek()) && file->Peek() != 0)
                    {
                        file->Read();
                    }
                }
                else if(file->Peek() == '/' && file->Peek(2) == '*')
                {
                    while(file->Peek() !=0 && !(file->Peek() == '*' && file->Peek(2) == '/'))
                    {
                        file->Read();
                    }
                }
                else
                {
                    return;
                }
            }
        }

        Token SingleChar(FilePointer* file, TokenType tt)
        {
            const auto c = file->Read();
            return {tt, std::string(1, c)};
        }

        Token CharAndEqual(FilePointer* file, TokenType one, TokenType two)
        {
            auto c = file->Read();
            auto s = std::string(1, c);
            if(file->Peek() == '=')
            {
                file->Read();
                return {two, s + "="};
            }
            return {one, s};
        }

        Token ParseString(FilePointer* file)
        {
            std::ostringstream buffer;
            auto end = file->Read();
            while(file->Peek() != end)
            {
                auto c = file->Read();
                if( c == 0)
                {
                    // todo(Gustav): add error
                    return {TokenType::String, buffer.str()};
                }
                if(c == '\\')
                {
                    // todo(Gustav): handle escape characters
                    c = file->Read();
                }

                buffer << c;
            }
            file->Read();
            return {TokenType::String, buffer.str()};
        }
    }

    Token Lexer::GetNextToken()
    {
        EatWhitespace(&file);

        switch(file.Peek())
        {
        case 0:
            return {TokenType::EndOfStream, ""};

        case '{': return SingleChar(&file, TokenType::BeginBrace);
        case '}': return SingleChar(&file, TokenType::EndBrace);
        case '(': return SingleChar(&file, TokenType::OpenParen);
        case ')': return SingleChar(&file, TokenType::CloseParen);
        case '[': return SingleChar(&file, TokenType::OpenBracket);
        case ']': return SingleChar(&file, TokenType::CloseBracket);
        case '+': return SingleChar(&file, TokenType::Plus);
        case '-': return SingleChar(&file, TokenType::Minus);
        case '*': return SingleChar(&file, TokenType::Mult);
        case '/': return SingleChar(&file, TokenType::Div);
        case ',': return SingleChar(&file, TokenType::Comma);
        case '.': return SingleChar(&file, TokenType::Dot);
        case ':': return SingleChar(&file, TokenType::Colon);
        case ';': return SingleChar(&file, TokenType::Term);

        case '=': return CharAndEqual(&file, TokenType::Assign, TokenType::Equal);
        case '<': return CharAndEqual(&file, TokenType::Less, TokenType::LessEqual);
        case '>': return CharAndEqual(&file, TokenType::Greater, TokenType::GreaterEqual);
            file.Read();
            if(file.Peek() == '=')
            {
                file.Read();
                return {TokenType::Equal, "=="};
            }
            return {TokenType::Assign, "="};
        
        case '"':
        case '\'':
            return ParseString(&file);
        

        default:
            if(IsAlpha(file.Peek()))
            {
                std::ostringstream buffer;
                buffer << file.Read();
                while( IsAlpha(file.Peek()) || IsNumeric(file.Peek()) )
                {
                    buffer << file.Read();
                }
                auto s = buffer.str();
                if(s == "if") { return {TokenType::KeywordIf, s}; }
                else if(s == "if") { return {TokenType::KeywordIf, s}; }
                else if(s == "else") { return {TokenType::KeywordElse, s}; }
                else if(s == "for") { return {TokenType::KeywordFor, s}; }
                else if(s == "function") { return {TokenType::KeywordFunction, s}; }
                else if(s == "var") { return {TokenType::KeywordVar, s}; }
                else if(s == "true") { return {TokenType::KeywordTrue, s}; }
                else if(s == "false") { return {TokenType::KeywordFalse, s}; }
                else if(s == "null") { return {TokenType::KeywordNull, s}; }
                else
                {
                    return {TokenType::Identifier, s};
                }
                
            }
            else if(IsNumeric(file.Peek()))
            {
                std::ostringstream buffer;
                buffer << file.Read();
                while( IsNumeric(file.Peek()) )
                {
                    buffer << file.Read();
                }
                if(file.Peek() == '.')
                {
                    buffer << file.Read();
                    
                    while( IsNumeric(file.Peek()) )
                    {
                        buffer << file.Read();
                    }
                    auto s = buffer.str();
                    return {TokenType::Number, s};
                }
                auto s = buffer.str();
                return {TokenType::Int, s};
            }
            else
            {
                auto c = file.Read();
                return {TokenType::Unknown, std::string{1, c}};
            }
        }
    }

}
