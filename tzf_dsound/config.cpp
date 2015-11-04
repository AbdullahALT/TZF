/**
 * This file is part of Tales of Zestiria "Fix".
 *
 * Tales of Zestiria "Fix" is free software : you can redistribute it
 * and/or modify it under the terms of the GNU General Public License
 * as published by The Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * Tales of Zestiria "Fix" is distributed in the hope that it will be
 * useful,
 *
 * But WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Tales of Zestiria "Fix".
 *
 *   If not, see <http://www.gnu.org/licenses/>.
 *
**/

#include "config.h"
#include "parameter.h"
#include "ini.h"
#include "log.h"

std::wstring TZF_VER_STR = L"0.3.4";

static tzf::INI::File*  dll_ini = nullptr;

tzf_config_t config;

tzf::ParameterFactory g_ParameterFactory;

struct {
  tzf::ParameterInt*     sample_rate;
  tzf::ParameterInt*     channels;
  tzf::ParameterBool*    compatibility;
  tzf::ParameterBool*    enable_fix;
} audio;

struct {
  tzf::ParameterBool*    stutter_fix;
  tzf::ParameterFloat*   fudge_factor;
  tzf::ParameterBool*    allow_fake_sleep;
} framerate;

struct {
  tzf::ParameterInt*     fovy_addr;
  tzf::ParameterInt*     aspect_addr;
  tzf::ParameterFloat*   fovy;
  tzf::ParameterFloat*   aspect_ratio;
} render;

struct {
  tzf::ParameterBool*    allow_broadcasts;
} steam;

struct {
  tzf::ParameterStringW* intro_video;
  tzf::ParameterStringW* version;
} sys;


bool
TZF_LoadConfig (std::wstring name) {
  // Load INI File
  std::wstring full_name = name + L".ini";
  dll_ini = new tzf::INI::File ((wchar_t *)full_name.c_str ());

  bool empty = dll_ini->get_sections ().empty ();

  //
  // Create Parameters
  //

  audio.channels =
    static_cast <tzf::ParameterInt *>
      (g_ParameterFactory.create_parameter <int> (
        L"Audio Channels")
      );
  audio.channels->register_to_ini (
    dll_ini,
      L"TZFIX.Audio",
        L"Channels" );

  audio.sample_rate =
    static_cast <tzf::ParameterInt *>
      (g_ParameterFactory.create_parameter <int> (
        L"Sample Rate")
      );
  audio.sample_rate->register_to_ini (
    dll_ini,
      L"TZFIX.Audio",
        L"SampleRate" );

  audio.compatibility =
    static_cast <tzf::ParameterBool *>
      (g_ParameterFactory.create_parameter <bool> (
        L"Compatibility Mode")
      );
  audio.compatibility->register_to_ini (
    dll_ini,
      L"TZFIX.Audio",
        L"CompatibilityMode" );

  audio.enable_fix =
    static_cast <tzf::ParameterBool *>
      (g_ParameterFactory.create_parameter <bool> (
        L"Enable Fix")
      );
  audio.enable_fix->register_to_ini (
    dll_ini,
      L"TZFIX.Audio",
        L"EnableFix" );


  framerate.stutter_fix =
    static_cast <tzf::ParameterBool *>
      (g_ParameterFactory.create_parameter <bool> (
        L"Stutter Elimination")
      );
  framerate.stutter_fix->register_to_ini (
    dll_ini,
      L"TZFIX.FrameRate",
        L"FixStutter" );

  framerate.fudge_factor =
    static_cast <tzf::ParameterFloat *>
      (g_ParameterFactory.create_parameter <float> (
        L"Fudge Factor")
      );
  framerate.fudge_factor->register_to_ini (
    dll_ini,
      L"TZFIX.FrameRate",
        L"FudgeFactor" );

  framerate.allow_fake_sleep =
    static_cast <tzf::ParameterBool *>
      (g_ParameterFactory.create_parameter <bool> (
        L"Allow Fake Sleep")
      );
  framerate.allow_fake_sleep->register_to_ini (
    dll_ini,
      L"TZFIX.FrameRate",
        L"AllowFakeSleep" );


  render.aspect_ratio =
    static_cast <tzf::ParameterFloat *>
      (g_ParameterFactory.create_parameter <float> (
        L"Aspect Ratio")
      );
  render.aspect_ratio->register_to_ini (
    dll_ini,
      L"TZFIX.Render",
        L"AspectRatio" );

  render.fovy =
    static_cast <tzf::ParameterFloat *>
      (g_ParameterFactory.create_parameter <float> (
        L"Field of View Vertical")
      );
  render.fovy->register_to_ini (
    dll_ini,
      L"TZFIX.Render",
        L"FOVY" );

  render.aspect_addr =
    static_cast <tzf::ParameterInt *>
      (g_ParameterFactory.create_parameter <int> (
        L"Aspect Ratio Memory Address")
      );
  render.aspect_addr->register_to_ini (
    dll_ini,
      L"TZFIX.Render",
        L"AspectRatio_Address" );

  render.fovy_addr =
    static_cast <tzf::ParameterInt *>
      (g_ParameterFactory.create_parameter <int> (
        L"Field of View Vertical Address")
      );
  render.fovy_addr->register_to_ini (
    dll_ini,
      L"TZFIX.Render",
        L"FOVY_Address" );


  steam.allow_broadcasts =
    static_cast <tzf::ParameterBool *>
      (g_ParameterFactory.create_parameter <bool> (
        L"Allow Steam Broadcasting")
      );
  steam.allow_broadcasts->register_to_ini(
    dll_ini,
      L"TZFIX.Steam",
        L"AllowBroadcasts" );

  sys.version =
    static_cast <tzf::ParameterStringW *>
      (g_ParameterFactory.create_parameter <std::wstring> (
        L"Software Version")
      );
  sys.version->register_to_ini (
    dll_ini,
      L"TZFIX.System",
        L"Version" );

  sys.intro_video =
    static_cast <tzf::ParameterStringW *>
      (g_ParameterFactory.create_parameter <std::wstring> (
        L"Intro Video")
      );
  sys.intro_video->register_to_ini (
    dll_ini,
      L"TZFIX.System",
        L"IntroVideo" );

  //
  // Load Parameters
  //
  if (audio.channels->load ())
    config.audio.channels = audio.channels->get_value ();

  if (audio.sample_rate->load ())
    config.audio.sample_hz = audio.sample_rate->get_value ();

  if (audio.compatibility->load ())
    config.audio.compatibility = audio.compatibility->get_value ();

  if (audio.enable_fix->load ())
    config.audio.enable_fix = audio.enable_fix->get_value ();


  if (framerate.stutter_fix->load ())
    config.framerate.stutter_fix = framerate.stutter_fix->get_value ();

  if (framerate.fudge_factor->load ())
    config.framerate.fudge_factor = framerate.fudge_factor->get_value ();

  if (framerate.allow_fake_sleep->load ())
    config.framerate.allow_fake_sleep = framerate.allow_fake_sleep->get_value ();

  if (render.aspect_addr->load ())
    config.render.aspect_addr = render.aspect_addr->get_value ();

  if (render.fovy_addr->load ())
    config.render.fovy_addr = render.fovy_addr->get_value ();

  if (render.aspect_ratio->load ())
    config.render.aspect_ratio = render.aspect_ratio->get_value ();

  if (render.fovy->load ())
    config.render.fovy = render.fovy->get_value ();

  if (steam.allow_broadcasts->load ())
    config.steam.allow_broadcasts = steam.allow_broadcasts->get_value ();

  if (sys.version->load ())
    config.system.version = sys.version->get_value ();

  //if (sys.intro_video->load ())
    //config.system.intro_video = sys.intro_video->get_value ();

  if (empty)
    return false;

  return true;
}

