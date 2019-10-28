#include "lexer.h"

#include <sstream>

#include "log.h"

namespace fel
{
    std::string ToString(const TokenType tt)
    {
        switch(tt)
        {
            #define X(x) case TokenType::x: return #x

            X(Unknown);
            X(BeginBrace);
            X(EndBrace);
            X(OpenParen);
            X(CloseParen);
            X(OpenBracket);
            X(CloseBracket);
            
            X(Plus);
            X(Minus);
            X(Mult);
            X(Div);
            X(Comma);
            X(Colon);
            X(Term);

            X(Dot);
            X(DotDot);
            X(Equal);
            X(Assign);
            X(Less);
            X(LessEqual);
            X(Greater);
            X(GreaterEqual);

            X(String);

            X(Identifier);

            X(KeywordIf);
            X(KeywordElse);
            X(KeywordFor);
            X(KeywordFunction);
            X(KeywordVar);
            X(KeywordTrue);
            X(KeywordFalse);
            X(KeywordNull);

            X(Int);
            X(Number);
            
            X(EndOfStream);

            #undef X
        default:
            return "<unknown>";
        }
    }

    Lexer::Lexer(const File& a_file, Log* a_log) : file(a_file), log(a_log)
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
                    file->Read();
                    file->Read();
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

        Token CharAndChar(FilePointer* file, TokenType one_char, char second_char, TokenType two_chars)
        {
            auto first_char = std::string(1, file->Read());
            if(file->Peek() == second_char)
            {
                file->Read();
                return {two_chars, first_char + std::string(1, second_char)};
            }
            return {one_char, first_char};
        }

        Token ParseString(Log* log, FilePointer* file)
        {
            std::ostringstream buffer;
            auto end = file->Read();
            while(file->Peek() != end)
            {
                auto c = file->Read();
                if( c == 0)
                {
                    // todo(Gustav): add error
                    log->AddError(*file, log::Type::EosInString, {});
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
        case ':': return SingleChar(&file, TokenType::Colon);
        case ';': return SingleChar(&file, TokenType::Term);

        case '.': return CharAndChar(&file, TokenType::Dot, '.', TokenType::DotDot);
        case '=': return CharAndChar(&file, TokenType::Assign, '=', TokenType::Equal);
        case '<': return CharAndChar(&file, TokenType::Less, '=', TokenType::LessEqual);
        case '>': return CharAndChar(&file, TokenType::Greater, '=', TokenType::GreaterEqual);
            file.Read();
            if(file.Peek() == '=')
            {
                file.Read();
                return {TokenType::Equal, "=="};
            }
            return {TokenType::Assign, "="};
        
        case '"':
        case '\'':
            return ParseString(log, &file);
        

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

    LexerReader::LexerReader(const File& a_file, Log* a_log)
        : lexer(a_file, a_log)
    {
    }

    Token LexerReader::Peek()
    {
        if(token)
        {
            return *token;
        }
        else
        {
            token = lexer.GetNextToken();
            return *token;
        }
    }

    Token LexerReader::Read()
    {
        if(token)
        {
            const auto r = *token;
            token.reset();
            return r;
        }
        else
        {
            return lexer.GetNextToken();
        }
    }

    std::vector<Token> GetAllTokensInFile(LexerReader* reader)
    {
        std::vector<Token> ret;
        while( reader->Peek().type != TokenType::EndOfStream)
        {
            ret.emplace_back(reader->Read());
        }
        return ret;
    }


}
