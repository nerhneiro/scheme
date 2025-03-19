

#include <parser.h>
#include <memory>
#include "error.h"

std::shared_ptr<Object> Read1(Tokenizer* tokenizer, bool& in_quote, int& open, int& closed);

std::shared_ptr<Object> ReadList(Tokenizer* tokenizer, bool& from_quote, int& open, int& closed) {
    std::shared_ptr<Object> obj;
    std::shared_ptr<Object> ret_element;
    bool require_to_ret = false;
    if (!tokenizer->IsEnd()) {
        while (!Is<CloseBrace>(obj)) {
            obj = Read1(tokenizer, from_quote, open, closed);
            std::shared_ptr<Cell> pair = std::shared_ptr<Cell>(new Cell());

            if (Is<Dot>(obj)) {
                auto el = Read1(tokenizer, from_quote, open, closed);
                if ((el && Is<CloseBrace>(el))) {
                    throw SyntaxError("Empty element in pair");
                }
                if (tokenizer->IsEnd()) {
                    throw SyntaxError("Incorrect Input");
                }

                Token t = tokenizer->GetLastToken();
                if (BracketToken* z = std::get_if<BracketToken>(&t)) {
                    if (*z != BracketToken::CLOSE) {
                        throw SyntaxError("Incorrect Input");
                    }
                } else {
                    throw SyntaxError("Incorrect Input");
                }
                auto el_next = Read1(tokenizer, from_quote, open, closed);
                if (Is<CloseBrace>(el_next)) {
                    tokenizer->Next();
                    ++closed;
                }
                //                ret_element = el;
                //                bool is_end = tokenizer->IsEnd();
                //                tokenizer->Next();
                //                if (!is_end && tokenizer->IsPeekEnd()) {
                //                    ++closed;
                //                }
                //                tokenizer->Next();
                if (from_quote) {
                    std::shared_ptr<Cell> el_ret = std::shared_ptr<Cell>(new Cell());
                    el_ret->GetFirst() = el;
                    el_ret->GetSecond() = std::shared_ptr<Symbol>(new Symbol(")"));
                    std::shared_ptr<Cell> p_ret = std::shared_ptr<Cell>(new Cell());
                    p_ret->GetFirst() = std::shared_ptr<Symbol>(new Symbol("."));
                    p_ret->GetSecond() = el_ret;
                    return p_ret;
                }
                return el;
            } else if (Is<OpenBrace>(obj)) {
                require_to_ret = true;
                ret_element = ReadList(tokenizer, from_quote, open, closed);
            } else if (Is<CloseBrace>(obj)) {
                tokenizer->Next();
                ++closed;
                if (!from_quote) {
                    return nullptr;
                } else {
                    return std::shared_ptr<Object>(new Symbol(")"));
                }
            } else if (Is<Symbol>(obj)) {
                pair->GetFirst() = obj;
                pair->GetSecond() = ReadList(tokenizer, from_quote, open, closed);
                require_to_ret = true;
                ret_element = pair;
                return ret_element;
            } else if (Is<Quote>(obj)) {
                pair->GetFirst() = obj;
                int op = 0;
                int cl = 0;
                bool fr_q = true;
                pair->GetSecond() = ReadList(tokenizer, fr_q, op, cl);
                require_to_ret = true;
                ret_element = pair;
                return ret_element;
            } else if (Is<Number>(obj)) {
                pair->GetFirst() = obj;
                auto sec = ReadList(tokenizer, from_quote, open, closed);
                pair->GetSecond() = sec;
                require_to_ret = true;
                ret_element = pair;
                if (open == closed) {
                    from_quote = false;
                }
                return ret_element;
            } else if (Is<Cell>(obj)) {
                pair->GetFirst() = obj;
                pair->GetSecond() = ReadList(tokenizer, from_quote, open, closed);
                require_to_ret = true;
                ret_element = pair;
            } else if (!obj) {
                pair->GetFirst() = nullptr;
                pair->GetSecond() = ReadList(tokenizer, from_quote, open, closed);
                require_to_ret = true;
                ret_element = pair;
                if (!tokenizer->IsEnd()) {
                    auto el_next = Read1(tokenizer, from_quote, open, closed);
                    if (Is<CloseBrace>(el_next)) {
                        tokenizer->Next();
                        ++closed;
                    }
                }
            }
            if (require_to_ret) {
                tokenizer->Next();
                return ret_element;
            }
            tokenizer->Next();
            if (tokenizer->IsEnd()) {
                throw SyntaxError("Incorrect Input");
            }
            obj = Read(tokenizer);
        }
    }
    if (tokenizer->IsEnd()) {
        Token t = tokenizer->GetLastToken();
        if (BracketToken* z = std::get_if<BracketToken>(&t)) {
            if (*z != BracketToken::CLOSE) {
                throw SyntaxError("Incorrect Input");
            } else {
                //                ++closed;
            }
        } else {
            throw SyntaxError("Incorrect Input");
        }
    }

    return nullptr;
}

