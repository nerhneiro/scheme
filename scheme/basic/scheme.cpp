#include "scheme.h"
#include "parser.h"
#include "object.h"
#include <sstream>
#include <regex>
#include <algorithm>
#include <cmath>

std::string IsDigit(const std::string& res) {
    if (res[0] == '-') {
        for (size_t i = 1; i < res.size(); ++i) {
            if (res[i] > '9' || res[i] < '0') {
                return "#f";
            }
        }
        if (res.size() == 1) {
            return "#f";
        }
        return "#t";
    } else {
        for (size_t i = 0; i < res.size(); ++i) {
            if (res[i] > '9' || res[i] < '0') {
                return "#f";
            }
        }
        return "#t";
    }
}

std::string ReviveSeq(std::shared_ptr<Object> obj) {
    std::string s;
    if (Is<Cell>(obj)) {
        std::string s1, s2;
        int neighs = 0;
        if (As<Cell>(obj)->GetFirst() && As<Cell>(obj)->GetSecond() &&
            Is<Number>(As<Cell>(obj)->GetFirst()) && Is<Number>(As<Cell>(obj)->GetSecond())) {
            return std::to_string(As<Number>(As<Cell>(obj)->GetFirst())->GetValue()) + " . " +
                   std::to_string(As<Number>(As<Cell>(obj)->GetSecond())->GetValue());
        }
        if (As<Cell>(obj)->GetFirst()) {
            ++neighs;
            s1 = ReviveSeq(As<Cell>(obj)->GetFirst());
        }
        if (As<Cell>(obj)->GetSecond()) {
            ++neighs;
            s2 = ReviveSeq(As<Cell>(obj)->GetSecond());
        }
        if (!s2.empty()) {
            if (s1 != "(" && s1 != ")" && s2 != "(" && s2 != ")") {
                s = s1 + " " + s2;
            } else {
                s = s1 + s2;
            }
            //            if (IsDigit(s1) == "#t") {
            //                s = s1 + " " + s2;
            //            } else if (s1 != "(" && s1 != ")") {
            //                else {
            //                    s = s1 + s2;
            //                }
            //            }
        } else {
            s = s1;
        }
        //        if (neighs == 2) {
        //            s = "(" + s + ")";
        //        }
        //        if (neighs == 0) {
        //            throw RuntimeError("No neighs");
        //        }
        return s;

    } else {
        if (Is<Number>(obj)) {
            return std::to_string(As<Number>(obj)->GetValue());
        } else if (Is<Symbol>(obj)) {
            return As<Symbol>(obj)->GetName();
        } else {
            return "";
        }
    }
}

bool CheckForList(std::shared_ptr<Object> obj, int& depth) {
    if (!obj) {
        return false;
    }
    if (Is<Cell>(obj) && Is<Number>(As<Cell>(obj)->GetFirst())) {
        ++depth;
        if (!As<Cell>(obj)->GetSecond()) {
            return true;
        } else {
            return CheckForList(As<Cell>(obj)->GetSecond(), depth);
        }
    } else {
        return false;
    }
}

bool CheckForPair(std::shared_ptr<Object> obj) {
    int depth = 0;
    bool is_list = CheckForList(obj, depth);
    if (depth == 2) {
        return true;
    }
    if (!obj) {
        return false;
    }
    if (Is<Cell>(obj) && Is<Number>(As<Cell>(obj)->GetFirst()) &&
        Is<Number>(As<Cell>(obj)->GetSecond())) {
        return true;
    } else {
        return false;
    }
}

bool CheckForNull(std::shared_ptr<Object> obj) {
    if (!obj) {
        return true;
    }
    if (Is<Cell>(obj) && !As<Cell>(obj)->GetFirst()) {
        return true;
    }
    return false;
}

void BuildList(std::string& list, std::shared_ptr<Object> obj) {
    if (!obj) {
        return;
    }
    if (Is<Cell>(obj) && Is<Number>(As<Cell>(obj)->GetFirst())) {
        if (list.empty()) {
            list += std::to_string(As<Number>(As<Cell>(obj)->GetFirst())->GetValue());
        } else {
            list = list + " " + std::to_string(As<Number>(As<Cell>(obj)->GetFirst())->GetValue());
        }
        if (As<Cell>(obj)->GetSecond()) {
            BuildList(list, As<Cell>(obj)->GetSecond());
        }
    }
}

