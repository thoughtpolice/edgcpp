#ifndef INTEROP_OWNERSHIP_HPP
#define INTEROP_OWNERSHIP_HPP

#include <memory>
#include <string>

namespace ownership_abi {

int owners_alive();
struct Owned {
  int value;
  explicit Owned(int value);
  ~Owned();
  Owned(const Owned&) = delete;
  Owned& operator=(const Owned&) = delete;
};

// Aggregate with nontrivial, move-only members: return and parameter ABI differ
// from trivial register aggregates and the contained string uses the C++ ABI.
struct Packet {
  std::string label;
  std::unique_ptr<Owned> owner;
  int stamp;
};

struct Prefix {
  int prefix;
  explicit Prefix(int value);
  virtual ~Prefix();
};
struct MemberCarrier {
  int value;
  explicit MemberCarrier(int value);
  virtual ~MemberCarrier();
  int add(int amount) const;
  virtual int scale(int factor) const;
};
struct Carrier final : Prefix, MemberCarrier {
  int bias;
  Carrier();
  ~Carrier() override;
  int scale(int factor) const override;
};
struct MemberPointers {
  int Carrier::*data;
  int (Carrier::*add)(int) const;
  int (Carrier::*scale)(int) const;
};

inline int shared_value = 19;
inline int& shared_local() {
  static int value = 23;
  return value;
}
template<class T>
inline T shared_template_value = T(29);
template<class T>
inline T& shared_template_local() {
  static T value = T(31);
  return value;
}
inline int bump_shared(int amount) {
  shared_value += amount;
  return shared_value;
}

struct SharedAddresses {
  int* value;
  int* local;
  int* template_value;
  int* template_local;
  int (*bump)(int);
};

MemberPointers member_pointers();
SharedAddresses shared_addresses();
int advance_shared();
Packet make_packet(std::string label, int start);
Packet transform_packet(Packet packet, const Carrier& carrier, MemberPointers members);
int consume_packet(Packet packet);

}

#endif
