int AudioThreadFunc(int args, void* argp);

void pspaudio_init(void);
void pspaudio_clear(void);
void pspaudio_write(short* buffer, int num_samples);
void pspaudio_shutdown(void);
void pspaudio_SetEffect(int value);
