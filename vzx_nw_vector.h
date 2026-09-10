/*
  SPDX-FileCopyrightText: 2026 Vovoid Media Technologies AB
  Author: Jonatan Wallmander <jonatan@vovoid.com>

  SPDX-License-Identifier: MIT
*/
#pragma once

#include <cstdint>
#include <iterator>

#include <tools/vzx_platform.h>
#include <tools/vzx_req.h>

template<class T, typename sizeT = uint32_t>
class vzx_nw_vector
{
  sizeT allocated_items = 0;
  sizeT used_items = 0;
  T* A = nullptr;
  unsigned long allocation_increment: 31 = 1;
  bool data_volatile: 1 = false;
public:
  typedef T value_T;
  typedef sizeT size_T;
  typedef sizeT size_type;

  T* get_pointer()
  {
    return A;
  }

  size_t get_allocated()
  {
    return allocated_items;
  }

  size_t get_used()
  {
    return used_items;
  }

  /**
   * Adds an item to the vector.
   * @param val the value to add to the array
   * @return the new size() of the array
   */
  size_t push_back(const T& val)
  {
    (*this)[used_items] = val;
    return used_items;
  }

  size_t move_back(T&& val)
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


  size_t push_back_ref(T& val)
  {
    (*this)[used_items] = val;
    return used_items;
  }

  void push_front(const T& val)
  {
    insert(0, val);
  }

  T last()
  {
    T empty_result;
    if constexpr (std::is_arithmetic_v <T>)
      empty_result = 0;
    if (!used_items)
      return empty_result;
    return operator[](used_items - 1);
  }

