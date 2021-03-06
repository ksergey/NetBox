// ------------------------------------------------------------
// Copyright 2017-present Sergey Kovalevich <inndie@gmail.com>
// ------------------------------------------------------------

#ifndef KSERGEY_result_160918002953
#define KSERGEY_result_160918002953

#include <netbox/ErrorCode.h>
#include <netbox/Socket.h>

namespace netbox {

/// Hold result of socket operation (i.e. setsockopt, listen, etc...)
class OpResult
    : public ErrorCode
{
protected:
    int value_{-1};

public:
    /// Construct undefined result
    constexpr OpResult() = default;

    /// Construct result with native result value
    constexpr OpResult(int value) noexcept
        : ErrorCode{value != 0 ? errno : 0}
        , value_{value}
    {}

    /// Return true if operation was success
    explicit operator bool() const noexcept
    {
        return value_ == 0;
    }
};

/// Hold result of transmission operation (send/recv family)
class TransmitResult
    : public ErrorCode
{
private:
    ssize_t value_{-1};

public:
    /// Construct undefined result
    constexpr TransmitResult() = default;

    /// Construct result with native result value
    constexpr TransmitResult(ssize_t value) noexcept
        : ErrorCode{value == -1 ? errno : 0}
        , value_{value}
    {}

    /// Return true if operation was success
    explicit operator bool() const noexcept
    {
        return value_ > 0;
    }

    /// Return transmitted bytes count
    constexpr std::size_t bytes() const noexcept
    {
        return value_;
    }

    /// Return true if connection closed (stream oriented sockets only)
    /// @warning Also could be true if recv into buffer of size 0
    constexpr bool isClosed() const noexcept
    {
        return value_ == 0;
    }
};

/// Hold result of accept or connect
/// @see `man 2 accept`
class SocketResult
    : public ErrorCode
{
private:
    Socket socket_;

public:
    /// Construct undefined result
    SocketResult() = default;

    /// Construct result with result of accept
    SocketResult(int value)
        : ErrorCode{value == BadFd ? errno : 0}
        , socket_{value}
    {}

    /// @overload
    SocketResult(Socket&& socket)
        : ErrorCode{!socket_.operator bool() ? errno : 0}
        , socket_{std::move(socket)}
    {}

    /// Return true if operation was success
    explicit operator bool() const noexcept
    {
        return socket_.operator bool();
    }

    /// Return just accepted socket
    [[nodiscard]] Socket&& getSocket() noexcept
    {
        return std::move(socket_);
    }
};

} /* namespace netbox */

#endif /* KSERGEY_result_160918002953 */
