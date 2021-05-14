#pragma once

#include "sm/coro.h"

namespace gameplay {
	namespace itemshop {
        sm::coro::Task Show_ItemShopMenu(int id);
	}
}