#include <iostream>
#include <stdexcept>
#include <memory>
#include <string>
#include <thread>
#include <vector>
#include <cstring>
#include <stdint.h>
#include <ctime>

#include <SDL.h>
#undef main // SDL2...

#include "config.h"
#include "Emulator.h"
#include "Amiga.h"
#include "RomFile.h"
#include "ExtendedRomFile.h"
#include "EXEFile.h"
#include "DMSFile.h"
#include "EADFFile.h"
#include "IOUtils.h"
#include "Snapshot.h"
#include "VAmiga.h"

#include "microknight.h"

using namespace vamiga;

// Visible area
constexpr int xstart = (HBLANK_MAX + 1) * 4;
constexpr int xend   = HPIXELS;
constexpr int ystart = 0x1B;
constexpr int yend   = 0x137;
static_assert(xend - xstart <= HPIXELS);
static_assert(yend - ystart <= VPIXELS);

[[noreturn]] void throw_sdl_error(const std::string& what)
{
    throw std::runtime_error { what + " failed: " + SDL_GetError() };
}

class sdl_init {
public:
    explicit sdl_init(uint32_t flags)
        : flags_ { flags }
    {
        if (SDL_InitSubSystem(flags_))
            throw_sdl_error("SDL_InitSubSystem " + std::to_string(flags_));
    }

    sdl_init(const sdl_init&) = delete;
    sdl_init& operator=(const sdl_init&) = delete;

    ~sdl_init()
    {
        SDL_QuitSubSystem(flags_);
    }

private:
    const uint32_t flags_;
};

#define MAKE_SDL_PTR(type, destroyer) \
struct type##_destroyer { \
    void operator()(type* p) const { \
        if (p) destroyer(p); \
    } \
}; \
using type##_ptr = std::unique_ptr<type, type##_destroyer>

MAKE_SDL_PTR(SDL_Window, SDL_DestroyWindow);
MAKE_SDL_PTR(SDL_Renderer, SDL_DestroyRenderer);
MAKE_SDL_PTR(SDL_Texture, SDL_DestroyTexture);

struct sdl_freer {
    void operator()(void* ptr) const {
        if (ptr) SDL_free(ptr);
    }
};

uint8_t convert_key(SDL_Keycode key)
{
    switch (key) {
    case SDLK_RETURN: return 0x44;
    case SDLK_ESCAPE: return 0x45;
    case SDLK_BACKSPACE: return 0x41;
    case SDLK_TAB: return 0x42;
    case SDLK_SPACE: return 0x40;
    case SDLK_QUOTE: return 0x2A;
    case SDLK_COMMA: return 0x38;
    case SDLK_MINUS: return 0x0B;
    case SDLK_PERIOD: return 0x39;
    case SDLK_SLASH: return 0x3A;
    case SDLK_0: return 0x0A;
    case SDLK_1: return 0x01;
    case SDLK_2: return 0x02;
    case SDLK_3: return 0x03;
    case SDLK_4: return 0x04;
    case SDLK_5: return 0x05;
    case SDLK_6: return 0x06;
    case SDLK_7: return 0x07;
    case SDLK_8: return 0x08;
    case SDLK_9: return 0x09;
    case SDLK_SEMICOLON: return 0x29;
    case SDLK_EQUALS: return 0x0C;
    case SDLK_LEFTBRACKET: return 0x1A;
    case SDLK_BACKSLASH: return 0x0D;
    case SDLK_RIGHTBRACKET: return 0x1B;
    case SDLK_BACKQUOTE: return 0x00;
    case SDLK_a: return 0x20;
    case SDLK_b: return 0x35;
    case SDLK_c: return 0x33;
    case SDLK_d: return 0x22;
    case SDLK_e: return 0x12;
    case SDLK_f: return 0x23;
    case SDLK_g: return 0x24;
    case SDLK_h: return 0x25;
    case SDLK_i: return 0x17;
    case SDLK_j: return 0x26;
    case SDLK_k: return 0x27;
    case SDLK_l: return 0x28;
    case SDLK_m: return 0x37;
    case SDLK_n: return 0x36;
    case SDLK_o: return 0x18;
    case SDLK_p: return 0x19;
    case SDLK_q: return 0x10;
    case SDLK_r: return 0x13;
    case SDLK_s: return 0x21;
    case SDLK_t: return 0x14;
    case SDLK_u: return 0x16;
    case SDLK_v: return 0x34;
    case SDLK_w: return 0x11;
    case SDLK_x: return 0x32;
    case SDLK_y: return 0x15;
    case SDLK_z: return 0x31;
    //case SDLK_CAPSLOCK: return 0x62;
    case SDLK_F1: return 0x50;
    case SDLK_F2: return 0x51;
    case SDLK_F3: return 0x52;
    case SDLK_F4: return 0x53;
    case SDLK_F5: return 0x54;
    case SDLK_F6: return 0x55;
    case SDLK_F7: return 0x56;
    case SDLK_F8: return 0x57;
    case SDLK_F9: return 0x58;
    case SDLK_F10: return 0x59;
    case SDLK_INSERT: return 0x66; // Left amiga
    case SDLK_HOME: return 0x67; // Right amiga
    //case SDLK_PAGEUP: return 0xFF;
    case SDLK_DELETE: return 0x46;
    //case SDLK_END: return 0xFF;
    case SDLK_PAGEDOWN: return 0x5F; // Help
    case SDLK_RIGHT: return 0x4E;
    case SDLK_LEFT: return 0x4F;
    case SDLK_DOWN: return 0x4D;
    case SDLK_UP: return 0x4C;
    case SDLK_LCTRL: return 0x63;
    case SDLK_LSHIFT: return 0x60;
    case SDLK_LALT: return 0x64;
    //case SDLK_LGUI: return 0xFF;
    //case SDLK_RCTRL: return 0xFF;
    case SDLK_RSHIFT: return 0x61;
    case SDLK_RALT: return 0x65;
    //case SDLK_RGUI: return 0xFF;
    }
    return 0xFF;
}