std::string Evaluate(std::shared_ptr<Object> obj1, std::shared_ptr<Object> obj2 = nullptr) {
    if (!obj1 && !obj2) {
        throw RuntimeError("NULL");
    }
    if (!obj2) {
        if (Is<Number>(obj1)) {
            int64_t value = As<Number>(obj1)->GetValue();
            return std::to_string(value);
        } else if (Is<Symbol>(obj1)) {
            if (As<Symbol>(obj1)->GetName() == "=") {
                return "#t";
            } else if (As<Symbol>(obj1)->GetName() == "<") {
                return "#t";
            } else if (As<Symbol>(obj1)->GetName() == ">") {
                return "#t";
            } else if (As<Symbol>(obj1)->GetName() == "<=") {
                return "#t";
            } else if (As<Symbol>(obj1)->GetName() == ">=") {
                return "#t";
            } else if (As<Symbol>(obj1)->GetName() == "#t") {
                return "#t";
            } else if (As<Symbol>(obj1)->GetName() == "#f") {
                return "#f";
            } else if (As<Symbol>(obj1)->GetName() == "+") {
                return std::to_string(0);
            } else if (As<Symbol>(obj1)->GetName() == "*") {
                return std::to_string(1);
            } else if (As<Symbol>(obj1)->GetName() == "/") {
                throw RuntimeError("Incorrect / operation");
            } else if (As<Symbol>(obj1)->GetName() == "-") {
                throw RuntimeError("Incorrect - operation");
            } else if (As<Symbol>(obj1)->GetName() == "*") {
                throw RuntimeError("Incorrect * operation");
            } else if (As<Symbol>(obj1)->GetName() == "/") {
                throw RuntimeError("Incorrect / operation");
            } else if (As<Symbol>(obj1)->GetName() == "max") {
                throw RuntimeError("Empty max operation");
            } else if (As<Symbol>(obj1)->GetName() == "min") {
                throw RuntimeError("Empty min operation");
            } else if (As<Symbol>(obj1)->GetName() == "abs") {
                throw RuntimeError("Empty abs operation");
            } else if (As<Symbol>(obj1)->GetName() == "not") {
                throw RuntimeError("Empty not operation");
            } else if (As<Symbol>(obj1)->GetName() == "and") {
                return "#t";
            } else if (As<Symbol>(obj1)->GetName() == "or") {
                return "#f";
            } else if (As<Symbol>(obj1)->GetName() == "quote" ||
                       As<Symbol>(obj1)->GetName() == "\'") {
                throw RuntimeError("Invalid quote");
            } else if (As<Symbol>(obj1)->GetName() == "list") {
                return "()";
            }
        } else if (Is<Cell>(obj1)) {
            std::string res = Evaluate(As<Cell>(obj1)->GetFirst(), As<Cell>(obj1)->GetSecond());
            return res;
        }
    } else {
        if (Is<Symbol>(obj1)) {
            if (As<Symbol>(obj1)->GetName() == "+") {
                std::string res1;
                if (!Is<Cell>(obj2)) {
                    if (Is<Number>(obj2)) {
                        res1 = Evaluate(obj2);
                    } else {
                        throw RuntimeError("You can sum only numbers");
                    }
                } else {
                    res1 = Evaluate(As<Cell>(obj2)->GetFirst());
                }
                if (Is<Cell>(obj2) && As<Cell>(obj2)->GetSecond() &&
                    Is<Cell>(As<Cell>(obj2)->GetSecond())) {
                    std::shared_ptr<Object> ptr = std::shared_ptr<Cell>(new Cell());
                    As<Cell>(ptr)->GetFirst() = std::shared_ptr<Object>(new Symbol("+"));
                    As<Cell>(ptr)->GetSecond() = As<Cell>(obj2)->GetSecond();
                    std::string res3 = Evaluate(ptr);
                    if (IsDigit(res1) == "#t" && IsDigit(res3) == "#t") {
                        return std::to_string(std::stoi(res1) + std::stoi(res3));
                    } else {
                        throw RuntimeError("You can't sum non-digit operands");
                    }
                } else if (Is<Cell>(obj2) && As<Cell>(obj2)->GetSecond()) {
                    if (Is<Number>(As<Cell>(obj2)->GetSecond())) {
                        std::string res3 = Evaluate(As<Cell>(obj2)->GetSecond());
                        return std::to_string(std::stoi(res1) + std::stoi(res3));
                    }
                } else {
                    if (IsDigit(res1) == "#t") {
                        return res1;
                    }
                    throw RuntimeError("You can't sum non-digit operands");
                }
            } else if (As<Symbol>(obj1)->GetName() == "-") {
                if (Is<Cell>(obj2)) {
                    std::string res1 = Evaluate(As<Cell>(obj2)->GetFirst());
                    std::string res2;
                    if (As<Cell>(obj2)->GetSecond() && Is<Cell>(As<Cell>(obj2)->GetSecond())) {
                        std::shared_ptr<Object> ptr = std::shared_ptr<Cell>(new Cell());
                        As<Cell>(ptr)->GetFirst() = std::shared_ptr<Object>(new Symbol("+"));
                        As<Cell>(ptr)->GetSecond() = As<Cell>(obj2)->GetSecond();
                        std::string res3 = Evaluate(ptr);
                        if (IsDigit(res1) == "#t" && IsDigit(res3) == "#t") {
                            return std::to_string(std::stoi(res1) - std::stoi(res3));
                        } else {
                            throw RuntimeError("You can't sum non-digit operands");
                        }
                    } else if (As<Cell>(obj2)->GetSecond()) {
                        if (Is<Number>(As<Cell>(obj2)->GetSecond())) {
                            std::string res3 = Evaluate(As<Cell>(obj2)->GetSecond());
                            if (IsDigit(res1) == "#t" && IsDigit(res3) == "#t") {
                                return std::to_string(std::stoi(res1) - std::stoi(res3));
                            } else {
                                throw RuntimeError("You can't sum non-digit operands");
                            }
                        }
                    } else {
                        if (IsDigit(res1) == "#t") {
                            return res1;
                        }
                        throw RuntimeError("You can't substruct non-digit operands");
                    }
                }
                throw RuntimeError("Invalid operands for -");
            } else if (As<Symbol>(obj1)->GetName() == "*") {
                std::string res1 = Evaluate(As<Cell>(obj2)->GetFirst());
                std::string res2;
                if (As<Cell>(obj2)->GetSecond() && Is<Cell>(As<Cell>(obj2)->GetSecond())) {
                    std::shared_ptr<Object> ptr = std::shared_ptr<Cell>(new Cell());
                    As<Cell>(ptr)->GetFirst() = std::shared_ptr<Object>(new Symbol("*"));
                    As<Cell>(ptr)->GetSecond() = As<Cell>(obj2)->GetSecond();
                    std::string res3 = Evaluate(ptr);
                    if (IsDigit(res1) == "#t" && IsDigit(res3) == "#t") {
                        return std::to_string(std::stoi(res1) * std::stoi(res3));
                    } else {
                        throw RuntimeError("You can't multiply non-digit operands");
                    }
                } else if (As<Cell>(obj2)->GetSecond()) {
                    if (Is<Number>(As<Cell>(obj2)->GetSecond())) {
                        std::string res3 = Evaluate(As<Cell>(obj2)->GetSecond());
                        if (IsDigit(res1) == "#t" && IsDigit(res3) == "#t") {
                            return std::to_string(std::stoi(res1) * std::stoi(res3));
                        } else {
                            throw RuntimeError("You can't multiply non-digit operands");
                        }
                    }
                } else {
                    if (IsDigit(res1) == "#t") {
                        return res1;
                    }
                    throw RuntimeError("You can't substruct non-digit operands");
                }
            } else if (As<Symbol>(obj1)->GetName() == "/") {
                std::string res1 = Evaluate(As<Cell>(obj2)->GetFirst());
                std::string res2;
                if (As<Cell>(obj2)->GetSecond() && Is<Cell>(As<Cell>(obj2)->GetSecond())) {
                    std::shared_ptr<Object> ptr = std::shared_ptr<Cell>(new Cell());
                    As<Cell>(ptr)->GetFirst() = std::shared_ptr<Object>(new Symbol("*"));
                    As<Cell>(ptr)->GetSecond() = As<Cell>(obj2)->GetSecond();
                    std::string res3 = Evaluate(ptr);
                    if (IsDigit(res1) == "#t" && IsDigit(res3) == "#t") {
                        return std::to_string(std::stoi(res1) / std::stoi(res3));
                    } else {
                        throw RuntimeError("You can't divide non-digit operands");
                    }
                } else if (As<Cell>(obj2)->GetSecond()) {
                    if (Is<Number>(As<Cell>(obj2)->GetSecond())) {
                        std::string res3 = Evaluate(As<Cell>(obj2)->GetSecond());
                        if (IsDigit(res1) == "#t" && IsDigit(res3) == "#t") {
                            return std::to_string(std::stoi(res1) / std::stoi(res3));
                        } else {
                            throw RuntimeError("You can't divide non-digit operands");
                        }
                    }
                } else {
                    if (IsDigit(res1) == "#t") {
                        return res1;
                    }
                    throw RuntimeError("You can't substruct non-digit operands");
                }
            }
            if (As<Symbol>(obj1)->GetName() == "cons") {
                std::string res1, res2;
                if (Is<Cell>(obj2)) {
                    if (As<Cell>(obj2)->GetFirst() && As<Cell>(obj2)->GetSecond()) {
                        res1 = Evaluate(As<Cell>(obj2)->GetFirst());
                        res2 = Evaluate(As<Cell>(obj2)->GetSecond());
                        std::string res = "(" + res1 + " . " + res2 + ")";
                        return res;
                    } else {
                        throw RuntimeError("Invalid arguments for cons");
                    }
                } else {
                    throw RuntimeError("Invalid arguments for cons");
                }
            }
            if (As<Symbol>(obj1)->GetName() == "car") {
                std::string res1, res2;
                if (Is<Cell>(obj2)) {
                    if (As<Cell>(obj2)->GetFirst() && Is<Cell>(As<Cell>(obj2)->GetFirst()) &&
                        Is<Quote>(As<Cell>(As<Cell>(obj2)->GetFirst())->GetFirst())) {
                        std::string string = Evaluate(As<Cell>(As<Cell>(obj2)->GetFirst()));
                        if (string == "()") {
                            throw RuntimeError("Invalid arguments for car");
                        }
                        try {
                            std::stringstream ss{string};
                            Tokenizer tokenizer{&ss};
                            auto obj = Read(&tokenizer);
                            std::shared_ptr<Object> el = obj;
                            while (Is<Cell>(el) && As<Cell>(el)->GetFirst()) {
                                el = As<Cell>(el)->GetFirst();
                            }
                            std::string ans = Evaluate(el);
                            return ans;
                        } catch (...) {
                            return string;
                        }
                    } else {
                        throw RuntimeError("Invalid arguments for car");
                    }
                } else {
                    return Evaluate(obj2);
                }
            }
            if (As<Symbol>(obj1)->GetName() == "cdr") {
                std::string res1, res2;
                if (Is<Cell>(obj2)) {
                    if (As<Cell>(obj2)->GetFirst()) {
                        if (As<Cell>(obj2)->GetFirst() && Is<Cell>(As<Cell>(obj2)->GetFirst()) &&
                            Is<Quote>(As<Cell>(As<Cell>(obj2)->GetFirst())->GetFirst())) {
                            std::string string = Evaluate(As<Cell>(As<Cell>(obj2)->GetFirst()));
                            if (string == "()") {
                                throw RuntimeError("Invalid arguments for car");
                            }
                            try {
                                std::stringstream ss{string};
                                Tokenizer tokenizer{&ss};
                                auto obj = Read(&tokenizer);
                                std::shared_ptr<Object> el = obj;
                                int depth = 0;
                                bool is_list = CheckForList(obj, depth);
                                bool is_pair = CheckForPair(obj);
                                if (is_list) {
                                    if (Is<Cell>(el) && !Is<Cell>(As<Cell>(el)->GetFirst())) {
                                        if (!As<Cell>(el)->GetSecond()) {
                                            return "()";
                                        }
                                    }
                                    std::string ans = ReviveSeq(As<Cell>(el)->GetSecond());
                                    return "(" + ans + ")";
                                } else if (is_pair) {
                                    if (Is<Cell>(el) && !Is<Cell>(As<Cell>(el)->GetFirst())) {
                                        if (!As<Cell>(el)->GetSecond()) {
                                            return "()";
                                        }
                                        return ReviveSeq(As<Cell>(el)->GetSecond());
                                    } else {
                                        return "()";
                                    }
                                } else {
                                    if (Is<Cell>(el) && !Is<Cell>(As<Cell>(el)->GetFirst())) {
                                        if (!As<Cell>(el)->GetSecond()) {
                                            return "()";
                                        }
                                    }
                                    std::string ans = ReviveSeq(As<Cell>(el)->GetSecond());
                                    return "(" + ans + ")";
                                }
                            } catch (...) {
                                return "()";
                            }
                        } else {
                            throw RuntimeError("Invalid arguments for car");
                        }
                    } else {
                        throw RuntimeError("Invalid arguments for car");
                    }
                } else {
                    return "()";
                }
            }
            if (As<Symbol>(obj1)->GetName() == "list-tail") {
                std::string res1, res2;
                if (Is<Cell>(obj2)) {
                    if (As<Cell>(obj2)->GetFirst()) {
                        if (As<Cell>(obj2)->GetFirst() && Is<Cell>(As<Cell>(obj2)->GetFirst()) &&
                            Is<Quote>(As<Cell>(As<Cell>(obj2)->GetFirst())->GetFirst())) {
                            std::string string = Evaluate(As<Cell>(As<Cell>(obj2)->GetFirst()));
                            std::string num = Evaluate(As<Cell>(As<Cell>(obj2)->GetSecond()));
                            if (string == "()") {
                                throw RuntimeError("Invalid arguments for list-tail");
                            }
                            std::stringstream ss{string};
                            Tokenizer tokenizer{&ss};
                            auto obj = Read(&tokenizer);
                            int depth = 0;
                            bool is_list = CheckForList(obj, depth);
                            if (!is_list) {
                                throw RuntimeError("Invalid arguments for list-tail");
                            }
                            std::shared_ptr<Object> el = obj;
                            int el_count = 0;
                            while (Is<Cell>(el) && As<Cell>(el)->GetSecond() &&
                                   el_count < std::stoi(num)) {
                                ++el_count;
                                el = As<Cell>(el)->GetSecond();
                            }
                            if (el_count + 1 < std::stoi(num) && Is<Cell>(el) &&
                                !As<Cell>(el)->GetSecond()) {
                                throw RuntimeError("Invalid arguments for list-tail");
                            } else if (el_count < std::stoi(num) && Is<Cell>(el) &&
                                       !As<Cell>(el)->GetSecond()) {
                                return "()";
                            }
                            std::string list;
                            BuildList(list, el);
                            return "(" + list + ")";
                        } else {
                            throw RuntimeError("Invalid arguments for list-tail");
                        }
                    } else {
                        throw RuntimeError("Invalid arguments for list-tail");
                    }
                } else {
                    return "()";
                }
            } else if (As<Symbol>(obj1)->GetName() == "max") {
                std::string res1 = Evaluate(As<Cell>(obj2)->GetFirst());
                std::string res2;
                if (As<Cell>(obj2)->GetSecond() && Is<Cell>(As<Cell>(obj2)->GetSecond())) {
                    std::shared_ptr<Object> ptr = std::shared_ptr<Cell>(new Cell());
                    As<Cell>(ptr)->GetFirst() = std::shared_ptr<Object>(new Symbol("max"));
                    As<Cell>(ptr)->GetSecond() = As<Cell>(obj2)->GetSecond();
                    std::string res3 = Evaluate(ptr);
                    if (IsDigit(res1) == "#t" && IsDigit(res3) == "#t") {
                        return std::to_string(std::max(std::stoi(res1), std::stoi(res3)));
                    } else {
                        throw RuntimeError("You can't max non-digit operands");
                    }
                } else if (As<Cell>(obj2)->GetSecond()) {
                    if (Is<Number>(As<Cell>(obj2)->GetSecond())) {
                        std::string res3 = Evaluate(As<Cell>(obj2)->GetSecond());
                        if (IsDigit(res1) == "#t" && IsDigit(res3) == "#t") {
                            return std::to_string(std::max(std::stoi(res1), std::stoi(res3)));
                        } else {
                            throw RuntimeError("You can't max non-digit operands");
                        }
                    }
                } else {
                    if (IsDigit(res1) == "#t") {
                        return res1;
                    }
                    throw RuntimeError("You can't sum non-digit operands");
                }
            } else if (As<Symbol>(obj1)->GetName() == "min") {
                std::string res1 = Evaluate(As<Cell>(obj2)->GetFirst());
                std::string res2;
                if (As<Cell>(obj2)->GetSecond() && Is<Cell>(As<Cell>(obj2)->GetSecond())) {
                    std::shared_ptr<Object> ptr = std::shared_ptr<Cell>(new Cell());
                    As<Cell>(ptr)->GetFirst() = std::shared_ptr<Object>(new Symbol("min"));
                    As<Cell>(ptr)->GetSecond() = As<Cell>(obj2)->GetSecond();
                    std::string res3 = Evaluate(ptr);
                    if (IsDigit(res1) == "#t" && IsDigit(res3) == "#t") {
                        return std::to_string(std::min(std::stoi(res1), std::stoi(res3)));
                    } else {
                        throw RuntimeError("You can't min non-digit operands");
                    }
                } else if (As<Cell>(obj2)->GetSecond()) {
                    if (Is<Number>(As<Cell>(obj2)->GetSecond())) {
                        std::string res3 = Evaluate(As<Cell>(obj2)->GetSecond());
                        if (IsDigit(res1) == "#t" && IsDigit(res3) == "#t") {
                            return std::to_string(std::min(std::stoi(res1), std::stoi(res3)));
                        } else {
                            throw RuntimeError("You can't min non-digit operands");
                        }
                    }
                } else {
                    if (IsDigit(res1) == "#t") {
                        return res1;
                    }
                    throw RuntimeError("You can't sum non-digit operands");
                }
            } else if (As<Symbol>(obj1)->GetName() == "abs") {
                std::string res1 = Evaluate(As<Cell>(obj2)->GetFirst());
                if (As<Cell>(obj2)->GetSecond()) {
                    throw RuntimeError("To complex query for abs");
                }
                if (IsDigit(res1) == "#t") {
                    return std::to_string(std::abs(std::stoi(res1)));
                }
                throw RuntimeError("Incorrect query for abs");
            } else if (As<Symbol>(obj1)->GetName() == "number?") {
                std::string res;
                if (Is<Cell>(obj2)) {
                    res = Evaluate(As<Cell>(obj2)->GetFirst(), As<Cell>(obj2)->GetSecond());
                } else {
                    res = Evaluate(obj2);
                }
                if (!res.empty()) {
                    return IsDigit(res);
                }
                throw RuntimeError("Empty number? query");
            } else if (As<Symbol>(obj1)->GetName() == "pair?") {
                std::string res = Evaluate(obj2);
                if (!res.empty()) {
                    try {
                        std::stringstream ss{res};
                        Tokenizer tokenizer{&ss};
                        auto obj = Read(&tokenizer);
                        bool is_pair = CheckForPair(obj);
                        if (is_pair) {
                            return "#t";
                        }
                        return "#f";
                    } catch (...) {
                        return "#f";
                    }
                }
                throw RuntimeError("Empty number? query");
            } else if (As<Symbol>(obj1)->GetName() == "null?") {
                std::string res = Evaluate(obj2);
                if (!res.empty()) {
                    try {
                        std::stringstream ss{res};
                        Tokenizer tokenizer{&ss};
                        auto obj = Read(&tokenizer);
                        bool is_null = CheckForNull(obj);
                        if (is_null) {
                            return "#t";
                        }
                        return "#f";
                    } catch (...) {
                        return "#f";
                    }
                }
                return "#t";
            } else if (As<Symbol>(obj1)->GetName() == "list?") {
                std::string res = Evaluate(obj2);
                if (!res.empty()) {
                    try {
                        std::stringstream ss{res};
                        Tokenizer tokenizer{&ss};
                        auto obj = Read(&tokenizer);
                        bool is_null = CheckForNull(obj);
                        int depth = 0;
                        bool is_list = CheckForList(obj, depth);
                        if (is_null || is_list) {
                            return "#t";
                        }
                        return "#f";
                    } catch (...) {
                        return "#f";
                    }
                }
                return "#t";
                throw RuntimeError("Empty number? query");
            } else if (As<Symbol>(obj1)->GetName() == "list") {
                std::string list;
                BuildList(list, obj2);
                return "(" + list + ")";
            } else if (As<Symbol>(obj1)->GetName() == "list-ref") {
                if (Is<Cell>(obj2)) {
                    if (As<Cell>(obj2)->GetFirst() && Is<Cell>(As<Cell>(obj2)->GetFirst()) &&
                        Is<Quote>(As<Cell>(As<Cell>(obj2)->GetFirst())->GetFirst())) {
                        std::string string = Evaluate(As<Cell>(As<Cell>(obj2)->GetFirst()));
                        std::string num = Evaluate(As<Cell>(As<Cell>(obj2)->GetSecond()));
                        if (IsDigit(num) == "#f") {
                            throw RuntimeError("Invalid arguments for list-ref");
                        }
                        if (string == "()") {
                            throw RuntimeError("Invalid arguments for list-ref");
                        }
                        std::stringstream ss{string};
                        Tokenizer tokenizer{&ss};
                        auto obj = Read(&tokenizer);
                        int depth = 0;
                        bool is_list = CheckForList(obj, depth);
                        if (!is_list) {
                            throw RuntimeError("Invalid arguments for list-ref");
                        }
                        std::shared_ptr<Object> el = obj;
                        int el_count = 0;
                        while (Is<Cell>(el) && As<Cell>(el)->GetSecond() &&
                               el_count < std::stoi(num)) {
                            ++el_count;
                            el = As<Cell>(el)->GetSecond();
                        }
                        if (el_count < std::stoi(num)) {
                            throw RuntimeError("Index ount of range in list-ref");
                        }
                        if (Is<Number>(el)) {
                            return Evaluate(el);
                        }
                        std::string ans = Evaluate(As<Cell>(el)->GetFirst());
                        return ans;

                    } else {
                        throw RuntimeError("Invalid arguments for list-ref");
                    }
                } else {
                    throw RuntimeError("Invalid arguments for list-ref");
                    ;
                }
            } else if (As<Symbol>(obj1)->GetName() == "boolean?") {
                std::string res;
                if (Is<Cell>(obj2)) {
                    res = Evaluate(As<Cell>(obj2)->GetFirst(), As<Cell>(obj2)->GetSecond());
                } else {
                    res = Evaluate(obj2);
                }
                if (res.empty()) {
                    throw RuntimeError("Empty boolean? query");
                }
                if (res == "#t" || res == "#f") {
                    return "#t";
                } else {
                    return "#f";
                }
            } else if (As<Symbol>(obj1)->GetName() == "not") {
                std::string res1;
                if (As<Cell>(obj2)) {
                    res1 = Evaluate(As<Cell>(obj2));
                }
                if (res1.empty()) {
                    throw RuntimeError("Empty boolean? query");
                }
                if (res1 == "()") {
                    return "#f";
                } else if (res1 == "#t") {
                    return "#f";
                } else if (res1 == "#f") {
                    return "#t";
                } else if (!res1.empty()) {
                    return "#f";
                } else {
                    throw RuntimeError("Empty boolean? query");
                }

            } else if (As<Symbol>(obj1)->GetName() == "and") {
                std::string res1 = Evaluate(As<Cell>(obj2)->GetFirst());
                if (res1 == "#f") {
                    return "#f";
                }
                if (As<Cell>(obj2)->GetSecond() && Is<Cell>(As<Cell>(obj2)->GetSecond())) {
                    std::shared_ptr<Object> ptr = std::shared_ptr<Cell>(new Cell());
                    As<Cell>(ptr)->GetFirst() = std::shared_ptr<Object>(new Symbol("and"));
                    As<Cell>(ptr)->GetSecond() = As<Cell>(obj2)->GetSecond();
                    std::string res3 = Evaluate(ptr);
                    if (res3 == "t") {
                        return "#t";
                    }
                    if (res3 == "#f") {
                        return "#f";
                    }
                    if (res3.empty()) {
                        throw RuntimeError("Incorrect and");
                    }
                    return res3;
                } else if (As<Cell>(obj2)->GetSecond()) {
                    if (Is<Number>(As<Cell>(obj2)->GetSecond())) {
                        std::string res3 = Evaluate(As<Cell>(obj2)->GetSecond());
                        if (res3 == "t") {
                            return "#t";
                        }
                        if (res3 == "#f") {
                            return "#f";
                        }
                        if (res3.empty()) {
                            throw RuntimeError("Incorrect and");
                        }
                        return res3;
                    }
                } else {
                    if (res1 == "#t" || res1 == "#f") {
                        return res1;
                    } else if (!res1.empty()) {
                        return res1;
                    }
                    throw RuntimeError("You can't sum non-digit operands");
                }
            } else if (As<Symbol>(obj1)->GetName() == "or") {
                std::string res1 = Evaluate(As<Cell>(obj2)->GetFirst());
                std::string res2;
                if (res1 == "#t") {
                    return "#t";
                }
                if (As<Cell>(obj2)->GetSecond()) {
                    std::string res3 = Evaluate(As<Cell>(obj2)->GetSecond());
                    if (res1 == "#t" || res3 == "#t") {
                        return "#t";
                    } else if (res1 == "#f" || res1 == "()") {
                        if (!res3.empty()) {
                            return res3;
                        }
                        return "#f";
                    }
                } else {
                    if (res1 == "#t" || res1 == "#f") {
                        return res1;
                    } else if (res1 == "()") {
                        return "#f";
                    } else if (!res1.empty()) {
                        return "#f";
                    }

                    throw RuntimeError("You can't sum non-digit operands");
                }
            } else if (As<Symbol>(obj1)->GetName() == "=") {
                if (Is<Cell>(obj2)) {
                    std::string res1 = Evaluate(As<Cell>(obj2)->GetFirst());
                    std::string res2;
                    if (As<Cell>(obj2)->GetSecond() && Is<Cell>(As<Cell>(obj2)->GetSecond())) {
                        res2 = Evaluate(As<Cell>(As<Cell>(obj2)->GetSecond())->GetFirst());
                    } else {
                        return "#t";
                    }
                    if (IsDigit(res1) == "#t" && IsDigit(res2) == "#f") {
                        throw RuntimeError("Incorrect input");
                    }
                    if (IsDigit(res1) == "#f" && IsDigit(res2) == "#t") {
                        throw RuntimeError("Incorrect input");
                    }
                    if (res1 == res2) {
                        std::shared_ptr<Object> ptr = std::shared_ptr<Cell>(new Cell());
                        As<Cell>(ptr)->GetFirst() = std::shared_ptr<Object>(new Symbol("="));
                        As<Cell>(ptr)->GetSecond() = As<Cell>(obj2)->GetSecond();
                        std::string res3 = Evaluate(ptr);
                        if (res3 == "#t") {
                            return "#t";
                        } else {
                            return "#f";
                        }
                    } else {
                        return "#f";
                    }
                } else {
                    throw SyntaxError("Boolean operation = must have two operants");
                }
            } else if (As<Symbol>(obj1)->GetName() == ">") {
                if (Is<Cell>(obj2)) {
                    std::string res1 = Evaluate(As<Cell>(obj2)->GetFirst());
                    std::string res2;
                    if (As<Cell>(obj2)->GetSecond() && Is<Cell>(As<Cell>(obj2)->GetSecond())) {
                        res2 = Evaluate(As<Cell>(As<Cell>(obj2)->GetSecond())->GetFirst());
                    } else {
                        return "#t";
                    }
                    if (IsDigit(res1) == "#t" && IsDigit(res2) == "#f") {
                        throw RuntimeError("Incorrect input");
                    }
                    if (IsDigit(res1) == "#f" && IsDigit(res2) == "#t") {
                        throw RuntimeError("Incorrect input");
                    }
                    if (IsDigit(res1) == "#t" && IsDigit(res2) == "#t") {
                        if (std::stoi(res1) > std::stoi(res2)) {
                            std::shared_ptr<Object> ptr = std::shared_ptr<Cell>(new Cell());
                            As<Cell>(ptr)->GetFirst() = std::shared_ptr<Object>(new Symbol(">"));
                            As<Cell>(ptr)->GetSecond() = As<Cell>(obj2)->GetSecond();
                            std::string res3 = Evaluate(ptr);
                            if (res3 == "#t") {
                                return "#t";
                            } else {
                                return "#f";
                            }
                        } else {
                            return "#f";
                        }
                    }
                }
            } else if (As<Symbol>(obj1)->GetName() == "<") {
                if (Is<Cell>(obj2)) {
                    std::string res1 = Evaluate(As<Cell>(obj2)->GetFirst());
                    std::string res2;
                    if (As<Cell>(obj2)->GetSecond() && Is<Cell>(As<Cell>(obj2)->GetSecond())) {
                        res2 = Evaluate(As<Cell>(As<Cell>(obj2)->GetSecond())->GetFirst());
                    } else {
                        return "#t";
                    }
                    if (IsDigit(res1) == "#t" && IsDigit(res2) == "#f") {
                        throw RuntimeError("Incorrect input");
                    }
                    if (IsDigit(res1) == "#f" && IsDigit(res2) == "#t") {
                        throw RuntimeError("Incorrect input");
                    }
                    if (IsDigit(res1) == "#t" && IsDigit(res2) == "#t") {
                        if (std::stoi(res1) < std::stoi(res2)) {
                            std::shared_ptr<Object> ptr = std::shared_ptr<Cell>(new Cell());
                            As<Cell>(ptr)->GetFirst() = std::shared_ptr<Object>(new Symbol("<"));
                            As<Cell>(ptr)->GetSecond() = As<Cell>(obj2)->GetSecond();
                            std::string res3 = Evaluate(ptr);
                            if (res3 == "#t") {
                                return "#t";
                            } else {
                                return "#f";
                            }
                        } else {
                            return "#f";
                        }
                    }
                }
            } else if (As<Symbol>(obj1)->GetName() == ">=") {
                if (Is<Cell>(obj2)) {
                    std::string res1 = Evaluate(As<Cell>(obj2)->GetFirst());
                    std::string res2;
                    if (As<Cell>(obj2)->GetSecond() && Is<Cell>(As<Cell>(obj2)->GetSecond())) {
                        res2 = Evaluate(As<Cell>(As<Cell>(obj2)->GetSecond())->GetFirst());
                    } else {
                        return "#t";
                    }
                    if (IsDigit(res1) == "#t" && IsDigit(res2) == "#f") {
                        throw RuntimeError("Incorrect input");
                    }
                    if (IsDigit(res1) == "#f" && IsDigit(res2) == "#t") {
                        throw RuntimeError("Incorrect input");
                    }
                    if (IsDigit(res1) == "#t" && IsDigit(res2) == "#t") {
                        if (std::stoi(res1) >= std::stoi(res2)) {
                            std::shared_ptr<Object> ptr = std::shared_ptr<Cell>(new Cell());
                            As<Cell>(ptr)->GetFirst() = std::shared_ptr<Object>(new Symbol(">="));
                            As<Cell>(ptr)->GetSecond() = As<Cell>(obj2)->GetSecond();
                            std::string res3 = Evaluate(ptr);
                            if (res3 == "#t") {
                                return "#t";
                            } else {
                                return "#f";
                            }
                        } else {
                            return "#f";
                        }
                    }
                }
            } else if (As<Symbol>(obj1)->GetName() == "<=") {
                if (Is<Cell>(obj2)) {
                    std::string res1 = Evaluate(As<Cell>(obj2)->GetFirst());
                    std::string res2;
                    if (As<Cell>(obj2)->GetSecond() && Is<Cell>(As<Cell>(obj2)->GetSecond())) {
                        res2 = Evaluate(As<Cell>(As<Cell>(obj2)->GetSecond())->GetFirst());
                    } else {
                        return "#t";
                    }
                    if (IsDigit(res1) == "#t" && IsDigit(res2) == "#f") {
                        throw RuntimeError("Incorrect input");
                    }
                    if (IsDigit(res1) == "#f" && IsDigit(res2) == "#t") {
                        throw RuntimeError("Incorrect input");
                    }
                    if (IsDigit(res1) == "#t" && IsDigit(res2) == "#t") {
                        if (std::stoi(res1) <= std::stoi(res2)) {
                            std::shared_ptr<Object> ptr = std::shared_ptr<Cell>(new Cell());
                            As<Cell>(ptr)->GetFirst() = std::shared_ptr<Object>(new Symbol("<="));
                            As<Cell>(ptr)->GetSecond() = As<Cell>(obj2)->GetSecond();
                            std::string res3 = Evaluate(ptr);
                            if (res3 == "#t") {
                                return "#t";
                            } else {
                                return "#f";
                            }
                        } else {
                            return "#f";
                        }
                    }
                }
            }
        } else if (Is<Quote>(obj1)) {
            std::string seq = ReviveSeq(obj2);
            try {
                std::stringstream ss{seq};
                Tokenizer tokenizer{&ss};
                auto obj = Read(&tokenizer);
                int depth = 0;
                bool is_list = CheckForList(obj, depth);
                if (!is_list) {
                    return seq;
                }
                std::string list;
                BuildList(list, obj);
                list = "(" + list + ")";
                return list;
            } catch (...) {
                return seq;
            }
            return seq;
        }
    }
    throw RuntimeError("Incorrect query");
}

std::string Interpreter::Run(const std::string& s) {
    int open = 0, closed = 0;
    for (size_t i = 0; i < s.size(); ++i) {
        if (i > 0 && (s[i - 1] == '(' && s[i] == '.')) {
            throw SyntaxError("Incorrect pair . operator");
        }
        if (s[i] == '(') {
            ++open;
        } else if (s[i] == ')') {
            ++closed;
        }
    }
    if (open != closed) {
        throw SyntaxError("There are " + std::to_string(open) + " ( and " + std::to_string(closed) +
                          " ).");
    }
    std::stringstream ss{s};
    Tokenizer tokenizer{&ss};
    auto obj = Read(&tokenizer);
    int depth = 0;
    bool is_list = CheckForList(obj, depth);
    if (is_list) {
        throw RuntimeError("Can't evaluate a list");
    }
    if (Is<Cell>(obj)) {
        std::string res = Evaluate(As<Cell>(obj));
        return res;
    } else {
        return Evaluate(obj);
    }
}
