#include "columns_alignment.hpp"

#include <string> // std::string
#include <vector> // std::vector
#include <locale> // std::locale, std::isdigit
#include <cstddef> // std::size_t
#include <cassert> // assert

namespace columns_alignment
{

class columns_alignment_parser
{
private:
	using positions_type = std::vector<std::vector<std::size_t> >;
	using max_positions_type = std::vector<std::size_t>;

	static std::size_t tabulate(std::size_t position_)
	{
		using std::size_t;

		constexpr size_t tab_size = 4;

		return (position_ / tab_size + 1) * tab_size;
	}

	static void columns_alignment_positions(
		const std::string & str_,
		char alignment_char_,
		positions_type & positions_,
		std::size_t & total_alignment_char_count_,
		std::size_t & alignment_count_)
	{
		using std::size_t;

		positions_type pos;
		size_t total_alignment_char_count = 0; // For entire str_
		size_t alignment_count = 0; // Maximum number of columns alignments in one line

		size_t pos_in_line = 0;
		size_t alignment_index_in_line = 0;

		for (const auto ch : str_)
		{
			if (ch == alignment_char_)
			{
				// For every line, which has at least one alignment character, store position in the line of alignment
                // character into pos
				if (alignment_index_in_line == 0)
				{
					pos.emplace_back(1, pos_in_line);
				}
				else
				{
					pos.back().push_back(pos_in_line);
				}

				++total_alignment_char_count;
				++alignment_index_in_line;

				// Increase alignment_count if new maximum is reached
				if (alignment_index_in_line > alignment_count)
				{
					alignment_count = alignment_index_in_line;
				}
			}
			else
			{
				switch (ch)
				{
					case '\n': // Next line
						// Virtual alignment at the end of line is required for right alignment
						if (alignment_index_in_line != 0)
						{
							pos.back().push_back(pos_in_line);
						}

						pos_in_line = 0;
						alignment_index_in_line = 0;
						break;

					case '\t': // Tab
						pos_in_line = tabulate(pos_in_line);
						break;

					default: // Any other character
						++pos_in_line;
						break;
				}
			}
		}

		// Virtual alignment at the end of line is required for right alignment
		if (alignment_index_in_line != 0)
		{
			pos.back().push_back(pos_in_line);
		}

		positions_.swap(pos);
		total_alignment_char_count_ = total_alignment_char_count;
		alignment_count_ = alignment_count;
	}

	static max_positions_type
		max_positions(positions_type & positions_, std::size_t alignment_count_, std::size_t & new_char_count_)
	{
		using std::size_t;

		max_positions_type result(alignment_count_ + 1);
		size_t alignment_index_in_line = 0;
		size_t total_new_char_count = 0;
		for (auto & mp : result)
		{
			size_t new_char_count = 0;
			size_t line_count = 0;
			size_t max_pos = 0;
			for (const auto & ln : positions_)
			{
				if (alignment_index_in_line < ln.size())
				{
					const auto p = ln[alignment_index_in_line];
					if (p > max_pos)
					{
						new_char_count += line_count * (p - max_pos);
						max_pos = p;
					}
					new_char_count += max_pos - p;
					++line_count;
				}
			}

			// Adjust subsequent positions if exists
			if (alignment_index_in_line < result.size() - 1)
			{
				for (auto & ln : positions_)
				{
					const auto delta = max_pos - ln[alignment_index_in_line];
					for (size_t i = alignment_index_in_line + 1; i < ln.size(); ++i)
					{
						ln[i] += delta;
					}
				}
			}

			mp = max_pos;
			total_new_char_count += new_char_count;
			++alignment_index_in_line;
		}

		new_char_count_ = total_new_char_count;
		return result;
	}

