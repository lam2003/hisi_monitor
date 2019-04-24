namespace nvr
{

template <typename VideoFrameT>
class VideoSinkInterface
{
  public:
    virtual ~VideoSinkInterface() = default;
    
    virtual void OnFrame(const VideoFrameT &) = 0;
};

} // namespace nvr
