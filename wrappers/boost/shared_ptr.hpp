#include <memory>

namespace boost {
	template<class...Args>
	using shared_ptr = std::shared_ptr<Args...>;
}