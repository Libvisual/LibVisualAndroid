#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include <jack/jack.h>

#include <libvisual/libvisual.h>

#define PCM_BUF_SIZE	1024

typedef struct {
	jack_client_t *client;
	jack_port_t *input_port;

	int shutdown;

	short fakebuf[PCM_BUF_SIZE];
} JackPrivate;

static int process_callback (jack_nframes_t nframes, void *arg);
static void shutdown_callback (void *arg);

int inp_jack_init (VisPluginData *plugin);
int inp_jack_cleanup (VisPluginData *plugin);
int inp_jack_upload (VisPluginData *plugin, VisAudio *audio);

const VisPluginInfo *get_plugin_info (int *count)
{
	static const VisInputPlugin input[] = {{
		.upload = inp_jack_upload
	}};

	static const VisPluginInfo info[] = {{
		.struct_size = sizeof (VisPluginInfo),
		.api_version = VISUAL_PLUGIN_API_VERSION,
		.type = VISUAL_PLUGIN_TYPE_INPUT,

		.plugname = "jack",
		.name = "jack",
		.author = "Dennis Smit <ds@nerds-incorporated.org>",
		.version = "0.1",
		.about = "The Jackit capture plugin",
		.help = "Use this plugin to capture PCM data from the jackd daemon.",

		.init = inp_jack_init,
		.cleanup = inp_jack_cleanup,

		.plugin = (void *) &input[0]
	}};

	*count = sizeof (info) / sizeof (*info);

	return info;
}

int inp_jack_init (VisPluginData *plugin)
{
	JackPrivate *priv;
	const char **ports;

	visual_log_return_val_if_fail (plugin != NULL, -1);
	
	priv = visual_mem_new0 (JackPrivate, 1);
	visual_log_return_val_if_fail (priv != NULL, -1);
	plugin->priv = priv;	

	if ((priv->client = jack_client_new ("Libvisual jackit capture")) == NULL) {
		visual_log (VISUAL_LOG_CRITICAL, "jack server probably not running");
		return -1;
	}
	
	jack_set_process_callback (priv->client, process_callback, priv);
	jack_on_shutdown (priv->client, shutdown_callback, priv);

	priv->input_port = jack_port_register (priv->client, "input", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);

	if (jack_activate (priv->client) == 1) {
		visual_log (VISUAL_LOG_CRITICAL, "Cannot activate the jack client");
		
		return -1;
	}

	if ((ports = jack_get_ports (priv->client, NULL, NULL, JackPortIsPhysical|JackPortIsOutput)) == NULL) {
		visual_log (VISUAL_LOG_CRITICAL, "Cannot find any physical capture ports.");
		
		return -1;
	}

	if (jack_connect (priv->client, ports[0], jack_port_name (priv->input_port))) {
		visual_log (VISUAL_LOG_CRITICAL, "Cannot connect input ports.");

		visual_mem_free (ports);
		
		return -1;
	}

	visual_mem_free (ports);
	
	return 0;
}

int inp_jack_cleanup (VisPluginData *plugin)
{
	JackPrivate *priv = plugin->priv;

	visual_log_return_val_if_fail( plugin != NULL, -1 );
	priv = plugin->priv;
	visual_log_return_val_if_fail( priv != NULL, -1 );

	jack_client_close (priv->client);

	visual_mem_free (priv);

	return 0;
}

int inp_jack_upload (VisPluginData *plugin, VisAudio *audio)
{
	JackPrivate *priv = NULL;
	int i;

	visual_log_return_val_if_fail(audio != NULL, -1);
	visual_log_return_val_if_fail(plugin != NULL, -1);
	priv = plugin->priv;
	visual_log_return_val_if_fail(priv != NULL, -1);

	if (priv->shutdown == TRUE) {
		visual_log (VISUAL_LOG_CRITICAL, "The jack server seems to have shutdown.");

		return -1;
	}

	for (i = 0; i < PCM_BUF_SIZE && i < 1024; i += 2) {
		audio->plugpcm[0][i >> 1] = priv->fakebuf[i];
		audio->plugpcm[1][i >> 1] = priv->fakebuf[i + 1];
	}

	return 0;
}

static int process_callback (jack_nframes_t nframes, void *arg)
{
	const size_t sample_size = sizeof (jack_default_audio_sample_t);
	JackPrivate *priv = arg;
	jack_default_audio_sample_t *in;
	int i;

	in = (jack_default_audio_sample_t *) jack_port_get_buffer (priv->input_port, nframes);

	/* And now ?! */
	
	return 0;
}

static void shutdown_callback (void *arg)
{
	JackPrivate *priv = arg;

	priv->shutdown = TRUE;
}

