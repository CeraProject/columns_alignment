// Copyright 2020 Michael Averbukh
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// 	http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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
