#define TEST(FUNC) \
    {\
        int ret = FUNC(); \
        if (ret < 0) { \
            return ret; \
        }\
    }

#define CHECK_EQ(LHS, RHS) \
    if ((LHS) != (RHS)) { \
        return -1; \
    }
