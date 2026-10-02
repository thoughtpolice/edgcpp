#include "ownership.hpp"
#include <cstdio>
#include <utility>

namespace {
int run() {
  using namespace ownership_abi;
  if (owners_alive() != 0) return 1;
  Carrier carrier;
  if (static_cast<const void*>(static_cast<const MemberCarrier*>(&carrier)) ==
      static_cast<const void*>(&carrier)) return 2;

  const MemberPointers peer_members = member_pointers();
  if (peer_members.data == nullptr || peer_members.add == nullptr ||
      peer_members.scale == nullptr) return 3;
  if (carrier.*peer_members.data != 11 || (carrier.*peer_members.add)(5) != 16 ||
      (carrier.*peer_members.scale)(5) != 58) return 4;
  carrier.*peer_members.data = 13;
  if (carrier.value != 13 || carrier.prefix != 7 || carrier.bias != 3 ||
      (carrier.*peer_members.add)(5) != 18 || (carrier.*peer_members.scale)(5) != 68) return 5;
  carrier.value = 11;

  const SharedAddresses addresses = shared_addresses();
  if (addresses.value != &shared_value || addresses.local != &shared_local() ||
      addresses.template_value != &shared_template_value<int> ||
      addresses.template_local != &shared_template_local<int>() ||
      addresses.bump != &bump_shared) return 6;
  if (shared_value != 19 || shared_local() != 23 || shared_template_value<int> != 29 ||
      shared_template_local<int>() != 31) return 7;
  if (addresses.bump(10) != 29) return 8;
  shared_local() += 10;
  shared_template_value<int> += 10;
  shared_template_local<int>() += 10;
  const int shared_sum = advance_shared();
  if (shared_sum != 152 || shared_value != 30 || shared_local() != 35 ||
      shared_template_value<int> != 42 || shared_template_local<int>() != 45) return 9;

  {
    Packet discarded = make_packet("discard", 9);
    if (!discarded.owner || discarded.owner->value != 10 || owners_alive() != 1) return 10;
  }
  if (owners_alive() != 0) return 11;

  const std::string original = "origin";
  Packet first = make_packet(original, 40);
  if (original != "origin" || first.label != "origin/peer" || first.stamp != 40 ||
      !first.owner || first.owner->value != 41 || owners_alive() != 1) return 12;
  Owned* original_owner = first.owner.get();
  const MemberPointers local_members = {
      &MemberCarrier::value, &MemberCarrier::add, &MemberCarrier::scale};
  Packet second = transform_packet(std::move(first), carrier, local_members);
  if (first.owner || second.owner.get() != original_owner || owners_alive() != 1) return 13;
  if (second.label != "origin/peer/done" || second.owner->value != 126 ||
      second.stamp != 50) return 14;
  const int result = consume_packet(std::move(second));
  if (second.owner || owners_alive() != 0 || result != 192) return 15;
  std::printf("interop-ownership result=%d shared=%d live=%d\n",
              result, shared_sum, owners_alive());
  return 0;
}
}

int main() {
  try {
    return run();
  } catch (int error) {
    return error;
  } catch (...) {
    return 102;
  }
}
