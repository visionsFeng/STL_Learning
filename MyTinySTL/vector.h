﻿#ifndef MYTINYSTL_VECTOR_H_
#define MYTINYSTL_VECTOR_H_

// 这个头文件包含一个模板类 vector
// vector : 向量

// notes:
//
// 异常保证：
// mystl::vecotr<T> 满足基本异常保证，部分函数无异常保证，并对以下函数做强异常安全保证：
//   * emplace
//   * emplace_back
//   * push_back
// 当 std::is_nothrow_move_assignable<T>::value == true 时，以下函数也满足强异常保证：
//   * reserve
//   * resize
//   * insert

#include <initializer_list>

#include "iterator.h"
#include "memory.h"
#include "util.h"
#include "exceptdef.h"
#include "al

namespace mystl
{

#ifdef max
#pragma message("#undefing marco max")
#undef max
#endif // max

#ifdef min
#pragma message("#undefing marco min")
#undef min
#endif // min

// 模板类: vector 
// 模板参数 T 代表类型
template <class T>

class vector
{
    static_assert(!std::is_same<bool, T>::value, "vector<bool> is abandoned in mystl");
public:
    // vector 的嵌套型别定义
    typedef mystl::allocator<T>                      allocator_type;
    typedef mystl::allocator<T>                      data_allocator;

    typedef typename allocator_type::value_type      value_type;
    typedef typename allocator_type::pointer         pointer;
    typedef typename allocator_type::const_pointer   const_pointer;
    typedef typename allocator_type::reference       reference;
    typedef typename allocator_type::const_reference const_reference;
    typedef typename allocator_type::size_type       size_type;
    typedef typename allocator_type::difference_type difference_type;

    typedef value_type*                              iterator;
    typedef const value_type*                        const_iterator;
    typedef mystl::reverse_iterator<iterator>        reverse_iterator;
    typedef mystl::reverse_iterator<const_iterator>  const_reverse_iterator;

    allocator_type get_allocator() { return data_allocator(); }

private:
    iterator begin_;
    iterator end_;
    iterator cap_;

public:
    // 构造、复制、移动、析构函数
    vector() noexcept
    { try_init(); }

    explicit vector(size_type n)
    { fill_init(n, value_type()); }

    vector(size_type n, const value_type& value) 
    { fill_init(n, value); } 

    template <class Iter, typename std::enable_if<mystl::is_input_iterator<Iter>::value, int>::type = 0>

    vector(Iter first, Iter last) {
        MYSTL_DEBUG(!(last < first));
        range_init(first, last);
    }

    vector(const vecotr& rhs) {
        range_init(rhs.begin_, rhs.end_);
    }

    vector(vector&& rhs) noexcept : begin_(rhs.begin_), end_(rhs.end_), cap_(rhs.cap_) {
        rhs.begin_ = nullptr;
        rhs.end_ = nullptr;
        rhs.cap_ = nullptr;
    }

    vector(std::initializer_list<value_type> ilist) {
        range_init(ilist.begin(), ilist.end());
    }

    vector& operator=(const vector& rhs);
    vector& operator=(vector&& rhs) noexcept;

    vector& operator=(std::initializer_list<value_type> ilist) {
        vector tmp(ilist.begin(), ilist.end());
        swap(tmp);
        return *this;
    }

    ~vector() {
        destroy_and_recover(begin_, end_, cap_ - begin_);
        begin_ = end_ = cap_ = nullptr;
    }

public:

  // 迭代器相关操作
  iterator               begin()         noexcept 
  { return begin_; }
  const_iterator         begin()   const noexcept
  { return begin_; }
  iterator               end()           noexcept
  { return end_; }
  const_iterator         end()     const noexcept 
  { return end_; }

  reverse_iterator       rbegin()        noexcept 
  { return reverse_iterator(end()); }
  const_reverse_iterator rbegin()  const noexcept
  { return const_reverse_iterator(end()); }
  reverse_iterator       rend()          noexcept
  { return reverse_iterator(begin()); }
  const_reverse_iterator rend()    const noexcept 
  { return const_reverse_iterator(begin()); }

