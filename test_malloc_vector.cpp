/*
  SPDX-FileCopyrightText: 2026 Vovoid Media Technologies AB
  Author: Jonatan Wallmander <jonatan@vovoid.com>

  SPDX-License-Identifier: MIT
*/
#include <tools/vzx_test.h>

#include <vzx_ma_vector.h>

namespace
{
void test_ma_vector_basic()
{
  vzx_ma_vector <uint32_t> integers;
  test_assert(integers.size() == 0);
  test_assert(integers.get_pointer() == nullptr);
  test_assert(integers.is_volatile() == false);

  {
    auto push_back_result = integers.push_back(2);
    test_assert(push_back_result == 1);
    auto* temp_pointer = integers.get_pointer();
    test_assert(temp_pointer != nullptr);
  }

  {
    auto push_back_result = integers.push_back(3);
    test_assert(push_back_result == 2);
  }

  {
    auto integers_size = integers.size();
    test_assert(integers_size == 2);
  }

  if (integers.get_pointer() == nullptr)
    return;

  {
    auto* temp_pointer = integers.get_pointer();
    if (integers.get_allocated_items() > 0)
      test_assert(temp_pointer != nullptr);
  }

  uint64_t sum = 0;
  #ifdef VZX_ASTREE
  __ASTREE_unroll((2))
  #endif
  for (auto individual_integer: integers)
  {
    sum += individual_integer;
  }
  test_assert(sum == 5);

  integers.clear();
  test_assert(integers.size() == 0);
  test_assert(integers.get_pointer() == nullptr);
}

void test_ma_vector_copy()
{
  vzx_ma_vector <uint32_t> v1;
  v1.push_back(100);
  v1.push_back(200);

  // Copy constructor - creates a volatile view
  vzx_ma_vector <uint32_t> v2 = v1;
  test_assert(v2.size() == v1.size());
  test_assert(v2.get_pointer() == v1.get_pointer());
  test_assert(v2.is_volatile() == true);
  test_assert(v1.is_volatile() == false);
  test_assert(v2[0] == 100);
  test_assert(v2[1] == 200);

  // Copy assignment - creates a volatile view
  vzx_ma_vector <uint32_t> v3;
  v3 = v1;
  test_assert(v3.size() == v1.size());
  test_assert(v3.get_pointer() == v1.get_pointer());
  test_assert(v3.is_volatile() == true);
}

void test_ma_vector_move()
{
  vzx_ma_vector <uint32_t> v1;
  v1.push_back(1000);
  v1.push_back(2000);
  void* original_ptr = v1.get_pointer();
  uint32_t original_size = v1.size();

  // Move constructor - transfers ownership
  vzx_ma_vector <uint32_t> v2 = std::move(v1);
  test_assert(v2.size() == original_size);
  test_assert(v2.get_pointer() == original_ptr);
  test_assert(v2.is_volatile() == false);
  test_assert(v1.size() == 0);
  test_assert(v1.get_pointer() == nullptr);

  // Move assignment - transfers ownership
  vzx_ma_vector <uint32_t> v3;
  v3 = std::move(v2);
  test_assert(v3.size() == original_size);
  test_assert(v3.get_pointer() == original_ptr);
  test_assert(v3.is_volatile() == false);
  test_assert(v2.size() == 0);
  test_assert(v2.get_pointer() == nullptr);
}

void test_ma_vector_volatile()
{
  uint32_t data[3] = {10, 20, 30};
  vzx_ma_vector <uint32_t> v1;
  v1.set_volatile_data(data, 3);

  test_assert(v1.is_volatile() == true);
  test_assert(v1.get_pointer() == data);
  test_assert(v1.size() == 3);
  test_assert(v1[0] == 10);
  test_assert(v1[2] == 30);

  // Move volatile vector - should preserve volatile flag
  vzx_ma_vector <uint32_t> v2 = std::move(v1);
  test_assert(v2.is_volatile() == true);
  test_assert(v2.get_pointer() == data);
  test_assert(v2.size() == 3);
  test_assert(v1.size() == 0);
  test_assert(v1.is_volatile() == false); // Source reset to 0
  test_assert(v1.get_pointer() == nullptr);
}

void test_ma_vector_allocation_and_size()
{
  vzx_ma_vector <uint32_t> v;
  test_assert(v.get_used() == 0);
  test_assert(v.get_allocated_items() == 0);
  test_assert(v.get_allocated_bytes() == 0);
  test_assert(v.get_sizeof() == 0);
  test_assert(v.size_bytes() == 0);
  test_assert((size_t) v == 0);
  test_assert(!(bool) v);

  v.set_allocation_increment(10);
  v.push_back(1);
  test_assert(v.get_used() == 1);
  // Initial allocation uses increment
  test_assert(v.get_allocated_items() == 10);
  test_assert(v.get_allocated_bytes() == 10 * sizeof(uint32_t));
  test_assert(v.get_sizeof() == 1 * sizeof(uint32_t));
  test_assert(v.size_bytes() == 1 * sizeof(uint32_t));
  test_assert((size_t) v == 1);
  test_assert((bool) v);
}

void test_ma_vector_access()
{
  vzx_ma_vector <uint32_t> v;
  test_assert(v.front() == nullptr);
  test_assert(v.back() == nullptr);

  v.push_back(10);
  test_assert(*v.front() == 10);
  test_assert(*v.back() == 10);
  test_assert(v.last() == 10);
  test_assert(v.get_end_pointer() == v.get_pointer());

  v.push_back(20);
  test_assert(*v.front() == 10);
  test_assert(*v.back() == 20);
  test_assert(v.last() == 20);
  test_assert(v.get_end_pointer() == v.get_pointer() + 1);

  v.last() = 30;
  test_assert(v[1] == 30);
}

void test_ma_vector_manipulation()
{
  vzx_ma_vector <uint32_t> v;
  v.push_back(10);
  v.push_back(20);
  v.push_back(30);

  test_assert(v.has(20));
  test_assert(!v.has(40));

  v.swap(0, 2);
  test_assert(v[0] == 30);
  test_assert(v[2] == 10);

  uint32_t val = v.pop_back();
  test_assert(val == 10);
  test_assert(v.size() == 2);

  v.decrease_used();
  test_assert(v.size() == 1);

  v.reset_used(5);
  test_assert(v.size() == 5);
  // reset_used to 5 might require allocation if we want to access it safely,
  // but vzx_ma_vector's reset_used doesn't allocate.
  // The user of reset_used is responsible for ensuring enough memory is allocated.
  
  vzx_ma_vector <uint32_t> v2;
  v2.reserve(10);
  v2.memory_clear(0);
  for (uint32_t i = 0; i < 10; ++i)
    test_assert(v2.get_pointer()[i] == 0);
    
  struct move_tester
  {
    bool moved = false;
    move_tester() = default;
    move_tester(const move_tester&) = default;
    move_tester(move_tester&& other) noexcept { other.moved = true; }
    move_tester& operator=(move_tester&& other) noexcept { other.moved = true; return *this; }
  };

  vzx_ma_vector <move_tester> v3;
  move_tester mt;
  v3.move_back(std::move(mt));
  test_assert(mt.moved);
}

void test_ma_vector_memory()
{
  vzx_ma_vector <uint32_t> v;
  v.reserve(100);
  test_assert(v.get_allocated_items() >= 100);
  test_assert(v.size() == 0);

  v.allocate_count(50);
  test_assert(v.size() == 50);

  v.trim();
  test_assert(v.get_allocated_items() == 50);

  v.free_and_allocate_bytes(1024);
  test_assert(v.size() == 1024 / sizeof(uint32_t));
  test_assert(v.get_allocated_bytes() == 1024);

  vzx_ma_vector <uint32_t> v2;
  v2.push_back(5);
  v2.push_back(6);
  v.clone(&v2);
  test_assert(v.size() == 2);
  test_assert(v[0] == 5);
  test_assert(v[1] == 6);

  uint32_t data[2] = {1, 2};
  v.set_volatile_data(data, 2);
  test_assert(v.is_volatile());
  v.unset_volatile();
  test_assert(!v.is_volatile());
  test_assert(v.get_pointer() == nullptr);
  test_assert(v.size() == 0);
}

void test_ma_vector_growth()
{
  vzx_ma_vector <uint32_t> v;
  v.reserve(10);
  test_assert(v.can_grow_without_reallocating());
  
  for (int i = 0; i < 10; ++i)
  {
    test_assert(v.can_grow_without_reallocating());
    uint32_t& last = v.grow_and_refer_to_last();
    last = i * 10;
  }
  
  test_assert(v.size() == 10);
  test_assert(v[9] == 90);
  // It might still be able to grow if reserve allocated more than 10
  // but we know it's at least 10.
}

void test_ma_vector_iterators()
{
  vzx_ma_vector <uint32_t> v;
  v.push_back(10);
  v.push_back(20);
  v.push_back(30);

  auto it = v.begin();
  test_assert(it != v.end());
  test_assert(*it == 10);
  test_assert(it.get_pointer() == v.get_pointer());

  auto it2 = it + 1;
  test_assert(*it2 == 20);

  test_assert(++it == it2);
  test_assert(*(it++) == 20);
  test_assert(*it == 30);

  test_assert(--it == it2);
  test_assert(*(it--) == 20);
  test_assert(*it == 10);

  struct Tmp { int x; };
  vzx_ma_vector <Tmp> vt;
  vt.push_back({5});
  auto itt = vt.begin();
  test_assert(itt->x == 5);
}

void test_ma_vector_clear_volatile_permutations()
{
  // 1. Normal -> clear
  vzx_ma_vector<uint32_t> v;
  v.push_back(1);
  v.clear();
  test_assert(v.size() == 0);
  test_assert(v.is_volatile() == false);
  test_assert(v.get_pointer() == nullptr);

  // 2. Volatile -> clear
  uint32_t data[3] = {10, 20, 30};
  v.set_volatile_data(data, 3);
  test_assert(v.is_volatile() == true);
  v.clear();
  test_assert(v.size() == 0);
  test_assert(v.is_volatile() == false);
  test_assert(v.get_pointer() == nullptr);
  test_assert(data[0] == 10); // Data should be untouched

  // 3. Normal -> set_volatile_data
  v.push_back(100);
  v.set_volatile_data(data, 3);
  test_assert(v.is_volatile() == true);
  test_assert(v.get_pointer() == data);

  // 4. Volatile -> push_back (triggers conversion to normal)
  v.push_back(40);
  test_assert(v.is_volatile() == false);
  test_assert(v.size() == 4);
  test_assert(v.get_pointer() != data);
  test_assert(v[0] == 10);
  test_assert(v[1] == 20);
  test_assert(v[2] == 30);
  test_assert(v[3] == 40);
  v[0] = 11; // memory is copied at this point
  test_assert(v[0] == 11);
  test_assert(data[0] == 10); // Original data untouched

  // 5. Volatile -> unset_volatile -> clear
  v.set_volatile_data(data, 3);
  v.unset_volatile();
  test_assert(v.is_volatile() == false);
  test_assert(v.get_pointer() == nullptr);
  v.clear();
  test_assert(v.size() == 0);

  // 6. set_volatile() on non-empty normal vector
  vzx_ma_vector<uint32_t> v2;
  v2.push_back(500);
  v2.set_volatile();
  test_assert(v2.is_volatile() == true);
  test_assert(v2.size() == 0);
  test_assert(v2.get_pointer() == nullptr);
  
  // 7. clone of volatile vector
  v.set_volatile_data(data, 3);
  v2.clear();
  v2.clone(&v);
  test_assert(v2.is_volatile() == false); // clone should create a real copy
  test_assert(v2.size() == 3);
  test_assert(v2.get_pointer() != v.get_pointer());
  test_assert(v2[0] == 10);

  // 8. Copy assignment from normal to volatile view
  vzx_ma_vector<uint32_t> v3;
  v3.push_back(1);
  v3.push_back(2);
  v.clear();
  v = v3; // This creates a volatile view of v3's data
  test_assert(v.is_volatile() == true);
  test_assert(v.get_pointer() == v3.get_pointer());
  test_assert(v.size() == 2);

  // 9. Move assignment from volatile to normal
  v.set_volatile_data(data, 3);
  v2.clear();
  v2.push_back(1000);
  v2 = std::move(v);
  test_assert(v2.is_volatile() == true);
  test_assert(v2.get_pointer() == data);
  test_assert(v.size() == 0);
  test_assert(v.is_volatile() == false);
  test_assert(v.get_pointer() == nullptr);

  // 10. set_volatile() then push_back()
  v.clear();
  v.set_volatile();
  test_assert(v.is_volatile() == true);
  v.push_back(123);
  test_assert(v.is_volatile() == false);
  test_assert(v[0] == 123);
}
}

int main()
{
  initialize_test_suite();

  auto* other_integers = static_cast<int32_t*>(malloc(sizeof(int32_t) * 16));
  test_assert(other_integers != nullptr);

  auto* some_integers = new int32_t[16];
  test_assert(some_integers != nullptr);
  // add bits to upper parts of pointer

  test_ma_vector_basic();
  test_ma_vector_allocation_and_size();
  test_ma_vector_access();
  test_ma_vector_manipulation();
  test_ma_vector_memory();
  test_ma_vector_growth();
  test_ma_vector_iterators();
  test_ma_vector_copy();
  test_ma_vector_move();
  test_ma_vector_volatile();
  test_ma_vector_clear_volatile_permutations();

  test_complete
}
