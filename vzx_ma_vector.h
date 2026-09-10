/*
  SPDX-FileCopyrightText: 2026 Vovoid Media Technologies AB
  Author: Jonatan Wallmander <jonatan@vovoid.com>

  SPDX-License-Identifier: MIT
*/
#pragma once

#include <iterator>
#include <cstdint>
#include <cassert>

#include <tools/vzx_platform.h>
#include <tools/vzx_req.h>

// VZX malloc vector class
//
// This is a special case array aimed at speed - for mesh data etc.
// It uses a nasty trick - allocating class pointers with malloc.
// Rules if you want to avoid segfault:
// * DON'T STORE POINTERS TO CLASSES WITH VIRTUAL FUNCTIONS
// * DON'T POINT TO ANY ELEMENT/DATA STORED IN THE ARRAY
//   (data is realloc'd, such pointers would be invalid)

template<class T, typename sizeT = uint32_t>
class vzx_ma_vector
{
  T* A = nullptr;
  sizeT allocated_items = 0;
  sizeT used_items = 0;

  uint32_t data_volatile: 1 = 0;
  uint32_t allocation_item_increment: 31 = 1;

public:
  typedef T value_T;
  typedef sizeT size_T;
  typedef size_T size_type;

  void set_allocation_increment(unsigned long new_increment)
  {
    allocation_item_increment = new_increment;
  }


  // clones another array of same type into this one
  void clone(vzx_ma_vector <T, size_T>* F)
  {
    if (F->size() == 0)
    {
      clear();
      return;
    }
    allocate(F->size() - 1, false);
    used_items = F->size();
    memcpy(static_cast <void*>(A), static_cast <void*>(F->get_pointer()), sizeof(T) * used_items);
  }

  // nA  - pointer to data
  // nsize - number of elements = byte_count / sizeof(t)
  void set_data(T* nA, size_T nsize)
  {
    if (A && !data_volatile)
      vsx_aligned_free(A);
    A = nA;
    used_items = allocated_items = nsize;
  }

  void set_volatile()
  {
    if (0 == data_volatile && A && allocated_items)
    {
      clear();
    }
    data_volatile = 1;
  }

  /**
   *
   * @param data_pointer pointer to data of the same type as T
   * @param length number of items (not bytes)
   */
  void set_volatile_data(T* data_pointer, size_T length)
  {
    set_volatile();
    set_data(data_pointer, length);
  }

  void unset_volatile()
  {
    if (!data_volatile)
      return;
    A = nullptr;
    allocated_items = 0;
    used_items = 0;
    data_volatile = false;
  }

  [[nodiscard]] bool is_volatile() const
  {
    return data_volatile == 1;
  }


  auto get_pointer() -> T*
  {
    return A;
  }

  auto get_pointer() const -> T*
  {
    return A;
  }

  auto get_end_pointer() -> T*
  {
    return &A[used_items - 1];
  }

  auto get_allocated_items() -> size_T
  {
    return allocated_items;
  }

  auto get_allocated_bytes() -> size_t
  {
    return allocated_items * sizeof(T);
  }

  auto get_used() -> size_T
  {
    return used_items;
  }

  /**
   * Adds an item to the vector.
   * @param val the value to add to the array
   * @return the new size() of the array
   */
  auto push_back(const T& val) -> size_T
  {
    (*this)[used_items] = val;
    return used_items;
  }

  auto move_back(T&& val) -> size_T
  {
    (*this)[used_items] = std::move(val);
    return used_items;
  }

  bool can_grow_without_reallocating()
  {
    return used_items < allocated_items;
  }

  /**
   * This is a shortcut / optimization.
   * _Always_ call can_grow_without_reallocating() before calling this!
   * @return
   */
  T& grow_and_refer_to_last()
  {
    used_items++;
    return A[used_items - 1];
  }

  T pop_back()
  {
    T empty_result;
    if constexpr (std::is_arithmetic_v <T>)
      empty_result = 0;
    reqrv(used_items, empty_result);
    auto result = last();
    used_items--;
    return result;
  }

  auto size() const -> size_T
  {
    return used_items;
  }

