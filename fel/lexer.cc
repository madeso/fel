#include "lexer.h"

#include <sstream>

#include "fel/log.h"

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


            X(Not);
            X(BitNot);
            X(And);
            X(Or);
            X(BitAnd);
            X(BitOr);

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
            X(KeywordReturn);

            X(Int);
            X(Number);
            
            X(EndOfStream);

            #undef X
        default:
            return "<unknown>";
        }
    }


    Token::Token
    (
        TokenType t,
        const std::string& lex,
        std::shared_ptr<Object> lit,
        const Location& w
    )
        : type(t)
        , lexeme(lex)
        , literal(lit)
        , where(w)
    {
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



        bool
        PeekBlockComment(FilePointer* file)
        {
            return file->Peek() == '/' && file->Peek(2) == '*';
        }

        void
        EatBlockComment(FilePointer* file)
        {
            file->Read();

            while(file->Peek() !=0 && !(file->Peek() == '*' && file->Peek(2) == '/'))
            {
                if(PeekBlockComment(file))
                {
                    EatBlockComment(file);
                }
                else
                {
                    file->Read();
                }
            }
            file->Read();
            file->Read();
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
                else if(PeekBlockComment(file))
                {
                    EatBlockComment(file);
                }
                else
                {
                    return;
                }
            }
        }


        Token SingleChar(FilePointer* file, TokenType tt)
        {
            const auto location = file->location;
            const auto c = file->Read();
            return {tt, std::string(1, c), nullptr, location};
        }


        Token CharAndChar(FilePointer* file, TokenType one_char, char second_char, TokenType two_chars)
        {
            const auto location = file->location;
            auto first_char = std::string(1, file->Read());
            if(file->Peek() == second_char)
            {
                file->Read();
                return {two_chars, first_char + std::string(1, second_char), nullptr, location};
            }
            return {one_char, first_char, nullptr, location};
        }


        Token ParseString(Log* log, FilePointer* file)
        {
            const auto location = file->location;
            std::ostringstream buffer;
            auto end = file->Read();
            while(file->Peek() != end)
            {
                auto c = file->Read();
                if( c == 0)
                {
                    log->AddError(*file, log::Type::EosInString, {});
                    const auto str = buffer.str();
                    return {TokenType::String, str, Object::FromString(str), location};
                }
                if(c == '\\')
                {
                    // todo(Gustav): handle escape characters
                    c = file->Read();
                }

                buffer << c;
            }
            file->Read();
            const auto str = buffer.str();
            return {TokenType::String, str, Object::FromString(str), location};
        }
    }


    Token Lexer::GetNextToken()
    {
        EatWhitespace(&file);

        switch(file.Peek())
        {
        case 0:
            return {TokenType::EndOfStream, "", nullptr, file.location};

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

        case '!': return SingleChar(&file, TokenType::Not);
        case '~': return SingleChar(&file, TokenType::BitNot);
        case '&': return CharAndChar(&file, TokenType::BitAnd, '&', TokenType::And);
        case '|': return CharAndChar(&file, TokenType::BitOr, '|', TokenType::Or);
        
        case '"':
        case '\'':
            return ParseString(log, &file);
        

        default:
            if(IsAlpha(file.Peek()))
            {
                std::ostringstream buffer;
                const auto location = file.location;
                buffer << file.Read();
                while( IsAlpha(file.Peek()) || IsNumeric(file.Peek()) )
                {
                    buffer << file.Read();
                }
                auto s = buffer.str();
                if(s == "if") { return {TokenType::KeywordIf, s, nullptr, location}; }
                else if(s == "if") { return {TokenType::KeywordIf, s, nullptr, location}; }
                else if(s == "else") { return {TokenType::KeywordElse, s, nullptr, location}; }
                else if(s == "for") { return {TokenType::KeywordFor, s, nullptr, location}; }
                else if(s == "fun") { return {TokenType::KeywordFunction, s, nullptr, location}; }
                else if(s == "return") { return {TokenType::KeywordReturn, s, nullptr, location}; }
                else if(s == "var") { return {TokenType::KeywordVar, s, nullptr, location}; }
                else if(s == "true") { return {TokenType::KeywordTrue, s, nullptr, location}; }
                else if(s == "false") { return {TokenType::KeywordFalse, s, nullptr, location}; }
                else if(s == "null") { return {TokenType::KeywordNull, s, nullptr, location}; }
                else
                {
                    return {TokenType::Identifier, s, nullptr, location};
                }
                
            }
            else if(IsNumeric(file.Peek()))
            {
                const auto location = file.location;
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
                    return {TokenType::Number, s, Object::FromFloat(std::stof(s)), location};
                }
                auto s = buffer.str();
                return {TokenType::Int, s, Object::FromInt(std::stoi(s)), location};
            }
            else
            {
                const auto location = file.location;
                auto c = file.Read();
                const auto unknown_character = std::string{1, c};
                log->AddError(file, log::Type::UnknownCharacter, {unknown_character});
                return {TokenType::Unknown, unknown_character, nullptr, location};
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

