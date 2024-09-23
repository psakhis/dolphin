#include "Mister.h"

#ifdef _WIN32
#define GROOVYMISTER_WIN32_STATIC
#endif

#include "mister/groovymister_wrapper.h"
#include <Common/Logging/Log.h>
#include <mutex>
#include <Common/CommonTypes.h>

MiSTer::MiSTer()
{
  m_error_connection = false;
}

MiSTer::~MiSTer()
{
}

char* MiSTer::getPBufferBlit(void)
{
  return gmw_get_pBufferBlit(0);
}

char* MiSTer::getPBufferBlitDelta(void)
{
  return gmw_get_pBufferBlitDelta();
}


char* MiSTer::getPBufferAudio(void)
{
  return gmw_get_pBufferAudio();
}

void MiSTer::Close(void)
{
  if (m_connected)
    gmw_close();
  m_connected = false;
}

void MiSTer::Init(const char* mister_host, short mister_port, uint8_t lz4_frames,
                  uint32_t sound_rate, uint8_t sound_chan, uint8_t rgb_mode, uint16_t mister_mtu)
{
  if (m_error_connection)
    return;

  if (m_connected)
    gmw_close();

  gmw_bindInputs(mister_host);
  if (gmw_init(mister_host, lz4_frames, sound_rate, sound_chan, rgb_mode, mister_mtu) < 0)
  {
    NOTICE_LOG_FMT(VIDEO, "GroovyMiSTer init failed");  
    m_connected = false;
    m_error_connection = true;
  }
  else
    m_connected = true;
  
   m_width_core = 0;
   m_height_core = 0;
   m_real_width = 0;
   m_real_height = 0;
   m_interlaced = 0;
   m_buffer_prog = 1;
   m_frame = 0;     
   m_pal = false;
   m_samples = 0;
   m_mode = 9;
   m_match = 0;
}

