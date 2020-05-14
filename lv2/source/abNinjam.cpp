#include "../include/ports.h"
#include <lv2.h>
#include <stdlib.h>

#include "common.h"

#define p_uri "http://hippie.lt/lv2/abNinjam"

using namespace AbNinjam::Common;

namespace AbNinjam {
namespace Lv2 {

static LV2_Descriptor *abNinjamDescriptor = nullptr;

class AbNinjamPlugin {
public:
  NinjamClient *ninjamClient;
  NinjamClientStatus ninjamClientStatus = disconnected;
  ConnectionProperties *connectionProperties;
  float *connect, *metronomeVolume;
  float *output_buffers[2], *input_buffers[2];
  double sampleRate;
  AbNinjamPlugin() {
    // processor = new AbNinjamProcessor();
  }
  ~AbNinjamPlugin() {}
  // AbNinjamProcessor *processor;
};

static LV2_Handle instantiate(const _LV2_Descriptor *descriptor, double s_rate,
                              const char *path,
                              const LV2_Feature *const *features) {
  AbNinjamPlugin *plugin_data = new AbNinjamPlugin;
  plugin_data->ninjamClient = new NinjamClient();
  plugin_data->connectionProperties = new ConnectionProperties();
  plugin_data->sampleRate = s_rate;
  return static_cast<LV2_Handle>(plugin_data);
}

static void connectPort(LV2_Handle instance, uint32_t port, void *data) {
  AbNinjamPlugin *plugin = static_cast<AbNinjamPlugin *>(instance);

  switch (port) {
  case CONNECT:
    plugin->connect = static_cast<float *>(data);
    break;
  case METRONOME_VOLUME:
    plugin->metronomeVolume = static_cast<float *>(data);
    break;
  case INPUT_LEFT:
    plugin->input_buffers[0] = static_cast<float *>(data);
    break;
  case INPUT_RIGHT:
    plugin->input_buffers[1] = static_cast<float *>(data);
    break;
  case OUTPUT_LEFT:
    plugin->output_buffers[0] = static_cast<float *>(data);
    break;
  case OUTPUT_RIGHT:
    plugin->output_buffers[1] = static_cast<float *>(data);
    break;
  }
}

static void run(LV2_Handle instance, uint32_t sample_count) {

  AbNinjamPlugin *plugin_data = static_cast<AbNinjamPlugin *>(instance);

  if (*plugin_data->connect > 0.f) {
    // Getting port values
    plugin_data->ninjamClient->gsNjClient()->config_metronome =
        *plugin_data->metronomeVolume;

    if (plugin_data->ninjamClientStatus != ok) {
      plugin_data->ninjamClientStatus = plugin_data->ninjamClient->connect(
          *plugin_data->connectionProperties);
    } else {
      plugin_data->ninjamClient->audiostreamOnSamples(
          plugin_data->input_buffers, 2, plugin_data->output_buffers, 2,
          static_cast<int>(sample_count),
          static_cast<int>(plugin_data->sampleRate));
    }
  } else {
    if (plugin_data->ninjamClientStatus == ok) {
      plugin_data->ninjamClient->disconnect();
      // Assume disconnected
      plugin_data->ninjamClientStatus = disconnected;
    }
    // Bypassing
    if (plugin_data->output_buffers[0] != plugin_data->input_buffers[0] ||
        plugin_data->output_buffers[1] != plugin_data->input_buffers[1]) {
      for (uint32_t i = 0; i < sample_count; ++i) {
        plugin_data->output_buffers[0][i] = plugin_data->input_buffers[0][i];
        plugin_data->output_buffers[1][i] = plugin_data->input_buffers[1][i];
      }
    }
  }
}

static void cleanup(LV2_Handle instance) {
  AbNinjamPlugin *plugin_data = static_cast<AbNinjamPlugin *>(instance);
  delete plugin_data;
}

static void init() {
  abNinjamDescriptor =
      static_cast<LV2_Descriptor *>(malloc(sizeof(LV2_Descriptor)));
  abNinjamDescriptor->URI = p_uri;
  abNinjamDescriptor->instantiate = instantiate;
  abNinjamDescriptor->connect_port = connectPort;
  abNinjamDescriptor->activate = nullptr;
  abNinjamDescriptor->run = run;
  abNinjamDescriptor->deactivate = nullptr;
  abNinjamDescriptor->cleanup = cleanup;
  abNinjamDescriptor->extension_data = nullptr;
}

LV2_SYMBOL_EXPORT
const LV2_Descriptor *lv2_descriptor(uint32_t index) {

  if (!abNinjamDescriptor) {
    init();
  }

  switch (index) {
  case 0:
    return abNinjamDescriptor;
  default:
    return nullptr;
  }
}

} // namespace Lv2
} // namespace AbNinjam
