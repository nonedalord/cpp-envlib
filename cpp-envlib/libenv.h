//
//  libenv.h
//
//  Copyright (c) 2025 Daniil Aleev (sokeone1@gmail.com). All rights reserved.
//  MIT License
//

#pragma once

#define CPPLIBENV_VERSION "1.0"

#include <string>
#include <optional>
#include <variant>
#include <unordered_map>
#include <type_traits>
#include <algorithm>
#include <stdexcept>
#include <memory>

namespace env_cfg
{
	enum class EnvCfgTypes
	{
		string_,
		int_,
		double_,
		longlong_,
		bool_
	};
	class EnvCfgException : public std::runtime_error
	{
	public:
		EnvCfgException(const std::string& message) : std::runtime_error(FormatErrorMessage(message)) {}
	private:
		static std::string FormatErrorMessage(const std::string& message)
		{
			return "Error while getting environment value: " + message;
		}
	};

	class EnvCfgBadGet : public EnvCfgException
	{
	public:
		EnvCfgBadGet(const std::string& message) : EnvCfgException(message) {}
	};

	class EnvCfg
	{
	public:
		using EnvValue = std::optional<std::variant<int, double, long long, std::string, bool, char*, EnvCfgTypes>>;
		/**
 		* @brief Retrieves a pre-initialized environment value by key or returns a default value.
 		* 
 		* This method attempts to fetch the value of the environment variable `env_name` initialized via `InitEnv`. 
 		* If the value is unavailable (not found, empty, or type mismatch), it returns default value it it was  
 		* provided during `InitEnv` initialization.
 		* 
 		* @tparam T Supported types: `int`, `double`, `std::string`, `long long`, `bool`.
 		*           Unsupported types will be blocked at compile time.
 		* 
 		* @param env_name Key name initialized via `InitEnv`.
 		* 
 		* @return T 
		*         - Returns a copy of the initialized value if:
 		*             * Key exists in pre-initialized data
 		*             * Value type matches `T`
 		*             * Value is non-empty
		*		  - On failure: Throw EnvCfgBadGet exception in cases:
		*             * Key not found in initialization data
 		*             * Value is empty
 		*             * Type mismatch with stored value
 		* @note This method throw EnvCfgBadGet exception on errors.
		*/
		template <typename T, typename = std::enable_if_t <
			std::disjunction_v <
			std::is_same<T, int>,
			std::is_same<T, double>,
			std::is_same<T, std::string>,
			std::is_same<T, long long>,
			std::is_same<T, bool>
			>
			>>
		T Get(const std::string& env_name) const;
		/**
 		* @brief Checks if the environment configuration data is empty.
 		* 
 		* This method verifies whether any environment variables were successfully 
 		* initialized and stored during the `InitEnv` call.
 		* 
 		* @return bool 
 		*         - `true`: No environment data was loaded (container is empty).
 		*         - `false`: Environment data exists (at least one key-value pair is present).
 		* 
 		* @note This check is performed on the initialized data, not the system environment.
 		*/
		inline bool Empty() const
		{
			return m_env_result.empty();
		}
		/**
 		* @brief Retrieves a pre-initialized environment value by key or returns a default value.
 		* 
 		* This method attempts to fetch the value of the environment variable `env_name` initialized via `InitEnv`. 
 		* If the value is unavailable (not found, empty, or type mismatch), it returns default value it it was  
 		* provided during `InitEnv` initialization.
 		* @tparam T Supported types: `int`, `double`, `std::string`, `long long`, `bool`.
 		*           Unsupported types will be blocked at compile time.
 		* 
 		* @param env_name Environment name used during `InitEnv` initialization.
 		* 
 		* @return std::unique_ptr<T>
 		*         - Contains a copy of the initialized value if:
 		*             * Key exists in pre-initialized data
 		*             * Value type matches `T`
 		*             * Value is non-empty
 		*         - Returns `nullptr` if:
 		*             * Key not found in initialization data
 		*             * Value is empty
 		*             * Type mismatch with stored value
 		* 
 		* @note Noexcept guarantee: This method never throws exceptions.
		**/
		template <typename T, typename = std::enable_if_t <
			std::disjunction_v <
			std::is_same<T, int>,
			std::is_same<T, double>,
			std::is_same<T, std::string>,
			std::is_same<T, long long>,
			std::is_same<T, bool>
			>
			>>
		const std::unique_ptr<T> GetP(const std::string& env_name) const noexcept;
		/**
 		* @brief Checks if a specific environment key has a valid initialized value.
 		* 
 		* This method verifies whether the key `env_name` exists in the pre-initialized data via `InitEnv`  
 		* and contains a non-empty value of any supported type.
 		* 
 		* @param env_name Key name to check in the initialized environment data.
 		* 
 		* @return bool 
 		*         - `true`: Key exists **and** has a valid value (not `std::nullopt`).
 		*         - `false`: Key does not exist **or** value is empty.
 		* 
 		* @note This check is performed only on the data initialized via `InitEnv`, 
 		*       not the live system environment.
 		* @note Does not validate the value's type. Use `IsType<T>()` for type-specific checks.
		*/
		bool HasValue(const std::string& env_name) const noexcept;
		/**
 		* @brief Initializes environment configuration from a provided key-value map(EnvCfg::EnvMap).
 		* 
 		* This method populates the internal environment store using a map of expected keys 
 		* and their type hints or default values. Each entry is processed as follows:
 		* - If the entry contains a value of `EnvCfgTypes`, it attempts to fetch and parse 
 		*   the corresponding environment variable to the specified type.
 		* - If the entry contains a direct value (e.g., `int`, `std::string`), it uses it 
 		*   as a default if the environment variable is not set.
 		* - If neither the environment variable nor a default value is provided, 
 		*   the entry is stored as `std::nullopt`.
 		* 
 		* @param env_map A map where:
 		*   - **Key**: Environment variable name (e.g., "PORT")
 		*   - **Value**: Either:
 		*     * A type tag from `EnvCfgTypes` (e.g., `EnvCfgTypes::int_`) to enforce parsing.
		*
		* @note This method throw EnvCfgException exception on errors.
 		*/
		void InitEnv(std::unordered_map<std::string, EnvValue>& env_map);
		/**
 		* @brief Checks if the initialized environment value for a key matches the specified type.
 		* 
 		* This method verifies whether the value stored for the key `env_name` initialized via `InitEnv` 
 		* is of type `T` and is non-empty.
 		* 
 		* @tparam T Supported types: `int`, `double`, `std::string`, `long long`, `bool`.
 		*           Unsupported types will be blocked at compile time.
 		* 
 		* @param env_name Key name to check in the initialized environment data.
 		* 
 		* @return bool 
 		*         - `true`: If all conditions are met:
 		*             * Key exists in initialized data
 		*             * Value is non-empty
 		*             * Value's type **exactly matches** `T`
 		*         - `false`: If any condition fails:
 		*             * Key does not exist
 		*             * Value is empty
 		*             * Type mismatch
 		* 
 		* @note This check is performed only on data initialized via `InitEnv`.
 		* @note For existence check without type validation, use `HasValue()`
 		*/
		template <typename T, typename = std::enable_if_t <
			std::disjunction_v <
			std::is_same<T, int>,
			std::is_same<T, double>,
			std::is_same<T, std::string>,
			std::is_same<T, long long>,
			std::is_same<T, bool>
			>
			>>
		bool IsType(const std::string& env_name) const noexcept;
		/**
 		* @brief Retrieves an environment variable and parses it into the specified type.
 		* 
 		* This static method fetches the value of the environment variable `env_name`, 
 		* converts it to the requested type `T`, and returns the parsed value directly.
 		* 
 		* @tparam T Supported types: `int`, `double`, `std::string`, `long long`, `bool`.
 		*           Attempts to use unsupported types will result in a compile-time error.
 		* 
 		* @param env_name Name of the environment variable to retrieve (case-sensitive).
 		*	 
 		* @return T 
 		*         - On success: Returns the parsed value as type `T`
 		*         - On failure: Throw EnvCfgBadGet exception in cases:
 		*             * Environment variable doesn't exist
 		*             * Variable exists but has empty value
 		*             * Parsing to target type fails
 		* 
 		* @note This method throw EnvCfgBadGet exception on errors.
		*/
		template <typename T, typename = std::enable_if_t <
			std::disjunction_v <
			std::is_same<T, int>,
			std::is_same<T, double>,
			std::is_same<T, std::string>,
			std::is_same<T, long long>,
			std::is_same<T, bool>
			>
			>>
		static T GetW(const std::string& env_name);
		/**
		* @brief Retrieves an environment variable and parses it into the specified type.
		*
		* This static method fetches the value of the environment variable `env_name`, converts it
		* to the requested type `T`, and returns ownership via a `std::unique_ptr<T>`.
		*
		* @tparam T Supported types: `int`, `double`, `std::string`, `long long`, `bool`.
		*           Attempts to use unsupported types will result in a compile-time error.
		*
		* @param env_name Name of the environment variable to retrieve .
		*
		* @return std::unique_ptr<T>
		*         - On success: Contains the parsed value (caller owns the pointer)
		*         - On failure: Returns `nullptr` if:
		*             * Environment variable doesn't exist
		*             * Variable exists but has empty value
		*             * Parsing to target type fails
		*
		* @note Noexcept guarantee: This method never throws exceptions.
		*/
		template <typename T, typename = std::enable_if_t <
			std::disjunction_v <
			std::is_same<T, int>,
			std::is_same<T, double>,
			std::is_same<T, std::string>,
			std::is_same<T, long long>,
			std::is_same<T, bool>
			>
			>>
		static const std::unique_ptr<T> GetWP(const std::string& env_name) noexcept;
	private:
		template <class T>
		static std::optional<T> GetEnvByType(const std::string& env_name);
		static std::string GetEnv(const std::string& env_name) noexcept;
		void ProcessEntry(const std::pair<std::string, EnvValue>& entry);
		template <typename T>
		void HandleType(const EnvValue& value, const std::string& env_name);
		void HandleEnumType(const EnvValue& value, const std::string& env_name);
		template<typename T>
		bool TryHandleType(const EnvValue& value, const std::string& env_name);
		