  T& last_ref()
  {
    return operator[](used_items - 1);
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

  T pop_front()
  {
    if (!used_items)
      return std::move(T());
    T v = std::move(A[0]);
    remove_index(0);
    return v;
  }

  T* front()
  {
    if (!used_items)
      return nullptr;

    return &A[0];
  }

  const T* front() const
  {
    if (!used_items)
      return nullptr;

    return &A[0];
  }

  T* back()
  {
    if (!used_items)
      return nullptr;

    return &A[used_items - 1];
  }

  const T* back() const
  {
    if (!used_items)
      return nullptr;

    return &A[used_items - 1];
  }

  void swap(size_t a, size_t b)
  {
    req(a < used_items);
    req(b < used_items);
    req(a != b);
    T temp = std::move(A[b]);
    A[b] = std::move(A[a]);
    A[a] = std::move(temp);
  }

  [[nodiscard]] size_T size() const
  {
    return used_items;
  }

  size_t get_sizeof()
  {
    return used_items * sizeof(T);
  }

  size_t size_bytes()
  {
    return get_sizeof();
  }

  bool has(T o)
  {
    for (size_t i = 0; i < used_items; i++)
      if (A[i] == o)
        return true;
    return false;
  }

  void clear()
  {
    req(!data_volatile);

    req(A);
    delete[] A;
    A = nullptr;
    allocated_items = used_items = 0;
    allocation_increment = 1;
  }

  void reset_used(size_t val = 0)
  {
    used_items = static_cast<size_T>(val);
  }

  [[maybe_unused]] void set_allocation_increment(unsigned long new_increment)
  {
    allocation_increment = new_increment;
  }

  void set_data(T* data_pointer, size_t element_count)
  {
    if (A && !data_volatile)
      delete[] A;

    A = data_pointer;
    used_items = allocated_items = static_cast<size_T>(element_count);
  }

  void set_volatile()
  {
    if (0 == data_volatile && A && allocated_items)
      clear();
    data_volatile = true;
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

  void remove_value(const T& value)
  {
    req(!data_volatile);
    for (unsigned long i = 0; i < used_items; i++)
    {
      if (A[i] == value)
      {
        remove_index(i);
        return;
      }
    }
  }

  void remove_index(size_t index)
  {
    req(!data_volatile);
    req(index < used_items);
    for (size_t i = index; i < used_items - 1; i++)
      A[i] = std::move(A[i + 1]);
    used_items--;
  }

  void insert(size_t index, T value)
  {
    req(!data_volatile);
    req(index < used_items + 1);

    grow(1);

    // only element
    if (used_items == 1)
      return_after(A[index] = value);

    for (size_t i = used_items - 1; i > index; i--)
      A[i] = std::move(A[i - 1]);
    A[index] = value;
  }

  void insert_move(size_t index, T&& value)
  {
    req(!data_volatile);
    req(index < used_items + 1);

    grow(1);

    // only element
    if (used_items == 1)
      return_after(A[index] = std::move(value));

    for (size_t i = used_items - 1; i > index; i--)
      A[i] = std::move(A[i - 1]);
    A[index] = std::move(value);
  }

  void allocate_count(size_T count)
  {
    allocate(count - 1);
  }

  void grow(size_T count = 1)
  {
    allocate_count(used_items + count);
  }

  void allocate(size_T index)
  {
    req(!data_volatile);
    if (index + 1 > allocated_items || allocated_items == 0)
    {
      if (A)
      {
        if (allocation_increment == 0)
          allocation_increment = 1;

        allocated_items = index + allocation_increment;
        T* B = new T[allocated_items];

        // move the old ones to the new array
        for (size_t i = 0; i < used_items; ++i)
          B[i] = std::move(A[i]);

        delete[] A;
        A = B;
      }
      else
      {
        allocated_items = index + allocation_increment;
        A = new T[allocated_items];
      }

      if (allocation_increment < 64)
        allocation_increment *= 2;
      //      else
      //      {
      //        allocation_increment = (size_t)((float)allocation_increment * 1.3f);
      //        if (debug_allocation_increment)
      //          debug(L"allocation inc: %d", allocation_increment);
      //      }
    }

    if (index >= used_items)
      used_items = index + 1;
  }

  void free_and_allocate_bytes(size_t num_bytes)
  {
    delete[] A;
    size_t needed = num_bytes / sizeof(T);
    A = new T[needed];
    used_items = static_cast<size_T>(needed);
    allocated_items = used_items;
  }

  T& operator[](size_t index)
  {
#if VSX_COMMON_DEBUG
    if (data_volatile && index > (used_items - 1))
      exit(500);
#endif
    allocate(static_cast<size_T>(index));
    return A[index];
  }

  T& operator[](size_t index) const
  {
    if (index > (used_items - 1))
      exit(500);
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

  vzx_nw_vector& operator=(vzx_nw_vector <T>&& other)
  {
    reqrv(!data_volatile, *this);

    if (A)
      clear();

    allocated_items = other.allocated_items;
    used_items = other.used_items;
    allocation_increment = other.allocation_increment;
    A = other.A;

    other.allocated_items = 0;
    other.used_items = 0;
    other.allocation_increment = 1;
    other.A = nullptr;
    return *this;
  }

  vzx_nw_vector& operator=(const vzx_nw_vector <T>& other)
  {
    reqrv(!data_volatile, *this);

    allocate(other.allocated_items);
    used_items = other.used_items;
    allocation_increment = other.allocation_increment;
    for (size_t i = 0; i < used_items; i++)
      A[i] = other.A[i];
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

    reference operator*() const
    {
      return *m_ptr;
    }

    pointer operator->()
    {
      return m_ptr;
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

    bool operator<(const iterator& other)
    {
      return m_ptr < other.m_ptr;
    }

    bool operator>(const iterator& other)
    {
      return m_ptr > other.m_ptr;
    }

    auto operator+(const int64_t diff) const -> iterator
    {
      iterator result = *this;
      result.m_ptr += diff;
      return result;
    }

    int64_t operator-(const iterator& t) const
    {
      return static_cast <int64_t>(m_ptr - t.m_ptr);
    }

    iterator operator-(const int64_t& t) const
    {
      iterator result = *this;
      result.m_ptr -= t;
      return result;
    }

    explicit operator int64_t() const
    {
      return static_cast <int64_t>(m_ptr);
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

  iterator begin()
  {
    return iterator(&A[0]);
  }

  iterator begin() const
  {
    return iterator(&A[0]);
  }

  iterator end()
  {
    return iterator(&A[used_items]);
  }

  iterator end() const
  {
    return iterator(&A[used_items]);
  }

  vzx_nw_vector(const vzx_nw_vector <T>& other)
  {
    req(!other.data_volatile);
    allocate(other.allocated_items);
    allocated_items = other.allocated_items;
    used_items = other.used_items;
    allocation_increment = other.allocation_increment;
    for (size_t i = 0; i < used_items; i++)
      A[i] = other.A[i];
  }

  vzx_nw_vector(vzx_nw_vector <T>&& other)
  {
    req(!other.data_volatile);
    allocate(other.allocated_items);
    allocated_items = other.allocated_items;
    used_items = other.used_items;
    allocation_increment = other.allocation_increment;
    for (size_t i = 0; i < used_items; i++)
      A[i] = std::move(other.A[i]);

    other.allocated_items = 0;
    other.used_items = 0;
    other.allocation_increment = 1;
    other.A = nullptr;
  }

  vzx_nw_vector() = default;

  ~vzx_nw_vector()
  {
    req(!data_volatile);
    req(A);
    delete[] A;
    A = nullptr; // valgrind
  }
};