  const_iterator         cbegin()  const noexcept 
  { return begin(); }
  const_iterator         cend()    const noexcept
  { return end(); }
  const_reverse_iterator crbegin() const noexcept
  { return rbegin(); }
  const_reverse_iterator crend()   const noexcept
  { return rend(); }

 // 容量相关操作
  bool      empty()    const noexcept
  { return begin_ == end_; }
  size_type size()     const noexcept
  { return static_cast<size_type>(end_ - begin_); }
  size_type max_size() const noexcept
  { return static_cast<size_type>(-1) / sizeof(T); }
  size_type capacity() const noexcept
  { return static_cast<size_type>(cap_ - begin_); }
  void      reserve(size_type n);
  void      shrink_to_fit();

  // 访问元素相关操作
  reference operator[](size_type n)
  {
    MYSTL_DEBUG(n < size());
    return *(begin_ + n);
  }
  const_reference operator[](size_type n) const
  {
    MYSTL_DEBUG(n < size());
    return *(begin_ + n);
  }
  reference at(size_type n)
  {
    THROW_OUT_OF_RANGE_IF(!(n < size()), "vector<T>::at() subscript out of range");
    return (*this)[n];
  }
  const_reference at(size_type n) const
  {
    THROW_OUT_OF_RANGE_IF(!(n < size()), "vector<T>::at() subscript out of range");
    return (*this)[n];
  }

  reference front()
  {
    MYSTL_DEBUG(!empty());
    return *begin_;
  }
  const_reference front() const
  {
    MYSTL_DEBUG(!empty());
    return *begin_;
  }
  reference back()
  {
    MYSTL_DEBUG(!empty());
    return *(end_ - 1);
  }
  const_reference back() const
  {
    MYSTL_DEBUG(!empty());
    return *(end_ - 1);
  }

  pointer       data()       noexcept { return begin_; }
  const_pointer data() const noexcept { return begin_; }

  // 修改容器相关操作

  // assign
  void assign(size_type n, const value_type& value) {
    fill_assign(n, value);
  }

  template <class Iter, typename std::enable_if<mystl::is_input_iterator<Iter>::value, int>::type = 0>
  void assign(Iter first, Iter last) {
    MYSTL_DEBUG(!(last < first));
    copy_assign(first, last, iterator_category(first));
  }

  void assign(std::initializer_list<value_type> il) {
    copy_assign(il.begin(), il.end(), mystl::forward_iterator_tag{});
  }

  // emplace / emplace_back

  template <class... Args>
  iterator emplace(const_iterator pos, Args&& ...args);

  template <class... Args>
  void emplace_back(Args&& ...args);

  //push_back / pop_back

  void push_back(const value_type& value);
  void push_back(value_type&& value)
  { emplace_back(mystl::move(value)); }

  void pop_back();

  // insert

  iterator insert(const_iterator pos, const value_type& value);
  iterator insert(const_iterator pos, value_type&& value)
  { return emplace(pos, mystl::move(value)); }

  iterator insert(const_iterator pos, size_type n, const value_type& value)
  {
    MYSTL_DEBUG(pos >= begin() && pos <= end());
    return fill_insert(const_cast<iterator>(pos), n, value);
  }

  template <class Iter, typename std::enable_if<
    mystl::is_input_iterator<Iter>::value, int>::type = 0>
  void     insert(const_iterator pos, Iter first, Iter last)
  {
    MYSTL_DEBUG(pos >= begin() && pos <= end() && !(last < first));
    copy_insert(const_cast<iterator>(pos), first, last);
  }

  // erase / clear
  iterator erase(const_iterator pos);
  iterator erase(const_iterator first, const_iterator last);
  void     clear() { erase(begin(), end()); }

  // resize / reverse
  void     resize(size_type new_size) { return resize(new_size, value_type()); }
  void     resize(size_type new_size, const value_type& value);

  void     reverse() { mystl::reverse(begin(), end()); }

  // swap
  void     swap(vector& rhs) noexcept;

private:
  // helper functions

  // initialize / destroy
  void      try_init() noexcept;

  void      init_space(size_type size, size_type cap);

