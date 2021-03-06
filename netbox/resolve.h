// ------------------------------------------------------------
// Copyright 2018-present Sergey Kovalevich <inndie@gmail.com>
// ------------------------------------------------------------

#ifndef KSERGEY_resolve_270618163249
#define KSERGEY_resolve_270618163249

#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <iterator>
#include <memory>
#include <string_view>

#include <netbox/compiler.h>
#include <netbox/Protocol.h>

namespace netbox {

/// Result of `resolve` function call
class AddressResolveResult
{
private:
    class Endpoint final
    {
    private:
        /* Resolver endpoint data */
        const addrinfo* data_{nullptr};

    public:
        Endpoint(const addrinfo* data)
            : data_(data)
        {}

        explicit operator bool() const noexcept
        {
            return data_ != nullptr;
        }

        int domain() const noexcept
        {
            return data_->ai_family;
        }

        int type() const noexcept
        {
            return data_->ai_socktype;
        }

        int protocol() const noexcept
        {
            return data_->ai_protocol;
        }

        const sockaddr* data() const noexcept
        {
            return data_->ai_addr;
        }

        socklen_t size() const noexcept
        {
            return data_->ai_addrlen;
        }
    };

    class Iterator final
        : std::iterator< std::forward_iterator_tag, Endpoint >
    {
    private:
        const addrinfo* data_{nullptr};

    public:
        using reference = Endpoint;

        Iterator(const addrinfo* data)
            : data_{data}
        {}

        Iterator& operator++() noexcept
        {
            if (data_ != nullptr) {
                data_ = data_->ai_next;
            }
            return *this;
        }

        bool operator==(const Iterator& it) const noexcept
        {
            return data_ == it.data_;
        }

        bool operator!=(const Iterator& it) const noexcept
        {
            return data_ != it.data_;
        }

        reference operator*() const noexcept
        {
            return {data_};
        }
    };

    // `addrinfo` deallocator
    struct Deleter
    {
        void operator()(addrinfo* a) const noexcept
        {
            if (a) {
                freeaddrinfo(a);
            }
        }
    };

    // Smart pointer for `addrinfo`
    using AddrinfoPtr = std::unique_ptr< addrinfo, Deleter >;

    AddrinfoPtr data_;

public:
    using value_type = Endpoint;
    using const_iterator = Iterator;

    AddressResolveResult() = default;
    ~AddressResolveResult() = default;

    /// Construct result
    AddressResolveResult(addrinfo* data)
        : data_{data}
    {}

    /// Return iterator for the first result
    const_iterator begin() const noexcept
    {
        return {data_.get()};
    }

    /// Return iterator for the next after last result
    const_iterator end() const noexcept
    {
        return {nullptr};
    }

    /// Check result valid
    explicit operator bool() const noexcept
    {
        return data_.operator bool();
    }
};

inline std::string toString(AddressResolveResult::value_type value)
{
    char storage[INET_ADDRSTRLEN];
    // TODO: there is should be networkToHost
    auto result = inet_ntop(value.domain(), value.data(), storage, sizeof(storage));
    if (result == nullptr) {
        return {};
    }
    return storage;
}

/// Resolve address
inline AddressResolveResult resolve(const Protocol& protocol, const char* address, const char* port) noexcept
{
    addrinfo hints{};
    hints.ai_family = protocol.domain;
    hints.ai_socktype = protocol.type;
    hints.ai_protocol = protocol.protocol;

    addrinfo* data{};
    getaddrinfo(address, port, &hints, &data);

    return {data};
}

/// @overload
inline AddressResolveResult resolve(const Protocol& protocol, std::string_view address, std::string_view port) noexcept
{
    return resolve(protocol, std::string{address}.c_str(), std::string{port}.c_str());
}

/// @overload
inline AddressResolveResult resolve(const Protocol& protocol, std::string_view address) noexcept
{
    std::size_t found = address.rfind(':');
    return (found != std::string::npos)
        ? resolve(protocol, address.substr(0, found), address.substr(found + 1))
        : resolve(protocol, std::string{address}.c_str(), nullptr);
}

} // namespace netbox

#endif /* KSERGEY_resolve_270618163249 */
