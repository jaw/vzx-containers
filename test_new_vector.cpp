/*
  SPDX-FileCopyrightText: 2026 Vovoid Media Technologies AB
  Author: Jonatan Wallmander <jonatan@vovoid.com>

  SPDX-License-Identifier: MIT
*/
#include <tools/vzx_test.h>

#include <vzx_nw_vector.h>

static void legacy_test()
{
  // INSERT

  // container empty
  {
    vzx_nw_vector <uint32_t> integers;
    integers.insert(0, 42);
    test_assert(integers.size() == 1);
    test_assert(integers[0] == 42);
  }

  // first place
  {
    vzx_nw_vector <uint32_t> integers;
    integers.push_back(33);
    integers.push_back(34);
    integers.insert(0, 42);
    test_assert(integers.size() == 3);
    test_assert(integers[0] == 42);
    test_assert(integers[1] == 33);
    test_assert(integers[2] == 34);
  }

  // last element
  {
    vzx_nw_vector <uint32_t> integers;
    integers.push_back(33);
    integers.push_back(34);
    integers.insert(2, 42);
    test_assert(integers.size() == 3);
    test_assert(integers[0] == 33);
    test_assert(integers[1] == 34);
    test_assert(integers[2] == 42);
  }

  // outside of range
  {
    vzx_nw_vector <uint32_t> integers;
    integers.push_back(33);
    integers.push_back(34);
    integers.insert(3, 42);
    test_assert(integers.size() == 2);
    test_assert(integers[0] == 33);
    test_assert(integers[1] == 34);
  }

  // INSERT_MOVE
  // container empty
  {
    vzx_nw_vector <uint32_t> integers;
    integers.insert_move(0, 42);
    test_assert(integers.size() == 1);
    test_assert(integers[0] == 42);
  }

  // first place
  {
    vzx_nw_vector <uint32_t> integers;
    integers.push_back(33);
    integers.push_back(34);
    integers.insert_move(0, 42);
    test_assert(integers.size() == 3);
    test_assert(integers[0] == 42);
    test_assert(integers[1] == 33);
    test_assert(integers[2] == 34);
  }

  // last element
  {
    vzx_nw_vector <uint32_t> integers;
    integers.push_back(33);
    integers.push_back(34);
    integers.insert_move(2, 42);
    test_assert(integers.size() == 3);
    test_assert(integers[0] == 33);
    test_assert(integers[1] == 34);
    test_assert(integers[2] == 42);
  }

  // outside of range
  {
    vzx_nw_vector <uint32_t> integers;
    integers.push_back(33);
    integers.push_back(34);
    integers.insert_move(3, 42);
    test_assert(integers.size() == 2);
    test_assert(integers[0] == 33);
    test_assert(integers[1] == 34);
  }

  {
    vzx_nw_vector <uint32_t> integers;
    integers.push_back(2);
    integers.push_back(3);
    uint32_t sum = 0;
    #ifdef VZX_ASTREE
    __ASTREE_unroll((2))
    #endif
    for (auto& i: integers)
      sum += i;
    test_assert(sum == 5);
  }

  // test last() and pop_back()
  {
    vzx_nw_vector <uint32_t> integers;
    integers.push_back(2);
    integers.push_back(3);
    test_assert(integers.last() == 3);
    auto last_value = integers.pop_back();
    test_assert(integers.size() == 1);
    test_assert(last_value == 3);
  }

  {
    vzx_nw_vector <uint32_t> integers;
    auto last_value = integers.pop_back();
    test_assert(integers.size() == 0);
    test_assert(last_value == 0);
  }

  {
    vzx_nw_vector <uint32_t> integers;
    auto last_value = integers.last();
    test_assert(last_value == 0);
  }

  static size_t a_value_when_destructor_run = 0;

  // test remove_value
  {
    struct test_struct
    {
      uint8_t a = 8;
      uint8_t b = 13;
      uint8_t c = 15;
      uint16_t d = 255;

      ~test_struct()
      {
        #ifndef VZX_ASTREE
        printf("destructor run with a: %d", a);
        #endif
        a_value_when_destructor_run = a;
      }

      bool operator==(const test_struct& other) const
      {
        if (other.a != a)
          return false;
        if (other.b != b)
          return false;
        if (other.c != c)
          return false;
        if (other.d != d)
          return false;
        return true;
      }
    };
    test_struct t1;
    test_struct t2;
    test_struct t3;

    {
      vzx_nw_vector <test_struct> items;
      {
        items.push_back(t1);
      }
      {
        t2.a++;
        t2.b++;
        t2.c++;
        t2.d++;
        items.push_back(t2);
      }
      {
        t3.a += 2;
        t3.b += 2;
        t3.c += 2;
        t3.d += 2;
        items.push_back(t3);
      }

      items.remove_value(t2);
      test_assert(items.size() == 2);
      test_assert(items.get_allocated() == 3);
      test_assert(items[0].a == 8);
      test_assert(items[1].a == 10);
      test_assert(items[0].b == 13);
      test_assert(items[1].b == 15);
      test_assert(items[0].c == 15);
      test_assert(items[1].c == 17);
      test_assert(items[0].d == 255);
      test_assert(items[1].d == 257);

      // push t2 at the end
      items.push_back(t2);
      test_assert(items.size() == 3);
      test_assert(items[0].a == 8);
      test_assert(items[1].a == 10);
      test_assert(items[0].b == 13);
      test_assert(items[1].b == 15);
      test_assert(items[0].c == 15);
      test_assert(items[1].c == 17);
      test_assert(items[0].d == 255);
      test_assert(items[1].d == 257);

      test_assert(items[2].a == 9);
      test_assert(items[2].b == 14);
      test_assert(items[2].c == 16);
      test_assert(items[2].d == 256);
      items[2].a = 63;
    }
    // last to deconstruct should be items[2]
    test_assert(a_value_when_destructor_run == 8); // reverse order
  }

  // test remove_index
  {
    a_value_when_destructor_run = 0;
    struct test_struct
    {
      uint8_t a = 8;
      uint8_t b = 13;
      uint8_t c = 15;
      uint16_t d = 255;

      test_struct()
      {
        printf("constructor run\n");
      }

      ~test_struct()
      {
        printf("destructor run with a: %d\n", a);
        a_value_when_destructor_run = a;
      }

      bool operator==(const test_struct& other) const
      {
        if (other.a != a)
          return false;
        if (other.b != b)
          return false;
        if (other.c != c)
          return false;
        if (other.d != d)
          return false;
        return true;
      }
    };
    test_struct t1;
    test_struct t2;
    test_struct t3;

    {
      vzx_nw_vector <test_struct> items;
      {
        items.push_back(t1);
      }
      {
        t2.a++;
        t2.b++;
        t2.c++;
        t2.d++;
        items.push_back(t2);
      }
      {
        t3.a += 2;
        t3.b += 2;
        t3.c += 2;
        t3.d += 2;
        items.push_back(t3);
      }

      items.remove_index(1);
      test_assert(items.size() == 2);
      test_assert(items.get_allocated() == 3);
      test_assert(items[0].a == 8);
      test_assert(items[1].a == 10);
      test_assert(items[0].b == 13);
      test_assert(items[1].b == 15);
      test_assert(items[0].c == 15);
      test_assert(items[1].c == 17);
      test_assert(items[0].d == 255);
      test_assert(items[1].d == 257);

      // push t2 at the end
      items.push_back(t2);
      test_assert(items.size() == 3);
      test_assert(items[0].a == 8);
      test_assert(items[1].a == 10);
      test_assert(items[0].b == 13);
      test_assert(items[1].b == 15);
      test_assert(items[0].c == 15);
      test_assert(items[1].c == 17);
      test_assert(items[0].d == 255);
      test_assert(items[1].d == 257);

      test_assert(items[2].a == 9);
      test_assert(items[2].b == 14);
      test_assert(items[2].c == 16);
      test_assert(items[2].d == 256);
      items[2].a = 63;
    }
    // last to deconstruct should be items[0]
    test_assert(a_value_when_destructor_run == 8); // reverse order
  }

  {
    struct holder
    {
      uint8_t a = 8;

      holder()
      {
        printf("Holder constructor run\n");
      }
    };
    vzx_nw_vector <holder> items;
    holder t1;
    items.move_back(std::move(t1));
  }
}

