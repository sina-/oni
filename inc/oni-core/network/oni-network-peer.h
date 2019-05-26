#pragma once

#include <cassert>
#include <map>
#include <functional>

#include <oni-core/common/oni-common-typedef.h>
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
    namespace network {
        struct Address {
            std::string host;
            common::u16 port;
        };

        class Peer {
        protected:
            Peer();

            Peer(const Address *address,
                 common::u8 peerCount,
                 common::u8 channelLimit,
                 common::u32 incomingBandwidth,
                 common::u32 outgoingBandwidth);

        public:
            virtual ~Peer();

            void
            poll();

            void
            flush();

            void
            registerPacketHandler(PacketType type,
                                  std::function<
                                          void(const common::PeerID &,
                                               const std::string &)> &&handler);

            void
            registerPostDisconnectHook(std::function<void(const common::PeerID &)> &&handler);

            common::r32
            getDownloadKBPS() const;

            common::r32
            getUploadKBPS() const;

        protected:
            virtual void
            handle(ENetPeer *peer,
                   common::u8 *data,
                   common::size size,
                   PacketType header) = 0;

            common::PeerID
            getPeerID(const ENetPeer &peer) const;

            PacketType
            getHeader(const common::u8 *data) const;

            // TODO: Add support for different types of send modes, for example unreliable, or none allocating packets
            void
            send(PacketType type,
                 std::string &data,
                 ENetPeer *peer);

            void
            send(const common::u8 *data,
                 common::size size,
                 ENetPeer *peer);

            void
            broadcast(PacketType type,
                      std::string &data);

            virtual void
            postConnectHook(const ENetEvent *event) = 0;

            virtual void
            postDisconnectHook(const ENetEvent *event) = 0;

        protected:
            ENetHost *mEnetHost{};
            std::map<common::PeerID, ENetPeer *> mPeers{};
            std::map<
                    PacketType, std::function<
                            void(common::PeerID,
                                 const std::string &)>> mPacketHandlers{};
            std::function<void(const common::PeerID &)> mPostDisconnectHook{};

        private:
            common::u64 mTotalDownload{}; // Number of bytes received
            common::u64 mTotalUpload{}; // Number of bytes sent

            utils::Timer mDownloadTimer{};
            utils::Timer mUploadTimer{};

            common::r32 mDownloadBPS{};
            common::r32 mUploadBPS{};
        };
    }
}
