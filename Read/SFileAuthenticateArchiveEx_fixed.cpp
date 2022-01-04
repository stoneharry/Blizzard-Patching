bool SFileAuthenticateArchiveEx ( Blizzard::Mopaq::HSARCHIVE__ *archive
                                , Blizzard::Mopaq::AuthResult *authresult
                                , const unsigned char *modulus
                                , unsigned int modulus_length
                                , const unsigned char *exponent
                                , unsigned int exponent_length
                                )
{
  *authresult = 5;
  return true;
}
