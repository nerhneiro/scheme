#pragma once

#include <memory>
#include <iostream>
#include <memory>

class Object : public std::enable_shared_from_this<Object> {
public:
    Object() = default;
    virtual ~Object() = default;
};

class Number : public Object {
public:
    Number(int64_t value) {
        number_ = value;
    }
    int64_t GetValue() const {
        return number_;
    }

private:
    int64_t number_;
};

class Symbol : public Object {
public:
    Symbol(const std::string& name) {
        name_ = name;
    }
    const std::string& GetName() const {
        return name_;
    }

private:
    std::string name_;
};

class OpenBrace : public Object {};

class CloseBrace : public Object {};

class Dot : public Object {};

class Quote : public Object {};

class Cell : public Object {
public:
    Cell() = default;
    Cell(std::shared_ptr<Object> f, std::shared_ptr<Object> s) : first_(f), second_(s) {
    }
    std::shared_ptr<Object> GetFirst() const {
        return first_;
    }
    std::shared_ptr<Object>& GetFirst() {
        return first_;
    }
    std::shared_ptr<Object> GetSecond() const {
        return second_;
    }
    std::shared_ptr<Object>& GetSecond() {
        return second_;
    }
    operator bool() {
        return first_ || second_;
    }

private:
    std::shared_ptr<Object> first_;
    std::shared_ptr<Object> second_;
};

///////////////////////////////////////////////////////////////////////////////

// Runtime type checking and convertion.
// This can be helpful: https://en.cppreference.com/w/cpp/memory/shared_ptr/pointer_cast

template <class T>
std::shared_ptr<T> As(const std::shared_ptr<Object>& obj) {
    if (auto ptr = dynamic_cast<typename std::shared_ptr<T>::element_type*>(obj.get())) {
        return std::shared_ptr<T>{obj, ptr};
    } else {
        return nullptr;
    }
}

template <class T>
bool Is(const std::shared_ptr<Object>& obj) {
    if (auto ptr = dynamic_cast<typename std::shared_ptr<T>::element_type*>(obj.get())) {
        return true;
    } else {
        return false;
    }
}
