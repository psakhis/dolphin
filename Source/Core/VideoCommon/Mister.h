#include <inttypes.h>

class MiSTer
{
public:
  MiSTer();
  ~MiSTer();

  char* getPBufferBlit(void);
  char* getPBufferBlitDelta(void);
  char* getPBufferAudio(void);
  void Close(void);
  void Init(const char* mister_host, short mister_port, uint8_t lz4_frames, uint32_t sound_rate,
            uint8_t sound_chan, uint8_t rgb_mode, uint16_t mister_mtu);
  void Switchres(int video_mode, int w, int h, double freq, bool progressive_fb); 
  void Blit(uint16_t vsync, uint8_t field);
  void setAudioSamples(uint16_t soundSize);
  void BlitAudio();
  void doSync(void);
  void setDeltaFrames(uint8_t delta);
  void setMatch(uint32_t match);

  uint16_t getAudioSamples();
  uint8_t getField(void); 
  uint16_t getWidth(void);
  uint16_t getHeight(void);
  uint16_t getRealWidth(void);
  uint16_t getRealHeight(void);
  uint8_t getMode(void);
  
  bool isInterlaced(void);
  bool isConnected(void);
  bool isPAL(void);
  
  char m_texture_bytes[704 * 538 * 3]; // always progressive when outputs interlaced to alternate fields  

private: 
  uint32_t m_frame = 0;  
  uint8_t m_frameField = 0;
  uint8_t m_mode = 9; //0-ntsc/pal, 1-ntsc 240p, 2-ntsc, 3-vga
  uint16_t m_real_width = 0;
  uint16_t m_real_height = 0;
  uint16_t m_width_core = 0;
  uint16_t m_height_core = 0; 
  bool m_connected = false;
  bool m_pal = false;  
  uint8_t m_interlaced = 0;
  uint8_t m_buffer_prog = 0;
  uint16_t m_samples = 0;
  uint8_t m_delta_frames = 0;
  uint8_t m_match = 0;
  bool m_error_connection = false;
};

extern MiSTer g_mister;