  void      fill_init(size_type n, const value_type& value);
  template <class Iter>
  void      range_init(Iter first, Iter last);

  void      destroy_and_recover(iterator first, iterator last, size_type n);

  // calculate the growth size
  size_type get_new_cap(size_type add_size);

  // assign

  void      fill_assign(size_type n, const value_type& value);

  template <class IIter>
  void      copy_assign(IIter first, IIter last, input_iterator_tag);

  template <class FIter>
  void      copy_assign(FIter first, FIter last, forward_iterator_tag);

  // reallocate

  template <class... Args>
  void      reallocate_emplace(iterator pos, Args&& ...args);
  void      reallocate_insert(iterator pos, const value_type& value);

  // insert

  iterator  fill_insert(iterator pos, size_type n, const value_type& value);
  template <class IIter>
  void      copy_insert(iterator pos, IIter first, IIter last);

  // shrink_to_fit

  void      reinsert(size_type size);
}

/*****************************************************************************************/

// 复制赋值操作符
template <class T>
vector<T>& vector<T>::operator=(const vector& rhs) {
    if (this != &rhs) {
        const auto len = rhs.size();
        if (len > capacity()) {
            vector tmp(rhs.begin(), rhs.end());
            swap(tmp);
        }
        else if (size() >= len) {
            auto i = mystl::copy(rhs.begin(), rhs.end(), begin());
            data_allocator::destroy(i, end_);
            end_ = begin_ + len;
        }
        else {
            mystl::copy(rhs.begin(), rhs.begin() + size(), begin_);
            mystl::uninitialized_copy(rhs.begin() + size(), rhs.end(), end_);
            cap_ = end_ = begin_ + len;
        }
    }
    return *this;
}

// 移动赋值操作符
template <class T>
vector<T>& vector<T>::operator=(vector&& rhs) noexcept {
    destroy_and_recover(begin_, end_, cap_ - begin_);
    begin_ = rhs.begin_;
    end_ = rhs.end_;
    cap_ = rhs.cap_;
    rhs.begin_ = nullptr;
    rhs.cap_ = nullptr;
    rhs.end_ = nullptr;
    return *this;
}

// 预留空间大小，当原容量小于要求大小时，才会重新分配
template <class T>
void vector<T>::reserve(size_type n) {
    if (capacity() < n) {
        THROW_OUT_OF_RANGE_IF(n > max_size, "n can not larger than max_size() in vector<T>::reserve(n)");
        const auto old_size = size();
        auto tmp = data_allocator::allocate(n);
        mystl::uninitialized_move(begin_, end_, tmp);
        data_allocator::deallocate(begin_, cap_ - begin_);
        begin_ = tmp;
        end_ = tmp + old_size;
        cap_ = begin_ + n;
    }
}

// 放弃多余的容量
template <class T>
void vector<T>::shrink_to_fit() {
    if (end_ < cap_) {
        reinsert(size());
    }
}

// 在 pos 位置就地构造元素，避免额外的复制或移动开销
template <class T>
template <class ...Args>
typename vector<T>::iterator
vector<T>::emplace(const_iterator pos, Args&& ...args) {
    
}

// 在尾部就地构造元素，避免额外的复制或移动开销


// 在尾部插入元素


// 弹出尾部元素

// 在 pos 处插入元素


// 删除 pos 位置上的元素


// 删除[first, last)上的元素


// 重置容器大小


// 与另一个 vector 交换


/*****************************************************************************************/
// helper function

// try_init 函数，若分配失败则忽略，不抛出异常


// init_space 函数


// fill_init 函数


// range_init 函数


// destroy_and_recover 函数


// get_new_cap 函数


// fill_assign 函数

// copy_assign 函数


// 用 [first, last) 为容器赋值


// 重新分配空间并在 pos 处就地构造元素


// 重新分配空间并在 pos 处插入元素


// fill_insert 函数


// copy_insert 函数


// reinsert 函数


/*****************************************************************************************/
// 重载比较操作符


// 重载 mystl 的 swap


} // namespace mystl
#endif // !MYTINYSTL_VECTOR_H_