void MiSTer::Switchres(int video_mode, int w, int h, double freq, bool progressive_fb)
{
  double px = 12.146841;
  uint16_t udp_hactive = 640;
  uint16_t udp_hbegin = 658;
  uint16_t udp_hend = 715;
  uint16_t udp_htotal = 772;
  uint16_t udp_vactive = 480;
  uint16_t udp_vbegin = 487;
  uint16_t udp_vend = 493;
  uint16_t udp_vtotal = 525;
  uint8_t udp_interlace = 1;
  m_pal = (freq > 55) ? false : true; 
  m_mode = video_mode;

  if (freq > 55 || video_mode == 1 || video_mode == 2)  // 15khz/ntsc/vga
  {
    if (w == 256)
    {
      px = 4.852622;
      udp_hactive = 256;
      udp_hbegin = 263;
      udp_hend = 286;
      udp_htotal = 309;
    }

    if (w == 320)
    {
      px = 6.046147;
      udp_hactive = 320;
      udp_hbegin = 329;
      udp_hend = 357;
      udp_htotal = 385;
    }

    if (w == 512)
    {
      px = 9.739500;
      udp_hactive = 512;
      udp_hbegin = 527;
      udp_hend = 573;
      udp_htotal = 619;
    }

    if (w == 608)
    {
      px = 11.533205;
      udp_hactive = 608;
      udp_hbegin = 625 ;
      udp_hend = 679;
      udp_htotal = 733;
    }

    if (w >= 656)
    {
      px = 12.445791;
      udp_hactive = 656;
      udp_hbegin = 675;
      udp_hend = 733;
      udp_htotal = 791;
    }

    if (w >= 660)
    {
      px = 12.540197;
      udp_hactive = 660;
      udp_hbegin = 679;
      udp_hend = 738;
      udp_htotal = 797;      
    }

    if (w >= 666)
    {
      px = 12.634602;
      udp_hactive = 666;
      udp_hbegin = 685;
      udp_hend = 744;
      udp_htotal = 803;
    }

    if (h == 224 && video_mode < 3)
    {
      udp_vactive = 224;
      udp_vbegin = 236;
      udp_vend = 239;
      udp_vtotal = 262;
      udp_interlace = 0;
    }

    if (h == 240 && video_mode < 3)  
    {      
      udp_vactive = 240;
      udp_vbegin = 244;
      udp_vend = 247;
      udp_vtotal = 262;
      udp_interlace = 0;
    }

    if (h >= 448 && video_mode == 1)  // 240p
    {      
      px = (w == 320) ? 06.046147 :
           (w == 512) ? 09.739500 :
           (w == 608) ? 11.533205 :
           (w == 656) ? 12.422085 :
           (w == 660) ? 12.516311 :
           (w == 666) ? 12.610536 :
                        12.123704;
      udp_vactive = 240;
      udp_vbegin = 244;
      udp_vend = 247;
      udp_vtotal = 262;
      udp_interlace = 0;
    }
    
    if (h == 448)
    {                       
      if (video_mode == 1)  // 240p
      {
        px = (w == 512) ? 09.739500 :
             (w == 608) ? 11.533205 :
             (w == 656) ? 12.422085 :
             (w == 660) ? 12.516311 :
             (w == 666) ? 12.610536 :
                          12.123704;
        udp_vactive = 224;
        udp_vbegin = 236;
        udp_vend = 239;
        udp_vtotal = 262;
        udp_interlace = 0;
      }    
      else 
      {
        udp_vactive = 448;
        udp_vbegin = 471;
        udp_vend = 477;  
      }
    }       
  }
  else //pal/vga
  {      
    px = 12.296875;      
    udp_hactive = 640;
    udp_hbegin = 658;
    udp_hend = 716;
    udp_htotal = 787;
    udp_vactive = 528;
    udp_vbegin = 558;
    udp_vend = 563;
    udp_vtotal = 625;

    if (w == 512)
    {
      px = 9.843750;
      udp_hactive = 512;
      udp_hbegin = 527;
      udp_hend = 573;
      udp_htotal = 630;
    }

    if (w == 608)
    {
      px = 11.703125;
      udp_hactive = 608;
      udp_hbegin = 626;
      udp_hend = 681;
      udp_htotal = 749;
    }

    if (w >= 650)
    {
      px = 12.5;
      udp_hactive = 650;
      udp_hbegin = 669;
      udp_hend = 728;
      udp_htotal = 800;
    }

    if (w >= 660)
    {
      px = 12.703125;
      udp_hactive = 660;
      udp_hbegin = 679;
      udp_hend = 739;
      udp_htotal = 813;
    }

    if (w >= 666)
    {
      px = 12.796875;
      udp_hactive = 666;
      udp_hbegin = 685;
      udp_hend = 745;
      udp_htotal = 819;
    }  

    if (h == 448)  
    {     
      udp_vactive = 448;
      udp_vbegin = 518;
      udp_vend = 523;
    }

    if (h >= 496)
    {
      udp_vactive = 496;
      udp_vbegin = 542;
      udp_vend = 547;
    }

    if (h >= 512)
    {
      udp_vactive = 512;
      udp_vbegin = 550;
      udp_vend = 555;
    }

    if (h >= 522)  
    {
      udp_vactive = 522;
      udp_vbegin = 555;
      udp_vend = 560;
    }

    if (h >= 528)
    {
      udp_vactive = 528;
      udp_vbegin = 558;
      udp_vend = 563;
    }    

    if (h >= 530) 
    {
      udp_vactive = 530;
      udp_vbegin = 559;
      udp_vend = 564;
    }

    if (h >= 538)
    {
      udp_vactive = 538;
      udp_vbegin = 563;
      udp_vend = 568;
    }

    if (h >= 574)
    {
      udp_vactive = 574;
      udp_vbegin = 581;
      udp_vend = 586;
    }
  }

  if (video_mode == 3)  // vga
  {
    px = px * 2;
    udp_interlace = 0;
    progressive_fb = 1;
  }
  m_interlaced = udp_interlace;
  m_frameField = 0;
  m_width_core = w;
  m_height_core = h;
  m_real_width = udp_hactive;
  m_real_height = udp_vactive;
  m_buffer_prog = (m_interlaced && !progressive_fb) ? 0 : 1;

  NOTICE_LOG_FMT(VIDEO, "GroovyMiSTer switchres applied {}x{}", udp_hactive, udp_vactive);                 

  gmw_switchres(px, udp_hactive, udp_hbegin, udp_hend, udp_htotal, udp_vactive, udp_vbegin,
                udp_vend, udp_vtotal, (progressive_fb && udp_interlace) ? 2 : udp_interlace);
}


