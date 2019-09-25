#pragma once

#include <cassert>
#include <map>
#include <functional>

#include <oni-core/common/oni-common-typedef.h>
#include <oni-core/network/oni-network-address.h>
#include <oni-core/network/oni-network-packet.h>
#include <oni-core/network/oni-network-packet-type.h>
#include <oni-core/util/oni-util-timer.h>


struct _ENetAddress;
typedef struct _ENetAddress ENetAddress;

struct _ENetHost;
typedef struct _ENetHost ENetHost;

struct _ENetPeer;
typedef struct _ENetPeer ENetPeer;

struct _ENetPacket;
typedef struct _ENetPacket ENetPacket;

struct _ENetEvent;
typedef struct _ENetEvent ENetEvent;

namespace oni {
    class Peer {
    protected:
        Peer();

        Peer(const Address *address,
             u8 peerCount,
             u8 channelLimit,
             u32 incomingBandwidth,
             u32 outgoingBandwidth);

    public:
        virtual ~Peer();

        void
        poll();

        void
        flush();

        void
        registerPacketHandler(PacketType type,
                              std::function<
                                      void(const std::string &,
                                           const std::string &)> &&handler);

        void
        registerPostDisconnectHook(std::function<void(const std::string &)> &&handler);

        r32
        getDownloadKBPS() const;

        r32
        getUploadKBPS() const;

    protected:
        virtual void
        handle(ENetPeer *peer,
               u8 *data,
               size size,
               PacketType header) = 0;

        std::string
        getPeerID(const ENetPeer &peer) const;

        PacketType
        getHeader(const u8 *data) const;

        // TODO: Add support for different types of send modes, for example unreliable, or none allocating packets
        void
        send(PacketType type,
             std::string &data,
             ENetPeer *peer);

        void
        send(const u8 *data,
             size size,
             ENetPeer *peer);

        void
        broadcast(PacketType type,
                  std::string &&data);

        virtual void
        postConnectHook(const ENetEvent *event) = 0;

        virtual void
        postDisconnectHook(const ENetEvent *event) = 0;

    protected:
        ENetHost *mEnetHost{};
        std::map<std::string, ENetPeer *> mPeers{};
        std::map<
                PacketType, std::function<
                        void(std::string,
                             const std::string &)>> mPacketHandlers{};
        std::function<void(const std::string &)> mPostDisconnectHook{};

    private:
        u64 mTotalDownload{}; // Number of bytes received
        u64 mTotalUpload{}; // Number of bytes sent

        Timer mDownloadTimer{};
        Timer mUploadTimer{};

        r32 mDownloadBPS{};
        r32 mUploadBPS{};
    };
}
