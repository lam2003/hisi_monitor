namespace nvr
{
template <unsigned BlockSize>
struct default_block_allocator_malloc_free
{
    enum
    {
        requested_size = BlockSize
    };

    static uint8_t *ordered_malloc()
    {
        return (uint8_t *)malloc(requested_size);
    }

    static void ordered_free(uint8_t *block)
    {
        free(block);
    }
};

typedef default_block_allocator_malloc_free<BUFFER_SIZE> default_allocator;

template <typename BlockAllocator = default_allocator> //max 1MB
class Buffer
{
public:
    typedef BlockAllocator allocator;
    Buffer()
    {
        data_ = allocator::ordered_malloc();
        start_pos_ = 0;
        end_pos_ = 0;
    }

    inline bool append(uint8_t *data, uint32_t len)
    {
        if (FreeSpace() < len)
            return false;

        if (allocator::requested_size - end_pos_ < len)
        {
            uint32_t size = Size();
            memmove(data_, data_ + start_pos_, size);
            start_pos_ = 0;
            end_pos_ = size;
        }
        memcpy(data_ + end_pos_, data, len);
        end_pos_ += len;
        return true;
    }

    inline bool get(uint8_t *buf, uint32_t size)
    {
        if (Size() < size)
            return false;

        memcpy(buf, data_ + start_pos_, size);
        start_pos_ += size;
        return true;
    }
    inline uint32_t Size() const
    {
        return end_pos_ - start_pos_;
    }

    inline uint32_t FreeSpace() const
    {
        return allocator::requested_size - Size();
    }

    virtual ~Buffer()
    {
        allocator::ordered_free(data_);
        start_pos_ = 0;
        end_pos_ = 0;
    }

private:
    uint8_t *data_;
    uint32_t start_pos_;
    uint32_t end_pos_;
};

}; // namespace nvr