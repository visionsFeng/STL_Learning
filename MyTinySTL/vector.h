#ifndef MYTINYSTL_VECTOR_H_
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
#include "algo.h"

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


/*****************************************************************************************/

// 复制赋值操作符


// 移动赋值操作符


// 预留空间大小，当原容量小于要求大小时，才会重新分配


// 放弃多余的容量


// 在 pos 位置就地构造元素，避免额外的复制或移动开销


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

