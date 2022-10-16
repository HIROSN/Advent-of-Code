file|Note
--|--
[day_1.cpp](day_1.cpp)|Read whitespace separated numbers from a file stream.
[day_1_faster.cpp](day_1_faster.cpp)|Read whitespace separated numbers into a vector in memory at once.
[day_1_part_2.cpp](day_1_part_2.cpp)|Read a number from a string stream.
[day_2.cpp](day_2.cpp)|Read whitespace separated string and number from a file stream.
[day_3.cpp](day_3.cpp)|Parse "00100 11110 ..." into std::bitset.
[day_4.cpp](day_4.cpp)|Parse comma separated numbers in a string.
[day_6.cpp](day_6.cpp)|My part 2 answer is 1.7 trillion lanternfish. Watching every lanternfish every day is not going to scale.
[day_9.cpp](day_9.cpp)|Reverse std::priority_queue.
[day_15.cpp](day_15.cpp)|Dijkstra's algorithm. May push and pop both to/from priority_queue in while loop.
[day_18.cpp](day_18.cpp)|std::unique_ptr.swap and std::move, ostream << operator overloading.
[day_22.cpp](day_22.cpp)|Override std::hash for unordered_set/map\<std::tuple\>.
days 21,22&23|unordered_set/map (O(1)~O(n)) is apparently slow when used in a tight loop. [1]
[day_24.cpp](day_24.cpp)|Do not think of a solution without unordered_map but luckily mysterious restrictions worked.
day_99.cpp|unordered_set/map (no ordering) changes the order of elements.

[1] You feel this difference as 30 seconds vs. 3 seconds elapsed time on a laptop.

Stack|Count|Weight_[ms]
--|--|--
`day_21_part_2.exe!Answer`|1921|1,917.04
`day_21_part_2.exe!std::_Hash<std::_Umap_traits<int,int,std::_Uhash_compare<int,std::hash<int>,std::equal_to<int> >,std::allocator<std::pair<int const ,int> >,0> >::_Try_emplace<int const &>`|613|612.5335
`day_21_part_2.exe!Answer<itself>`|556|554.9284
`day_21_part_2.exe!std::_Hash<std::_Umap_traits<int,unsigned __int64,std::_Uhash_compare<int,std::hash<int>,std::equal_to<int> >,std::allocator<std::pair<int const ,unsigned __int64> >,0> >::_Try_emplace<int const &>`|279|277.5927
`day_21_part_2.exe!operator new`|242|241.5179
`day_21_part_2.exe!std::deque<std::pair<Player,Player>,std::allocator<std::pair<Player,Player> > >::~deque<std::pair<Player,Player>,std::allocator<std::pair<Player,Player> > >`|215|214.4668

Stack|Count|Weight_[ms]
--|--|--
`day_21_part_2.exe!Answer`|1555|1,552.66
`day_21_part_2.exe!std::_Hash<std::_Umap_traits<int,int,std::_Uhash_compare<int,std::hash<int>,std::equal_to<int> >,std::allocator<std::pair<int const ,int> >,0> >::_Try_emplace<int const &>`|639|639.4092
`day_21_part_2.exe!Answer<itself>`|478|475.9797
`day_21_part_2.exe!std::deque<Game,std::allocator<Game> >::~deque<Game,std::allocator<Game> >`|214|213.909
`day_21_part_2.exe!operator new`|207|206.2792

day|N|elapsed|notes
--|--|--|--
day_21_part_2|95M|30 secs|unordered_map+pair
day_21_part_2|95M|3 secs|C array+struct
day_24_part_2|285M|2 mins|unordered_map
day_24_part_2|11M|11 secs|unordered_map+mysterious restrictions