		std::unordered_map<std::string, EnvValue> m_env_result;
	};

	using EnvMap = std::unordered_map<std::string, EnvCfg::EnvValue>;

	template <typename T, typename = std::enable_if_t <
		std::disjunction_v <
		std::is_same<T, int>,
		std::is_same<T, double>,
		std::is_same<T, std::string>,
		std::is_same<T, long long>,
		std::is_same<T, bool>
		>
		>>
	inline T EnvCfg::Get(const std::string& env_name) const
	{
		auto it = m_env_result.find(env_name);
		if (it == m_env_result.end()) 
		{
			throw EnvCfgBadGet(env_name + " not found ");
		}
		if (!it->second.has_value()) 
		{
			throw EnvCfgBadGet("no value for " + env_name);
		}
		try 
		{
			return std::get<T>(it->second.value());
		}
		catch (const std::bad_variant_access&) 
		{
			throw EnvCfgBadGet("invalid type for " + env_name);
		}
	}

	template <typename T, typename = std::enable_if_t <
		std::disjunction_v <
		std::is_same<T, int>,
		std::is_same<T, double>,
		std::is_same<T, std::string>,
		std::is_same<T, long long>,
		std::is_same<T, bool>
		>
		>>
	inline const std::unique_ptr<T> EnvCfg::GetP(const std::string& env_name) const noexcept
	{
		auto it = m_env_result.find(env_name);
		if (it == m_env_result.end())
		{
			return nullptr;
		}
		if (!it->second.has_value())
		{
			return nullptr;
		}
		try
		{
			if (auto* t_ptr = std::get_if<T>(&it->second.value()))
			{
				return std::make_unique<T>(*t_ptr);
			}
			return nullptr;
		}
		catch (const std::bad_variant_access&)
		{
			return nullptr;
		}
	}

