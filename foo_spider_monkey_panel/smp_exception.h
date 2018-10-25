#pragma once

#include <stdexcept>

namespace smp
{

/// @brief This exception is used when JS exception is not set
class SmpException
    : public std::runtime_error
{
public:
    explicit SmpException( const std::string& errorText )
        : std::runtime_error( errorText )
    {}

    virtual ~SmpException() = default;

    static void ExpectTrue( bool checkValue, std::string_view errorMessage );
};

/// @brief This exception is used when JS exception is set
class JsException
    : public std::exception
{
public:
    JsException() = default;
    virtual ~JsException() = default;
};

} // namespace smp