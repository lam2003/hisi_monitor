#ifndef RES_CODE_H_
#define RES_CODE_H_

#include <system_error>

enum err_code
{
    KSuccess = 0x0,
    KDupInitialize = 0x1,
    KArgsError = 0x2,
    KMPPError = 0x3,
    KMIPIError = 0x4,
    KCheckError = 0x5,
    KISPError = 0x6,
    KVIError = 0x7,
    KVIChnError = 0x8,
    KUnInitialize = 0x9
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
        case err_code::KArgsError:
            return "argument wrong";
        case err_code::KMPPError:
            return "hisi mpp error";
        case err_code::KMIPIError:
            return "hisi mipi error";
        case err_code::KCheckError:
            return "check error";
        case err_code::KISPError:
            return "hisi isp error";
        case err_code::KVIError:
            return "hisi vi error";
        case err_code::KVIChnError:
            return "hisi vi chn error";
        case err_code::KUnInitialize:
            return "uninitialize";
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