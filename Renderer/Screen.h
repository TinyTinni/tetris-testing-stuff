#pragma once

#include <memory>
#include <vector>
#include <algorithm>

struct Cursor
{
	void setX(unsigned int x);
	void setY(unsigned int y);
};


//todo: singleton
class Screen
{
	mutable std::vector<std::shared_ptr<Cursor>> m_cursors;
public:

	/// create and returns a cursor. Screen does not own the cursor and it will be destroyed, if not used anymore
	std::shared_ptr<Cursor> createCursor() { m_cursors.emplace_back(std::make_shared<Cursor>()); return m_cursors.back(); }

	const std::vector<std::shared_ptr<Cursor>>& getCursors() const
	{
		//checks, if all cursors are still valid
		m_cursors.erase(std::remove_if(std::begin(m_cursors), std::end(m_cursors), [](const auto it) {return it.unique();}), std::cend(m_cursors));
		return m_cursors;
	}
};