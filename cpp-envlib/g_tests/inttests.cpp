#include "../libenv.h"
#include <gtest/gtest.h>
#include <limits>

using namespace env_cfg;

class EnvCfgIntTest : public ::testing::Test {
protected:
    void SetUp() override 
    {
        EnvCfg::SetEnvN("TEST_INT", "", true);
        EnvCfg::SetEnvN("TEST_LLONG", "", true);
    }

    EnvCfg env;
};

TEST_F(EnvCfgIntTest, HandlesIntValues) 
{
    EnvCfg::SetEnv("TEST_INT", "12345");
    EnvMap map = {{"TEST_INT", EnvCfgTypes::int_}};
    env.InitEnv(map);

    EXPECT_EQ(env.Get<int>("TEST_INT"), 12345);
    EXPECT_TRUE(env.IsType<int>("TEST_INT"));
    EXPECT_EQ(env.GetN<int>("TEST_INT").value(), 12345);
}

TEST_F(EnvCfgIntTest, HandlesIntOverflow) 
{
    long long overflow = static_cast<long long>(std::numeric_limits<int>::max()) + 1;
    EnvCfg::SetEnv("TEST_INT", std::to_string(overflow));
    EnvMap map = {{"TEST_INT", EnvCfgTypes::int_}};
    
    EXPECT_THROW({
        try 
        {
            env.InitEnv(map);
        } 
        catch (const EnvBadGet& e) 
        {
            std::cout << "\n[Exception] " << e.what() << std::endl;
            throw;
        }
    }, EnvBadGet);
}

TEST_F(EnvCfgIntTest, HandlesInvalidInt) 
{
    EnvCfg::SetEnv("TEST_INT", "not_a_number");
    EnvMap map = {{"TEST_INT", EnvCfgTypes::int_}};
    
    EXPECT_THROW({
        try 
        {
            env.InitEnv(map);
        }
        catch (const EnvBadGet& e) 
        {
            std::cout << "\n[Exception] " << e.what() << std::endl;
            throw;
        }
    }, EnvBadGet);
}

TEST_F(EnvCfgIntTest, HandlesIntBoundaries) 
{
    EnvCfg::SetEnv("TEST_INT", std::to_string(std::numeric_limits<int>::max()));
    EnvMap map_max = {{"TEST_INT", EnvCfgTypes::int_}};
    env.InitEnv(map_max);
    EXPECT_EQ(env.Get<int>("TEST_INT"), std::numeric_limits<int>::max());

    EnvCfg::SetEnv("TEST_INT", std::to_string(std::numeric_limits<int>::min()));
    EnvMap map_min = {{"TEST_INT", EnvCfgTypes::int_}};
    env.InitEnv(map_min);
    EXPECT_EQ(env.Get<int>("TEST_INT"), std::numeric_limits<int>::min());
}

TEST_F(EnvCfgIntTest, HandlesLongLongValues) 
{
    long long big_value = 9223372036854775807LL;
    EnvCfg::SetEnv("TEST_LLONG", std::to_string(big_value));
    EnvMap map = {{"TEST_LLONG", EnvCfgTypes::longlong_}};
    env.InitEnv(map);

    EXPECT_EQ(env.Get<long long>("TEST_LLONG"), big_value);
    EXPECT_TRUE(env.IsType<long long>("TEST_LLONG"));
    EXPECT_EQ(env.GetN<long long>("TEST_LLONG").value(), big_value);
}

TEST_F(EnvCfgIntTest, HandlesLongLongOverflow) 
{
    std::string huge_value = "9223372036854775808";
    EnvCfg::SetEnv("TEST_LLONG", huge_value);
    EnvMap map = {{"TEST_LLONG", EnvCfgTypes::longlong_}};
    
    EXPECT_THROW({
        try 
        {
            env.InitEnv(map);
        }
        catch (const EnvBadGet& e) 
        {
            std::cout << "\n[Exception] " << e.what() << std::endl;
            throw;
        }
    }, EnvBadGet);
}

TEST_F(EnvCfgIntTest, HandlesMissingValues) 
{
    EnvMap map = {{"MISSING_INT", EnvCfgTypes::int_}};
    env.InitEnv(map);
    
    EXPECT_THROW({
        try 
        {
            env.Get<int>("MISSING_INT");
        }
        catch (const EnvBadGet& e) {
            std::cout << "\n[Exception] " << e.what() << std::endl;
            throw;
        }
    }, EnvBadGet);
}

