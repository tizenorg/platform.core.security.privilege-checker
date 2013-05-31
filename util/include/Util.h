/*
 * Copyright (c) 2013 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _UTILS_H_
#define _UTILS_H_

#include <vector>
#include <iostream>
#include <algorithm>
#include <dlog.h>
#include <sqlite3.h>
#include <memory>
#include <string>
#include <CheckerTypes.h>

#define	TryCatch(condition, expr, ...)	\
	if (!(condition)) { \
		LOGE(__VA_ARGS__); \
		expr; \
		goto CATCH; \
	} else {;}


#define TryReturn(condition, expr, r, ...) \
	if ( !(condition) ) { \
		LOGE(__VA_ARGS__); \
		expr; \
		return r; \
	} else {;}

auto StmtDeleter = [&](sqlite3_stmt* pPtr) { LOGI("sqlite3_finalize"); sqlite3_reset (pPtr); sqlite3_finalize(pPtr); };
auto DbDeleter = [&](sqlite3* pPtr) { LOGI("sqlite3_close"); sqlite3_close(pPtr); };

class Util
{
public:
	static std::string toHash (std::string src);
	static const char* GetErrorMessage(int errorNo);
};
#endif //_UTILS_H_