  auto get_sizeof() const -> size_T
  {
    return used_items * sizeof(T);
  }

  auto size_bytes() const -> size_T
  {
    return get_sizeof();
  }

  auto has(T o) -> bool
  {
    for (size_T i = 0; i < used_items; i++)
      if (A[i] == o)
        return true;
    return false;
  }

  T* front()
  {
    if (!used_items)
      return nullptr;

    return &A[0];
  }

  T* back()
  {
    if (!used_items)
      return 0x0;

    return &A[used_items - 1];
  }


  auto last() -> T&
  {
    if (!used_items)
      return (*this)[0];
    return (*this)[used_items - 1];
  }

  void swap(size_T a, size_T b)
  {
    req(a < used_items);
    req(b < used_items);
    req(a != b);
    T temp = std::move(A[b]);
    A[b] = std::move(A[a]);
    A[a] = std::move(temp);
  }


  void clear()
  {
    if (A && !data_volatile)
      vsx_aligned_free(A);

    A = 0;
    used_items = allocated_items = 0;
    data_volatile = 0;
    allocation_item_increment = 1;
  }

  void memory_clear(int c = 0)
  {
    assert(!data_volatile);
    memset(A, c, sizeof(T) * allocated_items);
  }

  void reset_used(size_T val = 0)
  {
    // TODO: if value larger than count of allocated items in memory handle this some way
    used_items = val;
  }

  /**
   * Allocates memory for items (if needed) and increases the used counter.
   * @param items number of items to allocate memory for
   */
  void allocate_count(size_T items)
  {
    req(items);
    allocate(items - 1, true);
  }

  /**
   * Allocate memory for items (if needed) and do not increase the used counter.
   * Priming the memory for push back / growth.
   * @param items
   */
  void reserve(size_T items)
  {
    req(items);
    allocate(items - 1, false);
  }

  void free_and_allocate_bytes(size_T b)
  {
    if (A && !data_volatile)
      vsx_aligned_free(A);

    A = static_cast <T*>(vsx_aligned_malloc(b));
    used_items = b / sizeof(T);
    allocated_items = used_items;
    data_volatile = 0;
  }

  void allocate(size_T index, bool adjust_used)
  {
    const unsigned long default_incrementation_amount = 32;
    const size_t kilobyte = 1024;
    const size_t megabyte = 1024 * 1024;
    const size_t large_block = 64 * megabyte;

    if (index >= allocated_items || allocated_items == 0)
    {
      if (allocation_item_increment == 0)
        allocation_item_increment = default_incrementation_amount;

      if (sizeof(T) * (index + allocation_item_increment) > large_block)
        printf("WARNING: Large memory block allocated.\n");

      if (A)
      {
        if (data_volatile)
        {
          T* old_A = A;
          A = static_cast <T*>(vsx_aligned_malloc(sizeof(T) * (index + allocation_item_increment)));
          if (A)
          {
            if (old_A && used_items > 0)
							/* RULECHECKER_comment(1:0,1:0, check_clang_warning, "this warns when A is of a non-trivial type, that is not how this class is used normally", false) */
              std::memcpy(static_cast<void*>(A), old_A, sizeof(T) * used_items);
            data_volatile = 0;
          }
        }
        else
        {
          A = static_cast <T*>(vsx_aligned_realloc(A, sizeof(T) * (index + allocation_item_increment)));
        }

        if (!A)
        {
          allocated_items = 0;
          used_items = 0;
          data_volatile = 0;
          return;
        }

        allocated_items = index + allocation_item_increment;
      }
      else
      {
        A = static_cast <T*>(vsx_aligned_malloc(sizeof(T) * (index + allocation_item_increment)));

        if (!A)
        {
          allocated_items = 0;
          used_items = 0;
          data_volatile = 0;
          return;
        }

        allocated_items = index + allocation_item_increment;
        data_volatile = 0;
      }

      if (allocation_item_increment < kilobyte)
      {
        allocation_item_increment *= 2;
      }
      else
      {
        const float allocation_percentage = 1.3F;
        allocation_item_increment = static_cast <size_T>((static_cast <float>(allocation_item_increment) * allocation_percentage));
      }

#ifdef VSX_ARRAY_ALLOCATE_CONSERVATIVE
      if (allocation_increment > 2)
        allocation_increment = 2;
#endif
    }

    if (adjust_used)
    {
      if (index >= used_items)
      {
        used_items = index + 1;
      }
    }
  }

