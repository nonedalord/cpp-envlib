#include "../cpp-envlib/libenv.h"
#include <gtest/gtest.h>
#include <limits>

using namespace env_cfg;

class EnvCfgTypeTest : public ::testing::Test {
protected:
    void SetUp() override 
    {
        EnvCfg::SetEnvN("TEST_TYPE", "", true);
    }

    EnvCfg env;
};

TEST_F(EnvCfgTypeTest, TypeCheckInt) 
{
    EnvMap map = {{"TEST_TYPE", EnvCfgTypes::int_}};
    EnvCfg::SetEnv("TEST_TYPE", "42");
    env.InitEnv(map);

    EXPECT_TRUE(env.IsType<int>("TEST_TYPE"));
    EXPECT_EQ(env.Get<int>("TEST_TYPE"), 42);
    EXPECT_FALSE(env.IsType<long long>("TEST_TYPE"));
}

TEST_F(EnvCfgTypeTest, TypeCheckLongLong) 
{
    EnvMap map = {{"TEST_TYPE", EnvCfgTypes::longlong_}};
    EnvCfg::SetEnv("TEST_TYPE", "9223372036854775807");
    env.InitEnv(map);

    EXPECT_TRUE(env.IsType<long long>("TEST_TYPE"));
    EXPECT_EQ(env.Get<long long>("TEST_TYPE"), 9223372036854775807LL);
    EXPECT_FALSE(env.IsType<int>("TEST_TYPE"));
}

TEST_F(EnvCfgTypeTest, TypeCheckDouble) 
{
    EnvMap map = {{"TEST_TYPE", EnvCfgTypes::double_}};
    EnvCfg::SetEnv("TEST_TYPE", "3.1415");
    env.InitEnv(map);

    EXPECT_TRUE(env.IsType<double>("TEST_TYPE"));
    EXPECT_DOUBLE_EQ(env.Get<double>("TEST_TYPE"), 3.1415);
    EXPECT_FALSE(env.IsType<int>("TEST_TYPE"));
}

TEST_F(EnvCfgTypeTest, TypeCheckBool) 
{
    EnvMap map = {{"TEST_TYPE", EnvCfgTypes::bool_}};
    EnvCfg::SetEnv("TEST_TYPE", "true");
    env.InitEnv(map);

    EXPECT_TRUE(env.IsType<bool>("TEST_TYPE"));
    EXPECT_TRUE(env.Get<bool>("TEST_TYPE"));
    EXPECT_FALSE(env.IsType<int>("TEST_TYPE"));
}

TEST_F(EnvCfgTypeTest, TypeCheckString) 
{

    EnvMap map = {{"TEST_TYPE", EnvCfgTypes::string_}};
    EnvCfg::SetEnv("TEST_TYPE", "hello world");
    env.InitEnv(map);

    EXPECT_TRUE(env.IsType<std::string>("TEST_TYPE"));
    EXPECT_EQ(env.Get<std::string>("TEST_TYPE"), "hello world");
}

TEST_F(EnvCfgTypeTest, AutoTypeConversion) 
{
    EnvMap map = {
        {"INT_VAL", 42},
        {"LLONG_VAL", 10000000000LL},
        {"DOUBLE_VAL", 3.14},
        {"BOOL_VAL", true},
        {"STRING_VAL", "test"}
    };
    env.InitEnv(map);

    EXPECT_TRUE(env.IsType<int>("INT_VAL"));
    EXPECT_TRUE(env.IsType<long long>("LLONG_VAL"));
    EXPECT_TRUE(env.IsType<double>("DOUBLE_VAL"));
    EXPECT_TRUE(env.IsType<bool>("BOOL_VAL"));
    EXPECT_TRUE(env.IsType<std::string>("STRING_VAL"));
}