	static std::size_t calculate_right_alignment(
		const std::string & str_,
		std::size_t char_index_,
		char alignment_char_,
		const std::locale & locale_,
		std::size_t result_pos_in_line_,
		const max_positions_type & max_pos_,
		std::size_t alignment_index_in_line_)
	{
		using std::isdigit;
		using std::size_t;

		if (char_index_ < str_.size() && isdigit(str_[char_index_], locale_))
		{
			size_t column_pos = result_pos_in_line_ + 1;
			for (size_t p = char_index_ + 1; p < str_.size(); ++p)
			{
				if (str_[p] == alignment_char_)
				{
					break;
				}

				switch (str_[p])
				{
					case '\n': // Next line
						p = str_.size() - 1; // Break the loop
						break;

					case '\t': // Tab
						column_pos = tabulate(column_pos);
						break;

					default: // Any other character
						++column_pos;
						break;
				}
			}
			return max_pos_[alignment_index_in_line_ + 1] - max_pos_[alignment_index_in_line_]
				- (column_pos - result_pos_in_line_);
		}

		return 0;
	}

	static std::string perform_columns_alignment(
		const std::string & s,
		char alignment_char_,
		const std::locale & locale_,
		const max_positions_type & max_pos_,
		std::size_t total_alignment_char_count_,
		std::size_t alignment_count_,
		std::size_t new_char_count_)
	{
		using std::size_t;
		using std::string;

		string result;
		result.resize(s.size() - total_alignment_char_count_ + new_char_count_);
		size_t result_index = 0;
		size_t result_pos_in_line = 0;
		size_t alignment_index_in_line = 0;
		size_t absolute_pos = 0;

		for (const auto ch : s)
		{
			++absolute_pos;
			if (ch == alignment_char_)
			{
				assert(alignment_index_in_line < alignment_count_);

				const auto right_alignment = calculate_right_alignment(
					s, absolute_pos, alignment_char_, locale_, result_pos_in_line, max_pos_, alignment_index_in_line);
				const size_t chars_to_insert = max_pos_[alignment_index_in_line] + right_alignment - result_pos_in_line;
				for (size_t i = 0; i < chars_to_insert; ++i)
				{
					result[result_index] = ' ';
					++result_index;
					assert(result_index < result.size());
					++result_pos_in_line;
				}
				++alignment_index_in_line;
			}
			else
			{
				switch (ch)
				{
					case '\n': // Next line
						result[result_index] = ch;
						++result_index;
						result_pos_in_line = 0;
						alignment_index_in_line = 0;
						break;

					case '\t': // Tab
						result[result_index] = ch;
						++result_index;
						result_pos_in_line = tabulate(result_pos_in_line);
						break;

					default: // Any other character
						result[result_index] = ch;
						++result_index;
						++result_pos_in_line;
						break;
				}
			}
		}
		result.resize(result_index);

		return result;
	}

public:
	columns_alignment_parser() = delete;
	columns_alignment_parser(const columns_alignment_parser &) = delete;
	columns_alignment_parser(columns_alignment_parser &&) = delete;
	columns_alignment_parser & operator=(const columns_alignment_parser &) = delete;
	columns_alignment_parser & operator=(columns_alignment_parser &&) = delete;

	static std::string columns_alignment(const std::string & str_, char alignment_char_, const std::locale & locale_)
	{
		using std::size_t;

		positions_type pos;
		size_t total_alignment_char_count = 0; // For entire str_
		size_t alignment_count = 0; // Maximum number of alignments in one line

		columns_alignment_positions(str_, alignment_char_, pos, total_alignment_char_count, alignment_count);
		if (pos.empty())
		{
			return str_;
		}

		size_t new_char_count = 0; // Number of characters to be inserted to perform alignment
		const auto max_pos = max_positions(pos, alignment_count, new_char_count);
		return perform_columns_alignment(
			str_, alignment_char_, locale_, max_pos, total_alignment_char_count, alignment_count, new_char_count);
	}
};

std::string columns_alignment(const std::string & str_, char alignment_char_, const std::locale & locale_)
{
	return columns_alignment_parser::columns_alignment(str_, alignment_char_, locale_);
}

} // namespace columns_alignment
