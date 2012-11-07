// a class to hold sounds as named buffers
class sound_manager {
  enum { num_sources = 8 };
  enum { max_buffers = 64 };
  ALuint sources[num_sources];
  ALuint buffers[max_buffers];
  char *buffer_names[max_buffers];
  unsigned cur_source;
  unsigned cur_buffer;
  
  sound_manager() {
    ALCdevice *dev = alcOpenDevice(NULL);
    assert(dev);
    ALCcontext *ctx = alcCreateContext(dev, NULL);
    alcMakeContextCurrent(ctx);
    assert(ctx);

    // pre-build sources and buffers
    alGenSources(num_sources, sources);
    alGenBuffers(max_buffers, buffers);
    memset(buffer_names, 0, sizeof(buffer_names));
    cur_source = cur_buffer = 0;

    alListener3f(AL_POSITION, 0, 0, 0);
  }
  
  void add_buffer(const char *name, unsigned char *data, int start_sample, int end_sample, int rate) {
    unsigned buffer = cur_buffer++ % max_buffers;
    free(buffer_names[buffer]);
    buffer_names[buffer] = (char*)malloc(strlen(name)+1);
    strcpy(buffer_names[buffer], name);
    
    enum { magic_offset = 44 };

    //printf("%d %d %d %s [%d]\n", buffer_idx, start_offset, end_offset, name, length);
    //assert(start_offset < length && end_offset > start_offset && end_offset < length);

    alBufferData(
      buffers[buffer],
      AL_FORMAT_MONO16,
      data + magic_offset + 2 * start_sample,
      2 * ( end_sample - start_sample ),
      rate
    );
  }
  
  void add_buffers_impl(const char *label_file_name, const char *sample_file_name) {
    unsigned char *data = (unsigned char*)file_manager::bytes(sample_file_name);
    if (data) {
      enum { magic_offset = 44 };
      
      /*for( int i = 0; i != 44; i += 2) {
        printf("%d %d\n", i, data[i]+data[i+1]*256);
      }*/

      int rate = data[24] + data[24+1]*256;

      // use an audacity label file to build the buffers for the sound
      FILE *labels = fopen(label_file_name, "r");
      assert(labels && "label file not found");
      char line[64];
      while (fgets(line, sizeof(line)-1, labels)) {
        float start, end;
        char name[64];
        sscanf(line, "%f %f %s", &start, &end, name);
        int start_sample = (int)(rate*start);
        int end_sample = (int)(rate*end);
        add_buffer(name, data, start_sample, end_sample, rate);
      }
      fclose(labels);
    }
  }

  void play_impl(const vec4 &pos, const char *name) {
    for (unsigned i = 0; i != max_buffers; ++i) {
      if (buffer_names[i] && !strcmp(buffer_names[i], name)) {
        // allocate a source
        ALuint source = sources[cur_source++ % num_sources];
        
        // play a sound on that source
        alSourcei(source, AL_BUFFER, buffers[i]);
        alSource3f(source, AL_POSITION, pos[0], pos[1], pos[2]);
        alSourcePlay(source);
        return;
      }
    }
  }

  static sound_manager &get() {
    static sound_manager the_sound_manager;
    return the_sound_manager;
  }
public:
  static void add_buffers(const char *label_file_name, const char *sample_file_name) {
    get().add_buffers_impl(label_file_name, sample_file_name);
  }

  static void play(const vec4 &pos, const char *name) {
    get().play_impl(pos, name);
  }
};