TEST_F(EnvCfgTypeTest, TypeMismatchHandling) 
{
    EnvMap map = {{"TEST_TYPE", 42}};
    env.InitEnv(map);

    EXPECT_TRUE(env.IsType<int>("TEST_TYPE"));
    
    EXPECT_FALSE(env.IsType<double>("TEST_TYPE"));
    EXPECT_FALSE(env.IsType<bool>("TEST_TYPE"));
    EXPECT_FALSE(env.IsType<std::string>("TEST_TYPE"));
    
    EXPECT_FALSE(env.GetN<double>("TEST_TYPE").has_value());

    EXPECT_THROW({
        try 
        {
            env.Get<double>("TEST_TYPE");
        }
        catch (const EnvBadGet& e) 
        {
            std::cout << "\n[Exception] " << e.what() << std::endl;
            throw;
        }
    }, EnvBadGet);
}

TEST_F(EnvCfgTypeTest, ConstCharPtrConversion) 
{
    EnvMap map = {{"TEST_TYPE", "const char* test"}};
    env.InitEnv(map);

    EXPECT_TRUE(env.IsType<std::string>("TEST_TYPE"));
    EXPECT_EQ(env.Get<std::string>("TEST_TYPE"), "const char* test");
}

TEST_F(EnvCfgTypeTest, EnvironmentTypePriority) 
{
    EnvMap map = {{"TEST_TYPE", EnvCfgTypes::int_}};
    EnvCfg::SetEnv("TEST_TYPE", "3.14");
    EXPECT_THROW({
        try
        {
            env.InitEnv(map);
        }
        catch (const EnvException& e) 
        {
            std::cout << "\n[Exception] " << e.what() << std::endl;
            throw;
        }
    }, EnvException);

    EXPECT_FALSE(env.IsType<int>("TEST_TYPE"));
    EXPECT_FALSE(env.GetN<int>("TEST_TYPE").has_value());
    EXPECT_THROW({
        try
        {
            env.Get<int>("TEST_TYPE");
        }
        catch (const EnvException& e) 
        {
            std::cout << "\n[Exception] " << e.what() << std::endl;
            throw;
        }
    }, EnvException);
}

TEST_F(EnvCfgTypeTest, DefaultValueTypePreservation) 
{
    EnvMap map = {
        {"INT_DEF", 42},
        {"LLONG_DEF", 10000000000LL},
        {"DOUBLE_DEF", 3.14},
        {"BOOL_DEF", true},
        {"STRING_DEF", "default"}
    };
    env.InitEnv(map);

    EXPECT_TRUE(env.IsType<int>("INT_DEF"));
    EXPECT_TRUE(env.IsType<long long>("LLONG_DEF"));
    EXPECT_TRUE(env.IsType<double>("DOUBLE_DEF"));
    EXPECT_TRUE(env.IsType<bool>("BOOL_DEF"));
    EXPECT_TRUE(env.IsType<std::string>("STRING_DEF"));
}

TEST_F(EnvCfgTypeTest, StaticIntValidValue) 
{
    EnvCfg::SetEnv("TEST_TYPE", "42");
    int env_value = EnvCfg::GetW<int>("TEST_TYPE");
    
    EXPECT_EQ(env_value, 42);
}

TEST_F(EnvCfgTypeTest, StaticIntInvalidValue) 
{
    EnvCfg::SetEnv("TEST_TYPE", "not_an_int");
    EXPECT_THROW({
        try
        {
            int env_value = EnvCfg::GetW<int>("TEST_TYPE");
        }
        catch (const EnvBadGet& e) 
        {
            std::cout << "\n[Exception] " << e.what() << std::endl;
            throw;
        }
    }, EnvBadGet);
}

TEST_F(EnvCfgTypeTest, StaticIntMissingKey) 
{
    EXPECT_THROW({
        try
        {
            int env_value = EnvCfg::GetW<int>("MISSING_KEY");
        }
        catch (const EnvBadGet& e) 
        {
            std::cout << "\n[Exception] " << e.what() << std::endl;
            throw;
        }
    }, EnvBadGet);
}

TEST_F(EnvCfgTypeTest, StaticStringConversion) 
{
    EnvCfg::SetEnv("TEST_TYPE", "test_value");
    std::string value = EnvCfg::GetW<std::string>("TEST_TYPE");
    
    EXPECT_EQ(value, "test_value");
}