std::shared_ptr<Object> Read1(Tokenizer* tokenizer, bool& in_quote, int& open, int& closed) {
    Token token = tokenizer->GetToken();
    if (tokenizer->IsEnd()) {
        throw SyntaxError("Incorrect input");
    }
    if (ConstantToken* x = std::get_if<ConstantToken>(&token)) {
        auto ptr = std::shared_ptr<Object>(new Number(x->value));
        tokenizer->Next();
        return ptr;
    } else if (SymbolToken* y = std::get_if<SymbolToken>(&token)) {
        if (y->name == "quote") {
            auto ptr = std::shared_ptr<Object>(new Quote());
            tokenizer->Next();
            if (tokenizer->IsEnd()) {
                throw SyntaxError("Invalid \'");
            }
            return ptr;
        } else {
            auto ptr = std::shared_ptr<Object>(new Symbol(y->name));
            tokenizer->Next();
            return ptr;
        }
    } else if (QuoteToken* p = std::get_if<QuoteToken>(&token)) {
        //        throw SyntaxError("Apostroth");
        auto ptr = std::shared_ptr<Object>(new Quote());
        tokenizer->Next();
        if (tokenizer->IsEnd()) {
            throw SyntaxError("Invalid \'");
        }
        std::shared_ptr<Object> pair = std::shared_ptr<Cell>(new Cell());
        As<Cell>(pair)->GetFirst() = ptr;
        int op = 0;
        int cl = 0;
        bool fr_q = true;
        //        As<Cell>(pair)->GetSecond() = ReadList(tokenizer, fr_q, op, cl);
        auto el = Read1(tokenizer, fr_q, op, cl);
        As<Cell>(pair)->GetSecond() = el;
        return pair;
    } else if (BracketToken* z = std::get_if<BracketToken>(&token)) {
        if (*z == BracketToken::OPEN) {
            tokenizer->Next();
            ++open;
            if (in_quote) {
                auto ptr = std::shared_ptr<Object>(new Symbol("("));
                std::shared_ptr<Object> pair = std::shared_ptr<Cell>(new Cell());
                As<Cell>(pair)->GetFirst() = ptr;
                //                bool fr_q = true;
                //                ++open;
                As<Cell>(pair)->GetSecond() = ReadList(tokenizer, in_quote, open, closed);
                if (closed == open) {
                    in_quote = false;
                }
                return pair;
            }
            return ReadList(tokenizer, in_quote, open, closed);
        } else if (*z == BracketToken::CLOSE) {
            //            --closed;
            return std::shared_ptr<Object>(new CloseBrace());
        }
    } else if (DotToken* a = std::get_if<DotToken>(&token)) {
        tokenizer->Next();
        return std::shared_ptr<Object>(new Dot());
    }

    return nullptr;
}

std::shared_ptr<Object> Read(Tokenizer* tokenizer) {
    bool in_quote = false;
    int open = 0;
    int closed = 0;
    std::shared_ptr<Object> obj = Read1(tokenizer, in_quote, open, closed);
    //    if (open != closed) {
    //        throw SyntaxError("There are " + std::to_string(open) + " ( and " + std::to_string(closed) + " ).");
    //    }
    if (!tokenizer->IsEnd()) {
        throw SyntaxError("Error");
    }
    return obj;
}
