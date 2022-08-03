#ifndef COLUMNS_ALIGNMENT_HPP
#define COLUMNS_ALIGNMENT_HPP

#include <string> // std::string
#include <locale> // std::locale

namespace columns_alignment
{

std::string columns_alignment(
	const std::string & str_,
	char alignment_char_ = '\v',
	const std::locale & locale_ = std::locale::classic());

} // namespace columns_alignment

#endif
