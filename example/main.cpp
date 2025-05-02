#include <iostream>
#include <cstdlib>
#include <string>

#include "libenv.h"

void SetIntEnv(const std::string& name, int value, bool overwrite = true) 
{
    std::string str_value = std::to_string(value);
    if (setenv(name.c_str(), str_value.c_str(), overwrite ? 1 : 0))
    {
        perror("setenv failed");
    }
}

void SetLongLongEnv(const std::string& name, long long value, bool overwrite = true) 
{
    std::string str_value = std::to_string(value);
    if (setenv(name.c_str(), str_value.c_str(), overwrite ? 1 : 0))
    {
        perror("setenv failed");
    }
}

void SetDoubleEnv(const std::string& name, double value, bool overwrite = true) 
{
    std::string str_value = std::to_string(value);
    if (setenv(name.c_str(), str_value.c_str(), overwrite ? 1 : 0))
    {
        perror("setenv failed");
    }
}

void SetStrEnv(const std::string& name, const std::string& value, bool overwrite = true)
{
    if (setenv(name.c_str(), value.c_str(), overwrite ? 1 : 0))
    {
        perror("setenv failed");
    }
}

int main()
{
    env_cfg::EnvMap env_conf =
	{
			{"TEST_ENV1", env_cfg::EnvCfgTypes::string_},
			{"TEST_ENV2", env_cfg::EnvCfgTypes::int_},
            {"TEST_ENV3", env_cfg::EnvCfgTypes::bool_},
            {"TEST_ENV4", env_cfg::EnvCfgTypes::double_},
            {"TEST_ENV5", env_cfg::EnvCfgTypes::longlong_},
            {"TEST_ENV1_1", "TEST_ENV1_1"},
            {"TEST_ENV1_2", env_cfg::EnvCfgTypes::string_},
            {"TEST_ENV1_3", env_cfg::EnvCfgTypes::string_},
            {"TEST_ENV1_4", env_cfg::EnvCfgTypes::string_},
            {"TEST_ENV3_1", env_cfg::EnvCfgTypes::bool_},
            {"TEST_ENV3_2", 1754},
            {"TEST_ENV4_1", env_cfg::EnvCfgTypes::int_}
	};
    SetStrEnv("TEST_ENV1", "STR_ENV");
    SetIntEnv("TEST_ENV2", 422);
    SetStrEnv("TEST_ENV3", "TRUE");
    SetDoubleEnv("TEST_ENV4", 3.14);
    SetLongLongEnv("TEST_ENV5", 35346532131);
    SetStrEnv("TEST_ENV1_4", "PTR_ENV");
    //SetIntEnv("TEST_ENV3_1", 543985);
    //SetLongLongEnv("TEST_ENV4_1", 54398532131322);
    env_cfg::EnvCfg a;
    try
    {
        a.InitEnv(env_conf);
    }
    catch (const env_cfg::EnvCfgException& err)
    {
        //If uncomment SetIntEnv("TEST_ENV3_1", 543985); than will throw : Error while getting environment value: expected bool 543985
        //If uncomment SetLongLongEnv("TEST_ENV4_1", 54398532131322); than will throw : Error while getting environment value: int overflow 54398532131322
        std::cout << err.what() << std::endl;
        return EXIT_FAILURE;
    }

    std::string env1 = a.Get<std::string>("TEST_ENV1");
    std::cout << "TEST_ENV1 = " << env1 << std::endl; //TEST_ENV1 = STR_ENV

    int env2 = a.Get<int>("TEST_ENV2");
    std::cout << "TEST_ENV2 = " << env2 << std::endl; //TEST_ENV2 = 422

    bool env3 = a.Get<bool>("TEST_ENV3");
    std::cout << "TEST_ENV3 = " << std::boolalpha << env3 << std::endl; //TEST_ENV3 = true

    double env4 = a.Get<double>("TEST_ENV4");
    std::cout << "TEST_ENV4 = " << env4 << std::endl; //TEST_ENV4 = 3.14

    long long env5 = a.Get<long long>("TEST_ENV5");
    std::cout << "TEST_ENV5 = " << env5 << std::endl; //TEST_ENV5 = 35346532131
    
    if (a.HasValue("TEST_ENV1_1") && a.IsType<std::string>("TEST_ENV1_1"))
    {
        std::cout << "TEST_ENV1_1 = " << a.Get<std::string>("TEST_ENV1_1") << std::endl; //TEST_ENV2_1 = TEST_ENV2_1
    
    }
    std::string env1_1 = a.Get<std::string>("TEST_ENV1_1");
    std::cout << "TEST_ENV1_1 = " << env1_1 << std::endl; //TEST_ENV1_1 = TEST_ENV1_1

    try
    {
        std::string env1_1 = a.Get<std::string>("TEST_ENV1_2"); 
    }
    catch (const env_cfg::EnvCfgBadGet& err)
    {
        std::cout << "TEST_ENV1_2 error: " << err.what() << std::endl; //TEST_ENV1_2 error: Error while getting environment value: no value for TEST_ENV1_2
    }

    if (auto ptr = a.GetP<std::string>("TEST_ENV1_3"))
    {
        std::cout << *ptr << std::endl;
    }
    else
    {
        std::cout << "no value for TEST_ENV1_3" << std::endl; //no value for TEST_ENV1_3
    }

    if (auto ptr = a.GetP<std::string>("TEST_ENV1_4"))
    {
        std::cout << *ptr << std::endl; //PTR_ENV
    }
    else
    {
        std::cout << "no value for TEST_ENV1_4" << std::endl;
    }
    
    SetStrEnv("TEST_ENV2_1", "true");

    std::cout << "TEST_ENV2_1 = " << env_cfg::EnvCfg::GetW<bool>("TEST_ENV2_1") << std::endl; //TEST_ENV2_1 = true

    try
    {
        std::cout << "TEST_ENV2_2 = " << env_cfg::EnvCfg::GetW<int>("TEST_ENV2_2") << std::endl;
    }
    catch (const env_cfg::EnvCfgBadGet& err)
    {
        std::cout << "TEST_ENV2_2 error: " << err.what() << std::endl; //TEST_ENV2_2 = TEST_ENV2_2 error: Error while getting environment value: no value for TEST_ENV2_2
    }

    SetStrEnv("TEST_ENV2_3", "STATIC_W_PTR");
    if (auto ptr = env_cfg::EnvCfg::GetWP<std::string>("TEST_ENV2_3"))
    {
        std::cout << *ptr << std::endl; //STATIC_W_PTR
    }

    SetIntEnv("TEST_ENV2_4", 53453);
    if (auto ptr = env_cfg::EnvCfg::GetWP<std::string>("TEST_ENV2_4"))
    {
        std::cout << *ptr << std::endl; //53453
    }

    if (auto ptr = a.GetP<bool>("TEST_ENV3_1"))
    {
        std::cout << *ptr << std::endl;
    }
    else
    {
        std::cout << "no value for TEST_ENV3_1" << std::endl; //no value for TEST_ENV3_1
    }

    try
    {
        a.Get<bool>("TEST_ENV3_1");
    }
    catch (const env_cfg::EnvCfgBadGet& err)
    {
        std::cout << err.what() << std::endl; //Error while getting environment value: no value for TEST_ENV3_1
    }

    try
    {
        a.Get<std::string>("TEST_ENV3_2");
    }
    catch (const env_cfg::EnvCfgBadGet& err)
    {
        std::cout << err.what() << std::endl; //Error while getting environment value: invalid type for TEST_ENV3_2
    }

    if (auto ptr = a.GetP<std::string>("TEST_ENV3_2"))
    {
        std::cout << *ptr << std::endl;
    }
    else
    {
        std::cout << "no value for TEST_ENV3_2" << std::endl; //no value for TEST_ENV3_2
    }

    return EXIT_SUCCESS;
}