char with_shift(char c)
{
    switch (c) {
    case '1': return '!';
    case '2': return '@';
    case '3': return '#';
    case '4': return '$';
    case '5': return '%';
    case '6': return '^';
    case '7': return '&';
    case '8': return '*';
    case '9': return '(';
    case '0': return ')';
    case '-': return '_';
    case '=': return '+';
    case '\\': return '|';
    case ',': return '<';
    case '.': return '>';
    case '/': return '?';
    case '[': return '{';
    case ']': return '}';
    case ';': return ':';
    case '\'': return '"';
    }
    return c;
}

class driver {
public:
    static constexpr int audio_sample_rate = 48000;
    static constexpr int screen_width = xend - xstart;
    static constexpr int screen_height = 2 * (yend - ystart);

    explicit driver()
        : current_frame_(HPIXELS * VPIXELS)
        , last_frame_(HPIXELS * VPIXELS)
    {
        window_.reset(SDL_CreateWindow("vAmiga", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screen_width, screen_height, SDL_WINDOW_SHOWN));
        if (!window_)
            throw_sdl_error("SDL_CreateWindow");

        renderer_.reset(SDL_CreateRenderer(window_.get(), -1, SDL_RENDERER_ACCELERATED/* | SDL_RENDERER_PRESENTVSYNC*/));
        if (!renderer_)
            throw_sdl_error("SDL_CreateRenderer");

        texture_.reset(SDL_CreateTexture(renderer_.get(), SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, screen_width, screen_height));
        if (!texture_)
            throw_sdl_error("SDL_CreateTexture");

        overlay_.reset(SDL_CreateTexture(renderer_.get(), SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, screen_width, screen_height));
        if (!texture_)
            throw_sdl_error("SDL_CreateTexture");

        if (SDL_SetTextureBlendMode(overlay_.get(), SDL_BLENDMODE_BLEND))
            throw_sdl_error("SDL_SetTextureBlendMode");

        SDL_AudioSpec want {};
        SDL_AudioSpec have;

        want.freq = audio_sample_rate;
        want.format = AUDIO_F32SYS;
        want.channels = 2;
        want.samples = 4096;
        want.callback = [](void* userdata, Uint8* stream, int len) { reinterpret_cast<driver*>(userdata)->audio_callback(stream, len); };
        want.userdata = this;

        dev_ = SDL_OpenAudioDevice(nullptr, 0, &want, &have, 0);
        if (!dev_)
            throw_sdl_error("SDL_OpenAudioDevice");

        if (want.freq != have.freq || want.format != have.format || want.channels != have.channels) {
            SDL_CloseAudioDevice(dev_);
            std::cerr << "Freq: " << have.freq << " Format: " << static_cast<int>(have.format) << " channels: " << static_cast<int>(have.channels) << " samples: " << have.samples << "\n";
            throw std::runtime_error { "Audio format not supported" };
        }

        emulator_.launch(this, [](const void* ptr, Message msg) {
            reinterpret_cast<driver*>(const_cast<void*>(ptr))->msg_queue_callback(msg);
        });
    }

