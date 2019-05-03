// #include "osd.h"

// #include "common/config.h"
// #include "common/res_code.h"

// namespace nvr
// {
// OSD *OSD::Instance()
// {
//     static OSD *instance = new OSD;
//     return instance;
// }

// OSD::~OSD()
// {
// }

// OSD::OSD() : fmt_(nullptr),
//              font_(nullptr),
//              init_(false)
// {
// }

// int32_t OSD::Initialize()
// {
//     if (init_)
//         return static_cast<int>(KDupInitialize);

//     if (TTF_Init() < 0)
//     {
//         log_e("TTF_Init failed");
//         return static_cast<int>(KThirdPartyError);
//     }

//     font_ = TTF_OpenFont(Config::Instance()->system.font_path.c_str(), Config::Instance()->system.font_size);
//     if (NULL == font_)
//     {
//         log_e("TTF_OpenFont failed");
//         return static_cast<int>(KThirdPartyError);
//     }

//     SDL_Color color = {0x0, 0x0, 0x0, 0x00};
//     fmt_ = (SDL_PixelFormat *)malloc(sizeof(SDL_PixelFormat));
//     memset(fmt_, 0, sizeof(SDL_PixelFormat));
//     fmt_->BitsPerPixel = 16;
//     fmt_->BytesPerPixel = 1;
//     fmt_->Rmask = 0x7C00;
//     fmt_->Gmask = 0x03E0;
//     fmt_->Bmask = 0x001f;
//     fmt_->Amask = 0x8000;
//     // fmt_->format = SDL_PIXELFORMAT_ARGB1555;

//     std::string temp = "test";
//     SDL_Surface *sdl_textsurface = TTF_RenderUTF8_Solid(font_, temp.c_str(), color);
//     // SDL_Surface *sdl_tmpsurface = SDL_CreateRGBSurface(SDL_SWSURFACE, sdl_textsurface->w, sdl_textsurface->h, 16, 0x7C00, 0x03E0, 0x001f, 0x8000); //ARGB1555
//     // SDL_Surface *sdl_tmpsurface = SDL_CreateRGBSurface(SDL_SWSURFACE, sdl_textsurface->w, sdl_textsurface->h, 16,0x000000ff, 0x0000ff00,  0x00ff0000, 0xff000000);
//     // SDL_Rect bounds;
//     // bounds.x = 0;
//     // bounds.y = 0;
//     // bounds.w = sdl_textsurface->w;
//     // bounds.h = sdl_textsurface->h;
//     // SDL_LowerBlit(sdl_textsurface, &bounds, sdl_tmpsurface, &bounds);
//     SDL_SaveBMP(sdl_textsurface,"1.bmp");
//     SDL_FreeSurface(sdl_textsurface);
//     // SDL_FreeSurface(sdl_tmpsurface);


//     init_ = true;

//     return static_cast<int>(KSuccess);
// }
// } // namespace nvr