	template <typename T, typename = std::enable_if_t <
		std::disjunction_v <
		std::is_same<T, int>,
		std::is_same<T, double>,
		std::is_same<T, std::string>,
		std::is_same<T, long long>,
		std::is_same<T, bool>
		>
		>>
	inline bool EnvCfg::IsType(const std::string& env_name) const noexcept
	{
		auto it = m_env_result.find(env_name);
		if (it == m_env_result.end())
		{
			return false;
		}
		if (!it->second.has_value())
		{
			return false;
		}
		if (auto* t_ptr = std::get_if<T>(&it->second.value()))
		{
			return true;
		}
		return false;
	}

	template <typename T, typename = std::enable_if_t <
		std::disjunction_v <
		std::is_same<T, int>,
		std::is_same<T, double>,
		std::is_same<T, std::string>,
		std::is_same<T, long long>,
		std::is_same<T, bool>
		>
		>>
	inline T EnvCfg::GetW(const std::string& env_name)
	{
		auto value = GetEnvByType<T>(env_name);
		if (value.has_value())
		{
			T ret_val = value.value();
			return ret_val;
		}
		else
		{
			throw EnvCfgBadGet("no value for " + env_name);
		}
	}

	template <typename T, typename = std::enable_if_t <
		std::disjunction_v <
		std::is_same<T, int>,
		std::is_same<T, double>,
		std::is_same<T, std::string>,
		std::is_same<T, long long>,
		std::is_same<T, bool>
		>
		>>
	inline const std::unique_ptr<T> EnvCfg::GetWP(const std::string& env_name) noexcept
	{
		auto value = GetEnvByType<T>(env_name);
		if (value.has_value())
		{
			return std::make_unique<T>(value.value());
		}
		else
		{
			return nullptr;
		}
	}

