#ifndef MAT_HPP
# define MAT_HPP

# include "vec.hpp"

template <typename T, size_t N, size_t M> struct mat;

using mat3d = mat<double,3,3>;
using mat3f = mat<float,3,3>;
using mat3i = mat<int,3,3>;


template <typename T, size_t N, size_t M>
struct mat {
	std::array<vec<T, M>, N> rows;

//	Default constructor
	mat(void) {
		for (auto& row : rows)
			for (size_t j = 0; j < M; ++j)
				row[j] = T(0);
	}

//	Copy constructor
	mat(const mat&) = default;

//	Constructor from N vectors
	template <typename... Rows, typename = std::enable_if_t<sizeof...(Rows) == N>>
	constexpr mat(const Rows&... rs) {
		static_assert((std::is_constructible_v<vec<T, M>, Rows> && ...),
					  "All rows must be constructible into vec<T, M>");
		size_t i = 0;
		((rows[i++] = vec<T, M>(rs)), ...);
	}

//	Constructor from list of lists
	constexpr mat(std::initializer_list<std::initializer_list<T>> init) {
		size_t i = 0;
		for (auto row_it = init.begin(); row_it != init.end() && i < N; ++row_it, ++i) {
			size_t j = 0;
			for (auto col_it = row_it->begin(); col_it != row_it->end() && j < M; ++col_it, ++j) {
				rows[i][j] = *col_it;
			}
		}
	}

//	Destructor
	~mat(void) {}

//	Constructing a matrix from a continuous array
	static constexpr
	mat			from_array(std::initializer_list<T> list) {
	mat result{};
	auto it = list.begin();
	for (size_t i = 0; i < N; ++i) {
		for (size_t j = 0; j < M; ++j) {
			if (it != list.end()) {
				result[i][j] = *it++;
			}
		}
	}
	return result;
}

	const
	vec<T,M>&	operator[](size_t i) const { return rows[i]; }
	vec<T,M>&	operator[](size_t i) { return rows[i]; }
	template <size_t K>
	mat<T,N,K>	operator*(const mat<T,M,K>& other) const {
		mat<T,N,K> result;
		auto otherT = other.transposed(); // обращаемся по столбцам
		for (size_t i = 0; i < N; ++i)
			for (size_t j = 0; j < K; ++j)
				result[i][j] = dot(rows[i], otherT[j]);
		return result;
	}
	vec<T,N>	operator*(const vec<T,M>& v) const {
		vec<T, N> result;
		for (size_t i = 0; i < N; ++i) {
			T sum = T(0);
			for (size_t j = 0; j < M; ++j)
				sum += rows[i][j] * v[j];
			result[i] = sum;
		}
		return result;
	}

	mat<T,M,N>				transposed(void) const {
		mat<T,M,N> result;
		for (size_t i = 0; i < N; ++i)
			for (size_t j = 0; j < M; ++j)
				result[j][i] = rows[i][j];
		return result;
	}
	constexpr mat<T,M,N>	transpose(void) const {
		mat<T,M,N> result{};
		for (size_t i = 0; i < N; ++i) {
			for (size_t j = 0; j < M; ++j) {
				result[j][i] = (*this)[i][j];
			}
		}
		return result;
	}
	constexpr mat<T, M, N>	inverse_orthonormal(void) const {
		return this->transpose();
	}
	static constexpr mat	identity(void) {
		mat result{};
		constexpr size_t min_dim = (N < M) ? N : M;
		for (size_t i = 0; i < min_dim; ++i) {
			result[i][i] = static_cast<T>(1);
		}
		return result;
	}
	static mat				rotation(const vec<T,3>& axis, T angle_rad) {
		auto u(axis);
		u.normalize();
		T cos = std::cos(angle_rad);
		T sin = std::sin(angle_rad);
		T one_minus_cos = T(1) - cos;
		return mat {
			vec<T,3>(
					 cos + u.x() * u.x() * one_minus_cos,
					 u.x() * u.y() * one_minus_cos - u.z() * sin,
					 u.x() * u.z() * one_minus_cos + u.y() * sin
					 ),
			vec<T,3>(
					 u.y() * u.x() * one_minus_cos + u.z() * sin,
					 cos + u.y() * u.y() * one_minus_cos,
					 u.y() * u.z() * one_minus_cos - u.x() * sin
					 ),
			vec<T,3>(
					 u.z() * u.x() * one_minus_cos - u.y() * sin,
					 u.z() * u.y() * one_minus_cos + u.x() * sin,
					 cos + u.z() * u.z() * one_minus_cos
					 )
		};
	}


	template<typename... Args>
	static constexpr mat	scale(Args... args) {
		static_assert(sizeof...(args) <= (N < M ? N : M), "Too many scale factors");
		mat result = identity();
		T factors[] = {static_cast<T>(args)...};
		for (size_t i = 0; i < sizeof...(args); ++i) {
			result[i][i] = factors[i];
		}
		return result;
	}
	
	template <typename > friend std::ostream& operator<<(std::ostream& os, const mat& m);
};

template <typename T, size_t N, size_t M>
std::ostream& operator<<(std::ostream& os, const mat<T, N, M>& m) {
	for (size_t i = 0; i < N; ++i) {
		os << "[ ";
		for (size_t j = 0; j < M; ++j) {
			os << m[i][j];
			if (j + 1 != M)
				os << " ";
		}
		os << " ]\n";
	}
	return os;
}

template <typename T>
inline mat<T,3,3>	buildONB(const vec<T,3>& n) {
	vec<T,3> t, b;
	if (std::abs(n.z()) < T(0.999)) {
		t = cross(n, {0,0,1}).normalize();
	} else {
		t = cross(n, {0,1,0}).normalize();
	}
	b = cross(n, t);
	return mat<T,3,3>{ t, b, n };
}

template <typename T, size_t N, size_t M, size_t K>
constexpr mat<T,N,K> operator*(const mat<T,N,M>& a, const mat<T,M,K>& b) {
	mat<T, N, K> result{};
	for (size_t i = 0; i < N; ++i) {
		for (size_t k = 0; k < K; ++k) {
			for (size_t j = 0; j < M; ++j) {
				result[i][k] += a[i][j] * b[j][k];
			}
		}
	}
	return result;
}

template <typename T, size_t N, size_t M>
constexpr vec<T,N>	operator*(const mat<T,N,M>& m, const vec<T,M>& v) {
	vec<T,N> result{};
	for (size_t i = 0; i < N; ++i) {
		for (size_t j = 0; j < M; ++j) {
			result[i] += m[i][j] * v[j];
		}
	}
	return result;
}

#endif
