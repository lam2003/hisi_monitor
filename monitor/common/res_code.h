#ifndef RES_CODE_H_
#define RES_CODE_H_

#include <system_error>

enum err_code
{
    KSuccess = 0x0,
    KDupInitialize = 0x1,
    KArgsError = 0x2,
    KMPPError = 0x3,
};

class nvr_error_category : public std::error_category
{
    const char *name() const noexcept override
    {
        return "video_capture";
    }
    std::string message(int i) const override
    {
        switch (static_cast<err_code>(i))
        {
        case err_code::KSuccess:
            return "成功";
        case err_code::KDupInitialize:
            return "重复初始化";
        case err_code::KArgsError:
            return "参数错误";
        case err_code::KMPPError:
            return "海思mpp错误";
        default:
            return "不明";
        }
    }
};

namespace std
{
template <>
struct is_error_code_enum<err_code> : std::true_type
{
};
} // namespace std

template <class T>
struct is_error_code_enum : std::false_type
{
};

inline std::error_code make_error_code(err_code code) noexcept
{
    return {static_cast<int>(code), nvr_error_category()};
}

#endif