	template<class T>
	inline std::optional<T> EnvCfg::GetEnvByType(const std::string& env_name)
	{
		std::string result = GetEnv(env_name);
		if (result.empty())
		{
			return std::nullopt;
		}
		else if constexpr (std::is_same_v<T, std::string>)
		{
			return result;
		}
		else if constexpr (std::is_same_v<T, const char*>)
		{
			return result.c_str();
		}
		else if constexpr (std::is_same_v<T, int>)
		{
			try
			{
				T result_int = std::stoi(result);
				return result_int;
			}
			catch (const std::invalid_argument&)
			{
				throw std::invalid_argument("expected int " + result);
			}
			catch (const std::out_of_range&)
			{
				throw std::out_of_range("int overflow " + result);
			}
			catch (const std::exception& err)
			{
				throw std::runtime_error("catch unhandled exception " + static_cast<std::string>(err.what()));
			}
		}
		else if constexpr (std::is_same_v<T, double>)
		{
			try
			{
				T result_double = std::stod(result);
				return result_double;
			}
			catch (const std::invalid_argument&)
			{
				throw std::invalid_argument("expected double " + result);
			}
			catch (const std::exception& err)
			{
				throw std::runtime_error("catch unhandled exception " + static_cast<std::string>(err.what()));
			}
		}
		else if constexpr (std::is_same_v<T, long long>)
		{
			try
			{
				T result_longlong = std::stoll(result);
				return result_longlong;
			}
			catch (const std::invalid_argument&)
			{
				throw std::invalid_argument("expected long long " + result);
			}
			catch (const std::out_of_range&)
			{
				throw std::out_of_range("long long overflow " + result);
			}
			catch (const std::exception& err)
			{
				throw std::runtime_error("catch unhandled exception " + static_cast<std::string>(err.what()));
			}
		}
		else if constexpr (std::is_same_v<T, bool>)
		{
			std::transform(result.begin(), result.end(), result.begin(),
				[](unsigned char a) {return std::tolower(a); });
			if (result == "true")
			{
				T result_bool = true;
				return result_bool;
			}
			else if (result == "false")
			{
				T result_bool = false;
				return result_bool;
			}
			else
			{
				throw std::logic_error("expected bool " + result);
			}
		}
		else
		{
			std::string type = typeid(T).name();
			throw std::runtime_error("unhandled type " + type);
		}
	}

