#pragma once

#include <variant>
#include <optional>
#include <istream>
#include <vector>
#include <regex>

struct SymbolToken {
    std::string name;

    SymbolToken(const std::string& s) {
        name = s;
    }
    SymbolToken(const char* s) {
        name = s;
    }
    SymbolToken(char s) {
        name.push_back(s);
    }
    bool operator==(const SymbolToken& other) const {
        if (this->name == other.name) {
            return true;
        }
        return false;
    }
};

struct QuoteToken {
    bool operator==(const QuoteToken&) const {
        return true;
    }
};

struct DotToken {
    bool operator==(const DotToken&) const {
        return true;
    }
};

enum class BracketToken { OPEN, CLOSE };

struct ConstantToken {
    int value;
    ConstantToken(int num) {
        value = num;
    }
    bool operator==(const ConstantToken& other) const {
        if (this->value == other.value) {
            return true;
        }
        return false;
    }
};

using Token = std::variant<ConstantToken, BracketToken, SymbolToken, QuoteToken, DotToken>;

class Tokenizer {
public:
    Tokenizer(std::istream* in) {
        in_ = in;
    }

    bool IsEnd() {
        if (token_processed_) {
            Next();
        }
        return is_end_;
    }

    void Next() {
        char symb;
        if (!is_end_ && in_->get(symb)) {
            if (symb == ' ' || symb == '\n') {
                Next();
            } else if (symb == '(') {
                tokens_.push_back(BracketToken::OPEN);
            } else if (symb == ')') {
                tokens_.push_back(BracketToken::CLOSE);
            } else if (symb == '\'') {
                tokens_.push_back(QuoteToken());
            } else if (symb == '.') {
                tokens_.push_back(DotToken());
            } else {
                std::string s;
                s.push_back(symb);
                if (std::regex_match(s, std::regex("[0-9]"))) {
                    char el = in_->peek();
                    while (el) {
                        std::string check;
                        check.push_back(el);
                        if (std::regex_match(check, std::regex("[0-9]"))) {
                            s.push_back(el);
                            in_->get(symb);
                            el = in_->peek();
                        } else {
                            break;
                        }
                    }
                    tokens_.push_back(ConstantToken(std::stoi(s)));
                } else if (std::regex_match(s, std::regex("[a-zA-Z<=>*/#]"))) {
                    int el = in_->peek();
                    while (el) {
                        std::string check;
                        check.push_back(el);
                        if (std::regex_match(check, std::regex("[a-zA-Z<=>*/#0-9?!-]"))) {
                            s.push_back(el);
                            in_->get(symb);
                            el = in_->peek();
                        } else {
                            break;
                        }
                    }
                    tokens_.push_back(SymbolToken(s));
                } else if (symb == '+') {
                    char el = in_->peek();
                    if (!el) {
                        tokens_.push_back(SymbolToken('+'));
                    } else {
                        std::string check;
                        check.push_back(el);
                        if (!std::regex_match(check, std::regex("[0-9]"))) {
                            tokens_.push_back(SymbolToken('+'));
                        } else {
                            while (el) {
                                check[0] = el;
                                if (std::regex_match(check, std::regex("[0-9]"))) {
                                    s.push_back(el);
                                    in_->get(symb);
                                    el = in_->peek();
                                } else {
                                    break;
                                }
                            }
                            tokens_.push_back(ConstantToken(std::stoi(s)));
                        }
                    }
                } else if (symb == '-') {
                    char el = in_->peek();
                    if (!el) {
                        tokens_.push_back(SymbolToken('-'));
                    } else {
                        std::string check;
                        check.push_back(el);
                        if (!std::regex_match(check, std::regex("[0-9]"))) {
                            tokens_.push_back(SymbolToken('-'));
                        } else {
                            while (el) {
                                check[0] = el;
                                if (std::regex_match(check, std::regex("[0-9]"))) {
                                    s.push_back(el);
                                    in_->get(symb);
                                    el = in_->peek();
                                } else {
                                    break;
                                }
                            }
                            tokens_.push_back(ConstantToken(std::stoi(s)));
                        }
                    }
                }
            }
            token_processed_ = false;
        } else {
            is_end_ = true;
        }
    }

    Token GetToken() {
        if (token_processed_) {
            Next();
        }
        return tokens_[tokens_.size() - 1];
    }

private:
    std::istream* in_;
    std::vector<Token> tokens_;
    bool token_processed_ = true;
    bool is_end_ = false;
};