void
TZF_SaveConfig (std::wstring name, bool close_config) {
  audio.channels->set_value    (config.audio.channels);
  audio.channels->store        ();

  audio.sample_rate->set_value (config.audio.sample_hz);
  audio.sample_rate->store     ();

  audio.compatibility->set_value (config.audio.compatibility);
  audio.compatibility->store     ();

  audio.enable_fix->set_value (config.audio.enable_fix);
  audio.enable_fix->store     ();

  framerate.stutter_fix->set_value (config.framerate.stutter_fix);
  framerate.stutter_fix->store     ();

  framerate.fudge_factor->set_value (config.framerate.fudge_factor);
  framerate.fudge_factor->store     ();

  framerate.allow_fake_sleep->set_value (config.framerate.allow_fake_sleep);
  framerate.allow_fake_sleep->store     ();

  render.aspect_addr->set_value (config.render.aspect_addr);
  render.aspect_addr->store     ();

  render.fovy_addr->set_value (config.render.fovy_addr);
  render.fovy_addr->store     ();

  render.aspect_ratio->set_value (config.render.aspect_ratio);
  render.aspect_ratio->store     ();

  render.fovy->set_value (config.render.fovy);
  render.fovy->store     ();

  steam.allow_broadcasts->set_value (config.steam.allow_broadcasts);
  steam.allow_broadcasts->store     ();

  sys.version->set_value       (TZF_VER_STR);
  sys.version->store           ();

  //sys.intro_video->set_value   (config.system.intro_video);
  //sys.intro_video->store       ();

  dll_ini->write (name + L".ini");

  if (close_config) {
    if (dll_ini != nullptr) {
      delete dll_ini;
      dll_ini = nullptr;
    }
  }
}