	template<typename T>
	inline void EnvCfg::HandleType(const EnvValue& value, const std::string& env_name) 
	{

		using ValueType = std::decay_t<T>;

		if (std::optional<ValueType> env_val = GetEnvByType<ValueType>(env_name))
		{
			m_env_result[env_name] = env_val.value();
		}
		else if (value.has_value()) 
		{
			const auto& variant_val = value.value();

			if (std::holds_alternative<ValueType>(variant_val))
			{
				const ValueType* ptr = std::get_if<ValueType>(&variant_val);
				if (ptr) 
				{
					m_env_result[env_name] = *ptr;
				}
				else 
				{
					m_env_result[env_name] = std::nullopt;
				}
			}
			else 
			{
				m_env_result[env_name] = std::nullopt;
			}
		}
		else 
		{
			m_env_result[env_name] = std::nullopt;
		}
	}

	template<typename T>
	inline bool EnvCfg::TryHandleType(const EnvValue& value, const std::string& env_name)
	{
		const bool is_type_present = std::visit([&env_name, this](const auto& v) {
			if constexpr (std::is_same_v<std::decay_t<decltype(v)>, std::decay_t<T>>)
			{
				HandleType<T>(v, env_name);
				return true;
			}
			return false;
			}, value.value());
		return is_type_present;
	}

	inline void EnvCfg::InitEnv(std::unordered_map<std::string, EnvValue>& env_map)
	{
		for (const auto& entry : env_map)
		{
			ProcessEntry(entry);
		}
	}

	inline std::string EnvCfg::GetEnv(const std::string& env_name) noexcept
	{
		if (char* env_value = std::getenv(env_name.c_str()))
		{
			return env_value;
		}
		else
		{
			return std::string();
		}
	}

	inline void EnvCfg::ProcessEntry(const std::pair<std::string, EnvValue>& entry)
	{
		const auto& [env_name, default_value] = entry;
		try
		{
			std::visit([&](const auto& val) {
				if constexpr (std::is_same_v<std::decay_t<decltype(val)>, EnvCfgTypes>)
				{
					HandleEnumType(val, env_name);
				}
				else
				{
					TryHandleType<decltype(val)>(val, env_name);
				}
				}, default_value.value());
		}
		catch (const std::exception& err)
		{
			throw EnvCfgException(err.what());
		}
	}

	inline void EnvCfg::HandleEnumType(const EnvValue& value, const std::string& env_name)
	{
		auto type = std::get<EnvCfgTypes>(value.value());
		switch (type)
		{
		case EnvCfgTypes::string_:
			HandleType<std::string>(value, env_name);
			break;
		case EnvCfgTypes::int_:
			HandleType<int>(value, env_name);
			break;
		case EnvCfgTypes::double_:
			HandleType<double>(value, env_name);
			break;
		case EnvCfgTypes::longlong_:
			HandleType<long long>(value, env_name);
			break;
		case EnvCfgTypes::bool_:
			HandleType<bool>(value, env_name);
			break;
		default:
			throw std::runtime_error("unknown enum type");
		}
	}

	inline bool EnvCfg::HasValue(const std::string& env_name) const noexcept
	{
		auto it = m_env_result.find(env_name);
		if (it == m_env_result.end())
		{
			return false;
		}
		if (!it->second.has_value())
		{
			return false;
		}
		return true;
	}
} // namespace env_cgf