#include "stdint.h"
#include <iostream>
#include <vector>
#include <map>

#include "render_name_list.h"

using namespace std;

#define HR "--------------------------"

#define SINGLE_ARG(...) __VA_ARGS__
#define STRING(...) #__VA_ARGS__
#define ASSERT(cond)\
{\
	if(cond)\
	{\
		cout << "ASSERT(" STRING(cond) ") passed" <<  endl;\
	}\
	else\
	{\
		cout << "ASSERT(" STRING(cond) ") failed" << endl;\
		exit(1);\
	}\
}
#define PRINT(str) cout << str << endl
// needed varargs magic to handle printing statements with commas
#define _DO(statement)\
	statement; PRINT( STRING(statement) ";");
#define DO(...) _DO( SINGLE_ARG(__VA_ARGS__) )

bool test_render_name_list()
{
	PRINT("creating mask\n");
	DO(RenderNameList list("red", vec4f(1,0,0,0));)
	DO(list.add("transparent", vec4f(0,0,0,0)));
	DO(list.add("transparent", vec4f(0,0,0,0)));
	DO(list.add("abc", vec4f(0,0,0,1)));
	DO(list.add("transparent1", vec4f(0,0,0,0)));
	DO(list.add("transparent2", vec4f(0,0,0,0)));
	DO(list.add("abc1", vec4f(0,0,0,1)));
	DO(list.add("abc2", vec4f(0,0,0,1)));
	DO(list.add("abc3", vec4f(0,0,0,1)));

	PRINT(list.size());
	ASSERT(list.size() == 8);

	DO(list.sortByColorMasks());

	ASSERT(list.size() == 8);

	PRINT("\ntesting validity\n");

	vec4f last_mask = list.getColorMask(0);
	vec4f cur_mask = list.getColorMask(0);
	std::cout << "0: cur_mask(" << cur_mask.x() << "," << cur_mask.y()
			  << "," << cur_mask.z() << "," << cur_mask.w() << ")" << std::endl;
	// after sorted, 1 mask should not be seen twice when looping through neighbor groups.
	std::vector<vec4f> seen_masks;
	seen_masks.push_back(last_mask);
	for(unsigned int i=1; i<list.size(); i++)
	{
		cur_mask = list.getColorMask(i);
		if(list.getColorMask(i) != last_mask)
		{
			bool already_seen_mask = std::find(
						seen_masks.begin(),
						seen_masks.end(),
						list.getColorMask(i)) != seen_masks.end();
			std::cout << i << ": cur_mask(" << cur_mask.x() << "," << cur_mask.y()
					  << "," << cur_mask.z() << "," << cur_mask.w() << ")" << std::endl;
			ASSERT(already_seen_mask == false);
			seen_masks.push_back(list.getColorMask(i));
		}
		last_mask = list.getColorMask(i);
	}
	PRINT(seen_masks.size());
	ASSERT(seen_masks.size() == 3);

	return true;
}

int main()
{
	PRINT("doing render_name_list tests\n" HR);
	if (test_render_name_list())
	{
		PRINT(HR "\nall render_name_list tests passed.");
	}

	PRINT("\n" HR "\nall tests passed.");
}
