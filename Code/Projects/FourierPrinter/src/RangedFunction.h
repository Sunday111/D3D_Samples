#pragma once

template<typename T>
class RangedFunction
{
public:
    virtual T Compute(T argument) const = 0;
    virtual T GetMinArgument() const = 0;
    virtual T GetMaxArgument() const = 0;
    virtual ~RangedFunction() = default;
};
