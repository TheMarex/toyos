// writes a message directly in the VGA framebuffer as red text
#define WRITE_ERROR(MSG, SCOPE) \
    {\
        uint16_t* vga_buffer = (uint16_t*) 0xC00B8000;\
        vga_buffer += 80*SCOPE;\
        const char* msg = MSG;\
        while(*msg != '\0') {\
            *(vga_buffer++) = ((uint16_t) *(msg++)) | (4u << 8u); \
        }\
    }

#define TEST(FUNC) \
    {\
        int ret = FUNC(); \
        if (ret < 0) { \
            WRITE_ERROR("Test " #FUNC " failed:", 0); \
            return ret; \
        }\
    }

#define CHECK_EQ(LHS, RHS) \
    if ((LHS) != (RHS)) { \
        WRITE_ERROR("Test " #LHS " == " #RHS " failed", 1); \
        return -1; \
    }