    ~driver() {
        emulator_.powerOff();
        SDL_CloseAudioDevice(dev_);
    }

    int run(int argc, char* argv[])
    {
        emulator_.set(ConfigScheme::A500_OCS_1MB);
        emulator_.set(Option::HOST_SAMPLE_RATE, audio_sample_rate);
        for (int n = 0; n < 4; ++n)
            emulator_.set(Option::HDC_CONNECT, false, n);

        bool auto_power_on = true;
        int drive = 0, hd = 0;
        std::string ext_rom;
        for (int i = 1; i < argc; ++i) {
            if (!strcmp(argv[i], "-bigbox")) {
                emulator_.set(ConfigScheme::A500_ECS_1MB);
                emulator_.set(Option::MEM_CHIP_RAM, 2048);
                emulator_.set(Option::MEM_FAST_RAM, 8192);
                emulator_.set(Option::MEM_SLOW_RAM, 0);
                emulator_.set(Option::CPU_OVERCLOCKING, 14);
                emulator_.set(Option::CPU_REVISION, (i64)CPURevision::CPU_68EC020);
                continue;
            } else if (!strcmp(argv[i], "-a600")) {
                emulator_.set(ConfigScheme::A500_ECS_1MB);
                emulator_.set(Option::MEM_CHIP_RAM, 1024);
                emulator_.set(Option::MEM_SLOW_RAM, 0);
                continue;
            }

            std::filesystem::path p { argv[i] };

            const auto suffix = util::uppercased(p.extension().string());
            if (suffix == ".TXT") {
                std::cout << "Executing script: " << argv[i] << "\n";
                std::ifstream in{argv[i]};
                if (!in || !in.is_open())
                    throw std::runtime_error{"Error opening: " + std::string{argv[i]}};
                emulator_.retroShell.execScript(in);
                auto_power_on = false;
                continue;
            } else if (suffix == ".SNP") {
                std::cout << "Loading snapshot: " << argv[i] << "\n";
                Snapshot snp{argv[i]};
                emulator_.powerOn();
                amiga_.loadSnapshot(snp);
                emulator_.run();
                auto_power_on = false;
                break;
            } else if (suffix == ".ROM") {
                emulator_.mem.loadRom(argv[i]);
            } else if (suffix == ".BIN") {
#if 0  // XXX
                if (ExtendedRomFile::isExtendedRomFile(p))
                    ext_rom = argv[i]; // load outside loop as loadRom deletes any extended rom (!)
                else if (RomFile::isRomFile(p))
                    amiga_.mem.loadRom(p);
                else
                    throw std::runtime_error { "Unknown binary file: " + std::string { argv[i] } };
#else
                emulator_.mem.loadRom(p);
#endif
            } else if (suffix == ".HDF" && hd < 4) {
                emulator_.set(Option::HDC_CONNECT, true, hd);
                HardDriveAPI *dh[] = { &emulator_.hd0, &emulator_.hd1, &emulator_.hd2, &emulator_.hd3 };
                dh[hd]->attach(p);
                WT_DEBUG = 1;
                Emulator::defaults.set("HD" + std::to_string(hd) + "_PATH", p);
                emulator_.set(Option::HDR_WRITE_THROUGH, true, hd);
                ++hd;
            } else if (drive < 4) {
                std::cout << "Inserting in DF" << drive << ": " << argv[i] << "\n";
                if (drive)
                    emulator_.set(Option::DRIVE_CONNECT, true, { drive });
                FloppyDriveAPI *df[] = { &emulator_.df0, &emulator_.df1, &emulator_.df2, &emulator_.df3 };
                const bool wp = false; // TODO write-protect true as default
                MediaFile *floppy = MediaFile::make(p);
                df[drive]->insertMedia(*floppy, wp); // TODO df1 ...
                delete floppy;
                ++drive;
            }
        }

        if (!emulator_.mem.getInfo().hasRom) {
            RomFile rom { "kick13.rom" };
            emulator_.mem.loadRom(rom);
        } else if (!ext_rom.empty()) {
            emulator_.mem.loadExt(ext_rom);
        }

        if (auto_power_on) {
            emulator_.powerOn();
            emulator_.run();
        }

        SDL_PauseAudioDevice(dev_, false); // unpause

        for (uint32_t frame = 0;; ++frame) {
            SDL_Event e;
            while (SDL_PollEvent(&e)) {
                switch (e.type) {
                case SDL_QUIT:
                    return 0;
                case SDL_KEYDOWN:
                    if (overlay_active_) {
                        handle_overlay_key(e.key.keysym);
                        break;
                    } else if (e.key.keysym.sym == SDLK_F11) {
                        if (e.key.keysym.mod & KMOD_SHIFT) {
#ifdef SCREEN_RECORDER
                            if (!amiga_.denise.screenRecorder.isRecording()) {
                                amiga_.denise.screenRecorder.startRecording(0, 0, HPIXELS, VPIXELS, 100'000, 1, 2);
                                std::cout << "Recording started\n";
                            } else {
                                amiga_.denise.screenRecorder.stopRecording();
                            }
#endif
                        } else {
                            std::unique_ptr<MediaFile> snapshot { amiga_.takeSnapshot() };
                            assert(snapshot);
                            if (snapshot) {
                                char filename[256];
                                auto t = std::time(nullptr);
                                tm* local = std::localtime(&t);
                                snprintf(filename, sizeof(filename), "snapshot_%04d%02d%02d%02d%02d%02d.snp", 1900 + local->tm_year, 1 + local->tm_mon, local->tm_mday, local->tm_hour, local->tm_min, local->tm_sec);
                                std::cout << "Saving snapshot to " << filename << "\n";
                                snapshot->writeToFile(filename);
                            }
                        }
                        break;
                    } else if (e.key.keysym.sym == SDLK_F12) {
                        overlay_active_ = true;
                        overlay_dirty_ = true;
                        capture_mouse(false);
                        break;
                    } else if (e.key.keysym.sym == SDLK_F4 && (e.key.keysym.mod & KMOD_ALT)) {
                        return 0;
                    } else if (e.type == SDL_KEYUP) {
                        std::cout << "Unhandled key: " << e.key.keysym.sym << " " << SDL_GetKeyName(e.key.keysym.sym) << "\n";
                    }
                    [[fallthrough]];
                case SDL_KEYUP:
                    if (overlay_active_)
                        break;
                    if (e.key.repeat && 0) /* TODO problem in warp mode */
                        break;
                    if (handle_joystick_key(e.key.keysym.sym, e.type == SDL_KEYUP))
                        break;
                    if (const auto key = convert_key(e.key.keysym.sym); key != 0xFF) {
                        if (e.type == SDL_KEYUP)
                            emulator_.keyboard.release(key);
                        else
                            emulator_.keyboard.press(key);
                    }
                    break;
                case SDL_MOUSEBUTTONDOWN:
                case SDL_MOUSEBUTTONUP:
                    if (e.button.button == SDL_BUTTON_LEFT || e.button.button == SDL_BUTTON_RIGHT) {
                        if (!mouse_captured_) {
                            if (overlay_active_)
                                handle_overlay_mouse(e.button);
                            else
                                capture_mouse(true);
                        } else {
			  const bool pressed = e.type == SDL_MOUSEBUTTONDOWN; // TODO middle ?
			  bool left = (e.button.button == SDL_BUTTON_LEFT);
			  auto& mouse = emulator_.controlPort1.mouse;
			  if (pressed)
			    mouse.trigger(left ? GamePadAction::PRESS_LEFT : GamePadAction::PRESS_RIGHT);
			  else
			    mouse.trigger(left ? GamePadAction::RELEASE_LEFT : GamePadAction::RELEASE_RIGHT);
                        }
                    }
                    break;
                case SDL_MOUSEMOTION:
                    if (mouse_captured_ && (e.motion.xrel || e.motion.yrel)) {
#ifdef WSL2_MOUSE_HACK
                        // Probably only for WSL2: xrel/yrel are actually *not* relative (and x/y don't update)??
                        amiga_.controlPort1.mouse.setDxDy(e.motion.xrel - last_mouse_x_, e.motion.yrel - last_mouse_y_);
                        last_mouse_x_ = e.motion.xrel;
                        last_mouse_y_ = e.motion.yrel;
#else
                        emulator_.controlPort1.mouse.setDxDy(e.motion.xrel, e.motion.yrel);
                        // Make sure mouse doesn't end up on the window border
                        SDL_WarpMouseInWindow(window_.get(), screen_width / 2, screen_height / 2);
#endif
                    }
                    break;
                case SDL_WINDOWEVENT:
                    if (e.window.event == SDL_WINDOWEVENT_FOCUS_LOST || e.window.event == SDL_WINDOWEVENT_LEAVE) {
                        capture_mouse(false);
                    }
                }
            }

            bool update = overlay_active_ && overlay_dirty_;
            if (power_is_on_) {
                // TODO: Implement new long frame logic

                VideoPortAPI& vp = emulator_.videoPort;
                vp.lockTexture();
                isize nr;
                bool lof, prevlof;
                const u32 *ptr = vp.getTexture(&nr, &lof, &prevlof);
                if (ptr != last_buffer_pointer_) { // HACK: Don't update if not a new frame
                    std::memcpy(&current_frame_[0], ptr, HPIXELS * VPIXELS * sizeof(uint32_t));

                    void* pixels;
                    int pitch;
                    if (SDL_LockTexture(texture_.get(), nullptr, &pixels, &pitch))
                        throw_sdl_error("SDL_LockTexture");
                    uint8_t* dest1 = reinterpret_cast<uint8_t*>(pixels) + !lof * pitch;
                    uint8_t* dest2 = reinterpret_cast<uint8_t*>(pixels) + lof * pitch;
                    const uint32_t* src1 = &current_frame_[0];
                    const uint32_t* src2 = (lof == last_frame_type_) ? &current_frame_[0] : &last_frame_[0];

                    src1 += HPIXELS * ystart + HBLANK_MAX * 4;//xstart;
                    src2 += HPIXELS * ystart + HBLANK_MAX * 4; // xstart;
                    for (uint32_t y = 0; y < screen_height / 2; ++y) {
                        std::memcpy(dest1, src1, screen_width * sizeof(uint32_t));
                        std::memcpy(dest2, src2, screen_width * sizeof(uint32_t));
                        dest1 += 2 * pitch;
                        dest2 += 2 * pitch;
                        src1 += HPIXELS;
                        src2 += HPIXELS;
                    }
                    SDL_UnlockTexture(texture_.get());
                    //SDL_RenderClear(renderer_.get());

                    std::swap(current_frame_, last_frame_);
                    last_frame_type_ = lof;
                    last_buffer_pointer_ = ptr;
                    update = true;
                }
                vp.unlockTexture();
                emulator_.wakeUp();
            } else {
                void* pixels;
                int pitch;
                if (SDL_LockTexture(texture_.get(), nullptr, &pixels, &pitch))
                    throw_sdl_error("SDL_LockTexture");

                static uint32_t rand_state = 1;
                uint8_t* dest = reinterpret_cast<uint8_t*>(pixels);
                for (uint32_t y = 0; y < screen_height / 2; ++y) {
                    for (uint32_t x = 0; x < screen_width; ++x) {
                        rand_state ^= rand_state << 13;
                        rand_state ^= rand_state >> 17;
                        rand_state ^= rand_state << 5;
                        const uint32_t r = (uint8_t)rand_state;
                        const uint32_t c = r << 16 | r << 8 | r;
                        *(uint32_t*)(dest + x * sizeof(uint32_t)) = c;
                        *(uint32_t*)(dest + x * sizeof(uint32_t) + pitch) = c;
                    }

                    dest += pitch * 2;
                }
                SDL_UnlockTexture(texture_.get());

                update = true;
                last_buffer_pointer_ = nullptr;
            }

            if (abort_)
                return abort_ & 0xFF;

            if (overlay_active_)
                update_overlay();

            if (update) {
                SDL_RenderCopy(renderer_.get(), texture_.get(), nullptr, nullptr);
                if (overlay_active_)
                    SDL_RenderCopy(renderer_.get(), overlay_.get(), nullptr, nullptr);
                SDL_RenderPresent(renderer_.get());
            }
            
            SDL_Delay(5);
        }
    }

private:
    sdl_init sdl_init_ { SDL_INIT_VIDEO | SDL_INIT_AUDIO };
    SDL_Window_ptr window_;
    SDL_Renderer_ptr renderer_;
    SDL_Texture_ptr texture_;
    SDL_Texture_ptr overlay_;
    SDL_AudioDeviceID dev_;
    bool need_halt_ = false;
    bool mouse_captured_ = false;
#ifdef WSL2_MOUSE_HACK
    int last_mouse_x_ = 0;
    int last_mouse_y_ = 0;
#endif
    const u32* last_buffer_pointer_ = nullptr;
    bool last_frame_type_ = false;
    std::vector<uint32_t> current_frame_;
    std::vector<uint32_t> last_frame_;
    bool overlay_active_ = false;
    bool overlay_dirty_ = true;
    bool overlay_blink_ = false;
    bool power_is_on_ = false;
    int abort_ = 0;
    uint64_t last_overlay_blink_ = 0;
    std::string ser_buffer_;
    VAmiga emulator_;
    AmigaAPI& amiga_ = emulator_.amiga;

    void capture_mouse(bool enabled)
    {
        if (enabled == mouse_captured_)
            return;
        SDL_SetRelativeMouseMode(enabled ? SDL_TRUE : SDL_FALSE);
        SDL_SetWindowTitle(window_.get(), (std::string{"vAmiga"}+(enabled ? " - mouse captured" : "")).c_str());
        mouse_captured_ = enabled;
    }

    void msg_queue_callback(Message msg)
    {
        switch (msg.type) {
            case MsgType::RSH_UPDATE:
            case MsgType::RSH_DEBUGGER:
            case MsgType::DRIVE_SELECT:
            case MsgType::DRIVE_STEP:
            case MsgType::DRIVE_POLL:
            case MsgType::DISK_INSERT:
            case MsgType::DISK_EJECT:
            case MsgType::DRIVE_LED:
            case MsgType::DRIVE_MOTOR:
            case MsgType::SER_IN:
            case MsgType::HDR_READ:
            case MsgType::HDR_WRITE:
            case MsgType::HDR_IDLE:
            case MsgType::HDR_STEP:
            case MsgType::HDC_STATE:
            case MsgType::HDC_CONNECT:
            case MsgType::VIEWPORT:
            case MsgType::CONFIG:
            case MsgType::POWER_LED_ON:
            case MsgType::POWER_LED_OFF:
            case MsgType::POWER_LED_DIM:
            case MsgType::DRIVE_CONNECT:
            case MsgType::MEM_LAYOUT:
            case MsgType::OVERCLOCKING:
            case MsgType::VIDEO_FORMAT:
            case MsgType::DMA_DEBUG:
            case MsgType::MUTE:
            case MsgType::RUN:
            case MsgType::PAUSE:
            case MsgType::RESET:
                return;
            case MsgType::ABORT:
                abort_ = msg.value | 0x100;
                power_is_on_ = false;
                break;
            case MsgType::POWER:
                if (msg.value) {
                    power_is_on_ = true;
                } else {
                    power_is_on_ = false;
                    std::memset(&current_frame_[0], 0, sizeof(uint32_t)*current_frame_.size());
                    std::memset(&last_frame_[0], 0, sizeof(uint32_t)*last_frame_.size());
                }
                return;

//            case MsgType::CLOSE_CONSOLE:
//                overlay_active_ = false;
//                return;
//            case MsgType::UPDATE_CONSOLE:
//                overlay_dirty_ = true;
//                return;
            case MsgType::RECORDING_STOPPED:
#ifdef SCREEN_RECORDER
                amiga_.denise.screenRecorder.exportAs("test.mp4");
#endif
                std::cout << "Recording exported\n";
                break;
//            case MsgType::SER_OUT:
//                if ((data1 & 0xff) != '\n') {
//                    ser_buffer_.push_back(static_cast<char>(data1 & 0xff));
//                    return;
//                }
//                while (!ser_buffer_.empty() && ser_buffer_.back() == '\r')
//                    ser_buffer_.pop_back();
//                std::cout << "Serial data: \"" << ser_buffer_.c_str() << "\"\n";
//                ser_buffer_.clear();
//                return;
            default:
               break;
        }
	std::cerr << "MsgQueue: type=" << (long)msg.type
		  << "(" << MsgTypeEnum::key(msg.type)
		  << ") value=" << msg.value << "\n";
    }

    void audio_callback(Uint8* stream, int len)
    {
        emulator_.audioPort.copyInterleaved(reinterpret_cast<float*>(stream), len / (2 * sizeof(float)));
    }

    static constexpr int char_scale = 1;
    static constexpr int char_width = 8 * char_scale;
    static constexpr int char_height = 8 * char_scale;
    static void draw_char(void* pixels, int pitch, int x, int y, char c, uint32_t color)
    {
        assert(x >= 0 && x + char_width <= screen_width);
        assert(y >= 0 && y + char_height <= screen_height);
        if (c < 0x20 || c > 0x7f)
            c = 0x20;

        const uint8_t* chdata = &microknight[c - 0x20][0];

        for (int cy = 0; cy < char_height; ++cy) {
            uint32_t* dst = reinterpret_cast<uint32_t*>(reinterpret_cast<uint8_t*>(pixels) + x * 4 + (y + cy) * pitch);
            uint8_t bits = *chdata;
            for (int cx = 0; cx < char_width; ++cx) {
                if (bits & 0x80)
                    dst[cx] = color;
                if ((cx + 1) % char_scale == 0)
                    bits <<= 1;
            }
            if ((cy + 1) % char_scale == 0)
                ++chdata;
        }
    }

    static void draw_string(void* pixels, int pitch, int x, int y, const char* str, uint32_t color)
    {
        assert(x >= 0);
        assert(y >= 0 && y + char_height <= screen_width);
        while (*str && x + char_width*2 <= screen_height) {
            draw_char(pixels, pitch, x, y, *str++, color);
            x += char_width;
        }
    }

    static void draw_cursor(void* pixels, int pitch, int x, int y, uint32_t color)
    {
        for (int cy = 0; cy < char_height; ++cy) {
            uint32_t* dst = reinterpret_cast<uint32_t*>(reinterpret_cast<uint8_t*>(pixels) + x * 4 + (y + cy) * pitch);
            for (int cx = 0; cx < char_width; ++cx) {
                if ((cx ^ cy) & 1)
                    dst[cx] = color;
            }
        }
    }

    void update_overlay()
    {
        if (!overlay_dirty_) {
            const auto now = SDL_GetTicks();
            if (now - last_overlay_blink_ < 100)
                return;
            last_overlay_blink_ = now;
        }

        // Hmm...
        std::vector<std::string> lines;
        std::istringstream iss { emulator_.retroShell.text() };
        for (std::string line; std::getline(iss, line);)
            lines.push_back(line);

        void* pixels;
        int pitch;
        if (SDL_LockTexture(overlay_.get(), nullptr, &pixels, &pitch))
            throw_sdl_error("SDL_LockTexture");

        const uint32_t alpha = 192U << 24;
        for (int y = 0; y < screen_height; ++y) {
            auto row = reinterpret_cast<uint32_t*>(reinterpret_cast<uint8_t*>(pixels) + y * pitch);
            for (int x = 0 ; x < screen_width; ++x)
                row[x] = alpha | 0;
        }

        int y = char_height;
        const int max_lines = screen_height/char_height - 2;
        for (size_t linecnt = lines.size() > max_lines ? lines.size() - max_lines : 0; linecnt < lines.size(); ++linecnt) {
            const auto& l = lines[linecnt];
            draw_string(pixels, pitch, char_width, y, l.c_str(), alpha | 0xffffff);
            y += char_height;
        }

        if (!overlay_blink_ && !lines.empty()) {
            const auto cpos = static_cast<int>(emulator_.retroShell.cursorRel() + lines.back().length());
            if ((cpos + 1) * char_width < screen_width)
                draw_cursor(pixels, pitch, cpos * char_width, y - char_height, 0xffffffff);
        }

        SDL_UnlockTexture(overlay_.get());
        overlay_dirty_ = false;
        overlay_blink_ = !overlay_blink_;
    }

    void handle_overlay_mouse(const SDL_MouseButtonEvent& b)
    {
        if (b.button == SDL_BUTTON_RIGHT && b.state == SDL_RELEASED) {
            // Paste
            std::unique_ptr<char, sdl_freer> text{SDL_GetClipboardText()};
            if (text) {
                emulator_.retroShell.press(text.get());
            }
        }
    }

    void handle_overlay_key(const SDL_Keysym& k)
    {
        // TODO: Shift+Enter
        assert(overlay_active_);
        auto& rs = emulator_.retroShell;
        switch (k.sym) {
        case SDLK_ESCAPE:
        case SDLK_F12:
            overlay_active_ = false;
            last_buffer_pointer_ = nullptr; // Force update (dirty)
            break;
        case SDLK_UP:
            rs.press(RetroShellKey::UP);
            break;
        case SDLK_DOWN:
            rs.press(RetroShellKey::DOWN);
            break;
        case SDLK_LEFT:
            rs.press(RetroShellKey::LEFT);
            break;
        case SDLK_RIGHT:
            rs.press(RetroShellKey::RIGHT);
            break;
        case SDLK_HOME:
            rs.press(RetroShellKey::HOME);
            break;
        case SDLK_END:
            rs.press(RetroShellKey::END);
            break;
        case SDLK_TAB:
            rs.press(RetroShellKey::TAB);
            break;
        case SDLK_BACKSPACE:
            rs.press(RetroShellKey::BACKSPACE);
            break;
        case SDLK_DELETE:
            rs.press(RetroShellKey::DEL);
            break;
        case SDLK_RETURN:
            rs.press(RetroShellKey::RETURN);
            break;
        default:
            if (k.sym >= SDLK_a && k.sym <= SDLK_z) {
                const char ch = static_cast<char>((k.sym-SDLK_a) + (k.mod & KMOD_SHIFT ? 'A' : 'a'));
                rs.press(ch);
                break;
            } else if (k.sym >= ' ' && k.sym <= 0x7e) {
                if (k.mod & KMOD_SHIFT)
                    rs.press(with_shift(static_cast<char>(k.sym)));
                else
                    rs.press(static_cast<char>(k.sym));
                break;
            }
            std::cout << "Unhandled key: " << k.sym << " " << SDL_GetKeyName(k.sym) << "\n";
            return;
        }
    }

    bool handle_joystick_key(SDL_Keycode key, bool up)
    {
        auto& joystick = emulator_.controlPort2.joystick;
        switch (key) {
        case SDLK_KP_0:
        case SDLK_KP_5:
            joystick.trigger(up ? GamePadAction::RELEASE_FIRE : GamePadAction::PRESS_FIRE);
            return true;
        case SDLK_KP_8:
            joystick.trigger(up ? GamePadAction::RELEASE_Y : GamePadAction::PULL_UP);
            return true;
        case SDLK_KP_2:
            joystick.trigger(up ? GamePadAction::RELEASE_Y : GamePadAction::PULL_DOWN);
            return true;
        case SDLK_KP_4:
            joystick.trigger(up ? GamePadAction::RELEASE_X : GamePadAction::PULL_LEFT);
            return true;
        case SDLK_KP_6:
            joystick.trigger(up ? GamePadAction::RELEASE_X : GamePadAction::PULL_RIGHT);
            return true;
        default:
            return false;
        }
    }
};

int main(int argc, char* argv[])
{
    std::ios::sync_with_stdio(true);
    try {
        driver d;
        return d.run(argc, argv);

    } catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        return 1;
    }
    return 0;
}
