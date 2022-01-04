bool SFileAuthenticateArchiveEx ( Blizzard::Mopaq::HSARCHIVE__ *archive
                                , Blizzard::Mopaq::AuthResult *authresult
                                , const unsigned char *modulus
                                , unsigned int modulus_length
                                , const unsigned char *exponent
                                , unsigned int exponent_length
                                )
{
  Blizzard::Mopaq::AuthResult authresult_temp;

  bool result (true);

  if (!Blizzard::Mopaq::SFileAuthenticateArchiveEx ( archive, &authresult_temp
                                                   , modulus, modulus_length
                                                   , exponent, exponent_length
                                                   , "ARCHIVE"
                                                   )
     )
  {
    SErrSetLastError(Blizzard::Mopaq::SFileGetLastError());
    result = false;
  }

  *authresult = authresult_temp;
  return result;
}
