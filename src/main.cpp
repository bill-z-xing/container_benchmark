
#include <algorithm>
#include <iterator>
#include <memory>
#include <set>
#include <string>
#include <unordered_set>

#include <boost/container/flat_set.hpp>

#include "profiler.hpp"
#include "rand_gen.hpp"

using INT_TYPE = uint64_t;

static constexpr INT_TYPE seed       = 1;
static constexpr INT_TYPE rand_range = 20;
static constexpr size_t   num_iters   = 1'000'000;

struct HASH;

class MY_OBJ
{
public:

	friend struct HASH;

	MY_OBJ(RAND_GEN<INT_TYPE> & rand_gen)
	:
		_a(rand_gen.get()),
		_b(rand_gen.get()),
		_c(rand_gen.get()),
		_d(rand_gen.get())
	{

	}

	friend bool operator<(const MY_OBJ & a, const MY_OBJ & b)
	{
		if (a._a != b._a)
		{
			return a._a < b._a;
		}
		else if (a._b != b._b)
		{
			return a._b < b._b;
		}
		else if (a._c != b._c)
		{
			return a._c < b._c;
		}
		else if (a._d != b._d)
		{
			return a._d < b._d;
		}
		else
		{
			return false;
		}
	}

	size_t hash() const
	{
		return std::hash<INT_TYPE>()(_a ^ _b ^ _c ^ _d);
	}

	friend bool operator==(const MY_OBJ & a, const MY_OBJ & b)
	{
		return a._a == b._a &&
			a._b == b._b &&
			a._c == b._c &&
			a._d == b._d;
	}
private:
	INT_TYPE _a;
	INT_TYPE _b;
	INT_TYPE _c;
	INT_TYPE _d;
};


struct HASH
{
	size_t operator()(const MY_OBJ & obj) const
	{
		return obj.hash();
	}
};


using ELEM_TYPE = MY_OBJ;


auto make_rand_gen()
{
	return RAND_GEN<INT_TYPE>::make(seed, 1, rand_range);
}

template <class SET_TYPE>
void do_random_insertion(SET_TYPE & my_set)
{
	auto rand_gen = make_rand_gen();
	for (size_t i = 0; i < num_iters; ++i)
	{
		my_set.emplace(*rand_gen);
	}
}

template <class SET_TYPE>
void do_sequential_traversal(const SET_TYPE & my_set)
{
	size_t total_hash = 0;
	for (const auto & elem : my_set)
	{
		total_hash = total_hash ^ elem.hash();
	}
	std::cout << "Traversal result: " << total_hash << std::endl;
}


void flat_set()
{
	PROFILER p("flat_set");

	boost::container::flat_set<ELEM_TYPE> my_set;
	my_set.reserve(rand_range);
	do_random_insertion(my_set);
	do_sequential_traversal(my_set);
}

void std_set()
{
	PROFILER p("std_set");

	std::set<ELEM_TYPE> my_set;
	do_random_insertion(my_set);
	do_sequential_traversal(my_set);
}

void hash_set()
{
	PROFILER p("hash_set");

	std::unordered_set<ELEM_TYPE, HASH> my_set;
	my_set.reserve(rand_range * 4);
	do_random_insertion(my_set);
	std::cout << "Load factor: " << my_set.load_factor() << std::endl;

	std::vector<ELEM_TYPE> vec;
	vec.reserve(my_set.size());
	{
		PROFILER p("std::copy");
		std::copy(my_set.begin(), my_set.end(), std::back_inserter(vec));
	}
	{
		PROFILER p("std::sort");
		std::sort(vec.begin(), vec.end());
	}
}




int main()
{
	std_set();
	hash_set();
	flat_set();
	return 0;
}
