#ifndef RES_CODE_H_
#define RES_CODE_H_

#include <system_error>

enum err_code
{
    KSuccess = 0x0,
    KDupInitialize = 0x1,
    KMPPError = 0x3,
    KVIChnError = 0x4,
    KUnInitialize = 0x5,
    KThirdPartyError = 0x6,
};

class NVRErrorCategory : public std::error_category
{
    const char *name() const noexcept override
    {
        return "nvr";
    }
    std::string message(int i) const override
    {
        switch (static_cast<err_code>(i))
        {
        case err_code::KSuccess:
            return "success";
        case err_code::KDupInitialize:
            return "duplicate initialize";
        case err_code::KMPPError:
            return "hisi mpp error";
        case err_code::KUnInitialize:
            return "uninitialize";
        case err_code::KThirdPartyError:
            return "thirdparty library error";
        default:
            return "unknow";
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

static NVRErrorCategory nvr_error_category;

inline std::error_code
make_error_code(err_code code) noexcept
{
    return {static_cast<int>(code), nvr_error_category};
}

#endif