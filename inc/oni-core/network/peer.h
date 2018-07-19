#pragma once

#include <cassert>
#include <sstream>

#include <enet/enet.h>
#include <cereal/archives/portable_binary.hpp>
#include <cereal/types/string.hpp>

#include <oni-core/common/typedefs.h>
#include <oni-core/network/packet.h>

struct _ENetAddress;
typedef struct _ENetAddress ENetAddress;

struct _ENetHost;
typedef struct _ENetHost ENetHost;

struct _ENetPeer;
typedef struct _ENetPeer ENetPeer;

struct _ENetPacket;
typedef struct _ENetPacket ENetPacket;

namespace oni {
    namespace network {
        struct Address {
            std::string host;
            common::uint16 port;
        };


        class Peer {
        protected:
            Peer();

            Peer(const Address *address, common::uint8 peerCount,
                 common::uint8 channelLimit,
                 common::uint32 incomingBandwidth,
                 common::uint32 outgoingBandwidth);

        public:
            virtual ~Peer();

            void poll();

        protected:
            virtual void handle(ENetEvent *event) = 0;

            PacketType getHeader(const common::uint8 *data) const;

            template<class T>
            std::string serialize(T data) {
                std::ostringstream storage;
                {
                    cereal::PortableBinaryOutputArchive output{storage};

                    output(data);
                }

                return storage.str();
            }

            template<class T>
            T deserialize(const common::uint8 *data) {
                std::istringstream storage(reinterpret_cast<const char *>(data));

                T result;
                {
                    cereal::PortableBinaryInputArchive input{storage};
                    input(result);
                }

                return result;
            }


            // TODO: Add support for different types of send modes, for example unreliable, or none allocating packets
            void send(PacketType type, const std::string &data, ENetPeer *peer);

            void send(const common::uint8 *data, size_t size, ENetPeer *peer);

        protected:
            ENetHost *mEnetHost;
        };
    }
}
