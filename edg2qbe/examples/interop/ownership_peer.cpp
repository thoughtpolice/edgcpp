#include "ownership.hpp"
#include <utility>

namespace ownership_abi {
namespace {
int live_owners = 0;
}
int owners_alive() { return live_owners; }
Owned::Owned(int initial) : value(initial) { ++live_owners; }
Owned::~Owned() { --live_owners; }

Prefix::Prefix(int initial) : prefix(initial) {}
Prefix::~Prefix() = default;
MemberCarrier::MemberCarrier(int initial) : value(initial) {}
MemberCarrier::~MemberCarrier() = default;
int MemberCarrier::add(int amount) const { return value + amount; }
int MemberCarrier::scale(int factor) const { return value * factor; }
Carrier::Carrier() : Prefix(7), MemberCarrier(11), bias(3) {}
Carrier::~Carrier() = default;
int Carrier::scale(int factor) const { return value * factor + bias; }

MemberPointers member_pointers() {
  return {&MemberCarrier::value, &MemberCarrier::add, &MemberCarrier::scale};
}
SharedAddresses shared_addresses() {
  return {&shared_value, &shared_local(), &shared_template_value<int>,
          &shared_template_local<int>(), &bump_shared};
}
int advance_shared() {
  bump_shared(1);
  shared_local() += 2;
  shared_template_value<int> += 3;
  shared_template_local<int>() += 4;
  return shared_value + shared_local() + shared_template_value<int> +
         shared_template_local<int>();
}

Packet make_packet(std::string label, int start) {
  label += "/peer";
  return {std::move(label), std::make_unique<Owned>(start + 1), start};
}
Packet transform_packet(Packet packet, const Carrier& carrier, MemberPointers members) {
  if (!packet.owner) throw 101;
  packet.label += "/done";
  packet.owner->value += carrier.*members.data;
  packet.owner->value += (carrier.*members.add)(5);
  packet.owner->value += (carrier.*members.scale)(5);
  packet.stamp += carrier.prefix + carrier.bias;
  return packet;
}
int consume_packet(Packet packet) {
  if (!packet.owner) return -1;
  if (packet.label != "origin/peer/done" || packet.stamp != 50) return -2;
  return packet.owner->value + packet.stamp + static_cast<int>(packet.label.size());
}
}
