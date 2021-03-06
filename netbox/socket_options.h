// ------------------------------------------------------------
// Copyright 2017-present Sergey Kovalevich <inndie@gmail.com>
// ------------------------------------------------------------

#ifndef KSERGEY_socket_options_160918003043
#define KSERGEY_socket_options_160918003043

#include <netbox/result.h>
#include <netbox/Socket.h>
#include <netbox/details/socket_options.h>

namespace netbox {

/// Set socket option
template< class Option >
inline OpResult setOption(Socket& socket, const Option& option) noexcept
{
    return setsockopt(socket.native(), option.level(), option.name(), option.data(), option.size());
}

/// Get socket option
template< class Option >
inline OpResult getOption(Socket& socket, Option& option) noexcept
{
    socklen_t size = option.size();
    OpResult result = getsockopt(socket.native(), option.level(), option.name(), option.data(), &size);
    if (NETBOX_LIKELY(result)) {
        option.resize(size);
    }
    return result;
}

/// Options `namespace`
struct Options
{
    /// Socket options
    struct Socket
    {
        using ReuseAddr = details::BooleanOption< SOL_SOCKET, SO_REUSEADDR >;
        using Timestamp = details::BooleanOption< SOL_SOCKET, SO_TIMESTAMP >;
        using TimestampNS = details::BooleanOption< SOL_SOCKET, SO_TIMESTAMPNS >;
        using RcvBuf = details::IntegerOption< SOL_SOCKET, SO_RCVBUF >;
        using SndBuf = details::IntegerOption< SOL_SOCKET, SO_SNDBUF >;
    };

    /// Packet options
    struct Packet
    {
        using Timestamp = details::IntegerOption< SOL_PACKET, PACKET_TIMESTAMP >;
        using Version = details::IntegerOption< SOL_PACKET, PACKET_VERSION >;
        using RxRing = details::StructOption< SOL_PACKET, PACKET_RX_RING, tpacket_req& >;
    };

    /// TCP options
    struct TCP
    {
        using NoDelay = details::BooleanOption< IPPROTO_TCP, TCP_NODELAY >;
    };

    /// Multicast options
    struct Multicast
    {
        using JoinGroup = details::MulticastRequestOption<
            IPPROTO_IP, IP_ADD_MEMBERSHIP, IPPROTO_IPV6, IPV6_JOIN_GROUP
        >;

        using LeaveGroup = details::MulticastRequestOption<
            IPPROTO_IP, IP_DROP_MEMBERSHIP, IPPROTO_IPV6, IPV6_LEAVE_GROUP
        >;

        using JoinGroupSource = details::MulticastSourceRequestOption<
            IPPROTO_IP, IP_ADD_SOURCE_MEMBERSHIP
        >;

        using LeaveGroupSource = details::MulticastSourceRequestOption<
            IPPROTO_IP, IP_DROP_SOURCE_MEMBERSHIP
        >;
    };
};

} /* namespace netbox */

#endif /* KSERGEY_socket_options_160918003043 */
