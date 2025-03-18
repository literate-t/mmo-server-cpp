#pragma once

#include "Types.h"
#include "Allocator.h"
#include <vector>
#include <list>
#include <queue>
#include <stack>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
using namespace std;

template<typename Type>
using xvector = vector<Type, StlAllocator<Type>>;

template<typename Type>
using xlist = list<Type, StlAllocator<Type>>;

template<typename Key, typename Value, typename Pred = less<Key>, typename Alloc = StlAllocator<pair<const Key, Value>>>
using xmap = map<Key, Value, Pred, Alloc>;

template<typename Key, typename Pred = less<Key>, typename Alloc = StlAllocator<Key>>
using xset = set<Key, Pred, Alloc>;

template<typename Type, typename Alloc = StlAllocator<Type>>
using xdeque = deque<Type, Alloc>;

template<typename Type, typename Container = xdeque<Type>>
using xqueue = queue<Type, Container>;

template<typename Type, typename Container = xdeque<Type>>
using xstack = stack<Type, Container>;

template<typename Type, typename Container = xvector<Type>, typename Pred = less<typename Container::value_type>>
using xpriority_queue = priority_queue<Type, Container, Pred>;

using xstring = basic_string<char, char_traits<char>, StlAllocator<char>>;
using xwstring = basic_string<wchar_t, char_traits<wchar_t>, StlAllocator<wchar_t>>;

template<typename Key, typename Value, typename Hasher = hash<Key>, typename KeyEq = equal_to<Key>>
using xhash_map = unordered_map<Key, Value, Hasher, KeyEq, StlAllocator<pair<const Key, Value>>>;

template<typename Key, typename Hasher = hash<Key>, typename KeyEq = equal_to<Key>>
using xhash_set = unordered_set<Key, Hasher, KeyEq, StlAllocator<Key>>;