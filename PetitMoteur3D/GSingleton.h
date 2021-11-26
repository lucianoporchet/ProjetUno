#pragma once
class Incopiable {
    Incopiable(const Incopiable&) = delete;
    Incopiable& operator=(const Incopiable&) = delete;
protected:
    Incopiable() = default;
    ~Incopiable() = default;
};