struct counter
{
  static int constructions;
  static int destructions;
  static int copies;
  static int moves;

  int id;

  counter(int i = 0) : id(i) { constructions++; }

  counter(const counter& other) : id(other.id)
  {
    constructions++;
    copies++;
  }

  counter(counter&& other) noexcept : id(other.id)
  {
    other.id = -1;
    constructions++;
    moves++;
  }

  ~counter() { destructions++; }

  counter& operator=(const counter& other)
  {
    id = other.id;
    copies++;
    return *this;
  }

  counter& operator=(counter&& other) noexcept
  {
    id = other.id;
    other.id = -1;
    moves++;
    return *this;
  }

  bool operator==(const counter& other) const { return id == other.id; }

  static void reset() { constructions = destructions = copies = moves = 0; }
};

int counter::constructions = 0;
int counter::destructions = 0;
int counter::copies = 0;
int counter::moves = 0;

int main(int argc, char* argv[])
{
  (void)argc;
  (void)argv;

  initialize_test_suite();
  legacy_test();

  // 1. Basic operations and accessors
  {
    struct int_holder {
      int i = 0;
      bool operator==(const int_holder &other) const {
        return i == other.i;
      }
    };

    vzx_nw_vector <int_holder> v;
    test_assert(v.size() == 0);
    test_assert(v.get_used() == 0);
    test_assert(v.get_allocated() == 0);
    test_assert(v.get_pointer() == nullptr);
    test_assert(!v);
    test_assert(static_cast <size_t>(v) == 0);

    v.push_back({10});
    test_assert(v.size() == 1);
    test_assert(v[0].i == 10);
    test_assert(v.front() != nullptr);
    test_assert((*v.front()).i == 10);
    test_assert(v.back() != nullptr);
    test_assert((*v.back()).i == 10);
    test_assert(v.last().i == 10);
    test_assert(v.last_ref().i == 10);
    test_assert(static_cast <size_t>(v) == 1);

    v.push_back({20});
    test_assert(v.size() == 2);
    test_assert(v[1].i == 20);
    test_assert((*v.back()).i == 20);
    test_assert(v.last().i == 20);

    int_holder val30 = {30};
    v.push_back_ref({val30});
    test_assert(v.size() == 3);
    test_assert(v[2].i == 30);

    int_holder val5 = {5};
    v.push_front(val5);
    test_assert(v.size() == 4);
    test_assert(v[0].i == 5);
		{
      auto v1_i = v[1].i;
      auto v2_i = v[2].i;
      auto v3_i = v[3].i;
      test_assert(v1_i == 10);
      test_assert(v2_i == 20);
      test_assert(v3_i == 30);
		}

    test_assert(v.has(int_holder{10}));
    test_assert(!v.has({100}));

    v.swap(0, 3);
    test_assert(v[0].i == 30);
    test_assert(v[3].i == 5);

    v.remove_value(int_holder{20});
    test_assert(v.size() == 3);
    test_assert(v[1].i == 10);

    v.remove_index(0);
    test_assert(v.size() == 2);
    test_assert(v[0].i == 10);

    test_assert(v.get_sizeof() == 2 * sizeof(int));
    test_assert(v.size_bytes() == 2 * sizeof(int));
  }

  // 2. Constructors and Assignment
  {
    vzx_nw_vector <int> v1;
    v1.push_back(1);
    v1.push_back(2);
    v1.push_back(3);

    // Copy constructor
    vzx_nw_vector <int> v2(v1);
    test_assert(v2.size() == 3);
    test_assert(v2[0] == 1);
    test_assert(v2[1] == 2);
    test_assert(v2[2] == 3);

    // Move constructor
    vzx_nw_vector <int> v3(std::move(v2));
    test_assert(v3.size() == 3);
    test_assert(v3[0] == 1);
    test_assert(v2.size() == 0);
    test_assert(v2.get_pointer() == nullptr);

    // Copy assignment
    vzx_nw_vector <int> v4;
    v4 = v1;
    test_assert(v4.size() == 3);
    test_assert(v4[1] == 2);

    // Move assignment
    vzx_nw_vector <int> v5;
    v5 = std::move(v4);
    test_assert(v5.size() == 3);
    test_assert(v5[2] == 3);
    test_assert(v4.size() == 0);
    test_assert(v4.get_pointer() == nullptr);
  }

  // 3. Modifiers: pop, clear, reset
  {
    vzx_nw_vector <int> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);

    test_assert(v.pop_back() == 3);
    test_assert(v.size() == 2);

    test_assert(v.pop_front() == 1);
    test_assert(v.size() == 1);
    test_assert(v[0] == 2);

    v.reset_used(0);
    test_assert(v.size() == 0);
    test_assert(v.get_allocated() > 0);

    v.push_back(100);
    test_assert(v.size() == 1);
    v.clear();
    test_assert(v.size() == 0);
    test_assert(v.get_allocated() == 0);
    test_assert(v.get_pointer() == nullptr);
  }

  // 4. Capacity and Allocation
  {
    vzx_nw_vector <int> v;
    v.set_allocation_increment(10);
    v.allocate(4); // should allocate enough for index 4 (5 elements)
    test_assert(v.size() == 5);
    test_assert(v.get_allocated() >= 5);

    v.reset_used(0);
    v.allocate_count(20);
    test_assert(v.size() == 20);

    v.reset_used(5);
    test_assert(v.can_grow_without_reallocating());
    v.grow_and_refer_to_last() = 99;
    test_assert(v.size() == 6);
    test_assert(v[5] == 99);

    v.grow(10);
    test_assert(v.size() == 16);

    v.push_back(88);;
    test_assert(v.size() == 17);
    test_assert(v[16] == 88);

    v.free_and_allocate_bytes(100 * sizeof(int));
    test_assert(v.size() == 100);
    test_assert(v.get_allocated() == 100);
  }

  // 5. Volatile data
  {
    vzx_nw_vector <int> v;
    v.push_back(1);
    v.set_volatile();
    // clear() should NOT be called if we just set it?
    // Actually set_volatile calls clear() if A is already set.
    test_assert(v.size() == 0);

    int arr[5] = {1, 2, 3, 4, 5};
    v.set_data(arr, 5);
    test_assert(v.size() == 5);
    test_assert(v.get_pointer() == arr);
    test_assert(v[2] == 3);

    v.unset_volatile();
    test_assert(v.get_pointer() == nullptr);
    test_assert(v.size() == 0);
  }

  // 6. Move semantics with custom objects
  {
    counter::reset();
    {
      vzx_nw_vector <counter> v;
      v.push_back(counter(1));
      v.move_back(counter(2));
      test_assert(v.size() == 2);
      test_assert(v[0].id == 1);
      test_assert(v[1].id == 2);
    }
    test_assert(counter::constructions == counter::destructions);
  }

  // 7. Iterators
  {
    vzx_nw_vector <int> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);

    int sum = 0;
    #ifdef VZX_ASTREE
    __ASTREE_unroll((50))
    #endif
    for (auto it = v.begin(); it != v.end(); ++it)
    {
      sum += *it;
    }
    test_assert(sum == 6);

    auto it = v.begin();
    test_assert(*(it++) == 1);
    test_assert(*it == 2);
    test_assert(*(++it) == 3);
    test_assert(*(--it) == 2);
    test_assert(*(it--) == 2);
    test_assert(*it == 1);

    test_assert(v.begin() < v.end());
    test_assert(v.end() > v.begin());
    test_assert(v.end() - v.begin() == 3);
    test_assert(*(v.begin() + 2) == 3);
    test_assert(*(v.end() - 1) == 3);
  }

  test_complete;
}