void MiSTer::Blit(uint16_t vsync, uint8_t field)
{
  if (m_real_width && m_real_height)
  {
    gmw_fpgaStatus status;
    gmw_getStatus(&status);
    m_frame++;         
 
    if (status.frame >= m_frame)
      m_frame = status.frame + 1;    

    //int matchDelta = m_buffer_prog ? m_match : 0;
    int matchDelta = 0;
    if (m_delta_frames || !m_buffer_prog) //only blit 1 field from progressive bytes
    {     
      uint8_t* tmp_buffer = (uint8_t*) gmw_get_pBufferBlit(field);
      uint8_t* tmp_buffer_delta = (uint8_t*) gmw_get_pBufferBlitDelta();      
      int dst_buff = 0;
      int src_buff = field ? m_real_width * 3 : 0;
     
      for (int y = 0; y < m_real_height; y++)
      {
        for (int x = 0; x < m_real_width; x++)
        {
          if (m_delta_frames)
          {
            if (g_mister.m_texture_bytes[src_buff + 0] == tmp_buffer[dst_buff + 0])
            {
              matchDelta++;
              tmp_buffer_delta[dst_buff + 0] = 0x00;
            }
            else
            {
              tmp_buffer_delta[dst_buff + 0] = (uint8_t)g_mister.m_texture_bytes[src_buff + 0] -
                                               (uint8_t)tmp_buffer[dst_buff + 0];
            }
            if (g_mister.m_texture_bytes[src_buff + 1] == tmp_buffer[dst_buff + 1])
            {
              matchDelta++;
              tmp_buffer_delta[dst_buff + 1] = 0x00;
            }
            else
            {
              tmp_buffer_delta[dst_buff + 1] = (uint8_t)g_mister.m_texture_bytes[src_buff + 1] -
                                               (uint8_t)tmp_buffer[dst_buff + 1];
            }
            if (g_mister.m_texture_bytes[src_buff + 2] == tmp_buffer[dst_buff + 2])
            {
              matchDelta++;
              tmp_buffer_delta[dst_buff + 2] = 0x00;
            }
            else
            {
              tmp_buffer_delta[dst_buff + 2] = (uint8_t)g_mister.m_texture_bytes[src_buff + 2] -
                                               (uint8_t)tmp_buffer[dst_buff + 2];
            }
          }
          tmp_buffer[dst_buff + 0] = g_mister.m_texture_bytes[src_buff + 0];
          tmp_buffer[dst_buff + 1] = g_mister.m_texture_bytes[src_buff + 1];
          tmp_buffer[dst_buff + 2] = g_mister.m_texture_bytes[src_buff + 2];
          dst_buff += 3;
          src_buff += 3;
        }                
        if (!m_buffer_prog)
        {
          y++;
          src_buff += m_real_width * 3;
        }                
      }          
    } 
    gmw_blit(m_frame, field, vsync, 15000, matchDelta);
    gmw_waitSync();
  }
}

void MiSTer::doSync(void)
{
  gmw_waitSync();
}

void MiSTer::setAudioSamples(uint16_t soundSize)
{
  m_samples += soundSize;
}

uint16_t MiSTer::getAudioSamples()
{
  return m_samples;
}

void MiSTer::BlitAudio()
{
  if (m_real_width && m_real_height && m_samples)
    gmw_audio(m_samples);
  m_samples = 0;
}

void MiSTer::setDeltaFrames(uint8_t delta)
{
  m_delta_frames = delta;
}

void MiSTer::setMatch(uint32_t match)
{
  m_match = match;
}

uint8_t MiSTer::getField(void)
{
  uint8_t field = 0;
  if (!m_buffer_prog)
  {
    gmw_fpgaStatus status;
    gmw_getStatus(&status);    
    m_frameField = !status.vgaF1 ^ ((m_frame - status.frame) % 2);  // Calamity formula
    field = m_frameField;
  }
  return field;
}


bool MiSTer::isPAL(void)
{
  return m_pal;
}

bool MiSTer::isInterlaced(void)
{
  return (!m_buffer_prog);
}

bool MiSTer::isConnected(void)
{
  return m_connected;
}

uint16_t MiSTer::getWidth(void)
{
  return m_width_core;
}
uint16_t MiSTer::getHeight(void)
{
  return m_height_core;
}

uint16_t MiSTer::getRealWidth(void)
{
  return m_real_width;
}
uint16_t MiSTer::getRealHeight(void)
{
  return m_real_height;
}
uint8_t MiSTer::getMode(void)
{
  return m_mode;
}

MiSTer g_mister;