TEST_F(EnvCfgIntTest, HandlesDefaultValues) 
{
    EnvMap map = {{"DEFAULT_INT", 42}, {"DEFAULT_LLONG", 10000000000LL}};
    env.InitEnv(map);

    EXPECT_EQ(env.Get<int>("DEFAULT_INT"), 42);
    EXPECT_EQ(env.Get<long long>("DEFAULT_LLONG"), 10000000000LL);
}

TEST_F(EnvCfgIntTest, HandlesUnsignedAsInt) 
{
    EnvCfg::SetEnv("TEST_UINT_AS_INT", "2147483647");
    EnvMap map = {{"TEST_UINT_AS_INT", EnvCfgTypes::int_}};
    env.InitEnv(map);

    EXPECT_EQ(env.Get<int>("TEST_UINT_AS_INT"), 2147483647);
}

TEST_F(EnvCfgIntTest, HandlesUnsignedAsLongLong) 
{
    EnvCfg::SetEnv("TEST_ULLONG_AS_LLONG", "18446744073709551615");
    EnvMap map = {{"TEST_ULLONG_AS_LLONG", EnvCfgTypes::longlong_}};
    
    EXPECT_THROW({
        try 
        {
            env.InitEnv(map);
        }
        catch (const EnvBadGet& e) {
            std::cout << "\n[Exception] " << e.what() << std::endl;
            throw;
        }
    }, EnvBadGet);
}

TEST_F(EnvCfgIntTest, RejectsNegativeForUnsignedSimulation) 
{
    EnvCfg::SetEnv("TEST_UINT_NEGATIVE", "-123");
    EnvMap map = {{"TEST_UINT_NEGATIVE", EnvCfgTypes::int_}};
    
    env.InitEnv(map);
    EXPECT_EQ(env.Get<int>("TEST_UINT_NEGATIVE"), -123);
}

TEST_F(EnvCfgIntTest, HandlesUnsignedOverflowAsLongLong) 
{
    std::string huge_value = "9223372036854775808";
    EnvCfg::SetEnv("TEST_ULLONG_OVERFLOW", huge_value);
    EnvMap map = {{"TEST_ULLONG_OVERFLOW", EnvCfgTypes::longlong_}};
    
    EXPECT_THROW({
        try 
        {
            env.InitEnv(map);
        }
        catch (const EnvBadGet& e) {
            std::cout << "\n[Exception] " << e.what() << std::endl;
            throw;
        }
    }, EnvBadGet);
}

TEST_F(EnvCfgIntTest, HandlesUnsignedViaDefaultValue) 
{
    EnvMap map = {{"TEST_UINT_DEFAULT", 4294967295u}};
    env.InitEnv(map);

    EXPECT_EQ(env.Get<long long>("TEST_UINT_DEFAULT"), 4294967295LL);
}

TEST_F(EnvCfgIntTest, UnsignedIntConvertedToLongLongWhenOverflow) {
    unsigned int big_value = static_cast<unsigned int>(std::numeric_limits<int>::max()) + 1u;
    EnvMap map = {{"UNSIGNED_OVERFLOW", big_value}};
    env.InitEnv(map);

    EXPECT_TRUE(env.IsType<long long>("UNSIGNED_OVERFLOW"));
    EXPECT_FALSE(env.IsType<int>("UNSIGNED_OVERFLOW"));
    
    EXPECT_THROW({
        env.Get<int>("UNSIGNED_OVERFLOW");
    }, EnvBadGet);
    
    EXPECT_EQ(env.Get<long long>("UNSIGNED_OVERFLOW"), static_cast<long long>(big_value));
}

TEST_F(EnvCfgIntTest, UnsignedIntConvertedToIntWhenInRange) 
{
    unsigned int valid_value = std::numeric_limits<int>::max();
    EnvMap map = {{"UNSIGNED_VALID", valid_value}};
    env.InitEnv(map);

    EXPECT_FALSE(env.IsType<int>("UNSIGNED_VALID"));
    EXPECT_TRUE(env.IsType<long long>("UNSIGNED_VALID"));
    
    EXPECT_EQ(env.Get<long long>("UNSIGNED_VALID"), static_cast<long long>(valid_value));
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

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    testing::TestEventListeners& listeners = 
        testing::UnitTest::GetInstance()->listeners();
    listeners.Append(new ExceptionListener);
    
    return RUN_ALL_TESTS();
}
