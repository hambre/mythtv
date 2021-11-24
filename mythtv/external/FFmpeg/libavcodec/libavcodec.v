LIBAVCODEC_MAJOR {
    global:
        av*;
        ff_ue_golomb_vlc_code;
        ff_golomb_vlc_len;
        ff_se_golomb_vlc_code;
        avpriv_find_start_code;
        ff_fft_init;
        ff_fft_end;
    local:
        *;
};
