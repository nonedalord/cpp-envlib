#include <iostream>
#include <cstdlib>
#include <string>

#include "libenv.h"

int main()
{
    env_cfg::EnvMap env_conf =
	{
			{"TEST_ENV1", env_cfg::EnvCfgTypes::string_},
			{"TEST_ENV2", env_cfg::EnvCfgTypes::int_},
            {"TEST_ENV3", env_cfg::EnvCfgTypes::bool_}
	};
    try
    {
        env_cfg::EnvCfg::SetEnv("TEST_ENV1", "STR_ENV");
        env_cfg::EnvCfg::SetEnv("TEST_ENV2", "321");
    }
    catch (const env_cfg::EnvSetError& err)
    {
        std::cerr << err.what() << std::endl;
    }
    
    if (env_cfg::EnvCfg::SetEnvN("TEST_ENV3", "TRUE"))
    {
        std::cout << "TEST_ENV3 is setted to TRUE" << std::endl;
    }

    env_cfg::EnvCfg a;
    try
    {
        a.InitEnv(env_conf);
    }
    catch (const env_cfg::EnvException& err)
    {
        std::cerr << err.what() << std::endl;
        return EXIT_FAILURE;
    }

    try
    {
        std::cout << "TEST_ENV1 = " << a.Get<std::string>("TEST_ENV1") << std::endl;
        std::cout << "TEST_ENV2 = " << a.Get<int>("TEST_ENV2") << std::endl;
        std::cout << "TEST_ENV3 = " << a.Get<bool>("TEST_ENV3") << std::endl;
    }
    catch (const env_cfg::EnvBadGet& err)
    {
        std::cerr << err.what() << std::endl;
    }

    try
    {
        std::cout << env_cfg::EnvCfg::GetW<int>("TEST_ENV1") << std::endl;
    }
    catch (const env_cfg::EnvBadGet& err)
    {
        std::cout << err.what() << std::endl;
    }

    std::cout << env_cfg::EnvCfg::GetW<int>("TEST_ENV143").default_value(543) << std::endl;

    for (auto it = a.begin(); it != a.end(); ++it)
    {
        auto [k, v] = *it;
        std::cout << k << " = " << v << '\n';
    }

    for (const auto& [k, v] : a) 
    {
        std::cout << k << " = " << v << '\n';
    }
    return EXIT_SUCCESS;
}