  void decrease_used()
  {
    if (used_items > 0)
      used_items--;
  }

  void trim()
  {
    if (allocated_items == used_items || data_volatile)
      return;
    A = (T*) vsx_aligned_realloc(A, sizeof(T) * used_items);
    allocated_items = used_items;
  }

  auto operator[](size_T index) -> T&
  {
    allocate(index, true);
    return A[index];
  }

  auto operator[](size_T index) const -> T&
  {
    const int exit_code_out_of_bounds = 500;
    if (index > (used_items - 1))
      exit(exit_code_out_of_bounds);
    return A[index];
  }

  explicit operator bool() const
  {
    return size() > 0;
  }

  explicit operator size_t() const
  {
    return size();
  }


  // assignment and construction
  auto operator=(const vzx_ma_vector <T, size_T>& other) -> vzx_ma_vector <T, size_T>&
  {
    data_volatile = 1;
    allocated_items = other.allocated_items;
    used_items = other.used_items;
    allocation_item_increment = other.allocation_item_increment;
    A = other.A;
    return *this;
  }

  vzx_ma_vector(const vzx_ma_vector <T, size_T>& other)
  {
    data_volatile = 1;
    allocated_items = other.allocated_items;
    used_items = other.used_items;
    allocation_item_increment = other.allocation_item_increment;
    A = other.A;
  }

  // move assignment
  auto operator=(vzx_ma_vector <T, size_T>&& other) noexcept -> vzx_ma_vector <T, size_T>&
  {
    if (A && !data_volatile)
      vsx_aligned_free(A);

    data_volatile = other.data_volatile;
    allocated_items = other.allocated_items;
    used_items = other.used_items;
    allocation_item_increment = other.allocation_item_increment;
    A = other.A;

    other.data_volatile = 0;
    other.allocated_items = 0;
    other.used_items = 0;
    other.allocation_item_increment = 1;
    other.A = 0x0;
    return *this;
  }

  struct iterator
  {
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer = T*;
    using reference = T&;

    explicit iterator(pointer ptr) : m_ptr(ptr)
    {
    }

    T* get_pointer()
    {
      return m_ptr;
    }

    reference operator*() const
    {
      return *m_ptr;
    }

    pointer operator->()
    {
      return m_ptr;
    }

    iterator operator+(const size_t n)
    {
      return iterator{m_ptr + n};
    }

    iterator& operator++()
    {
      m_ptr++;
      return *this;
    }

    iterator operator++(int)
    {
      iterator tmp = *this;
      ++(*this);
      return tmp;
    }

    iterator& operator--()
    {
      m_ptr--;
      return *this;
    }

    iterator operator--(int)
    {
      iterator tmp = *this;
      --(*this);
      return tmp;
    }


    friend bool operator==(const iterator& a, const iterator& b)
    {
      return a.m_ptr == b.m_ptr;
    };

    friend bool operator!=(const iterator& a, const iterator& b)
    {
      return a.m_ptr != b.m_ptr;
    };

  private:
    pointer m_ptr;
  };

  using iterator_type = iterator;

  iterator begin()
  {
    return iterator(&A[0]);
  }

  iterator end()
  {
    return iterator(&A[used_items]);
  }

  vzx_ma_vector(vzx_ma_vector <T, size_T>&& other) noexcept
  {
    data_volatile = other.data_volatile;
    allocated_items = other.allocated_items;
    used_items = other.used_items;
    allocation_item_increment = other.allocation_item_increment;
    A = other.A;

    other.data_volatile = 0;
    other.allocated_items = 0;
    other.used_items = 0;
    other.allocation_item_increment = 1;
    other.A = 0x0;
  }

  vzx_ma_vector() = default;

  ~vzx_ma_vector()
  {
    req(!data_volatile);
    req(A);
    vsx_aligned_free(A);
  }
};
