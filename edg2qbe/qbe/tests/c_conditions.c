int masked_condition(unsigned long bits, const int *index) {
  if (bits & (1UL << *index)) return 1;
  return 0;
}
int while_condition(unsigned long bits) {
  int count = 0;
  while (bits) { ++count; bits = 0; }
  return count;
}
int for_condition(unsigned long bits) {
  int count = 0;
  for (; bits; bits = 0) ++count;
  return count;
}
int do_condition(unsigned long bits) {
  int count = 0;
  do { if (++count == 2) bits = 0; } while (bits);
  return count;
}
int float_condition(double value) {
  if (value) return 1;
  return 0;
}
int wide_condition(unsigned __int128 value) {
  if (value) return 1;
  return 0;
}
int first_static(void) { static int value = 11; return ++value; }
int second_static(void) { static int value = 21; return ++value; }
int generic_increment(int value) { return value + 1; }
int generic_decrement(int value) { return value - 1; }
int generic_selection(void) {
  int selector = 0, selected = 0, unselected = 0;
  int result = _Generic(selector++, int: ++selected, default: ++unselected);
  if (result != 1 || selector != 0 || selected != 1 || unselected != 0) return 0;
  _Generic(selected, int: selected, default: unselected) = 42;
  int *address = &_Generic(selected, int: selected, default: unselected);
  int (*callback)(int) = _Generic(selected, int: generic_increment,
                                 default: generic_decrement);
  return address == &selected && selected == 42 && unselected == 0 &&
         callback(selected) == 43;
}
int c_nothrow_function(int value) __attribute__((nothrow));
int c_nothrow_function(int value) {
  int (*volatile callback)(int) = generic_increment;
  return callback(value);
}
int scoped_vla(int columns) {
  int evaluations = 0;
  int outer[3][columns++ + evaluations++];
  if (evaluations != 1 || sizeof outer != 3 * (columns - 1) * sizeof(int)) return 0;
  outer[2][columns - 2] = 12345;
  volatile unsigned char *persistent = 0;
  for (int iteration = 0; iteration < 12000; ++iteration) {
    volatile unsigned char scratch[16384 + (iteration & 15)];
    scratch[0] = (unsigned char)iteration;
    if (!iteration) {
      persistent = __builtin_alloca(32);
      persistent[0] = 73;
      persistent[31] = 91;
    }
    int inner[columns];
    inner[columns - 1] = iteration;
    if (inner[columns - 1] != iteration || scratch[0] != (unsigned char)iteration) return 0;
    if (iteration & 1) continue;
  }
  return persistent[0] == 73 && persistent[31] == 91 &&
         outer[2][columns - 2] == 12345;
}
int main(void) {
  for (int index = 0; index < 64; ++index) {
    unsigned long bit = 1UL << index;
    if (masked_condition(bit, &index) != 1 || masked_condition(0, &index) != 0) return 1;
    if (while_condition(bit) != 1 || while_condition(0) != 0) return 2;
    if (for_condition(bit) != 1 || for_condition(0) != 0) return 3;
    if (do_condition(bit) != 2 || do_condition(0) != 1) return 4;
  }
  if (float_condition(0.25) != 1 || float_condition(-0.0) != 0) return 5;
  if (wide_condition((unsigned __int128)1 << 100) != 1 || wide_condition(0) != 0) return 6;
  if (first_static() != 12 || second_static() != 22 ||
      first_static() != 13 || second_static() != 23) return 7;
  if (!generic_selection()) return 8;
  if (c_nothrow_function(41) != 42) return 9;
  if (!scoped_vla(5) || !scoped_vla(11)) return 10;
  return 0;
}