TEST_F(EnvCfgTypeTest, StaticBoolConversion) 
{
    EnvCfg::SetEnv("TEST_TYPE", "true");
    bool env_value = EnvCfg::GetW<bool>("TEST_TYPE");
    
    EXPECT_TRUE(env_value);
}

TEST_F(EnvCfgTypeTest, StaticDefaultValueNoThrow) 
{
    EXPECT_NO_THROW(EnvCfg::GetW<long long>("MISSING_KEY_2").default_value(999LL));
    long long env_value_ll = EnvCfg::GetW<long long>("MISSING_KEY_2").default_value(999LL);
    EXPECT_EQ(env_value_ll, 999LL);

    EnvCfg::SetEnv("TEST_TYPE", "true");

    EXPECT_NO_THROW(EnvCfg::GetW<int>("TEST_TYPE").default_value(932));

    int env_value_int = EnvCfg::GetW<int>("TEST_TYPE").default_value(932);
    EXPECT_EQ(env_value_int, 932);
}

TEST_F(EnvCfgTypeTest, NoexceptReturnsValueWhenValid) 
{
    EnvMap map = {{"TEST_INT", 42}};
    env.InitEnv(map);

    auto env_value_int = env.GetN<int>("TEST_INT");
    EXPECT_TRUE(env_value_int .has_value());
    EXPECT_EQ(env_value_int .value(), 42);

    EnvMap str_map = {{"TEST_STR", "hello"}};
    env.InitEnv(str_map);
    auto env_value_str = env.GetN<std::string>("TEST_STR");
    EXPECT_EQ(env_value_str.value(), "hello");
}

TEST_F(EnvCfgTypeTest, NoexceptReturnsNulloptOnTypeMismatch) 
{
    EnvMap map = {{"TEST_TYPE", 42}};
    env.InitEnv(map);

    auto env_value_doub = env.GetN<double>("TEST_TYPE");
    EXPECT_FALSE(env_value_doub.has_value());

    auto env_value_str  = env.GetN<std::string>("TEST_TYPE");
    EXPECT_FALSE(env_value_str.has_value());
}

TEST_F(EnvCfgTypeTest, NoexceptReturnsNulloptOnMissingKey) 
{
    auto env_value_int = env.GetN<int>("MISSING_KEY");
    EXPECT_FALSE(env_value_int.has_value());
}

TEST_F(EnvCfgTypeTest, NoexceptHandlesNulloptValue) 
{
    EnvMap map = {{"TEST_NULL", EnvCfgTypes::int_}};
    env.InitEnv(map);

    auto env_value_int = env.GetN<int>("TEST_NULL");
    EXPECT_FALSE(env_value_int.has_value());
}

TEST_F(EnvCfgTypeTest, NoexceptBoundaryValues) 
{
    long long max_ll = std::numeric_limits<long long>::max();
    EnvMap map = {{"TEST_LLONG", max_ll}};
    env.InitEnv(map);

    auto env_value_ll = env.GetN<long long>("TEST_LLONG");
    EXPECT_EQ(env_value_ll.value(), max_ll);
}

TEST_F(EnvCfgTypeTest, NoexceptBoolConversion) 
{
    EnvMap map = {{"TEST_BOOL", true}};
    env.InitEnv(map);

    auto env_value_bool = env.GetN<bool>("TEST_BOOL");
    EXPECT_TRUE(env_value_bool.value());
}

class ExceptionListener : public testing::EmptyTestEventListener {
    public:
        void OnTestPartResult(const testing::TestPartResult& result) override 
        {
            if (result.fatally_failed()) {
                std::cout << "[Unhandled Exception] "<< result.message() << std::endl;
            }
        }
    };

int main(int argc, char **argv) 
{
    testing::InitGoogleTest(&argc, argv);
    testing::TestEventListeners& listeners = 
        testing::UnitTest::GetInstance()->listeners();
    listeners.Append(new ExceptionListener);
    
    return RUN_ALL_TESTS();
}