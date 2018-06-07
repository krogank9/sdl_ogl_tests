#ifndef RENDER_NAME_LIST_H
#define RENDER_NAME_LIST_H

#include <vector>
#include <algorithm>
#include <string>

#include "color.h"

#define MAX_MASKS 4

class RenderNameList
{
public:
	RenderNameList() : is_sorted_(true)
	{
	}

	RenderNameList(const RenderNameList& copy)
		: is_sorted_(copy.is_sorted())
	{
		for (unsigned int i=0; i<copy.size(); i++)
		{
			names.push_back(copy.getName(i));
			color_masks.push_back(copy.getColorMask(i));
		}
	}

	RenderNameList(std::string name, Color color_mask=Color(1.f,1.f,1.f,1.f))
		: is_sorted_(true)
	{
		names.push_back(name);
		color_masks.push_back(color_mask);
	}

	RenderNameList(const char* name, Color color_mask=Color(1.f,1.f,1.f,1.f))
		: is_sorted_(true)
	{
		names.push_back(std::string(name));
		color_masks.push_back(color_mask);
	}

	RenderNameList filterAllExcept(std::string name)
	{
		RenderNameList ret;
		for (unsigned int i=0; i<names.size(); i++)
		{
			if (name == names[i])
			{
				ret.add(name, color_masks[i]);
				break;
			}
		}
		return ret;
	}

	void clear()
	{
		names.clear();
		color_masks.clear();
		is_sorted_ = true;
	}

	void add(const std::string& name, const Color& color_mask)
	{
		for (unsigned int i=0; i<names.size(); i++)
		{
			if (names[i] == name)
			{
				color_masks[i] = color_mask;
				return;
			}
		}
		names.push_back(name);
		color_masks.push_back(color_mask);
		is_sorted_ = false;
	}

	void erase(std::string& name)
	{
		for(unsigned int i=0; i<names.size(); i++)
		{
			if (names[i] == name)
			{
				names.erase(names.begin()+i);
				color_masks.erase(color_masks.begin()+i);
				return;
			}
		}
	}

	std::string getName(int i) const
	{
		return names[i];
	}

	Color getColorMask(int i) const
	{
		return color_masks[i];
	}

	unsigned int size() const { return names.size(); }

	// sort so identical color masks are next to each other
	// so rendering can be done in batches
	void sortByColorMasks()
	{
		if (is_sorted_)
			return;

		std::vector<Color> new_color_masks;
		std::vector<std::string> new_names;

		for (unsigned int i=0; i<color_masks.size(); i++)
		{
			bool new_has_mask = std::find(
						new_color_masks.begin(),
						new_color_masks.end(),
						color_masks[i]) != new_color_masks.end();
			// if current mask has not been added to new list, loop through
			// color_masks and add all of the masks which match the new one
			if (!new_has_mask)
			{
				for (unsigned int j=i; j<color_masks.size(); j++)
				{
					if (color_masks[j] == color_masks[i])
					{
						new_color_masks.push_back(color_masks[j]);
						new_names.push_back(names[j]);
					}
				}
			}
		}

		names = new_names;
		color_masks = new_color_masks;
		is_sorted_ = true;
	}

	bool is_sorted() const { return is_sorted_; }

private:
	bool is_sorted_;
	std::vector<std::string> names;
	std::vector<Color> color_masks;
};

#endif // RENDER_NAME_LIST_H
