void opendcp_encoder_last(odcp_image_t **image_ptr, const char *infile) {
    return;
}

opendcp_encoder_t opendcp_encoders[] = {
    FOREACH_OPENDCP_ENCODER(GENERATE_STRUCT)
};
